#include "Header.hpp"
#include "DefferredPipeline.hpp"
#include "Context.hpp"
#include "Renderer.hpp"
#include "Context.hpp"
#include "MainWindow.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "Transformation.hpp"
#include "Renderable.hpp"
#include "Camera.hpp"
#include "Buffer.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "Primitive.hpp"
#include "PipelineDetail.hpp"
#include "Shader.hpp"
#include "ThreadedTaskPool.hpp"
#include "Rasterizer.hpp"
#include "PerformanceCounter.hpp"
#include "GeometryLayout.hpp"

#include <ppl.h>

namespace X
{
	namespace
	{
		struct GBufferElement
		{
			Material* material;
			f32V2 textureCoordinate;
			f32V3 position;
			f32V3 normal;
		};

		struct GeometryPassPixelInputPackage
		{
			Vertex const& vertex;

			GeometryPassPixelInputPackage(Vertex const& vertex)
				: vertex(vertex)
			{
			}
		};

		struct SceneConstantPackage
			: Noncopyable
		{
			AmbientLight* ambientLight;
			DirectionalLight* directionalLight;
			f32V3 directionalLightViewDirection;
			f32V3 directionalLightHalfVector;
			std::vector<PointLight*> pointLights;
			std::vector<f32V3> pointLightViewPositions;
			f32M44 projectionMatrix;
			f32 far;
			PerformanceCounter* pc;
		};

		struct ConstantPackage
		{
			Material* material;
			f32M44 modelToClipMatrix;
			f32M44 modelToViewMatrix;
		};

		struct ShadingResource
		{
			ConcreteTexture2D<GBufferElement>* gBuffer;
			ConcreteTexture2D<f32V3>* colorBuffer;
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

		struct AttributeWritingPixelShader
			: public FragmentShader
		{
			virtual void Execute(void const* fragmentInput, void const* constantInput, void* fragmentOutput) override
			{
				AttributeOutputPackage const* input = static_cast<AttributeOutputPackage const*>(fragmentInput);
				ConstantPackage const* constant = static_cast<ConstantPackage const*>(constantInput);
				GBufferElement* output = static_cast<GBufferElement*>(fragmentOutput);

				output->material = constant->material;
				output->position = input->vertex.position;
				output->normal = input->vertex.normal;
				output->textureCoordinate = input->vertex.textureCoordinate;
			}

		};

		static const u32 TileSize = 16;

