#include "PrecompiledHeader.h"

#include <All.hpp>
#include <ForwardPipeline.hpp>
#include <DefferredPipeline.hpp>
#include <PerformanceCounter.hpp>

#include <string>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace X;

struct PhongShader
	: public SurfaceShader
{
	virtual f32V3 Shading(f32V3 const& diffuse, f32V3 const& normal, f32V3 const& half, f32V3 const& viewDirection, f32V3 const& lightDirection) override
	{
		f32 halfCos = std::max(Dot(half, normal), 0.f);
		static f32 const m = 10;
		f32V3 color = diffuse * (1 / PI + (m + 8) / (8 * PI) * std::pow(halfCos, m));
		return color;
	}
};

std::pair<std::shared_ptr<VertexBuffer>, std::shared_ptr<IndexBuffer>> MakeLayout()
{
	f32 const halfLength = 5.f;
	std::vector<Vertex> vertices;

	vertices.push_back(Vertex(f32V3(-halfLength, -halfLength, -halfLength), f32V3(0, 0, -1), f32V2(0, 0)));
	vertices.push_back(Vertex(f32V3(halfLength, -halfLength, -halfLength), f32V3(0, 0, -1), f32V2(1, 0)));
	vertices.push_back(Vertex(f32V3(-halfLength, halfLength, -halfLength), f32V3(0, 0, -1), f32V2(0, 1)));
	vertices.push_back(Vertex(f32V3(halfLength, halfLength, -halfLength), f32V3(0, 0, -1), f32V2(1, 1)));

	vertices.push_back(Vertex(f32V3(halfLength, -halfLength, -halfLength), f32V3(1, 0, 0), f32V2(0, 0)));
	vertices.push_back(Vertex(f32V3(halfLength, -halfLength, halfLength), f32V3(1, 0, 0), f32V2(1, 0)));
	vertices.push_back(Vertex(f32V3(halfLength, halfLength, -halfLength), f32V3(1, 0, 0), f32V2(0, 1)));
	vertices.push_back(Vertex(f32V3(halfLength, halfLength, halfLength), f32V3(1, 0, 0), f32V2(1, 1)));

	vertices.push_back(Vertex(f32V3(halfLength, -halfLength, halfLength), f32V3(0, 0, 1), f32V2(0, 0)));
	vertices.push_back(Vertex(f32V3(-halfLength, -halfLength, halfLength), f32V3(0, 0, 1), f32V2(1, 0)));
	vertices.push_back(Vertex(f32V3(halfLength, halfLength, halfLength), f32V3(0, 0, 1), f32V2(0, 1)));
	vertices.push_back(Vertex(f32V3(-halfLength, halfLength, halfLength), f32V3(0, 0, 1), f32V2(1, 1)));

	vertices.push_back(Vertex(f32V3(-halfLength, -halfLength, halfLength), f32V3(-1, 0, 0), f32V2(0, 0)));
	vertices.push_back(Vertex(f32V3(-halfLength, -halfLength, -halfLength), f32V3(-1, 0, 0), f32V2(1, 0)));
	vertices.push_back(Vertex(f32V3(-halfLength, halfLength, halfLength), f32V3(-1, 0, 0), f32V2(0, 1)));
	vertices.push_back(Vertex(f32V3(-halfLength, halfLength, -halfLength), f32V3(-1, 0, 0), f32V2(1, 1)));

	vertices.push_back(Vertex(f32V3(-halfLength, halfLength, -halfLength), f32V3(0, 1, 0), f32V2(0, 0)));
	vertices.push_back(Vertex(f32V3(halfLength, halfLength, -halfLength), f32V3(0, 1, 0), f32V2(1, 0)));
	vertices.push_back(Vertex(f32V3(-halfLength, halfLength, halfLength), f32V3(0, 1, 0), f32V2(0, 1)));
	vertices.push_back(Vertex(f32V3(halfLength, halfLength, halfLength), f32V3(0, 1, 0), f32V2(1, 1)));

	vertices.push_back(Vertex(f32V3(-halfLength, -halfLength, halfLength), f32V3(0, -1, 0), f32V2(0, 0)));
	vertices.push_back(Vertex(f32V3(halfLength, -halfLength, halfLength), f32V3(0, -1, 0), f32V2(1, 0)));
	vertices.push_back(Vertex(f32V3(-halfLength, -halfLength, -halfLength), f32V3(0, -1, 0), f32V2(0, 1)));
	vertices.push_back(Vertex(f32V3(halfLength, -halfLength, -halfLength), f32V3(0, -1, 0), f32V2(1, 1)));

	std::vector<u16> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);

	indices.push_back(0 + 4);
	indices.push_back(1 + 4);
	indices.push_back(2 + 4);
	indices.push_back(2 + 4);
	indices.push_back(1 + 4);
	indices.push_back(3 + 4);

	indices.push_back(0 + 8);
	indices.push_back(1 + 8);
	indices.push_back(2 + 8);
	indices.push_back(2 + 8);
	indices.push_back(1 + 8);
	indices.push_back(3 + 8);

	indices.push_back(0 + 12);
	indices.push_back(1 + 12);
	indices.push_back(2 + 12);
	indices.push_back(2 + 12);
	indices.push_back(1 + 12);
	indices.push_back(3 + 12);

	indices.push_back(0 + 16);
	indices.push_back(1 + 16);
	indices.push_back(2 + 16);
	indices.push_back(2 + 16);
	indices.push_back(1 + 16);
	indices.push_back(3 + 16);

	indices.push_back(0 + 20);
	indices.push_back(1 + 20);
	indices.push_back(2 + 20);
	indices.push_back(2 + 20);
	indices.push_back(1 + 20);
	indices.push_back(3 + 20);

	return std::make_pair(std::make_shared<VertexBuffer>(std::move(vertices)), std::make_shared<IndexBuffer>(std::move(indices)));
}

