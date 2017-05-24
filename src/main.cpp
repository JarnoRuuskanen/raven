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
    raven.start("Raven");
    return 0;
}
