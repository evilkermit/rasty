#ifndef RASTY_CONFIGURATION_H
#define RASTY_CONFIGURATION_H

#include <ConfigReader.h>
#include "rapidjson/document.h"

#include <string>
#include <vector>

namespace rasty {

    enum CONFSTATE {ERROR_NODATA, ERROR_MULTISET, SINGLE_NOVAR, SINGLE_VAR,
        MULTI_NOVAR, MULTI_VAR};

    class Configuration {

        public:
            Configuration(rapidjson::Document& json);
            CONFSTATE getGeoConfigState();
            CONFSTATE getDataConfigState();
            
            // file(s) for the terrain
            std::string geoFilename;
            std::vector<std::string> geoGlobbedFilenames;

            // file(s) for the data on the terrain
            std::string dataFilename;
            std::vector<std::string> dataGlobbedFilenames;

            // set variable for data if needed
            std::string dataVariable;

            // image metadata
            int imageWidth;
            int imageHeight;
            std::string imageFilename;
            std::vector<unsigned char> bgColor;
            std::string colorMap;
            // std::vector<float> colorMap;
            // std::vector<float> opacityMap;
            float opacityAttenuation;
            
            // camera metadata
            unsigned int samples;

            float cameraX;
            float cameraY;
            float cameraZ;

            float cameraUpX;
            float cameraUpY;
            float cameraUpZ;

            // terrain scaling
            float elevationScale;
            float heightWidthScale;
            
            // not used for now
            std::vector<float> isosurfaceValues;
            float specularity;

        private:
            ConfigReader *reader;

            void selectColorMap(std::string userInput);
            void selectOpacityMap(std::string userInput);
    };

}

#endif
