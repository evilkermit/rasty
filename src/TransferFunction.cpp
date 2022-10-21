#include "TransferFunction.h"

#include <iostream>
#include <vector>
#include <ospray/ospray_util.h>

namespace rasty {

TransferFunction::TransferFunction()
{
    this->colorMap.reserve(256*3);
    this->opacityMap.reserve(256);

    //default black to white color map
    //and ramp opacity map
    for(int i = 0; i < 256; i++) {
        this->colorMap.push_back(i/255.0);
        this->colorMap.push_back(i/255.0);
        this->colorMap.push_back(i/255.0);
        this->opacityMap.push_back(i/255.0);
    }

    // setup OSPRay object(s)
    this->oTF = ospNewTransferFunction("piecewise_linear");
    this->oColorData = ospNewSharedData2D(this->colorMap.data(), OSP_FLOAT, 
            this->colorMap.size()/3, 3);
    this->oOpacityData = ospNewSharedData1D(this->opacityMap.data(), OSP_FLOAT,
            this->opacityMap.size());
    ospSetObject(this->oTF, "colors", this->oColorData);
    ospSetObject(this->oTF, "opacities", this->oOpacityData);
    ospCommit(this->oTF);
}

TransferFunction::~TransferFunction()
{
    ospRelease(this->oTF);
    ospRelease(this->oColorData);
    ospRelease(this->oOpacityData);
}

void TransferFunction::setRange(float minimum, float maximum)
{
    if(minimum > maximum) {
        std::cerr << "Minimum is larger than maximum!" << std::endl;
        return;
    }

    this->minVal = minimum;
    this->maxVal = maximum;

    // float temp[] = {this->minVal, this->maxVal};
    ospSetVec2f(this->oTF, "valueRange", this->minVal, this->maxVal);
    ospCommit(this->oTF);
}

void TransferFunction::attenuateOpacity(float amount)
{
    if(amount >= 1.0)
        return;
    for(int i = 0; i < this->opacityMap.size(); i++)
        this->opacityMap[i] = this->opacityMap[i] * amount;

    this->oOpacityData = ospNewSharedData1D(this->opacityMap.data(), OSP_FLOAT,
            this->opacityMap.size());
    ospSetObject(this->oTF, "opacities", this->oOpacityData);
    ospCommit(this->oTF);
}

OSPTransferFunction TransferFunction::asOSPObject()
{
    return this->oTF;
}

void TransferFunction::setColorMap(std::vector<float> &map)
{
    //map may be empty if the config file is used
    if(map.empty())
        return;

    //clear in case the new map is shorter than the previous, in which case
    //reserve won't do anything
    //reserve in case the new map is longer than the previous
    this->colorMap.clear();
    this->colorMap.reserve(map.size());

    for(int i = 0; i < map.size(); i++)
        this->colorMap.push_back(map[i]);

    this->oColorData = ospNewSharedData2D(this->colorMap.data(), OSP_FLOAT,
            this->colorMap.size()/3, 3);
    ospSetObject(this->oTF, "colors", this->oColorData);

    ospCommit(this->oTF);
}

void TransferFunction::setOpacityMap(std::vector<float> &map)
{
    //map may be empty if the config file is used
    if(map.empty())
        return;

    //clear in case the new map is shorter than the previous
    //reserver in case the new map is longer than the previous
    this->opacityMap.clear();
    this->opacityMap.reserve(map.size());

    for(int i = 0; i < map.size(); i++)
        this->opacityMap.push_back(map[i]);

    this->oOpacityData = ospNewSharedData1D(this->opacityMap.data(), OSP_FLOAT,
            this->opacityMap.size());
    ospSetObject(this->oTF, "opacities", this->oOpacityData);
    ospCommit(this->oTF);
}

}
