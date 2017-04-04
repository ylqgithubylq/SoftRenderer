#include "Header.hpp"
#include "ResourceLoader.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "GeometryLayout.hpp"

#include "FreeImage.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <tuple>
#include <filesystem>

namespace X
{
	namespace
	{
		bool Locate(std::vector<std::tr2::sys::path> const& locations, bool containsDirectory, std::tr2::sys::path const& path, std::tr2::sys::path* locatedPath)
		{
			if (path.has_root_path())
			{
				if (std::tr2::sys::exists(path) && std::tr2::sys::is_regular_file(path))
				{
					*locatedPath = path;
					return true;
				}
			}
			else
			{
				for (std::tr2::sys::path const& location : locations)
				{
					std::tr2::sys::path pathToTry = location / path;
					bool found = false;
					if (!containsDirectory)
					{
						found = std::tr2::sys::exists(pathToTry) && std::tr2::sys::is_regular_file(pathToTry);
					}
					else
					{
						found = std::tr2::sys::exists(pathToTry) && (std::tr2::sys::is_regular_file(pathToTry) || std::tr2::sys::is_directory(pathToTry));
					}
					if (found)
					{
						*locatedPath = pathToTry;
						return true;
					}
				}
			}
			return false;
		}

		bool LocatePathString(std::vector<std::tr2::sys::path> const& locations, bool containsDirectory, std::string const& relativePath, std::string* resultPath)
		{
			std::tr2::sys::path resourceLocation;
			if (!Locate(locations, containsDirectory, std::tr2::sys::path(relativePath), &resourceLocation))
			{
				return false;
			}
			*resultPath = resourceLocation.string();
			return true;
		}
	}

	struct ResourceLoader::Impl
	{
		std::tr2::sys::path rootPath;
		std::vector<std::tr2::sys::path> paths;
		Impl(std::string root)
			: rootPath(std::tr2::sys::system_complete(std::tr2::sys::path(std::move(root))))
		{
			assert(std::tr2::sys::exists(rootPath) && std::tr2::sys::is_directory(rootPath));
			paths.push_back(rootPath);
		}
	};

	ResourceLoader::ResourceLoader(std::string rootPath)
		: impl(std::make_unique<Impl>(std::move(rootPath)))
	{
	}


	ResourceLoader::~ResourceLoader()
	{
	}

	bool ResourceLoader::AddResourceLocation(std::string path)
	{
		std::tr2::sys::path pathObj = std::tr2::sys::path(std::move(path));
		if (!pathObj.has_root_path())
		{
			pathObj = impl->rootPath / pathObj;
		}
		if (std::tr2::sys::exists(pathObj) && std::tr2::sys::is_directory(pathObj))
		{
			if (std::find(impl->paths.begin(), impl->paths.end(), pathObj) == impl->paths.end())
			{
				impl->paths.push_back(pathObj);
			}
			return true;
		}
		return false;
	}


	std::shared_ptr<ConcreteTexture2D<f32V3>> ResourceLoader::LoadTexture(std::string const& path)
	{
		std::string locatedPath;
		if (!LocatePathString(impl->paths, false, path, &locatedPath))
		{
			return nullptr;
		}
		FREE_IMAGE_FORMAT imageFormat = FIF_UNKNOWN;

		//check the file signature and deduce its format
		imageFormat = FreeImage_GetFileType(locatedPath.c_str(), 0);
		//if still unknown, try to guess the file format from the file extension
		if (imageFormat == FIF_UNKNOWN)
		{
			imageFormat = FreeImage_GetFIFFromFilename(locatedPath.c_str());
		}
		//if still unknown, return failure
		if (imageFormat == FIF_UNKNOWN)
		{
			return false;
		}

		//pointer to the image, once loaded
		FIBITMAP* bitmap = nullptr;

		//check that the plugin has reading capabilities and load the file
		if (FreeImage_FIFSupportsReading(imageFormat))
		{
			bitmap = FreeImage_Load(imageFormat, locatedPath.c_str());
		}
		//if the image failed to load, return failure
		if (!bitmap)
		{
			return false;
		} // after this, make sure to call FreeImage_Unload(FIBITMAP*)
		//Free FreeImage's copy of the data

		//retrieve the image data
		u8* bits = FreeImage_GetBits(bitmap);

		u32 width = FreeImage_GetWidth(bitmap);
		u32 height = FreeImage_GetHeight(bitmap);
		//if this somehow one of these failed (they shouldn't), return failure
		if (!bits || width == 0 || height == 0)
		{
			FreeImage_Unload(bitmap);
			return nullptr;
		}

		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(bitmap);
		FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);

