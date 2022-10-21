#ifndef OSP_RASTER_RASTER_H
#define OSP_RASTER_RASTER_H


#include <osp_raster.h>
#include <DataFile.h>

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
            OSPVolume asOSPRayObject();

            std::string ID;

        private:
            DataFile *dataFile;
            TransferFunction *transferFunction;

            OSPVolume oVolume;
            OSPData oData;

            void init();
            void loadFromFile(std::string filename);
    };
} 


#endif