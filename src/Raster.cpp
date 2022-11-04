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
    

/*
 * TODO:
 * change volume into geometry mesh
 */


Raster::Raster(std::string filename)
{   
    std::cout << "[Raster] Init" << std::endl;
    this->ID = createID();
    std::cout << "[Raster] ID: " << this->ID << std::endl;

    /* load data from file */
    std::cout << "[Raster] Creating DataFile Object" << std::endl;
    this->dataFile = new rasty::DataFile();
    this->loadFromFile(filename);

    /* convert data to mesh */
    // this->rasterMesh = new rasty::RasterMesh(this->dataFile);
    // this->rasterMesh->generateMesh();

    this->init();
}

Raster::~Raster()
{
    std::cout << "[Raster] Deleting Raster" << std::endl;
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
    // std::cout << "[Raster] Releasing data" << std::endl;
    // ospRelease(this->oData);
    std::cout << "[Raster] Releasing mesh" << std::endl;
    
    ospRelease(this->oMesh);
    std::cout << "[Raster] Deleted Raster" << std::endl;

}



void Raster::init()
{
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


    

    /* set up the material for the mesh */
    // this->oMaterial = ospNewMaterial("", "obj"); // first argument no longer matters
    // ospCommit(this->oMaterial);

    /* create a geometric model */
    // this->oModel = ospNewGeometricModel(this->oMesh);
    // ospSetObject(this->oModel, "material", this->oMaterial);
    // ospCommit(this->oModel);
    // ospRelease(mesh);
    // ospRelease(mat);
    // ospSetObject(this->oModel, "material", material);
    // ospSetObject(this->oModel, "transferFunction",
            // this->transferFunction->asOSPObject());
    // std::cout << "comiting volume model" << std::endl;
    // ospCommit(this->oModel);
    // std::cout << "done" << std::endl;
}
rkcommon::math::vec2f Raster::getHW()
{
    return rkcommon::math::vec2f(this->dataFile->height, this->dataFile->width);
}

std::vector<rkcommon::math::vec3f> Raster::getBounds() 
{
    std::vector<rkcommon::math::vec3f> bounds{
        rkcommon::math::vec3f( // min
            this->dataFile->originX, 
            this->dataFile->originY,
            0//this->dataFile->minVal
            ), 
        rkcommon::math::vec3f( // max
            this->dataFile->originX + (this->dataFile->width * this->dataFile->pixelSizeX), 
            this->dataFile->originY + (this->dataFile->height * this->dataFile->pixelSizeY),
            0//this->dataFile->maxVal
        )
    };
    return bounds;
}

rkcommon::math::affine3f Raster::getCenterTransformation()
{
    const float spacing = 2.5f;
    std::vector<rkcommon::math::vec3f> bounds = this->getBounds();
    
    rkcommon::math::vec3f center = (bounds[0] + bounds[1]) * 0.5f;
    return rkcommon::math::affine3f::translate(-center);
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
