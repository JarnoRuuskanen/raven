#include <iostream>
#include "RavenEngine.h"

/**
 * @brief Starts the Raven.
 * @return Returns 0 when application exits.
 */
int main(int argc, char* argv[])
{
    //Start Raven
    RavenEngine raven;
    if(!raven.start("Raven"))
        std::cerr << "Failed to start the engine!" << std::endl;

    return 0;
}
