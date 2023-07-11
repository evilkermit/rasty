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
            std::vector<std::string> getVariableNames(); /* netcdf data only */

            void calculateStatistics();
            void printStatistics();

            std::string filename;
            FILETYPE filetype;
            
            /* netcdf metadata */
            unsigned long int lonDim;
            unsigned long int latDim;
            unsigned long int timeDim;
            unsigned long int height;
            unsigned long int width;
            unsigned long int numValues;
            unsigned long int numVertices;

            /* geo info */
            double originX;
            double originY;
            double pixelSizeX;
            double pixelSizeY;
            double geoTransform[6];

            /* stats about terrain data*/
            float minVal;
            float maxVal;
            float avgVal;
            float stdDev;
            float *data;
            float *boundary;

            /* texture coordinates for each vertex */
            std::vector<rkcommon::math::vec2f> texcoords;

            /* used for geo file loading */
            std::vector<rkcommon::math::vec3f> vertex;
            std::vector<rkcommon::math::vec4f> color;
            std::vector<rkcommon::math::vec3ui> index;
        
            /* used for nc file loading */
            std::multimap<std::string, netCDF::NcVar> varmap;
            std::vector<std::string> variables;

            std::string basinName;
            std::string varName;
            
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
