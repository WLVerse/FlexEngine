// WLVERSE [https://wlverse.web.app]
// assimp.cpp
// 
// Rough implementation of assimp.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "pch.h"

#include "assimp.h"

#include "FlexMath/vector3.h"
#include "AssetManager/assetmanager.h"
#include "Renderer/OpenGL/opengltexture.h"
#include "DataStructures/freequeue.h"

namespace
{
  // Internal variables

  // The current working directory of AssimpWrapper.
  // This is used to resolve relative paths of textures when creating the asset keys
  // for the textures.
  static FlexEngine::Path internal_current_working_directory = FlexEngine::Path::current();
  #define CURRENT_WORKING_DIRECTORY internal_current_working_directory

  struct Internal_Context
  {
    FlexEngine::Path path;
    std::string local_path; // the path of the model file relative to the current working directory
    const aiScene* scene;
    std::string node_name;
  };
  static Internal_Context internal_ctx;

  // Internal functions

  // Transforms an aiMatrix4x4 to a FlexEngine::Matrix4x4.
  // The aiMatrix4x4 is row-major, while the FlexEngine::Matrix4x4 (along with glm and OpenGL) is column-major.
  // The matrix is transposed to match the column-major layout of the FlexEngine::Matrix4x4.
  FlexEngine::Matrix4x4 Internal_ConvertMatrix(const aiMatrix4x4& matrix)
  {
    return FlexEngine::Matrix4x4(
      matrix.a1, matrix.a2, matrix.a3, matrix.a4,
      matrix.b1, matrix.b2, matrix.b3, matrix.b4,
      matrix.c1, matrix.c2, matrix.c3, matrix.c4,
      matrix.d1, matrix.d2, matrix.d3, matrix.d4
    ).Transpose();
  }
}

namespace FlexEngine
{

  // static member initialization
  Assimp::Importer AssimpWrapper::importer;
  unsigned int AssimpWrapper::import_flags = aiProcess_ValidateDataStructure | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

  // Each model holds a list of meshes and materials
  // Each mesh has a material index that points to the material in the list of materials
  Asset::Model AssimpWrapper::LoadModel(const Path& path)
  {
    // load the model
    const aiScene* scene = importer.ReadFile(path.string().c_str(), import_flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      Log::Error(std::string("Assimp import error: ") + importer.GetErrorString());
      return Asset::Model::Null;
    }

    // set the current working directory
    CURRENT_WORKING_DIRECTORY = path.parent_path();

    // update internal context
    internal_ctx.path = path;
    std::string temp = path.string();
    internal_ctx.local_path = temp.substr(Path::current().string().length());
    internal_ctx.scene = scene;
    internal_ctx.node_name = "";

    // process the scene
    std::vector<Asset::Material> materials{};
    std::vector<Asset::Mesh> meshes = Internal_ProcessNode(scene->mRootNode, Matrix4x4::Identity, &materials);
    materials.shrink_to_fit();
    meshes.shrink_to_fit();
    return Asset::Model(meshes, materials);
  }

  std::vector<Asset::Mesh> AssimpWrapper::Internal_ProcessNode(
    aiNode* node,
    Matrix4x4 parent_transform,
    std::vector<Asset::Material>* out_materials
  )
  {
    #if 0
    Log::Debug("Processing node...\n"
      "- Node: " + std::string(node->mName.C_Str()) + "\n" +
      "    Number of meshes: " + std::to_string(node->mNumMeshes) + "\n" +
      "    Number of children: " + std::to_string(node->mNumChildren) + "\n"
    );
    #endif

    std::vector<Asset::Mesh> meshes;

    Matrix4x4 transform = Matrix4x4::Identity;
    transform = parent_transform * Internal_ConvertMatrix(node->mTransformation);

    // process all the meshes in the node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
      aiMesh* mesh = internal_ctx.scene->mMeshes[node->mMeshes[i]];
      internal_ctx.node_name = node->mName.C_Str();
      meshes.push_back( Internal_ProcessMesh(mesh, transform, out_materials) );
    }

