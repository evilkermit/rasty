#ifndef RASTY_CONFIGREADER_H
#define RASTY_CONFIGREADER_H

#include <rasty.h>

#include <string>

#include <rapidjson/document.h>

namespace rasty {
    
    class ConfigReader {
        public:

            ConfigReader();

            void parseConfigFile(std::string filename,
                    rapidjson::Document& config);
    };

}

#endif
