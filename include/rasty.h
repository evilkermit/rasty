#ifndef RASTY_H
#define RASTY_H

#include <ospray/ospray.h>

#include <string>


namespace rasty{
    /* contains metadata about the data being requested */
    class DataFile;

    /* abstraction wrapper around OSPRay mesh */
    class Raster;

    /* class that converts values [0,1] to rgba values */
    class Cbar;

    class TransferFunction;

    /* abstraction wrapper around OSPRay renderer */
    class Renderer; 
    
    /* abstraction wrapper around OSPRay camera
     * Provides simplified camera movement
     */
    class Camera; 

    /* configuration class, uses rapidjson to parse JSON
     * config files
     */
    class ConfigReader;
    class Configuration;

    void rastyInit(int argc, const char **argv);
    void rastyDestroy();

    std::string createID();
} 


#endif