    // process all the children of the node
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      std::vector<Asset::Mesh> child( Internal_ProcessNode(node->mChildren[i], transform, out_materials) );
      meshes.insert(meshes.end(), child.begin(), child.end());
    }

    return meshes;
  }

  Asset::Mesh AssimpWrapper::Internal_ProcessMesh(
    aiMesh* mesh,
    Matrix4x4 mesh_transform,
    std::vector<Asset::Material>* out_materials
  )
  {
    #pragma region Vertex Processing

    std::vector<Vertex> vertices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
      Vertex vertex;

      if (mesh->HasPositions())
      {
        vertex.position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
      }

      // the default vertex color is black with an alpha of 0
      if (mesh->HasVertexColors(i))
      {
        vertex.color = Vector4(mesh->mColors[i]->r, mesh->mColors[i]->g, mesh->mColors[i]->b, mesh->mColors[i]->a);
      }

      // this implementation only supports one set of texture coordinates
      if (mesh->HasTextureCoords(0))
      {
        vertex.tex_coords = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
      }

      if (mesh->HasNormals())
      {
        vertex.normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
      }

      if (mesh->HasTangentsAndBitangents())
      {
        vertex.tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        vertex.bitangent = Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
      }

      vertices.push_back(vertex);
    }

    #pragma endregion

    #pragma region Index Processing

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
      {
        indices.push_back(face.mIndices[j]);
      }
    }

    #pragma endregion

    #pragma region Material Processing

    aiMaterial* ai_material = internal_ctx.scene->mMaterials[mesh->mMaterialIndex];

    // parse material
    Asset::Material material = Asset::Material(
      Internal_ProcessMaterialTextures(ai_material, aiTextureType_DIFFUSE ),
      Internal_ProcessMaterialTextures(ai_material, aiTextureType_SPECULAR)
    );
    std::size_t material_index = out_materials->size();

    // optimization
    // check if the material already exists
    auto it = std::find(out_materials->begin(), out_materials->end(), material);
    if (it != out_materials->end())
    {
      // material already exists, use the existing material
      material_index = std::distance(out_materials->begin(), it);
    }
    else
    {
      // material does not exist, add the material to the list of materials
      out_materials->push_back(material);
    }

    #pragma endregion

    // create the mesh
    return Asset::Mesh(vertices, indices, mesh_transform, material_index, internal_ctx.node_name);
  }

  // Only supports diffuse and specular textures, for now
  Asset::Material::TextureVariant AssimpWrapper::Internal_ProcessMaterialTextures(
    aiMaterial* material,
    aiTextureType type
  )
  {
    Asset::Material::TextureVariant textures = "";
    //Asset::Material::TextureVariant textures = Asset::Texture::None;
    //Asset::Material::TextureVariant textures = Asset::Texture::Default();

    // process the material
    if (material->GetTextureCount(type) > 0)
    {
      aiString texture_path;
      material->GetTexture(type, 0, &texture_path);

      const aiTexture* texture = internal_ctx.scene->GetEmbeddedTexture(texture_path.C_Str());
      if (texture)
      {
        // create custom name for embedded textures
        std::string texture_name =
          std::string(internal_ctx.local_path) + "\\" +
          internal_ctx.node_name + "_" + std::to_string(type)
        ;

        // guard: texture is compressed if the height is 0
        // the width represents the size of the compressed texture
        if (texture->mHeight == 0)
        {
          #if 0

          Log::Error("AssimpWrapper: Embedded textures are compressed, decompression is not supported.");
          return textures;

          #else

          // decompress the texture
          //Asset::Texture embedded_texture(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth);
          Asset::Texture embedded_texture = Asset::Texture::Default();

          // add the texture to the list of textures
          textures = AssetManager::AddTexture(texture_name, embedded_texture);

          #endif
        }
        // no decompression needed
        else
        {
          // create a buffer for the texture data
          std::size_t size = static_cast<std::size_t>(texture->mWidth * texture->mHeight * 4);
          unsigned char* data = new unsigned char[size];

          // ARGB -> RGBA
          for (std::size_t j = 0; j < size; j += 4)
          {
            data[j + 0] = texture->pcData[j].r;
            data[j + 1] = texture->pcData[j].g;
            data[j + 2] = texture->pcData[j].b;
            data[j + 3] = texture->pcData[j].a;
          }

          // create the texture
          Asset::Texture embedded_texture(data, texture->mWidth, texture->mHeight);

          // cleanup
          delete[] data;

          // add the texture to the list of textures
          textures = AssetManager::AddTexture(texture_name, embedded_texture);
        }

      }
      else
      {
        // guard: texture is not a file
        if (texture_path.length == 0)
        {
          Log::Error("AssimpWrapper: Texture path is empty.");
          return textures;
        }
        
        // create an asset key for the texture
        // TODO: add handling for other kinds of texture paths
        // currently, the texture path is assumed to be relative to the model path
        Path full_path;
        try
        {
          full_path = CURRENT_WORKING_DIRECTORY.append(texture_path.C_Str());
        }
        catch (std::exception e)
        {
          Log::Error(e.what());
          return textures;
        }

        // remove the default directory from the asset key
        AssetKey assetkey = full_path.string().substr(AssetManager::DefaultDirectory().string().length());
        
        // add the asset key to the list of textures
        textures = assetkey;
      }
      
    }
    
    return textures;
  }

}
