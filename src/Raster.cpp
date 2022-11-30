#include "Raster.h"
#include "DataFile.h"
#include "TransferFunction.h"
// #include "rasty.h"
#include <iostream>
#include <vector>
#include <ospray/ospray.h>
// #include <ospray/ospray_cpp.h>
#include <ospray/ospray_util.h>

namespace rasty
{
    

/*
 * TODO:
 * change volume into geometry mesh
 */


Raster::Raster(std::string filename)
{   
    this->ID = createID();

    /* load data from file */
    this->dataFile = new rasty::DataFile();
    this->loadFromFile(filename);

    this->init();
}

Raster::~Raster()
{
   //std::cout << "[Raster] Deleting Raster" << std::endl;
    delete this->dataFile;
    this->dataFile = NULL;



    // delete this->transferFunction;
    // this->transferFunction = NULL;
    // ospRemoveParam(this->oVolume, "voxelData");
    // ospRemoveParam(this->oVolume, "dimensions");
    // ospRemoveParam(this->oVolume, "voxelType");
    // ospRemoveParam(this->oVolume, "voxelRange");
    // ospRemoveParam(this->oVolume, "gridOrigin");
    // ospRemoveParam(this->oVolume, "transferFunction");
    // ospRelease(this->oVolume);
    ////std::cout << "[Raster] Releasing data" << std::endl;
    // ospRelease(this->oData);
   //std::cout << "[Raster] Releasing mesh" << std::endl;
    
    ospRelease(this->oMesh);
   //std::cout << "[Raster] Deleted Raster" << std::endl;

}



void Raster::init()
{
    this->elev_scale = 1.f / 10000.f;
    this->hw_scale = 1.f;
    // ospRelease(this->oMesh);

    /* mesh:set up the mesh */
    this->oMesh = ospNewGeometry("mesh");

    /* mesh: add data to the mesh */
    OSPData data = ospNewSharedData1D(this->dataFile->vertex.data(), OSP_VEC3F, this->dataFile->vertex.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "vertex.position", data);
    ospRelease(data); // we are done using this handle

    /* mesh: set vertex colors */
    data = ospNewSharedData1D(this->dataFile->color.data(), OSP_VEC4F, this->dataFile->color.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "vertex.color", data);
    ospRelease(data);

    /* mesh: set triangle indices */
    data = ospNewSharedData1D(this->dataFile->index.data(), OSP_VEC3UI, this->dataFile->index.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "index", data);
    ospRelease(data);

    ospCommit(this->oMesh);

}

void Raster::setColor(std::vector<rkcommon::math::vec4f> color) {
    this->dataFile->color = color;
    OSPData data = ospNewSharedData1D(this->dataFile->color.data(), OSP_VEC4F, this->dataFile->color.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "vertex.color", data);
    ospRelease(data);
    ospCommit(this->oMesh);
}
void Raster::setElevationScale(float elevationScale) {
    this->elev_scale = elevationScale;
}
void Raster::setHeightWidthScale(float heightWidthScale) {
    this->hw_scale = heightWidthScale;
}

rkcommon::math::vec2f Raster::getHW()
{
    return rkcommon::math::vec2f(this->dataFile->height, this->dataFile->width);
}

rkcommon::math::box3f Raster::getBounds() 
{
    rkcommon::math::box3f bounds{
        rkcommon::math::vec3f( // min
            this->dataFile->originX, 
            this->dataFile->minVal,
            this->dataFile->originY
            ), 
        rkcommon::math::vec3f( // max
            this->dataFile->originX + (this->dataFile->width * this->dataFile->pixelSizeX), 
            this->dataFile->maxVal,
            this->dataFile->originY + (this->dataFile->height * this->dataFile->pixelSizeY)
        )
    };
    return bounds;
}

rkcommon::math::affine3f Raster::getCenterTransformation()
{
    // setup scaling vector
    rkcommon::math::vec3f scale_vec = rkcommon::math::vec3f(hw_scale, elev_scale, hw_scale);

    // create translation to center of coordinate system
    rkcommon::math::box3f bounds = this->getBounds();
    rkcommon::math::vec3f center_vec = (bounds.lower + bounds.upper) * 0.5f;

    // build transformation vector
    return rkcommon::math::affine3f::scale(scale_vec) * 
            rkcommon::math::affine3f::translate(-center_vec);


}

OSPGeometry Raster::asOSPRayObject()
{
    return this->oMesh;
}

void Raster::loadFromFile(std::string filename)
{
    this->dataFile->loadFromFile(filename);
    this->dataFile->calculateStatistics();
    // this->dataFile->printStatistics();
}

}
