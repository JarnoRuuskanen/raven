#pragma once
#include <string>

namespace Raven
{
    //Namespace for reading and writing into files.
    namespace FileIO
    {
        //Reads a file and adds it to the data variable.
        bool readImageFile(std::string filename, std::vector<unsigned char> &imageData,
                           int *imageWidth, int *imageHeight, int *imageComponentCount,
                           int requestedComponentCount,
                           int *imageDataSize) noexcept;

        //Reads the contents of a SPIR-V file.
        std::vector<char> readBinaryFile(std::string filename);

        //Writes data into a file.
        bool writeBinaryFile(std::string destinationFilename);

    }
}
