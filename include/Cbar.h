#ifndef RASTY_CBAR_H
#define RASTY_CBAR_H
#include <rasty.h>
#include <string>
#include "ospray/ospray_cpp/ext/rkcommon.h"

namespace rasty
{
    class Cbar {
        public:
            Cbar(std::string cmap_type);
            rkcommon::math::vec4f getColor(float value);


        private:
            rkcommon::math::vec4f HSVtoRGB(float h, float s, float v);
            rkcommon::math::vec4f (rasty::Cbar::*colorMap)(float, float, float) = NULL;
            std::string cmap_type;

    };
}
#endif