std::shared_ptr<Material> MakeMaterial(std::shared_ptr<ConcreteTexture2D<f32V3>> diffuseTexture)
{
	std::shared_ptr<Material> material = std::make_shared<Material>();

// 	Size<u32, 2> textureSize(64, 64);
// 	std::shared_ptr<ConcreteTexture2D<f32V3>> diffuseTexture = std::make_shared<ConcreteTexture2D<f32V3>>(textureSize);
// 	for (u32 y = 0; y < textureSize.Y(); ++y)
// 	{
// 		for (u32 x = 0; x < textureSize.X(); ++x)
// 		{
// 			diffuseTexture->SetValue(0, Point<u32, 2>(x, y),
// 				f32V3(Lerp(0.f, 1.f, f32(x) / textureSize.X()), Lerp(0.f, 1.f, f32(y) / textureSize.Y()), Lerp(0.5f, 1.f, f32(x) / textureSize.X())));
// 		}
// 	}
	// 	diffuseTexture->SetValue(0, Point<u32, 2>(0, 0), f32V3(0, 0, 1));
	// 	diffuseTexture->SetValue(0, Point<u32, 2>(0, 1), f32V3(0, 1, 0));
	// 	diffuseTexture->SetValue(0, Point<u32, 2>(1, 0), f32V3(1, 0, 0));
	// 	diffuseTexture->SetValue(0, Point<u32, 2>(1, 1), f32V3(1, 1, 1));

	material->SetDiffuseTexture(diffuseTexture);

	std::shared_ptr<PointSampler<Sampler::RepeatAddresser>> diffuseSampler = std::make_shared<PointSampler<Sampler::RepeatAddresser>>();
	material->SetDiffuseSampler(diffuseSampler);

	std::shared_ptr<SurfaceShader> surfaceShader = std::make_shared<PhongShader>();
	material->SetSurfaceShader(surfaceShader);

	material->SetRasterizeMode(Material::RasterizeMode::Fill);

	return material;
}

std::shared_ptr<Entity> MakeObject(std::wstring name, std::shared_ptr<GeometryLayout> layout, std::shared_ptr<Material> material)
{
	std::shared_ptr<Entity> objectEntity = std::make_shared<Entity>(std::move(name));

	std::unique_ptr<Renderable> mesh = std::make_unique<Mesh>();
	static_cast<Mesh*>(mesh.get())->CreateSubMesh(layout, material);
	objectEntity->SetComponent<Renderable>(std::move(mesh));
	return objectEntity;
}

