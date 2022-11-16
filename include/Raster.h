#ifndef RASTY_RASTER_H
#define RASTY_RASTER_H


#include <rasty.h>
#include <DataFile.h>

#include <string>
#include <vector>

#include <ospray/ospray.h>
// #include <ospray/ospray_cpp.h>
#include <ospray/ospray_util.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"



namespace rasty
{
    class Raster {
        public:
            Raster(std::string filename);
            ~Raster();

            rkcommon::math::box3f getBounds();
            rkcommon::math::vec2f getHW();
            rkcommon::math::affine3f getCenterTransformation();

            void setColor(std::vector<rkcommon::math::vec4f> color);

            OSPGeometry asOSPRayObject();

            std::string ID;
            
        // private:
            DataFile *dataFile;
            TransferFunction *transferFunction;
            rkcommon::math::affine3f centerTranslation;

            OSPGeometry oMesh;
            // OSPMaterial oMaterial;
            // OSPGeometricModel oModel;
            OSPData oData;

            void init();
            void loadFromFile(std::string filename);
    };
} 


#endif