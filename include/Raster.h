#ifndef RASTY_RASTER_H
#define RASTY_RASTER_H


#include <rasty.h>
#include <DataFile.h>

#include <string>
#include <vector>

#include <ospray/ospray.h>
#include <ospray/ospray_util.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"



namespace rasty
{
    class Raster {
        public:
            Raster(std::string filename);
            ~Raster();

            /* helpers for scaling geo data */
            rkcommon::math::box3f getBounds();
            rkcommon::math::vec2f getHW();
            rkcommon::math::affine3f getCenterTransformation();

            /* setters for scaling/rendering geo data*/
            void setColor();
            void setColor(std::vector<rkcommon::math::vec4f> color);
            void setElevationScale(float elevationScale);
            void setHeightWidthScale(float heightWidthScale);

            /* returns raster mesh */
            OSPGeometry asOSPRayObject();

            std::string ID;

        // private:

            DataFile *dataFile;
            TransferFunction *transferFunction;

            /* internal ospray objects */
            OSPGeometry oMesh;
            OSPData oData;
            // OSPMaterial oMaterial;
            // OSPGeometricModel oModel;

            /* scaling */
            float elev_scale;
            float hw_scale;
            rkcommon::math::affine3f centerTranslation;

            /* setup internals */
            void init();
            void loadFromFile(std::string filename);
    };
} 


#endif
