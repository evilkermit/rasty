#ifndef OSP_RASTER_DATAFILE_H
#define OSP_RASTER_DATAFILE_H

#include <string>
#include <vector>

#include <osp_raster.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"


namespace rasty
{

    enum FILETYPE {TIFF, NETCDF, UNKNOWN};


    class DataFile {
        public:
            DataFile();
            ~DataFile();
            void loadFromFile(std::string filename);
            void calculateStatistics();
            void printStatistics();

            std::string filename;
            FILETYPE filetype;

            unsigned long int height;
            unsigned long int width;
            unsigned long int numValues;

            double originX;
            double originY;
            double pixelSizeX;
            double pixelSizeY;

            double geoTransform[6];


            float minVal;
            float maxVal;
            float avgVal;
            float stdDev;
            float *data;

            /* basic representation */
            std::vector<rkcommon::math::vec3f> vertex;
            std::vector<rkcommon::math::vec4f> color;
            std::vector<rkcommon::math::vec3ui> index;
        

            bool statsCalculated;
        private:
            FILETYPE getFiletype();
            void readTIFF();

    }; 
} 


#endif