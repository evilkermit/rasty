#include "Raster.h"
#include "DataFile.h"
#include "TransferFunction.h"
// #include "osp_raster.h"
#include<iostream>
#include <vector>
#include <ospray/ospray.h>
// #include <ospray/ospray_cpp.h>
#include <ospray/ospray_util.h>

namespace rasty
{
    
Raster::Raster(std::string filename)
{   
    this->ID = createID();

    this->dataFile = new rasty::DataFile();
    this->loadFromFile(filename);

    this->init();
}

Raster::~Raster()
{

    delete this->dataFile;
    this->dataFile = NULL;
    // delete this->transferFunction;
    // this->transferFunction = NULL;
    ospRemoveParam(this->oVolume, "voxelData");
    ospRemoveParam(this->oVolume, "dimensions");
    ospRemoveParam(this->oVolume, "voxelType");
    ospRemoveParam(this->oVolume, "voxelRange");
    ospRemoveParam(this->oVolume, "gridOrigin");
    ospRemoveParam(this->oVolume, "transferFunction");
    ospRelease(this->oVolume);
    ospRelease(this->oData);
}



void Raster::init()
{
    /* TODO - setup transfer function here */
    std::cout<<"init"<<std::endl;
    this->transferFunction = new TransferFunction();
    this->transferFunction->setRange(this->dataFile->minVal,
                                     this->dataFile->maxVal);
    std::cout<<"init"<<std::endl;
    /* setup OSPRay volume */
    this->oVolume = ospNewVolume("structuredRegular");
    this->oData = ospNewSharedData1D(this->dataFile->data, OSP_FLOAT, 
            this->dataFile->numValues);
    std::cout<<"init"<<std::endl;

    int dimensions[3] = {256, 256,256}; 
    float center[3] = {-dimensions[0]/(float)2.0,
                      -dimensions[1]/(float)2.0,
                      -dimensions[2]/(float)2.0};
    // float voxelRange[3] = {this->dataFile->minVal, 
    //                       this->dataFile->maxVal};
    std::cout<<"init"<<std::endl;

    ospSetObject(this->oVolume, "voxelData", this->oData);
    ospSetVec3i(this->oVolume, "dimensions", 256, 256, 256);
    ospSetString(this->oVolume, "voxelType", "float");
    ospSetVec3f(this->oVolume, "gridOrigin", center[0], center[1], center[2]);
    ospSetVec2f(this->oVolume, "voxelRange", this->dataFile->minVal, 
            this->dataFile->maxVal);
    std::cout<<"init222"<<std::endl;

    // ospSetObject(this->oVolume, "transferFunction",
    //         this->transferFunction->asOSPObject());
    std::cout<<"3333"<<std::endl;

    ospCommit(this->oVolume);
    std::cout<<"init222"<<std::endl;
    
}


OSPVolume Raster::asOSPRayObject()
{
    return this->oVolume;
}

void Raster::loadFromFile(std::string filename)
{
    this->dataFile->loadFromFile(filename);
    this->dataFile->calculateStatistics();
    this->dataFile->printStatistics();
}

}
