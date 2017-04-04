#include "Header.hpp"
#include "ForwardPipeline.hpp"
#include "Texture2D.hpp"
#include "Rasterizer.hpp"
#include "Renderable.hpp"
#include "Shader.hpp"
#include "Pipeline.hpp"
#include "Renderer.hpp"
#include "Context.hpp"
#include "Entity.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "MainWindow.hpp"
#include "PerformanceCounter.hpp"
#include "GeometryLayout.hpp"

#include <ppl.h>

namespace X
{

	namespace
	{
		struct SceneConstantPackage
			: Noncopyable
		{
			AmbientLight* ambientLight;
			DirectionalLight* directionalLight;
			f32V3 directionalLightViewDirection;
			f32V3 directionalLightHalfVector;
			std::vector<PointLight*> pointLights;
			std::vector<f32V3> pointLightViewPositions;
		};

		struct ConstantPackage
		{
			Material* material;
			f32M44 modelToClipMatrix;
			f32M44 modelToViewMatrix;
			SceneConstantPackage const* sceneConstantPackage;
		};


		struct PreZTransformVertexShader
			: public VertexShader
		{
			virtual void Execute(void const* attributeInput, void const* constantInput, void* attributeOutput) override
			{
				AttributeInputPackage const* input = static_cast<AttributeInputPackage const*>(attributeInput);
				ConstantPackage const* constant = static_cast<ConstantPackage const*>(constantInput);
				f32V4* output = static_cast<f32V4*>(attributeOutput);

				f32V4 position = Transform(f32V4(input->vertex.position.X(), input->vertex.position.Y(), input->vertex.position.Z(), 1), constant->modelToClipMatrix);
				*output = position;
			}
		};

		struct TransformVertexShader
			: public VertexShader
		{
			virtual void Execute(void const* attributeInput, void const* constantInput, void* attributeOutput) override
			{
				AttributeInputPackage const* input = static_cast<AttributeInputPackage const*>(attributeInput);
				ConstantPackage const* constant = static_cast<ConstantPackage const*>(constantInput);
				AttributeOutputPackage* output = static_cast<AttributeOutputPackage*>(attributeOutput);

				f32V3 positionInView = Transform(input->vertex.position, constant->modelToViewMatrix);
				f32V4 position = Transform(f32V4(input->vertex.position.X(), input->vertex.position.Y(), input->vertex.position.Z(), 1), constant->modelToClipMatrix);
				f32V3 normalInView = TransformDirection(input->vertex.normal, constant->modelToViewMatrix);
				*output = AttributeOutputPackage(Vertex(positionInView, normalInView, input->vertex.textureCoordinate), position);
			}
		};
		
		struct ShadingPixelShader
			: public FragmentShader
		{
			virtual void Execute(void const* fragmentInput, void const* constantInput, void* fragmentOutput) override
			{
				AttributeOutputPackage const* input = static_cast<AttributeOutputPackage const*>(fragmentInput);
				ConstantPackage const* constant = static_cast<ConstantPackage const*>(constantInput);
				f32V3* output = static_cast<f32V3*>(fragmentOutput);

				f32V3 finalColor = f32V3(0, 0, 0);
				if (constant->material != nullptr)
				{
					std::shared_ptr<SurfaceShader> const& surfaceShader = constant->material->GetSurfaceShader();
					if (constant->material->GetDiffuseTexture() == nullptr)
					{
						*output = finalColor;
						return;
					}
					f32V3 diffuseColor = constant->material->GetDiffuseSampler()->Sample<f32V3>(*constant->material->GetDiffuseTexture(), input->vertex.textureCoordinate);
					f32V3 viewDirection = -Normalize(input->vertex.position);

					f32V3 surfaceNormal = Normalize(input->vertex.normal);

					// point lights
					for (u32 i = 0; i < constant->sceneConstantPackage->pointLights.size(); ++i)
					{
						PointLight* light = constant->sceneConstantPackage->pointLights[i];
						f32V3 const& lightViewPosition = constant->sceneConstantPackage->pointLightViewPositions[i];
						f32V3 direction = light->GetLightDirection(lightViewPosition, input->vertex.position);
						f32 dot = Dot(direction, surfaceNormal);
						if (dot > 0)
						{
							f32V3 intensity = light->GetLightIntensity(lightViewPosition, input->vertex.position);
							if (intensity.LengthSquared() >= 0.0001)
							{
								f32V3 half = Normalize(viewDirection + direction);
								f32V3 shadedColor = intensity * dot * surfaceShader->Shading(diffuseColor, surfaceNormal, half, viewDirection, direction);
								finalColor = finalColor + shadedColor;
							}
						}
					}

					// directional light
					if (DirectionalLight* light = constant->sceneConstantPackage->directionalLight)
					{
						f32V3 direction = constant->sceneConstantPackage->directionalLightViewDirection;
						f32 dot = Dot(direction, surfaceNormal);
						if (dot > 0)
						{
							f32V3 intensity = light->GetLightIntensity();
							f32V3 half = Normalize(viewDirection + direction);
							f32V3 shadedColor = intensity * dot * surfaceShader->Shading(diffuseColor, surfaceNormal, half, viewDirection, direction);
							finalColor = finalColor + shadedColor;
						}
					}

					// ambient light
					if (AmbientLight* light = constant->sceneConstantPackage->ambientLight)
					{
						f32V3 shadedColor = diffuseColor / PI * light->GetLightIntensity();
						finalColor = finalColor + shadedColor;
					}

				}
				*output = finalColor;
			}
		};
	}

