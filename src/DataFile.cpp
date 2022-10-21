#include "DataFile.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"

namespace rasty
{

DataFile::DataFile(): statsCalculated(false){
    this->data = NULL;
}

DataFile::~DataFile(){

    if (this->data != NULL){
        free(this->data);
    }

    this->data = NULL;
}


void DataFile::loadFromFile(std::string filename)
{
    this->filename = filename;
    std::cout<<"inhere\n";

    /* read in the raster data */
    GDALAllRegister();

    /* open the file */
    GDALDataset  *dataset = (GDALDataset *) GDALOpen(filename.c_str(), GA_ReadOnly);
    if( dataset == NULL ) {
        std::cout << "Failed to open" << std::endl;
        exit(1);
    }

    /* get image metadata */
    this->width = dataset->GetRasterXSize();
    this->height = dataset->GetRasterYSize();
    this->numValues = this->width * this->height;
    if (dataset->GetGeoTransform(this->geoTransform) == CE_None ) {
        this->originX = this->geoTransform[0];
        this->originY = this->geoTransform[3];
        this->pixelSizeX = this->geoTransform[1];
        this->pixelSizeY = this->geoTransform[5];
    } else {
        std::cout << "Failed read geotransform" << std::endl;
        exit(1);
    }


    /* not memmaped */
    /* TODO - look into memmaping */

    /* get the reaster band (DEM has one raster band representing elevation) */
    GDALRasterBand  *elevationBand = dataset->GetRasterBand(1);


    /* read the raster band into an array */
    /* TODO - look into being able to scale this down need be */
    this->data = (float *)malloc(this->numValues * sizeof(float));

    CPLErr err = elevationBand->RasterIO(GF_Read, 0, 0, this->width, this->height, &this->data[0], this->width, this->height, GDT_Float32, 0, 0);
    if (err != CE_None) {
        std::cout << "Failed to read raster band" << std::endl;
        exit(1);
    }

    /* close the dataset */
    GDALClose(dataset); 
    std::cout << "width: " << this->width << std::endl;
    std::cout << "height: " << this->height << std::endl;
    std::cout << "numValues: " << this->numValues << std::endl;
    std::cout << "originX: " << this->originX << std::endl;
    std::cout << "originY: " << this->originY << std::endl;
    std::cout << "pixelSizeX: " << this->pixelSizeX << std::endl;
    std::cout << "pixelSizeY: " << this->pixelSizeY << std::endl;

    // for (int i = 0 ; i < 100; i++) {
    //     std::cout << "data[" << i << "]: " << this->data[i] << std::endl;
    // }
}   



void DataFile::calculateStatistics()
{
    // calculate min, max, avg, stddev
    // stddev and avg may be useful for automatic diverging color maps
    this->minVal = data[0];
    this->maxVal = data[0];
    double total = 0, totalSquares = 0;
    for(int i = 1; i < this->numValues; i++) {
        if(data[i] < this->minVal)
            this->minVal = data[i];
        if(data[i] > this->maxVal)
            this->maxVal = data[i];
        total += data[i];
        totalSquares += data[i]*data[i];
    }
    this->avgVal = total / this->numValues;
    this->stdDev = std::sqrt(totalSquares/this->numValues -
                             this->avgVal*this->avgVal);
    this->statsCalculated = true;
}


void DataFile::printStatistics()
{
    // debugging purposes
    if(!this->statsCalculated) {
        std::cerr << "Statistics not calculated for this data!" << std::endl;
        return;
    }

    std::cout << this->filename << std::endl;
    // std::cout << "dimensions: " << this->xDim << ", "
    //                             << this->yDim << ", "
    //                             << this->zDim << std::endl;
    std::cout << "minimum:    " << this->minVal << std::endl;
    std::cout << "maximum:    " << this->maxVal << std::endl;
    std::cout << "mean:       " << this->avgVal << std::endl;
    std::cout << "std. dev.:  " << this->stdDev << std::endl;
}

}
