#ifndef RASTY_DATAFILE_H
#define RASTY_DATAFILE_H

#include <string>
#include <vector>
#include <netcdf>
#include <map>
#include <rasty.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"


namespace rasty
{

    enum FILETYPE {TIFF, NETCDF, UNKNOWN};


    class DataFile {
        public:
            DataFile();
            ~DataFile();
            void loadFromFile(std::string filename);
            void loadVariable(std::string varname); /* netcdf data only */
            void loadTimeStep(size_t timestep); /* netcdf data only */
            void calculateStatistics();
            void printStatistics();

            std::string filename;
            FILETYPE filetype;

            unsigned long int lonDim;
            unsigned long int latDim;
            unsigned long int timeDim;
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

            /* used for geo file loading */
            std::vector<rkcommon::math::vec3f> vertex;
            std::vector<rkcommon::math::vec4f> color;
            std::vector<rkcommon::math::vec3ui> index;
        
            /* used for nc file loading */
            std::multimap<std::string, netCDF::NcVar> varmap;
            netCDF::NcFile *ncFile;
            netCDF::NcVar ncVariable;
            bool statsCalculated;
            bool ncLoaded;
            bool varLoaded;
        private:
            FILETYPE getFiletype();
            void readTIFF();
            void readNetCDF();

    }; 
} 


#endif