	struct ForwardPipeline::Impl
	{
		ForwardPipeline& pipeline_;
		std::shared_ptr<VertexShader> preZVertexShader_;
		std::shared_ptr<VertexShader> vertexShader_;
		std::shared_ptr<FragmentShader> fragmentShader_;

		std::unique_ptr<ConcreteTexture2D<f32>> depthBuffer_;
		std::unique_ptr<LineRasterizer> lineRasterizer_;
		std::unique_ptr<FillRasterizer> fillRasterizer_;
		std::vector<AttributeOutputPackage> attributeBuffer_;

		RenderablePackCollector collector_;

		PerformanceCounter& performanceCounter_;
		Context& context_;

		Impl(ForwardPipeline& pipeline)
			: pipeline_(pipeline), performanceCounter_(pipeline.GetRenderer().GetContext().GetPerformanceCounter()), context_(pipeline.GetRenderer().GetContext())
		{
			depthBuffer_ = std::make_unique<ConcreteTexture2D<f32>>(pipeline.GetBufferSize());

			lineRasterizer_ = std::make_unique<LineRasterizer>();
			fillRasterizer_ = std::make_unique<FillRasterizer>();

			preZVertexShader_ = std::make_shared<PreZTransformVertexShader>();
			vertexShader_ = std::make_shared<TransformVertexShader>();
			fragmentShader_ = std::make_shared<ShadingPixelShader>();

		}

		struct PreZContinuation
		{
			PreZContinuation(Impl& impl, ConstantPackage* constant)
			{
			}
			void operator() (AttributeOutputPackage const& fragmentInput, Point<u32, 2> sceenCoordinate) const
			{
			}
		};

		struct ShadingContinuation
		{
			Impl& impl;
			ConstantPackage constant;
			ShadingContinuation(Impl& impl, ConstantPackage* constant)
				: impl(impl), constant(*constant)
			{
			}
			void operator() (AttributeOutputPackage const& fragmentInput, Point<u32, 2> sceenCoordinate) const
			{
				f32V3 element;
				(*impl.fragmentShader_)(&fragmentInput, &constant, &element);
				impl.pipeline_.GetRenderer().GetColorBuffer().SetValue(0, sceenCoordinate, element);
			}
		};

