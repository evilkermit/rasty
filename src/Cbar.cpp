#include "Cbar.h"

#include "ospray/ospray_cpp/ext/rkcommon.h"
#include "rapidjson/filereadstream.h"
#include "tinycolormap.hpp"


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
        // get the basin/variable min/maxes for consistent color bars.
        FILE *fp = fopen("/data/ranges.json", "r");
        fseek(fp, 0, SEEK_END);
        long fileLength = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *json = (char *)malloc(fileLength + 1);
        fread(json, 1, fileLength, fp);
        fclose(fp);
        json[fileLength] = 0;

        this->ranges.ParseInsitu(json);

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
    rkcommon::math::vec3f Cbar::getColor(std::string basin, std::string variable, float value)
    {
        float min = std::stof(this->ranges[basin.c_str()][variable.c_str()][0].GetString());
        float max = std::stof(this->ranges[basin.c_str()][variable.c_str()][1].GetString());

        float normalizedValue = (value - min) / (max - min);
        tinycolormap::Color color = tinycolormap::GetColor(normalizedValue, tinycolormap::ColormapType::Viridis);

        return rkcommon::math::vec3f(color.r(), color.g(), color.b());
    }
}
