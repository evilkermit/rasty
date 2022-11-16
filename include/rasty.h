#ifndef RASTY_H
#define RASTY_H

#include <ospray/ospray.h>

#include <string>


namespace rasty{

    class DataFile;

    class Raster;

    class Cbar;

    class Camera; 

    class Renderer; 

    class TransferFunction;
    
    class ArcballCamera;

    void rastyInit(int argc, const char **argv);

    std::string createID();
} 


#endif