		template <typename FragmentContinuationT>
		void RenderMesh(std::shared_ptr<Entity> const& entity, f32M44 const& viewProjectionMatrix, f32M44 const& viewMatrix, Frustum const& frustum, SceneConstantPackage const* sceneConstant)
		{
			Renderable* renderable = entity->GetComponent<Renderable>();
			if (renderable != nullptr && renderable->IsActive())
			{
				Transformation* transformation = entity->GetComponent<Transformation>();
				f32M44 worldMatrix = transformation->GetWorldMatrix();
				f32M44 worldViewMatrix = worldMatrix * viewMatrix;
				RotatedBoundingBox box = Transform(renderable->GetBoundingBox(), worldViewMatrix);
				if (!IntersectRough(box, frustum))
				{
					return;
				}
				renderable->GetRenderablePackage(collector_, frustum, worldViewMatrix);

				for (auto& renderablePackage : collector_.GetAllPackages())
				{
					ConstantPackage constant;
					constant.material = renderablePackage.material.get();
					constant.modelToClipMatrix = worldMatrix * viewProjectionMatrix;
					constant.modelToViewMatrix = worldMatrix * viewMatrix;
					constant.sceneConstantPackage = sceneConstant;


					std::vector<u16> const& indices = renderablePackage.layout->GetIndexBuffer()->GetData();
					assert(indices.size() % 3 == 0);
					std::vector<Vertex> const& vertices = renderablePackage.layout->GetVertexBuffer()->GetData();
					if (attributeBuffer_.size() < vertices.size())
					{
						attributeBuffer_.resize(vertices.size());
					}

					Material::RasterizeMode mode = renderablePackage.material->GetRasterizeMode();

					// vertex shading
					for (u32 i = 0; i < vertices.size(); ++i)
					{
						(*vertexShader_)(&AttributeInputPackage(vertices[i]), &constant, &attributeBuffer_[i]);
					}

					FragmentContinuationT continuation(*this, &constant);

					// rasterize
					switch (mode)
					{
					case Material::RasterizeMode::Line:
						for (u32 i = 0; i < indices.size(); i += 3)
						{
							AttributeOutputPackage& v0 = attributeBuffer_[indices[i + 0]];
							AttributeOutputPackage& v1 = attributeBuffer_[indices[i + 1]];
							AttributeOutputPackage& v2 = attributeBuffer_[indices[i + 2]];
							lineRasterizer_->Rasterize(pipeline_.GetBufferSize(), *depthBuffer_, continuation, Triangle(v0, v1, v2));
						}
						break;
					case Material::RasterizeMode::Fill:
						performanceCounter_.Begin(PerformanceCounter::Term::ForwardTotalRasterize);
						if (context_.GetThreadSupport() == 1)
						{
							for (u32 i = 0; i < indices.size(); i += 3)
							{
								AttributeOutputPackage& v0 = attributeBuffer_[indices[i + 0]];
								AttributeOutputPackage& v1 = attributeBuffer_[indices[i + 1]];
								AttributeOutputPackage& v2 = attributeBuffer_[indices[i + 2]];
								Triangle triangle(v0, v1, v2);
								fillRasterizer_->Rasterize(pipeline_.GetBufferSize(), *depthBuffer_, continuation, Triangle(v0, v1, v2));
							}
						}
						else
						{
							concurrency::parallel_for(0u, indices.size() / 3, [this, &indices, &continuation] (u32 index)
							{
								AttributeOutputPackage& v0 = attributeBuffer_[indices[index * 3 + 0]];
								AttributeOutputPackage& v1 = attributeBuffer_[indices[index * 3 + 1]];
								AttributeOutputPackage& v2 = attributeBuffer_[indices[index * 3 + 2]];
								Triangle triangle(v0, v1, v2);
								fillRasterizer_->Rasterize(pipeline_.GetBufferSize(), *depthBuffer_, continuation, triangle);
							});
						}
						performanceCounter_.End(PerformanceCounter::Term::ForwardTotalRasterize);
						break;
					default:
						assert(false);
						break;
					}
				}
				collector_.Clear();
			}
		}

		void PreZ(std::shared_ptr<Entity> const& entity, f32M44 const& viewProjectionMatrix, f32M44 const& viewMatrix, Frustum const& frustum, SceneConstantPackage const* sceneConstant)
		{
			RenderMesh<PreZContinuation>(entity, viewProjectionMatrix, viewMatrix, frustum, sceneConstant);
		}