		/*
		*	Launch with 1 thread per work group and with total number of tiles of groups.
		*/
		struct TiledShadingShader
			: public ComputeShader
		{
			virtual void Execute(WorkSize const& size, Point<u32, 3> const& groupIndex, Point<u32, 3> const& threadIndex, void const* constantInput, void* resource) override
			{
				SceneConstantPackage const* constant = static_cast<SceneConstantPackage const*>(constantInput);
				ShadingResource* shadingResource = static_cast<ShadingResource*>(resource);
				ConcreteTexture2D<GBufferElement>* gBuffer = shadingResource->gBuffer;
				ConcreteTexture2D<f32V3>* colorBuffer = shadingResource->colorBuffer;

				constant->pc->Begin(PerformanceCounter::Term::TiledFrustumCulling);

				f32 minTileZ = std::numeric_limits<f32>::max();
				f32 maxTileZ = 0;
				u32 xStart = TileSize * groupIndex.X();
				u32 yStart = TileSize * groupIndex.Y();
				
				for (u32 y = 0; y < TileSize; ++y)
				{
					for (u32 x = 0; x < TileSize; ++x)
					{
						f32 z = gBuffer->GetValue(0, Point<u32, 2>(xStart + x, yStart + y)).position.Z();
						if (z <= constant->far)
						{
							minTileZ = std::min(z, minTileZ);
							maxTileZ = std::max(z, maxTileZ);
						}
					}
				}

				std::vector<u32> lightIndices;
				lightIndices.reserve(16);

				if (minTileZ <= maxTileZ)
				{
					// for details, see 'Deferred Rendering for Current and Future Rendering Pipelines' by Intel.
					f32V2 tileScale = f32V2(f32(size.groupCount.X()), f32(size.groupCount.Y()));
					f32V2 tileBias = tileScale - 2 * f32V2(f32(groupIndex.X()), f32(groupIndex.Y())) - f32V2(1, 1);
					// NOTE: below are used by Intel but it's not the most tight frustum
					//f32V2 tileScale = f32V2(f32(size.groupCount.X()), f32(size.groupCount.Y())) / 2;
					//f32V2 tileBias = tileScale - f32V2(f32(groupIndex.X()), f32(groupIndex.Y()));

					// projection matrix
					// relevant matrix columns for this tile frusta
					f32V3 c1 = f32V3(constant->projectionMatrix(0, 0) * tileScale.X(), 0.0f, tileBias.X());
					f32V3 c2 = f32V3(0.0f, constant->projectionMatrix(1, 1) * tileScale.Y(), tileBias.Y());
					f32V3 c4 = f32V3(0.0f, 0.0f, 1.0f);

					// derive frustum planes
					std::array<Plane, 6> frustumPlanes;
					// right/left/bottom/top
					frustumPlanes[0] = Plane(-Normalize(c4 - c1), 0);
					frustumPlanes[1] = Plane(-Normalize(c4 + c1), 0);
					frustumPlanes[2] = Plane(-Normalize(c4 - c2), 0);
					frustumPlanes[3] = Plane(-Normalize(c4 + c2), 0);
					// near/far
					frustumPlanes[4] = Plane(f32V3(0.0f, 0.0f, -1.0f), minTileZ);
					frustumPlanes[5] = Plane(f32V3(0.0f, 0.0f, 1.0f), -maxTileZ);

					Frustum frustum(frustumPlanes);
					
					for (u32 i = 0; i < constant->pointLights.size(); ++i)
					{
						PointLight* pointLight = constant->pointLights[i];
						Sphere sphere = Sphere(constant->pointLightViewPositions[i], pointLight->GetRadius());
						if (IntersectRough(frustum, sphere))
						{
							lightIndices.push_back(i);
						}
					}
				}
				constant->pc->End(PerformanceCounter::Term::TiledFrustumCulling);


				constant->pc->Begin(PerformanceCounter::Term::TiledShading);
				for (u32 y = 0; y < TileSize; ++y)
				{
					for (u32 x = 0; x < TileSize; ++x)
					{
						GBufferElement const& input = gBuffer->GetValue(0, Point<u32, 2>(xStart + x, yStart + y));
						f32V3 finalColor = Shading(input, constant, lightIndices);
						colorBuffer->SetValue(0, Point<u32, 2>(xStart + x, yStart + y), finalColor);
					}
				}
				constant->pc->End(PerformanceCounter::Term::TiledShading);

			}

			f32V3 Shading(GBufferElement const& input, SceneConstantPackage const* constant, std::vector<u32> const& pointLightIndices)
			{
				f32V3 finalColor = f32V3(0, 0, 0);
				if (input.material != nullptr)
				{
					std::shared_ptr<SurfaceShader> const& surfaceShader = input.material->GetSurfaceShader();
					if (input.material->GetDiffuseTexture() == nullptr)
					{
						return finalColor;
					}
					f32V3 diffuseColor = input.material->GetDiffuseSampler()->Sample<f32V3>(*input.material->GetDiffuseTexture(), input.textureCoordinate);
					f32V3 viewDirection = -Normalize(input.position);

					f32V3 surfaceNormal = Normalize(input.normal);

					// point lights
					for (u32 i = 0; i < pointLightIndices.size(); ++i)
					{
						PointLight* light = constant->pointLights[pointLightIndices[i]];
						f32V3 const& lightViewPosition = constant->pointLightViewPositions[pointLightIndices[i]];
						f32V3 direction = light->GetLightDirection(lightViewPosition, input.position);
						f32 dot = Dot(direction, surfaceNormal);
						if (dot > 0)
						{
							f32V3 intensity = light->GetLightIntensity(lightViewPosition, input.position);
							if (intensity.LengthSquared() >= 0.0001)
							{
								f32V3 half = Normalize(viewDirection + direction);
								f32V3 shadedColor = intensity * dot * surfaceShader->Shading(diffuseColor, surfaceNormal, half, viewDirection, direction);
								finalColor = finalColor + shadedColor;
							}
						}
					}

					// directional light
					if (DirectionalLight* light = constant->directionalLight)
					{
						f32V3 direction = constant->directionalLightViewDirection;
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
					if (AmbientLight* light = constant->ambientLight)
					{
						f32V3 shadedColor = diffuseColor / PI * light->GetLightIntensity();
						finalColor = finalColor + shadedColor;
					}
				}
				return finalColor;

			}
		};

	}

