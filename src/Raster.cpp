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
    


/**
 * Raster constructor/destructor
 * loads in tiff file and sets up ospray objects
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
    delete this->dataFile;
    this->dataFile = NULL;
    ospRelease(this->oMesh);
}


/**
 * init
 * sets up ospray objects data and mesh
*/
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

    /* mesh: set texture coordinates */
    data = ospNewSharedData1D(this->dataFile->texcoords.data(), OSP_VEC2F, this->dataFile->texcoords.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "vertex.texcoords", data);
    ospRelease(data);

    /* mesh: set triangle indices */
    data = ospNewSharedData1D(this->dataFile->index.data(), OSP_VEC3UI, this->dataFile->index.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "index", data);
    ospRelease(data);

    ospCommit(this->oMesh);

}

/**
 * setColor(color)
 * sets the color matrix of the mesh given a color matrix
*/
void Raster::setColor(std::vector<rkcommon::math::vec4f> color) {
    // clear previous color data to conserve memory
    this->dataFile->color.clear();   
    this->dataFile->color.shrink_to_fit();   

    // set new color data
    this->dataFile->color = color;
    this->setColor();
}

/**
 * setColor()
 * sets the color matrix of the mesh using the stored class color matrix
*/
void Raster::setColor() {
    // clear previous vertex colors
    ospRemoveParam(this->oMesh, "vertex.color");

    // set new vertex colors
    OSPData data = ospNewSharedData1D(this->dataFile->color.data(), OSP_VEC4F, this->dataFile->color.size());
    ospCommit(data);
    ospSetObject(this->oMesh, "vertex.color", data);
    ospRelease(data);
    ospCommit(this->oMesh);
}

/**
 * setElevationScale, setHeightWidthScale, getHW, getBounds, getCenterTransformation
 * helper functions for mesh scaling and transformation
*/
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

/** 
 * asOSPRayObject
 * returns the ospray mesh object
*/
OSPGeometry Raster::asOSPRayObject()
{
    return this->oMesh;
}

/**
 * loadFromFile
 * loads in a tiff file and calculates statistics
*/
void Raster::loadFromFile(std::string filename)
{
    this->dataFile->loadFromFile(filename);
    this->dataFile->calculateStatistics();
}

}
