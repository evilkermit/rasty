#ifndef OSP_RASTER_RASTER_H
#define OSP_RASTER_RASTER_H


#include <osp_raster.h>
#include <DataFile.h>
#include <RasterMesh.h>

#include <string>
#include <vector>

#include <ospray/ospray.h>
// #include <ospray/ospray_cpp.h>
// #include <ospray/ospray_util.h>



namespace rasty
{
    class Raster {
        public:
            Raster(std::string filename);
            ~Raster();


            std::vector<long unsigned int> getBounds();
            OSPGeometricModel asOSPRayObject();

            std::string ID;

        private:
            DataFile *dataFile;
            RasterMesh *rasterMesh;
            TransferFunction *transferFunction;

            OSPGeometry oMesh;
            OSPMaterial oMaterial;
            OSPGeometricModel oModel;
            OSPData oData;

            void init();
            void loadFromFile(std::string filename);
    };
} 


#endif