		void Render(std::shared_ptr<Entity> const& entity, f32M44 const& viewProjectionMatrix, f32M44 const& viewMatrix, Frustum const& frustum, SceneConstantPackage const* sceneConstant)
		{
			RenderMesh<ShadingContinuation>(entity, viewProjectionMatrix, viewMatrix, frustum, sceneConstant);
		}



	};

	ForwardPipeline::ForwardPipeline(Renderer& renderer, Size<u32, 2> const& bufferSize)
		: Pipeline(renderer, bufferSize)
	{
		impl_ = std::make_unique<Impl>(*this);
	}


	ForwardPipeline::~ForwardPipeline()
	{
	}

	void ForwardPipeline::RenderScene(f64 current, f32 delta)
	{
		Scene& scene = GetRenderer().GetContext().GetScene();
		std::vector<std::shared_ptr<Entity>> entities = scene.GetAllEntities();
		Entity* camera = scene.GetActiveCameraEntity();

		f32V3 colorClearValue = f32V3(0, 0, 0);
		ConcreteTexture2D<f32V3>& colorBuffer = GetRenderer().GetColorBuffer();
		colorBuffer.Clear(0, colorClearValue);

		impl_->depthBuffer_->Clear(0, 1.f);

		SceneConstantPackage sceneConstant;
		sceneConstant.ambientLight = nullptr;
		sceneConstant.directionalLight = nullptr;
		sceneConstant.directionalLightViewDirection = f32V3(0, 0, 0);
		sceneConstant.pointLights.clear();
		sceneConstant.pointLightViewPositions.clear();


		f32M44 viewMatrix = camera->GetComponent<Camera>()->GetViewMatrix();
		f32M44 projectionMatrix = camera->GetComponent<Camera>()->GetProjectionMatrix();
		Frustum const& frustum = camera->GetComponent<Camera>()->GetFrustum();
		f32M44 viewProjectionMatrix = viewMatrix * projectionMatrix;

		for (auto& entity : entities)
		{
			Light* light = entity->GetComponent<Light>();
			if (light != nullptr && light->IsActive())
			{
				if (AmbientLight* ambientLight = dynamic_cast<AmbientLight*>(light))
				{
					sceneConstant.ambientLight = ambientLight;
				}
				else if (DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(light))
				{
					sceneConstant.directionalLight = directionalLight;
					f32V3 lightPosition = directionalLight->GetOwner()->GetComponent<Transformation>()->GetPosition();
					if (lightPosition.LengthSquared() == 0)
					{
						lightPosition = f32V3(0, 1, 0); // hack
					}
					sceneConstant.directionalLightViewDirection = TransformDirection(Normalize(lightPosition), viewMatrix);
				}
				else if (PointLight* pointLight = dynamic_cast<PointLight*>(light))
				{
					sceneConstant.pointLights.push_back(pointLight);
					f32V3 lightPosition = pointLight->GetOwner()->GetComponent<Transformation>()->GetPosition();
					f32V3 lightViewPosition = Transform(lightPosition, viewMatrix);
					sceneConstant.pointLightViewPositions.push_back(lightViewPosition);
				}
				else
				{
					assert(false);
				}
			}
		}
		impl_->performanceCounter_.Begin(PerformanceCounter::Term::ForwardPreZPass);
		concurrency::parallel_for_each(entities.begin(), entities.end(), [this, &viewMatrix, &viewProjectionMatrix, &frustum] (std::shared_ptr<Entity> const& entity)
		{
			impl_->PreZ(entity, viewProjectionMatrix, viewMatrix, frustum, nullptr);
		});
		impl_->performanceCounter_.End(PerformanceCounter::Term::ForwardPreZPass);

		impl_->performanceCounter_.Begin(PerformanceCounter::Term::ForwardRenderPass);
		concurrency::parallel_for_each(entities.begin(), entities.end(), [this, &viewMatrix, &viewProjectionMatrix, &frustum, &sceneConstant] (std::shared_ptr<Entity> const& entity)
		{
			impl_->Render(entity, viewProjectionMatrix, viewMatrix, frustum, &sceneConstant);
		});
		impl_->performanceCounter_.End(PerformanceCounter::Term::ForwardRenderPass);

	}

}