		if (imageType != FREE_IMAGE_TYPE::FIT_BITMAP)
		{
			assert(imageType == FREE_IMAGE_TYPE::FIT_BITMAP);
			return nullptr;
		}
		
		u32 blueMask = FreeImage_GetBlueMask(bitmap);
		u32 greenMask = FreeImage_GetGreenMask(bitmap);
		u32 redMask = FreeImage_GetRedMask(bitmap);
		bool bgr = blueMask < redMask;

		u32 channelCount = 0;
		switch (colorType)
		{
		case FIC_MINISWHITE:
			channelCount = 1;
			assert(false);
			break;
		case FIC_MINISBLACK:
			channelCount = 1;
			break;
		case FIC_RGB:
			channelCount = 3;
			break;
		case FIC_PALETTE:
			assert(false);
			break;
		case FIC_RGBALPHA:
			channelCount = 4;
			break;
		case FIC_CMYK:
			assert(false);
			break;
		default:
			break;
		}


		u32 bpp = FreeImage_GetBPP(bitmap);
		u32 bytesOfPixelChannel = bpp / 8 / channelCount;

		u32 dataSize = channelCount * bytesOfPixelChannel * height * width;

		u32 pixelSize = width * height;

		std::vector<f32V3> dataContainer;
		dataContainer.resize(pixelSize);

		switch (bytesOfPixelChannel)
		{
		case 1:
		{
			u8* typedData = reinterpret_cast<u8*>(bits);
			auto get = [] (u8* typedData, u32 index, u32 offset)
			{
				return f32(typedData[index * 3 + offset]) / std::numeric_limits<u8>::max();
			};
			if (bgr)
			{

				for (u32 i = 0; i < pixelSize; ++i)
				{
					dataContainer[i] = f32V3(get(typedData, i, 2), get(typedData, i, 1), get(typedData, i, 0));
				}
			}
			else
			{
				assert(false);
			}
		}
			break;
		default:
			assert(false);
			break;
		}


		Size<u32, 2> size(width, height);
		std::shared_ptr<ConcreteTexture2D<f32V3>> texture = std::make_shared<ConcreteTexture2D<f32V3>>(size);

		texture->SetValues(0, dataContainer.data(), pixelSize);

		FreeImage_Unload(bitmap);