	struct DefferredPipeline::Impl
	{
		DefferredPipeline& pipeline_;
		std::shared_ptr<VertexShader> vertexShader_;
		std::shared_ptr<FragmentShader> fragmentShader_;
		std::shared_ptr<ComputeShader> tiledShadingShader_;

		std::unique_ptr<ConcreteTexture2D<GBufferElement>> gbuffer_;
		std::unique_ptr<ConcreteTexture2D<f32>> depthBuffer_;
		std::unique_ptr<LineRasterizer> lineRasterizer_;
		std::unique_ptr<FillRasterizer> fillRasterizer_;
		std::vector<AttributeOutputPackage> attributeBuffer_;

		RenderablePackCollector collector_;

		//ThreadedTaskPool pool_;
		PerformanceCounter& performanceCounter_;
		Context& context_;

		Impl(DefferredPipeline& pipeline)
			: pipeline_(pipeline), performanceCounter_(pipeline.GetRenderer().GetContext().GetPerformanceCounter()), context_(pipeline.GetRenderer().GetContext())
		{
			gbuffer_ = std::make_unique<ConcreteTexture2D<GBufferElement>>(pipeline.GetBufferSize());
			depthBuffer_ = std::make_unique<ConcreteTexture2D<f32>>(pipeline.GetBufferSize());

			lineRasterizer_ = std::make_unique<LineRasterizer>();
			fillRasterizer_ = std::make_unique<FillRasterizer>();

			vertexShader_ = std::make_shared<TransformVertexShader>();
			fragmentShader_ = std::make_shared<AttributeWritingPixelShader>();
			tiledShadingShader_ = std::make_shared<TiledShadingShader>();
		}

		void GeometryPass(std::shared_ptr<Entity> const& entity, f32M44 const& viewProjectionMatrix, f32M44 const& viewMatrix, Frustum const& frustum)
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
				renderable->GetRenderablePackage(collector_, frustum, worldMatrix * viewMatrix);

				for (auto& renderablePackage : collector_.GetAllPackages())
				{
					ConstantPackage constant;
					constant.material = renderablePackage.material.get();
					constant.modelToClipMatrix = worldMatrix * viewProjectionMatrix;
					constant.modelToViewMatrix = worldMatrix * viewMatrix;


					std::vector<u16> const& indices = renderablePackage.layout->GetIndexBuffer()->GetData();
					assert(indices.size() % 3 == 0);
					std::vector<Vertex> const& vertices = renderablePackage.layout->GetVertexBuffer()->GetData();
					if (attributeBuffer_.size() < vertices.size())
					{
						attributeBuffer_.resize(vertices.size());
					}

					Material::RasterizeMode mode = renderablePackage.material->GetRasterizeMode();

					// vertex shading
					performanceCounter_.Begin(PerformanceCounter::Term::DeferredVertex);
					for (u32 i = 0; i < vertices.size(); ++i)
					{
						(*vertexShader_)(&AttributeInputPackage(vertices[i]), &constant, &attributeBuffer_[i]);
					}

					performanceCounter_.End(PerformanceCounter::Term::DeferredVertex);

					auto continuation = [this, &constant] (AttributeOutputPackage const& fragmentInput, Point<u32, 2> sceenCoordinate)
					{
						GBufferElement element;
						(*fragmentShader_)(&fragmentInput, &constant, &element);
						gbuffer_->SetValue(0, sceenCoordinate, element);
					};
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
						performanceCounter_.Begin(PerformanceCounter::Term::DeferredRasterizeAndPixel);
						if (context_.GetThreadSupport() == 1)
						{
							for (u32 i = 0; i < indices.size(); i += 3)
							{
								AttributeOutputPackage& v0 = attributeBuffer_[indices[i + 0]];
								AttributeOutputPackage& v1 = attributeBuffer_[indices[i + 1]];
								AttributeOutputPackage& v2 = attributeBuffer_[indices[i + 2]];
								Triangle triangle(v0, v1, v2);

								fillRasterizer_->Rasterize(pipeline_.GetBufferSize(), *depthBuffer_, continuation, triangle);
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
						performanceCounter_.End(PerformanceCounter::Term::DeferredRasterizeAndPixel);
						break;
					default:
						assert(false);
						break;
					}
					//taskGroup_.wait();
				}
				collector_.Clear();
			}
		}
	};

