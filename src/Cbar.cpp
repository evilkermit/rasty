#include "Cbar.h"
#include "ospray/ospray_cpp/ext/rkcommon.h"


namespace rasty
{
    /**
     * Cbar constructor
     * Takes a string argument for the color map type
     * Currently only supports "base" color map
     * Sets function pointer to the appropriate color map function
    */
    Cbar::Cbar(std::string cmap_type)
    {
        this->cmap_type = cmap_type;
        if (cmap_type == "base"){
            this->colorMap = &rasty::Cbar::HSVtoRGB;
        }
        else {
            this->colorMap = &rasty::Cbar::HSVtoRGB;
        }
    }

    rkcommon::math::vec4f Cbar::HSVtoRGB(float h, float s, float v) {
        int i = std::floor(h * 6);
        float f = h * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);
        int m = i % 6;
        float r,g,b;
        if (m == 0)
            {r = v; g = t; b = p;}
            // r,g,b = v,t,p;
        else if (m == 1)
            {r = q; g = v; b = p;}
            // r,g,b = q,v,p;
        else if (m == 2)
            {r = p; g = q; b = t;}
            // r,g,b = p,q,t;
        else if (m == 3)
            {r = p; g = q; b = v;}
            // r,g,b = p,q,v;
        else if (m == 4)
            {r = t; g = p; b = v;}
            // r,g,b = t,p,v;
        else // m == 5
            {r = v; g = p; b = q;}
            // r,g,b = v,p,q;

        return rkcommon::math::vec4f(r,g,b,1.0f);
    }

    /**
     * getColor
     * Takes a float value and returns a vec4f color
     * Uses the configured function pointer to call the appropriate color map function
    */
    rkcommon::math::vec4f Cbar::getColor(float value)
    {
        return (this->*colorMap)(value,1,1);
    }
}