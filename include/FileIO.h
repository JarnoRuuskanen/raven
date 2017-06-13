#pragma once
#include <string>

namespace Raven
{
    //Namespace for reading and writing into files.
    namespace FileIO
    {
        //Reads a file and adds it to the data variable.
        bool readImageFile(std::string filename, unsigned char *data,
                           int &width, int &height, int &layerCount);
    }
}
