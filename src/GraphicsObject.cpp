#include "GraphicsObject.h"
#include "VulkanStructures.h"
#include "VulkanUtility.h"
#include "FileIO.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace Raven
{
    GraphicsObject::GraphicsObject()
    {

    }

    GraphicsObject::~GraphicsObject()
    {

    }

    /**
     * @brief Loads the given file and creates a model from it. This function is mostly
     *        from VulkanCookbook with some changes of mine.
     * @param filename
     * @return False if something went wrong.
     */
    bool GraphicsObject::loadModel(const VkDevice logicalDevice, const std::string filename,
                                   bool loadNormals, bool loadTextureCoordinates, bool generateTangentVectors,
                                   bool normalize, uint32_t *vertexStride)
    {
        //First load the model from .obj-file.
        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string error;

        bool result = tinyobj::LoadObj(&attributes, &shapes, &materials, &error, filename.c_str());
        if(!result)
        {
            std::cout << "Failed to load file: " << filename << std::endl;
            if(error.size() > 0)
            {
                std::cout << error << std::endl;
            }
            return false;
        }

        //If we want to generate tangent space vectors, we will also require normals and texture coordinates.
        if(!loadNormals || !loadTextureCoordinates)
        {
            generateTangentVectors = false;
        }

        //Load model data and normalize its size and position.
        float minX = attributes.vertices[0];
        float maxX = attributes.vertices[0];
        float minY = attributes.vertices[1];
        float maxY = attributes.vertices[1];
        float minZ = attributes.vertices[2];
        float maxZ = attributes.vertices[2];

        //Load the data to the mesh-object owned by the GraphicsObject class.
        mesh = {};
        uint32_t offset = 0;
        for(auto &shape : shapes)
        {
            uint32_t partOffset = offset;
            for(auto &index : shape.mesh.indices)
            {
                mesh.data.emplace_back(attributes.vertices[3 * index.vertex_index + 0]);
                mesh.data.emplace_back(attributes.vertices[3 * index.vertex_index + 1]);
                mesh.data.emplace_back(attributes.vertices[3 * index.vertex_index + 2]);
                offset++;

                //Load normal data.
                if(loadNormals)
                {
                    if(attributes.normals.size() == 0)
                    {
                        std::cout << "Failed to load normals for file: " << filename << std::endl;
                        return false;
                    }
                    else
                    {
                        mesh.data.emplace_back(attributes.normals[3 * index.normal_index + 0]);
                        mesh.data.emplace_back(attributes.normals[3 * index.normal_index + 1]);
                        mesh.data.emplace_back(attributes.normals[3 * index.normal_index + 2]);
                    }
                }

                //Load texture coordinates.
                if(loadTextureCoordinates)
                {
                    if(attributes.texcoords.size() == 0)
                    {
                        std::cout << "Failed to load normals for file: " << filename << std::endl;
                        return false;
                    }
                    else
                    {
                        mesh.data.emplace_back(attributes.texcoords[2 * index.texcoord_index + 0]);
                        mesh.data.emplace_back(attributes.texcoords[2 * index.texcoord_index + 1]);
                    }
                }

                //Generate tangent space vectors.
                if(generateTangentVectors)
                {
                    //Insert temporary tangent space vectors data.
                    for(int i = 0; i < 6; ++i)
                    {
                        mesh.data.emplace_back(0.0f);
                    }
                }

                if(normalize)
                {
                    if(attributes.vertices[3 * index.vertex_index + 0] < minX )
                    {
                        minX = attributes.vertices[3 * index.vertex_index + 0];
                    }
                    if(attributes.vertices[3 * index.vertex_index + 0] > maxX )
                    {
                        maxX = attributes.vertices[3 * index.vertex_index + 0];
                    }
                    if(attributes.vertices[3 * index.vertex_index + 1] < minY )
                    {
                        minY = attributes.vertices[3 * index.vertex_index + 1];
                    }
                    if(attributes.vertices[3 * index.vertex_index + 1] > maxY )
                    {
                        maxY = attributes.vertices[3 * index.vertex_index + 1];
                    }
                    if(attributes.vertices[3 * index.vertex_index + 2] < minZ )
                    {
                        minZ = attributes.vertices[3 * index.vertex_index + 2];
                    }
                    if(attributes.vertices[3 * index.vertex_index + 2] > maxZ )
                    {
                        maxZ = attributes.vertices[3 * index.vertex_index + 2];
                    }
                }
            }

            uint32_t partVertexCount = offset - partOffset;
            if(partVertexCount > 0)
            {
                mesh.parts.push_back({partOffset, partVertexCount});
            }
        }

        //Define the stride.
        uint32_t stride = 3 + (loadNormals ? 3 : 0) + (loadTextureCoordinates ? 2 : 0) +
                          (generateTangentVectors ? 6 : 0);
        if(vertexStride)
        {
            *vertexStride = stride * sizeof(float);
        }

        if(generateTangentVectors)
        {
            generateTangentSpaceVectors(mesh);
        }

        if(normalize)
        {
            float offsetX = 0.5f * (minX + maxX);
            float offsetY = 0.5f * (minY + maxY);
            float offsetZ = 0.5f * (minZ + maxZ);
            //abs returns the absolute value of a variable so, for an example 23 is 23 and -11 is 11.
            float scaleX = abs(minX - offsetX) > abs(maxX - offsetX) ? abs(minX - offsetX) : abs(maxX - offsetX);
            float scaleY = abs(minY - offsetY) > abs(maxY - offsetY) ? abs(minY - offsetY) : abs(maxY - offsetY);
            float scaleZ = abs(minZ - offsetZ) > abs(maxZ - offsetZ) ? abs(minZ - offsetZ) : abs(maxZ - offsetZ);
            float scale = scaleX > scaleY ? scaleX : scaleY;
            scale = scaleZ > scale ? 1.0f / scaleZ : 1.0f / scale;

            for(size_t i = 0; i < mesh.data.size(); ++i)
            {
                mesh.data[i + 0] = scale * (mesh.data[i + 0] - offsetX);
                mesh.data[i + 1] = scale * (mesh.data[i + 1] - offsetY);
                mesh.data[i + 2] = scale * (mesh.data[i + 2] - offsetZ);
            }
        }

        return true;
    }

    /**
     * @brief Reads an image file and creates an image + image view from the file
     *        which can then be used as a texture over the object.
     * @param logicalDevice
     * @param memoryProperties
     * @param filename
     * @param usage
     * @param format
     * @param samples
     * @param mipLevelCount
     * @return False if something went wrong.
     */
    bool GraphicsObject::addTexture(const VkDevice logicalDevice,
                                    VkPhysicalDeviceMemoryProperties memoryProperties,
                                    const std::string filename, VkImageUsageFlags usage, VkFormat format,
                                    VkSampleCountFlagBits samples, uint32_t mipLevelCount)
    {
        //First load the image data. Note that the data is not being
        //uploaded to the image yet.
        std::vector<unsigned char> pixels;
        int imageHeight, imageWidth, imageComponentCount, dataSize;
        FileIO::readImageFile(filename, pixels, &imageWidth, &imageHeight, &imageComponentCount, 4, &dataSize);

        //Create a staging buffer for the image data with host-visible memory.
        VulkanBuffer stagingBuffer;
        stagingBuffer.size = dataSize; //imageWidth * imageHeight * 4;

        VkBufferCreateInfo stagingBufferCreateInfo =
                VulkanStructures::bufferCreateInfo(stagingBuffer.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                   VK_SHARING_MODE_EXCLUSIVE);
        //Create the staging buffer.
        if(!createBuffer(logicalDevice, stagingBufferCreateInfo, stagingBuffer.buffer))
            return false;

        //Get buffer memory requirements and allocate memory for the buffer.
        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(logicalDevice, stagingBuffer.buffer, &memReq);

        VkDeviceMemory stagingMemory;
        //Allocate the memory.
        if(!allocateMemory(logicalDevice,memoryProperties, memReq, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingMemory))
        {
            return false;
        }
        //Bind the buffer.
        if(!stagingBuffer.bindMemoryObject(logicalDevice, stagingMemory,0))
            return false;

        //Flush the data to the memory object.
        void* dataPointer;
        flushDataToHostLocalMemory(logicalDevice, stagingMemory, 0, stagingBuffer.size,
                          pixels.data(), &dataPointer, true);

        //Add image extent information.
        VkExtent3D extent;
        extent.width = static_cast<uint32_t>(imageWidth);
        extent.height = static_cast<uint32_t>(imageHeight);
        extent.depth = 1;

        //Make sure that the image is possible to work as the destination of data.
        if(!(usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
        {
            usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        //Create the image and the image view.
        if(!createImageWithImageView(logicalDevice, memoryProperties, usage,
                                     VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, format, extent, imageComponentCount,
                                     samples, VK_IMAGE_LAYOUT_UNDEFINED, VK_SHARING_MODE_EXCLUSIVE,
                                     mipLevelCount, false, VK_IMAGE_ASPECT_COLOR_BIT, textureObject,
                                     textureObject.imageMemory))
        {
            return false;
        }
        return true;
    }

    // Based on:
    // Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh".
    // Terathon Software 3D Graphics Library, 2001.
    // http://www.terathon.com/code/tangent.html

    /**
     * @brief GraphicsObject::calculateTangentAndBitangent
     * @param normalData
     * @param faceTangent
     * @param faceBitangent
     * @param tangentData
     * @param bitangentData
     */
    void GraphicsObject::calculateTangentAndBitangent(float const *normalData,
                                                      const glm::vec3 &faceTangent,
                                                      const glm::vec3 &faceBitangent,
                                                      float *tangentData,
                                                      float *bitangentData)
    {
        // Gram-Schmidt orthogonalize
        const glm::vec3 normal = {normalData[0], normalData[1], normalData[2]};
        const glm::vec3 tangent = glm::normalize(faceTangent - normal * glm::dot(normal, faceTangent));

        //Calculate the "handedness".
        float handedness = (glm::dot(glm::cross(normal, tangent), faceBitangent) < 0.0f) ? -1.0f : 1.0f;

        const glm::vec3 bitangent = handedness * glm::cross(normal, tangent);

        tangentData[0] = tangent[0];
        tangentData[1] = tangent[1];
        tangentData[2] = tangent[2];

        bitangentData[0] = bitangent[0];
        bitangentData[1] = bitangent[1];
        bitangentData[2] = bitangent[2];
    }

    /**
     * @brief Generates tangent space vectors for a mesh object from normal and texture
     *        coordinate data.
     * @param mesh
     */
    void GraphicsObject::generateTangentSpaceVectors(Mesh &mesh)
    {
        size_t const normalOffset = 3;
        size_t const texCoordOffset = 6;
        size_t const tangentOffset = 8;
        size_t const bitangentOffset = 11;
        size_t const stride = 14;

        for(auto &part : mesh.parts)
        {
            for(size_t i = 0; i < mesh.data.size(); i += stride * 3)
            {
                size_t i1 = i;
                size_t i2 = i1 + stride;
                size_t i3 = i2 + stride;
                glm::vec3 const v1 = {mesh.data[i1], mesh.data[i1 + 1], mesh.data[i1 + 2]};
                glm::vec3 const v2 = {mesh.data[i2], mesh.data[i2 + 1], mesh.data[i2 + 2]};
                glm::vec3 const v3 = {mesh.data[i3], mesh.data[i3 + 1], mesh.data[i3 + 2]};

                std::array<float, 2> const w1 = { mesh.data[i1 + texCoordOffset], mesh.data[i1 + texCoordOffset +1]};
                std::array<float, 2> const w2 = { mesh.data[i2 + texCoordOffset], mesh.data[i2 + texCoordOffset +1]};
                std::array<float, 2> const w3 = { mesh.data[i3 + texCoordOffset], mesh.data[i3 + texCoordOffset +1]};

                float x1 = v2[0] - v1[0];
                float x2 = v3[0] - v1[0];
                float y1 = v2[1] - v1[1];
                float y2 = v3[1] - v1[1];
                float z1 = v2[2] - v1[2];
                float z2 = v3[2] - v1[2];

                float s1 = w2[0] - w1[0];
                float s2 = w3[0] - w1[0];
                float t1 = w2[1] - w1[1];
                float t2 = w3[1] - w1[1];

                float r = 1.0f / (s1 * t2 - s2 * t1);
                glm::vec3 faceTangent = {(t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r};
                glm::vec3 faceBitangent = {(s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r};

                calculateTangentAndBitangent(&mesh.data[i1 + normalOffset], faceTangent, faceBitangent,
                                             &mesh.data[i1 + tangentOffset], &mesh.data[i1 + bitangentOffset]);
                calculateTangentAndBitangent(&mesh.data[i2 + normalOffset], faceTangent, faceBitangent,
                                             &mesh.data[i2 + tangentOffset], &mesh.data[i2 + bitangentOffset]);
            }
        }
    }
}