struct GlobalController
	: public InputHandler
{
	static const s32 CameraControl = 0;
	static const s32 DirectionalLightControl = 1;
	static const s32 UseDeferredPipeline = 5;
	static const s32 UseForwardPipeline = 6;
	static const s32 IncreaseLight = 10;
	static const s32 DecreaseLight = 11;
	static const s32 IncreaseThread = 20;
	static const s32 DecreaseThread = 21;
	static const s32 ToggleStatistic = 30;
	static ActionMap CreateActionMap()
	{
		ActionMap map;
		map.Set(InputManager::InputSemantic::K_F1, CameraControl);
		map.Set(InputManager::InputSemantic::K_F2, DirectionalLightControl);
		map.Set(InputManager::InputSemantic::K_F5, UseDeferredPipeline);
		map.Set(InputManager::InputSemantic::K_F6, UseForwardPipeline);
		map.Set(InputManager::InputSemantic::K_Minus, DecreaseLight);
		map.Set(InputManager::InputSemantic::K_Equals, IncreaseLight);
		map.Set(InputManager::InputSemantic::K_LeftBracket, DecreaseThread);
		map.Set(InputManager::InputSemantic::K_RightBracket, IncreaseThread);
		map.Set(InputManager::InputSemantic::K_Enter, ToggleStatistic);
		return map;
	}

	struct StatisticPack
	{
		f64 startTime;
		u32 frameCount;		

		f32 fullTime;
		f32 renderTime;

		f32 geometryPassTime;
		f32 shadingPassTime;

		f32 lightTransformTime;
		f32 vertexTime;
		f32 rasterizeAndPixelTime;
		f32 tiledCullingTime;
		f32 tiledShadingTime;

		f32 prezTime;
		f32 renderingTime;

		f32 rasterizeTime;
	};


	GlobalController(Context& theContext)
		: InputHandler(theContext.GetInputManager(), CreateActionMap()), context(theContext), renderer(theContext.GetRenderer()),
		statisticState(false), statisticPreviousState(false)
	{
		ResetStaticsticPack();
		Scene& scene = context.GetScene();

		std::shared_ptr<Entity> cameraEntity = std::make_shared<Entity>(L"camera");
		Size<u32, 2> windowSize = context.GetMainWindow().GetClientRegionSize();
		std::unique_ptr<Camera> cameraComponent = std::make_unique<PerspectiveCamera>(PI / 2, f32(windowSize.X()) / windowSize.Y(), 1.f, 10000.f);
		cameraEntity->SetComponent(std::move(cameraComponent));

		cameraEntity->GetComponent<Transformation>()->Translate(f32V3(-1000.5f, 200.5f, -0.f));
		cameraEntity->GetComponent<Transformation>()->Rotate(RotationMatrix(PI / 2, f32V3(0, 1, 0)));

		cameraController = std::make_shared<FirstPersonCameraController>(context.GetInputManager(), 8.f);
		cameraController->AttachToCamera(cameraEntity);

		scene.SetActiveCamera(cameraEntity);

		//auto objectMesh = context.GetResourceLoader().LoadMesh("Data/jeep/jeep1.fbx");
		//auto objectMesh = context.GetResourceLoader().LoadMesh("Data/dabrovic-sponza/sponza.obj");
		auto objectMesh = context.GetResourceLoader().LoadMesh("Data/crytek-sponza/sponza.obj");

		auto surfaceShader = std::make_shared<PhongShader>();
		for (u32 i = 0; i < objectMesh->GetSubMeshCount(); ++i)
		{
			objectMesh->GetSubMesh(i).GetMaterial()->SetSurfaceShader(surfaceShader);
		}
		auto object = std::make_shared<Entity>(L"loaded object");
		object->SetComponent<Renderable>(std::unique_ptr<Renderable>(objectMesh.release()));
		scene.AddEntity(object);

		auto layout = MakeLayout();
		std::shared_ptr<ConcreteTexture2D<f32V3>> diffuseTexture = context.GetResourceLoader().LoadTexture("Data/dabrovic-sponza/reljef.JPG");
		auto material = MakeMaterial(diffuseTexture);

		// 	const u32 Count = 4;
		// 	for (u32 i = 0; i < Count; ++i)
		// 	{
		// 		for (u32 j = 0; j < Count; ++j)
		// 		{
		// 			for (u32 k = 0; k < Count; ++k)
		// 			{
		// 				std::wstring name = std::wstring(L"object") + std::to_wstring(k) + std::to_wstring(j) + std::to_wstring(i);
		// 				std::shared_ptr<Entity> object = MakeObject(std::move(name), layout, material);
		// 				auto get = [=] (u32 v)
		// 				{
		// 					return f32(v) / (Count - 1) - 0.5f;
		// 				};
		// 				f32V3 position(get(k), get(j), get(i));
		// 				object->GetComponent<Transformation>()->Translate(position * 15.f * f32(Count - 1));
		// 				scene.AddEntity(object);
		// 			}
		// 		}
		// 	}


		std::shared_ptr<Entity> ambientLightEntity = std::make_shared<Entity>(L"ambient light");
		std::unique_ptr<AmbientLight> ambientLight = std::make_unique<AmbientLight>(f32V3(0.3f, 0.3f, 0.3f));
		ambientLightEntity->SetComponent(std::move(ambientLight));
		scene.AddEntity(ambientLightEntity);

		std::shared_ptr<Entity> directionalLightEntity = std::make_shared<Entity>(L"directional light");
		std::unique_ptr<DirectionalLight> directionalLight = std::make_unique<DirectionalLight>(f32V3(0.9f, 0.9f, 0.9f));
		std::unique_ptr<Camera> dumyCameraD = std::make_unique<PerspectiveCamera>(1.f, 1.f, 1.f, 1.f);
		directionalLightEntity->SetComponent(std::move(directionalLight));
		directionalLightEntity->SetComponent(std::move(dumyCameraD));
		directionalLightController = std::make_shared<FirstPersonCameraController>(context.GetInputManager(), 0.2f);
		directionalLightController->AttachToCamera(directionalLightEntity);
		directionalLightEntity->GetComponent<Transformation>()->SetPosition(f32V3(5, 5, -5));
		scene.AddEntity(directionalLightEntity);


		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_real_distribution<float> distribution(0.5f, 1.f);

		static const u32 CountPerLayer = 32;
		static const u32 PointLightCount = 1024;
		for (u32 i = 0; i < PointLightCount; ++i)
		{
			std::shared_ptr<Entity> pointLightEntity = std::make_shared<Entity>(std::wstring(L"point light ") + std::to_wstring(i));
			std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>(f32V3(distribution(generator), distribution(generator), distribution(generator)), 250.5f, 100.f);
			pointLightEntity->SetComponent(std::move(pointLight));
			scene.AddEntity(pointLightEntity);
			pointLightEntity->GetComponent<Transformation>()->SetPosition(f32V3(0, f32((i / CountPerLayer) * 40 + 10), 0));
			pointLightEntities.push_back(pointLightEntity);
		}

		context.GetInputManager().AddInputHandler(cameraController);
		deferredPipeline = std::make_unique<X::DefferredPipeline>(renderer, context.GetMainWindow().GetClientRegionSize());
		forwardPipeline = std::make_unique<X::ForwardPipeline>(renderer, context.GetMainWindow().GetClientRegionSize());

		pDeferred = deferredPipeline.get();
		pForward = forwardPipeline.get();

		renderer.SetPipeline(std::move(deferredPipeline));

		currentLightCount = this->pointLightEntities.size();

		

		context.SetLogic([this] (f64 current, f32 delta)
		{
			bool deferred = context.GetRenderer().GetPipeline() == pDeferred;
			context.GetMainWindow().SetTitle(std::wstring()
				+ (deferred ? L"D " : L"F ")
				+ std::to_wstring(context.GetThreadSupport())
				+ L" " + std::to_wstring(currentLightCount)
				+ L" " + std::to_wstring(context.GetFPS())
				+ (statisticState ? L" S" : L" "));

			static f32 period = 50.f;
			for (u32 i = 0; i < pointLightEntities.size(); ++i)
			{
				Transformation* trans = pointLightEntities[i]->GetComponent<Transformation>();
				f32 height = trans->GetPosition().Y();
				f32 offset = f32(i % CountPerLayer) / CountPerLayer;
				f32 angle = (f32(current / f64(period)) + offset) * 2 * PI;
				f32V3 position(std::sin(angle) * 1000, height, std::cos(angle) * 200);

				trans->SetPosition(position);
			}

			PerformanceCounter& performanceCounter = context.GetPerformanceCounter();

			f32 fullTime = performanceCounter.Get(PerformanceCounter::Term::All);
			f32 renderTime = performanceCounter.Get(PerformanceCounter::Term::Render);

			f32 geometryPassTime = performanceCounter.Get(PerformanceCounter::Term::DeferredGeometryPass);
			f32 shadingPassTime = performanceCounter.Get(PerformanceCounter::Term::DeferredShadingPass);
			f32 lightTransformTime = performanceCounter.Get(PerformanceCounter::Term::DeferredLightTransform);
			f32 vertexTime = performanceCounter.Get(PerformanceCounter::Term::DeferredVertex);
			f32 rasterizeAndPixelTime = performanceCounter.Get(PerformanceCounter::Term::DeferredRasterizeAndPixel);
			f32 tiledCullingTime = performanceCounter.Get(PerformanceCounter::Term::TiledFrustumCulling);
			f32 tiledShadingTime = performanceCounter.Get(PerformanceCounter::Term::TiledShading);

			f32 prezTime = performanceCounter.Get(PerformanceCounter::Term::ForwardPreZPass);
			f32 renderingTime = performanceCounter.Get(PerformanceCounter::Term::ForwardRenderPass);
			f32 rasterizeTime = performanceCounter.Get(PerformanceCounter::Term::ForwardTotalRasterize);

			auto stringFromTime = [] (std::chrono::system_clock::time_point const& tp)
			{
				std::time_t t = std::chrono::system_clock::to_time_t(tp);
				tm tmstruct;
				localtime_s(&tmstruct, &t);
				std::stringstream ss;
				ss << std::put_time(&tmstruct, "%Y-%m-%d_%H-%M-%S"); // convert to calendar time
				return ss.str();
			};

			// TODO not work, need to find why
			if (!statisticPreviousState && statisticState) // start
			{
				ResetStaticsticPack();
				staticsticPack.startTime = performanceCounter.GetStartTime(PerformanceCounter::Term::All);
			}
			else if (statisticPreviousState && !statisticState) // stop
			{
				f64 endTime = performanceCounter.GetStartTime(PerformanceCounter::Term::All);
				f64 totalTime = endTime - staticsticPack.startTime;
				f32 averageFPS = f32(staticsticPack.frameCount / totalTime);
				StatisticPack average;
				std::memset(&average, 0, sizeof(StatisticPack));

				average.fullTime = staticsticPack.fullTime / staticsticPack.frameCount;
				average.renderTime = staticsticPack.renderTime / staticsticPack.frameCount;
				
				average.geometryPassTime = staticsticPack.geometryPassTime / staticsticPack.frameCount;
				average.shadingPassTime = staticsticPack.shadingPassTime / staticsticPack.frameCount;
				average.lightTransformTime = staticsticPack.lightTransformTime / staticsticPack.frameCount;
				average.vertexTime = staticsticPack.vertexTime / staticsticPack.frameCount;
				average.rasterizeAndPixelTime = staticsticPack.rasterizeAndPixelTime / staticsticPack.frameCount;
				average.tiledCullingTime = staticsticPack.tiledCullingTime / staticsticPack.frameCount;
				average.tiledShadingTime = staticsticPack.tiledShadingTime / staticsticPack.frameCount;
				
				average.prezTime = staticsticPack.prezTime / staticsticPack.frameCount;
				average.renderingTime = staticsticPack.renderingTime / staticsticPack.frameCount;
				average.rasterizeTime = staticsticPack.rasterizeTime / staticsticPack.frameCount;



				std::string configString = stringFromTime(std::chrono::system_clock::now()) + " " + (deferred ? "D " : "F ")
					+ std::to_string(context.GetThreadSupport()) + " " + std::to_string(currentLightCount);
				std::ofstream performanceLog("../log." + configString + ".txt");
				//if (performanceLog)
				{
					std::stringstream ss;
					if (context.GetThreadSupport() == 1)
					{
						ss << configString << "\n" << std::left
							<< "" << std::setw(32) << "fps: " << averageFPS << "\n"
							<< "" << std::setw(32) << "full time: " << average.fullTime << ", " << average.fullTime / average.fullTime << "\n"
							<< " " << std::setw(31) << "render: " << average.renderTime << ", " << average.renderTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "geometry pass: " << average.geometryPassTime << ", " << average.geometryPassTime / average.fullTime << "\n"
							<< "   " << std::setw(29) << "light transform: " << average.lightTransformTime << ", " << average.lightTransformTime / average.fullTime << "\n"
							<< "   " << std::setw(29) << "vertex: " << average.vertexTime << ", " << average.vertexTime / average.fullTime << "\n"
							<< "   " << std::setw(29) << "rasterize and pixel: " << average.rasterizeAndPixelTime << ", " << average.rasterizeAndPixelTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "shading pass: " << average.shadingPassTime << ", " << average.shadingPassTime / average.fullTime << "\n"
							<< "   " << std::setw(29) << "tiled culling: " << average.tiledCullingTime << ", " << average.tiledCullingTime / average.fullTime << "\n"
							<< "   " << std::setw(29) << "tiled shading: " << average.tiledShadingTime << ", " << average.tiledShadingTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "prez pass: " << average.prezTime << ", " << average.prezTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "rendering pass: " << average.renderingTime << ", " << average.renderingTime / average.fullTime << "\n"
							<< "   " << std::setw(29) << "rasterize: " << average.rasterizeTime << ", " << average.rasterizeTime / average.fullTime << "\n"
							<< "-------------------------------------------------------------------------" << std::endl;
					}
					else
					{
						ss << configString << "\n" << std::left
							<< "" << std::setw(32) << "fps: " << averageFPS << "\n"
							<< "" << std::setw(32) << "full time: " << average.fullTime << ", " << average.fullTime / average.fullTime << "\n"
							<< " " << std::setw(31) << "render: " << average.renderTime << ", " << average.renderTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "geometry pass: " << average.geometryPassTime << ", " << average.geometryPassTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "shading pass: " << average.shadingPassTime << ", " << average.shadingPassTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "prez pass: " << average.prezTime << ", " << average.prezTime / average.fullTime << "\n"
							<< "  " << std::setw(30) << "rendering pass: " << average.renderingTime << ", " << average.renderingTime / average.fullTime << "\n"
							<< "-------------------------------------------------------------------------" << std::endl;
					}
					performanceLog << ss.str();
				}

			}

			statisticPreviousState = statisticState;

			if (statisticState) // on
			{
				staticsticPack.frameCount += 1;

				staticsticPack.fullTime += fullTime;
				staticsticPack.renderTime += renderTime;

				staticsticPack.geometryPassTime += geometryPassTime;
				staticsticPack.shadingPassTime += shadingPassTime;
				staticsticPack.lightTransformTime += lightTransformTime;
				staticsticPack.vertexTime += vertexTime;
				staticsticPack.rasterizeAndPixelTime += rasterizeAndPixelTime;
				staticsticPack.tiledCullingTime += tiledCullingTime;
				staticsticPack.tiledShadingTime += tiledShadingTime;

				staticsticPack.prezTime += prezTime;
				staticsticPack.renderingTime += renderingTime;
				staticsticPack.rasterizeTime += rasterizeTime;
			}
			else // off
			{

			}

			std::string configString = (deferred ? "D " : "F ")
				+ std::to_string(context.GetThreadSupport()) + " " + std::to_string(currentLightCount) + (statisticState ? " S" : " ");

			std::stringstream ss;
			ss << configString << "\n" << std::left
				<< "" << std::setw(32) << "fps: " << context.GetFPS() << "\n"
				<< "" << std::setw(32) << "full time: " << fullTime << ", " << fullTime / fullTime << "\n"
				<< " " << std::setw(31) << "render: " << renderTime << ", " << renderTime / fullTime << "\n"
				<< "  " << std::setw(30) << "geometry pass: " << geometryPassTime << ", " << geometryPassTime / fullTime << "\n"
				<< "   " << std::setw(29) << "light transform: " << lightTransformTime << ", " << lightTransformTime / fullTime << "\n"
				<< "   " << std::setw(29) << "vertex: " << vertexTime << ", " << vertexTime / fullTime << "\n"
				<< "   " << std::setw(29) << "rasterize and pixel: " << rasterizeAndPixelTime << ", " << rasterizeAndPixelTime / fullTime << "\n"
				<< "  " << std::setw(30) << "shading pass: " << shadingPassTime << ", " << shadingPassTime / fullTime << "\n"
				<< "   " << std::setw(29) << "tiled culling: " << tiledCullingTime << ", " << tiledCullingTime / fullTime << "\n"
				<< "   " << std::setw(29) << "tiled shading: " << tiledShadingTime << ", " << tiledShadingTime / fullTime << "\n"
				<< "  " << std::setw(30) << "prez pass: " << prezTime << ", " << prezTime / fullTime << "\n"
				<< "  " << std::setw(30) << "rendering pass: " << renderingTime << ", " << renderingTime / fullTime << "\n"
				<< "   " << std::setw(29) << "rasterize: " << rasterizeTime << ", " << rasterizeTime / fullTime << "\n"
				<< "-------------------------------------------------------------------------" << std::endl;

			//logFile << ss.str();
			std::cout << ss.str();
			return true;
		});
	}

	virtual std::pair<bool, std::function<void()>> GenerateAction(InputManager::InputEvent const& inputEvent) override
	{
		s32 data = inputEvent.mappedSemantic;
		if (inputEvent.data == 0) // key up
		{
			return std::make_pair(true, [this, data] ()
			{
				switch (data)
				{
				case CameraControl:
					GetInputManager().RemoveInputHandler(directionalLightController);
					GetInputManager().AddInputHandler(cameraController);
					break;
				case DirectionalLightControl:
					GetInputManager().RemoveInputHandler(cameraController);
					GetInputManager().AddInputHandler(directionalLightController);
					break;
				case UseDeferredPipeline:
					if (renderer.GetPipeline() != pDeferred)
					{
						ForwardPipeline* forward = CheckedCast<ForwardPipeline*>(renderer.SetPipeline(std::move(deferredPipeline)).release());
						forwardPipeline = std::unique_ptr<ForwardPipeline>(forward);
					}
					break;
				case UseForwardPipeline:
					if (renderer.GetPipeline() != pForward)
					{
						DefferredPipeline* deferred = CheckedCast<DefferredPipeline*>(renderer.SetPipeline(std::move(forwardPipeline)).release());
						deferredPipeline = std::unique_ptr<DefferredPipeline>(deferred);
					}
					break;
				case IncreaseLight:
					if (currentLightCount < pointLightEntities.size())
					{
						for (u32 i = currentLightCount; i < currentLightCount * 2; ++i)
						{
							pointLightEntities[i]->GetComponent<Light>()->SetActive(true);
						}
						currentLightCount *= 2;
					}
					break;
				case DecreaseLight:
					if (currentLightCount > 1)
					{
						for (u32 i = currentLightCount / 2; i < currentLightCount; ++i)
						{
							pointLightEntities[i]->GetComponent<Light>()->SetActive(false);
						}
						currentLightCount /= 2;
					}
					break;
				case IncreaseThread:
					if (context.GetThreadSupport() < 32)
					{
						context.SetThreadSupport(context.GetThreadSupport() + 1);
					}					
					break;
				case DecreaseThread:
					if (context.GetThreadSupport() > 0)
					{
						context.SetThreadSupport(context.GetThreadSupport() - 1);
					}					
					break;
				case ToggleStatistic:
					statisticState = !statisticState;
					break;
				default:
					assert(false);
					break;
				}
			});
		}
		else
		{
			return std::make_pair(true, []
			{});
		}
	}

	void ResetStaticsticPack()
	{
		std::memset(&staticsticPack, 0, sizeof(staticsticPack));
	}

	Context& context;
	Renderer& renderer;
	std::shared_ptr<FirstPersonCameraController> cameraController;
	std::shared_ptr<FirstPersonCameraController> directionalLightController;
	std::vector<std::shared_ptr<Entity>> pointLightEntities;
	u32 currentLightCount;
	std::unique_ptr<ForwardPipeline> forwardPipeline;
	ForwardPipeline* pForward;
	std::unique_ptr<DefferredPipeline> deferredPipeline;
	DefferredPipeline* pDeferred;

	bool statisticPreviousState;
	bool statisticState;
	StatisticPack staticsticPack;

};


void Run()
{
	Setting setting;
	setting.top = 100;
	setting.left = 100;
	setting.width = 800;
	setting.height = 600;
	setting.title = L"0";
	setting.rootPath = "../";
	setting.threadSupport = 0;
	Context context(setting);
	
	std::shared_ptr<GlobalController> globalController = std::make_shared<GlobalController>(context);
	context.GetInputManager().AddInputHandler(globalController);

	context.Start();
}



int main()
{
	//Temp();
	Run();
	return 0;
}

#ifdef MEMORY_LEAK_CHECK


struct DML
{
	DML()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}
	~DML()
	{
		if (_CrtDumpMemoryLeaks())
		{
			std::cout << "memory leaks." << std::endl;
			std::cin.get();
		}
		else
		{
			std::cout << "no memory leaks." << std::endl;
		}
	}
} _dml;

#endif // MEMORY_LEAK_CHECK