		return texture;
	}

	namespace
	{
		struct SceneProcessor
		{
			ResourceLoader& loader_;

			aiScene const& scene_;
			std::string directoryPath_;

			std::shared_ptr<PointSampler<Sampler::RepeatAddresser>> sampler_;

			std::unique_ptr<Mesh> result_;

			std::vector<std::shared_ptr<Material>> createdMaterials_;
			std::vector<std::shared_ptr<GeometryLayout>> createdLayouts_;

			SceneProcessor(ResourceLoader& loader, aiScene const& theScene, std::string const& filePath)
				: loader_(loader), scene_(theScene)
			{
				std::tr2::sys::path scenePath(filePath);
				directoryPath_ = scenePath.parent_path().string() + "/";
				result_ = std::make_unique<Mesh>();
				sampler_ = std::make_shared<PointSampler<Sampler::RepeatAddresser>>();
				ProcessScene();
				result_->CalculateBoundingBox();
			}

			void ProcessScene()
			{


				ProcessMaterial();
				ProcessMesh();

				aiTexture** textures = scene_.mTextures;
				for (u32 i = 0; i < scene_.mNumTextures; ++i)
				{
					aiTexture* texture = textures[i];
					texture->pcData;
					texture->mHeight;
					texture->mWidth;
				}


				aiNode* rootNode = scene_.mRootNode;
				ProcessNode(*rootNode);
			}

			void ProcessMaterial()
			{
				createdMaterials_.resize(scene_.mNumMaterials);

				aiMaterial** materials = scene_.mMaterials;
				for (u32 i = 0; i < scene_.mNumMaterials; ++i)
				{
					aiMaterial* loaderMaterial = materials[i];

					aiString name;
					if (AI_SUCCESS != loaderMaterial->Get(AI_MATKEY_NAME, name))
					{
						assert(false);
					}
					std::shared_ptr<Material> material = std::make_shared<Material>();

					static_assert(sizeof(aiColor3D) == sizeof(f32V3), "size not match.");

//					vertex attributes, not use
// 					aiColor3D aiColor;
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor))
// 					{
// 						f32V3 color;
// 					}
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiColor))
// 					{
// 						f32V3 color;
// 					}
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor))
// 					{
// 						f32V3 color;
// 					}
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, aiColor))
// 					{
// 						f32V3 color;
// 					}
// 					float value;
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_OPACITY, value))
// 					{
// 					}
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_SHININESS, value))
// 					{
// 					}
// 					if (AI_SUCCESS == loaderMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, value))
// 					{
// 					}


					static std::tuple<aiTextureType,
						void (Material::*)(std::shared_ptr<ConcreteTexture2D<f32V3>>),
						void (Material::*)(std::shared_ptr<PointSampler<Sampler::RepeatAddresser>>)> TextureTypes[] =
					{
						std::make_tuple(aiTextureType::aiTextureType_DIFFUSE, &Material::SetDiffuseTexture, &Material::SetDiffuseSampler),
						std::make_tuple(aiTextureType::aiTextureType_SPECULAR, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_EMISSIVE, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_HEIGHT, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_NORMALS, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_SHININESS, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_OPACITY, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_DISPLACEMENT, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_LIGHTMAP, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_REFLECTION, nullptr, nullptr),
						std::make_tuple(aiTextureType::aiTextureType_UNKNOWN, nullptr, nullptr),
					};

					u32 textureCount = 0;
					aiString path;
					aiTextureMapping textureMapping;
					u32 uvIndex = 0;
					float blend = 0.f;
					aiTextureOp textureOp = _aiTextureOp_Force32Bit;
					std::array<aiTextureMapMode, 3> textureMapModes;
					textureMapModes.fill(_aiTextureMapMode_Force32Bit);

					for (auto& textureType : TextureTypes)
					{
						textureCount = loaderMaterial->GetTextureCount(std::get<0>(textureType));
						assert(textureCount <= 1); // larger are not support

						if (AI_SUCCESS == loaderMaterial->GetTexture(std::get<0>(textureType), 0, &path, &textureMapping, &uvIndex, &blend, &textureOp, textureMapModes.data()))
						{
							if (std::get<1>(textureType) == nullptr)
							{
								continue;
							}
							std::shared_ptr<ConcreteTexture2D<f32V3>> texureLoaded = loader_.LoadTexture(directoryPath_ + path.C_Str());
							assert(texureLoaded != nullptr);
							(material.get()->*std::get<1>(textureType))(texureLoaded);
							(material.get()->*std::get<2>(textureType))(sampler_);
							if (textureMapModes[0] != _aiTextureMapMode_Force32Bit)
							{
								textureMapModes[0] = textureMapModes[0];
							}
//							not implemented yet
// 							std::array<SamplerState::TextureAddressingMode, 3> addressingModes;
// 							for (u32 i = 0; i < 3; ++i)
// 							{
// 								switch (textureMapModes[i])
// 								{
// 								case aiTextureMapMode_Wrap:
// 									addressingModes[i] = SamplerState::TextureAddressingMode::Repeat;
// 									break;
// 								case aiTextureMapMode_Clamp:
// 									addressingModes[i] = SamplerState::TextureAddressingMode::ClampToEdge;
// 									break;
// 								case aiTextureMapMode_Decal:
// 									addressingModes[i] = SamplerState::TextureAddressingMode::ClampToBorder;
// 									break;
// 								case aiTextureMapMode_Mirror:
// 									addressingModes[i] = SamplerState::TextureAddressingMode::MirroredRepeat;
// 									break;
// 								case _aiTextureMapMode_Force32Bit: // default value
// 									addressingModes[i] = SamplerState::TextureAddressingMode::Repeat;
// 									break;
// 								default:
// 									assert(false);
// 									break;
// 								}
// 							}

						}
					}

					createdMaterials_[i] = material;
				}
			}

			void ProcessMesh()
			{
				createdLayouts_.resize(scene_.mNumMeshes);

				aiMesh** meshes = scene_.mMeshes;
				for (u32 i = 0; i < scene_.mNumMeshes; ++i)
				{
					aiMesh* mesh = scene_.mMeshes[i];

					// not handled
					//                              mesh->mNumAnimMeshes;
					//                              mesh->mAnimMeshes;
					// 
					//                              mesh->mNumBones;
					//                              mesh->mBones;


					u32 textureCoordinateCount = mesh->GetNumUVChannels();
					assert(textureCoordinateCount == 1);
					u32 vertexColorCount = mesh->GetNumColorChannels();

					std::vector<Vertex> vertices;
					vertices.resize(mesh->mNumVertices);
					for (u32 j = 0, currentLocation = 0; j < mesh->mNumVertices; ++j)
					{
						if (mesh->HasPositions())
						{
							auto& vertex = mesh->mVertices[j];
							vertices[j].position = f32V3(vertex.x, vertex.y, vertex.z);
						}
						if (mesh->HasNormals())
						{
							auto& normal = mesh->mNormals[j];
							vertices[j].normal = f32V3(normal.x, normal.y, normal.z);
						}
						for (u32 k = 0; k < textureCoordinateCount; ++k)
						{
							auto& textureCoordinate = mesh->mTextureCoords[k][j];
							vertices[j].textureCoordinate = f32V2(textureCoordinate.x, textureCoordinate.y);
						}
						for (u32 k = 0; k < vertexColorCount; ++k)
						{
							// vertex colors are ignored
						}
					}
					std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(std::move(vertices));

					std::vector<u16> indices;
					s32 indicesPerFace = mesh->mFaces[0].mNumIndices;
					assert(indicesPerFace == 3);
					if (mesh->mNumVertices > static_cast<u32>(std::numeric_limits<u16>::max()))
					{
						assert(false);
					}
					indices.resize(mesh->mNumFaces * indicesPerFace);
					for (u32 j = 0; j < mesh->mNumFaces; ++j)
					{
						aiFace& face = mesh->mFaces[j];
						assert(face.mNumIndices == indicesPerFace);
						for (int k = 0; k < indicesPerFace; ++k)
						{
							indices[j * indicesPerFace + k] = static_cast<u16>(face.mIndices[k]);
						}
					}
					std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(std::move(indices));


// 					IndexBuffer::TopologicalType primitiveType;
// 					switch (mesh->mPrimitiveTypes)
// 					{
// 					case aiPrimitiveType::aiPrimitiveType_TRIANGLE:
// 						primitiveType = IndexBuffer::TopologicalType::Triangles;
// 						break;
// 					case aiPrimitiveType::aiPrimitiveType_LINE:
// 						primitiveType = IndexBuffer::TopologicalType::Lines;
// 						break;
// 					case aiPrimitiveType::aiPrimitiveType_POINT:
// 						primitiveType = IndexBuffer::TopologicalType::Points;
// 						break;
// 					default:
// 						assert(false);
// 						break;
// 					}
					assert(mesh->mPrimitiveTypes == aiPrimitiveType::aiPrimitiveType_TRIANGLE);

					createdLayouts_[i] = std::make_shared<GeometryLayout>(vertexBuffer, indexBuffer);


				}
			}


			void ProcessNode(aiNode const& node)
			{
				u32* meshIndices = node.mMeshes;
				for (u32 i = 0; i < node.mNumMeshes; ++i)
				{
					aiMesh* mesh = scene_.mMeshes[meshIndices[i]];
					result_->CreateSubMesh(createdLayouts_[meshIndices[i]], createdMaterials_[mesh->mMaterialIndex]);
				}

				aiNode** children = node.mChildren;
				for (u32 i = 0; i < node.mNumChildren; ++i)
				{
					ProcessNode(*children[i]);
				}
			}

		};

	}

	std::unique_ptr<Mesh> ResourceLoader::LoadMesh(std::string const& path)
	{
		std::string locatedPath;
		if (!LocatePathString(impl->paths, false, path, &locatedPath))
		{
			return nullptr;
		}
		Assimp::Importer importer;

		aiScene const* scene = importer.ReadFile(locatedPath,
			//aiProcess_GenSmoothNormals |
			//aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			//aiProcess_SortByPType |
			0
			);

		if (!scene)
		{
			return nullptr;
		}
		// Everything will be cleaned up by the importer destructor
		return SceneProcessor(*this, *scene, locatedPath).result_;
	}

}

