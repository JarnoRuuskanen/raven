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
         * @brief Reads an image file and returns the data.
         * @param filename
         * @param data
         * @param width
         * @param height
         * @param layerCount
         * @return False if something went wrong.
         */
        bool readImageFile(std::string filename, unsigned char *data,
                           int &width, int &height, int &layerCount) noexcept
        {
            data = stbi_load(filename.c_str(), &width,
                             &height, &layerCount, STBI_rgb_alpha);
            if(!data)
            {
                std::cerr << "Failed to load image file!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Reads the contents of a shader file.
         * @param filename
         * @return The contents of the file inside a vector.
         */
        std::vector<char> readShaderFile(std::string filename)
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
    }
}
