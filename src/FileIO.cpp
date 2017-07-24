#include "Headers.h"
#include "FileIO.h"
#include <fstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Raven
{
    namespace FileIO
    {
    /**
         * @brief Reads an image file and loads the texture data.
         * @param filename
         * @param imageData
         * @param imageWidth
         * @param imageHeight
         * @param imageComponentCount
         * @param requestedComponentCount (4 for rgba)
         * @param imageDataSize
         * @return False if something went wrong.
         */
        bool readImageFile(std::string filename, std::vector<unsigned char> &imageData,
                           int *imageWidth, int *imageHeight, int *imageComponentCount,
                           int requestedComponentCount, int *imageDataSize) noexcept
        {
            int width = 0;
            int height = 0;
            int components = 0;

            std::unique_ptr<unsigned char, void(*)(void*)> stbiData(stbi_load(filename.c_str(),
                                                                              &width,
                                                                              &height,
                                                                              &components,
                                                                              requestedComponentCount),
                                                                    stbi_image_free);

            if((!stbiData)  ||
               (width <= 0) ||
               (height <= 0)||
               (components <= 0))
            {
                std::cerr << "Failed to load image file!" << std::endl;
                return false;
            }

            int dataSize =
                    width*height*(0 < requestedComponentCount ? requestedComponentCount : components);

            if(imageDataSize)
                *imageDataSize = dataSize;
            if(imageWidth)
                *imageWidth = width;
            if(imageHeight)
                *imageHeight = height;
            if(imageComponentCount)
                *imageComponentCount = components;

            imageData.resize(dataSize);
            std::memcpy(imageData.data(), stbiData.get(), dataSize);

            return true;
        }

        /**
         * @brief Reads the contents of a shader file.
         * @param filename
         * @return The contents of the file inside a vector.
         */
        std::vector<char> readBinaryFile(std::string filename)
        {
            //Open the file.
            std::ifstream file(filename, std::ios::ate | std::ios::binary);
            if(!file.is_open())
            {
                throw std::runtime_error("Failed to open shader file.");
            }

            //Get the size of the file and resize the vector accordingly.
            size_t fileSize = (size_t) file.tellg();
            std::vector<char> fileContents(fileSize);

            //Go back to the beginning of the file and read all the bytes in the file at once.
            file.seekg(0);
            file.read(fileContents.data(), fileSize);

            //Close the file.
            file.close();
            return fileContents;
        }

        /**
         * @brief Writes data into a file. Empties the file if it already exists.
         * @param destinationFilename
         * @return False if the writing operation fails.
         */
        bool writeBinaryFile(std::string destinationFilename)
        {
            //Implementation required.
            return false;
        }

    }
}
