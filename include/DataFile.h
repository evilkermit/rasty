#ifndef OSP_RASTER_DATAFILE_H
#define OSP_RASTER_DATAFILE_H

#include <string>

#include <osp_raster.h>

namespace rasty
{
    class DataFile {
        public:
            DataFile();
            ~DataFile();
            void loadFromFile(std::string filename);
            void calculateStatistics();
            void printStatistics();

            std::string filename;

            unsigned long int height;
            unsigned long int width;
            unsigned long int numValues;

            float originX;
            float originY;
            float pixelSizeX;
            float pixelSizeY;

            double geoTransform[6];


            float minVal;
            float maxVal;
            float avgVal;
            float stdDev;
            float *data;

            bool statsCalculated;

    }; 
} 


#endif