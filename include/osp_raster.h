#ifndef OSP_RASTER_H
#define OSP_RASTER_H

#include <ospray/ospray.h>

#include <string>


namespace rasty{

    /* contains metadata about the data being requested
     * This includes things like:
     *  - filename
     *  - dimensions
     *  etc.
     */
    class DataFile;

    class Raster;

    class Camera; 

    class Renderer; 

    class TransferFunction;

    void rastyInit(int argc, const char **argv);

    std::string createID();
} 


#endif