	DefferredPipeline::DefferredPipeline(Renderer& renderer, Size<u32, 2> const& bufferSize)
		: Pipeline(renderer, bufferSize)
	{
		impl_ = std::make_unique<Impl>(*this);
	}


	DefferredPipeline::~DefferredPipeline()
	{
	}




	// geometry pass:
	// vertex shader (transformation)
	// triangle setup
	// rasterize(culling and clipping first, then viewport transform)
	// depth test
	// fragment write
	// shading pass:
	// fragment shading (surface shader pixel shading process)
	void DefferredPipeline::RenderScene(f64 current, f32 delta)
	{
		Scene& scene = GetRenderer().GetContext().GetScene();
		std::vector<std::shared_ptr<Entity>> entities = scene.GetAllEntities();
		Entity* camera = scene.GetActiveCameraEntity();

		// clear gbuffer
		GBufferElement gBufferClearValue;
		gBufferClearValue.material = nullptr;
		gBufferClearValue.normal = f32V3(0, 0, 0);
		gBufferClearValue.textureCoordinate = f32V2(0, 0);
		impl_->gbuffer_->Clear(0, gBufferClearValue);
		impl_->depthBuffer_->Clear(0, 1.f);

		SceneConstantPackage sceneConstant;

		sceneConstant.pc = &impl_->performanceCounter_;
		sceneConstant.ambientLight = nullptr;
		sceneConstant.directionalLight = nullptr;
		sceneConstant.directionalLightViewDirection = f32V3(0, 0, 0);
		sceneConstant.pointLights.clear();
		sceneConstant.pointLightViewPositions.clear();


		f32M44 viewMatrix = camera->GetComponent<Camera>()->GetViewMatrix();
		f32M44 projectionMatrix = camera->GetComponent<Camera>()->GetProjectionMatrix();
		f32M44 viewProjectionMatrix = viewMatrix * projectionMatrix;

		sceneConstant.projectionMatrix = projectionMatrix;
		sceneConstant.far = CheckedCast<PerspectiveCamera*>(camera->GetComponent<Camera>())->GetFar();

		Frustum const& frustum = camera->GetComponent<Camera>()->GetFrustum();

		impl_->performanceCounter_.Begin(PerformanceCounter::Term::DeferredLightTransform);
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
		impl_->performanceCounter_.End(PerformanceCounter::Term::DeferredLightTransform);

		impl_->performanceCounter_.Begin(PerformanceCounter::Term::DeferredGeometryPass);
		if (impl_->context_.GetThreadSupport() == 1)
		{
			std::for_each(entities.begin(), entities.end(), [this, &viewMatrix, &viewProjectionMatrix, &frustum] (std::shared_ptr<Entity> const& entity)
			{
				impl_->GeometryPass(entity, viewProjectionMatrix, viewMatrix, frustum);
			});
		}
		else
		{
			concurrency::parallel_for_each(entities.begin(), entities.end(), [this, &viewMatrix, &viewProjectionMatrix, &frustum] (std::shared_ptr<Entity> const& entity)
			{
				impl_->GeometryPass(entity, viewProjectionMatrix, viewMatrix, frustum);
			});
		}
		impl_->performanceCounter_.End(PerformanceCounter::Term::DeferredGeometryPass);


		impl_->performanceCounter_.Begin(PerformanceCounter::Term::DeferredShadingPass);
		ShadingResource shadingResource;
		shadingResource.colorBuffer = &GetRenderer().GetColorBuffer();
		shadingResource.gBuffer = impl_->gbuffer_.get();
		ComputeLauncher l(impl_->tiledShadingShader_, &sceneConstant, &shadingResource);
		ComputeShader::WorkSize workSize(Size<u32, 3>(GetBufferSize().X() / TileSize, GetBufferSize().Y() / TileSize, 1), Size<u32, 3>(1, 1, 1));
		l.Launch(workSize, impl_->context_.GetThreadSupport());
		impl_->performanceCounter_.End(PerformanceCounter::Term::DeferredShadingPass);
	}

}

