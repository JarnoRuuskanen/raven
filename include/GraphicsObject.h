#pragma once
#include "Headers.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanUtility.h"

/** GraphicsObject class is for everything we want
    to draw onto the screen. Graphics objects should be created from
    vertex data, have their own materials, textures, positions etc etc. **/
namespace Raven
{
    struct Mesh
    {
        std::vector<float> data;
        struct Part
        {
            uint32_t vertexOffset;
            uint32_t vertexCount;
        };
        std::vector<Part> parts;
    };

    class GraphicsObject
    {
        public:
            GraphicsObject();
            virtual ~GraphicsObject();
            //Loads the model data of a file.
            bool loadModel(const VkDevice logicalDevice, const std::string filename,
                           bool loadNormals, bool loadTextureCoordinates, bool generateTangentSpaceVectors,
                           bool normalize, uint32_t *vertexStride);
            //Adds a texture to the object.
            bool addTexture(const VkDevice logicalDevice,
                            VkPhysicalDeviceMemoryProperties memoryProperties,
                            const std::string filename,
                            VkImageUsageFlags usage,
                            VkFormat format,
                            VkSampleCountFlagBits samples,
                            uint32_t mipLevelCount);

            Mesh *getMesh(){return &mesh;}
        private:
            void generateTangentSpaceVectors(Mesh &mesh);
            void calculateTangentAndBitangent(float const *normalData,
                                              const glm::vec3 &faceTangent,
                                              const glm::vec3 &faceBitangent,
                                              float *tangentData,
                                              float *bitangentData);
            VulkanImage textureObject;
            Mesh mesh;
    };
}
