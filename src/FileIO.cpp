#include "Headers.h"
#include "FileIO.h"
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
                           int &width, int &height, int &layerCount)
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
    }
}
