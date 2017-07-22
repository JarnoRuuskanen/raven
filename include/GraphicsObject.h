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
        uint32_t vertexOffset;
        uint32_t vertexCount;
    };

    class GraphicsObject
    {
        public:
            GraphicsObject();
            virtual ~GraphicsObject();
            //Loads the vertices of the object.
            bool loadFrame(const VkDevice logicalDevice, const std::string filename);
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
            VulkanImage textureObject;
            Mesh mesh;
    };
}
