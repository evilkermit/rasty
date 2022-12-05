#include "DataFile.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
#include "ospray/ospray_cpp/ext/rkcommon.h"

// for netcdf files
#include <netcdf>
#include <map>

namespace rasty
{

DataFile::DataFile(): 
statsCalculated(false), ncLoaded(false), varLoaded(false)
{
   //std::cout << "[DataFile] Init" << std::endl;
    this->data = NULL;
}

DataFile::~DataFile(){
   //std::cout << "[DataFile] Deleting DataFile" << std::endl;

    if (this->data != NULL){
        free(this->data);
    }

    this->data = NULL;
   //std::cout << "[DataFile] Deleted DataFile" << std::endl;

}


void DataFile::loadFromFile(std::string filename)
{
    this->filename = filename;
    this->filetype = getFiletype();

    if(this->filetype == UNKNOWN) {
        std::cerr << "Unknown filetype!" << std::endl;
        throw std::exception();
    }
    else if (this->filetype == TIFF) {
        readTIFF();
    }
    else if (this->filetype == NETCDF) {
        readNetCDF();
    }
    
        
}   

void DataFile::loadTimeStep(size_t timestep) {
    std::cout<<"[DataFile] Loading timestep "<<timestep<<std::endl;
    if (this->varLoaded == false) {
        std::cerr << "No netCDF variable loaded!" << std::endl;
        throw std::exception();
    }

    // load data

    if (0 <= timestep && timestep < this->timeDim) {
        if (this->data != NULL) {
            free(this->data);
        }
        this->data = (float*) malloc(this->numValues * sizeof(float));
        this->ncVariable.getVar(
            std::vector<size_t> {timestep, 0, 0}, 
            std::vector<size_t> {1, this->latDim, this->lonDim}, 
            this->data);
    }
    else {
        std::cerr << "Invalid timestep!" << std::endl;
    }
}

void DataFile::loadVariable(std::string varname)
{
    std::cout<< "[DataFile] Loading variable " << varname << std::endl;
    if (this->ncLoaded == false) {
        std::cerr << "No netCDF data loaded!" << std::endl;
        throw std::exception();
    }

    // search map for variable
    auto it = this->varmap.find(varname);
    this->ncVariable = it->second;

    if (this->ncVariable.isNull()) {
        std::cerr << "Variable ["<< varname <<"] not found!" << std::endl;
        throw std::exception();
    }

    // overwrite any configured values   with the file's values
    if (this->ncVariable.getDimCount() == 3) {
        this->timeDim = this->ncVariable.getDim(0).getSize();
        this->latDim = this->ncVariable.getDim(1).getSize();
        this->lonDim = this->ncVariable.getDim(2).getSize();
    }
    else if (this->ncVariable.getDimCount() == 2) {
        this->timeDim = 1;
        this->latDim = this->ncVariable.getDim(0).getSize();
        this->lonDim = this->ncVariable.getDim(1).getSize();
    }
    else {
        std::cerr << "Variable has wrong dimension!" << std::endl;
        throw std::exception();
    }
        this->numValues = this->latDim * this->lonDim;

    this->varLoaded = true;
    std::cout<< "[DataFile] Loaded variable " << varname << std::endl;
}

void DataFile::readNetCDF()
{
    this->ncFile = new netCDF::NcFile(filename.c_str(), netCDF::NcFile::read);
    
    // only get the first variable
    this->varmap = ncFile->getVars();

    for (auto it = varmap.begin(); it != varmap.end(); ++it)
    {
        if (it->first != "latitude" &&
            it->first != "longitude" &&
            it->first != "time") {
            this->variables.push_back(it->first);
        }
       std::cout << "Variable name: " << it->first << std::endl;
    }
    this->ncLoaded = true;

}
std::vector<std::string> DataFile::getVariableNames()
{
    return this->variables;
}

void DataFile::readTIFF()
{
    /* read in the raster data */
    GDALAllRegister();

    GDALDataset  *dataset;
    GDALRasterBand  *elevationBand;
    CPLErr err;
    int numIndices;

    /* open the file */
    dataset = (GDALDataset *) GDALOpen(filename.c_str(), GA_ReadOnly);
    if( dataset == NULL ) {
       //std::cout << "Failed to open" << std::endl;
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
       //std::cout << "Failed read geotransform" << std::endl;
        exit(1);
    }

    /* not memmaped */
    /* TODO - look into memmaping */

    /* get the reaster band (DEM has one raster band representing elevation) */
    elevationBand = dataset->GetRasterBand(1);


    /* read the raster band into an array */
    /* TODO - look into being able to scale this down need be */
    this->data = (float *)malloc(this->numValues * sizeof(float));

    err = elevationBand->RasterIO(GF_Read, 0, 0, this->width, this->height, &this->data[0], this->width, this->height, GDT_Float32, 0, 0);
    if (err != CE_None) {
       //std::cout << "Failed to read raster band" << std::endl;
        exit(1);
    }

    /* close the dataset */
    GDALClose(dataset); 
    GDALDestroy();

    /* convert data set to vertex and color arrays */
    numIndices = (this->width - 1) * (this->height - 1) * 2;
    this->vertex.reserve(this->numValues);
    this->color.reserve(this->numValues);
    this->index.reserve(numIndices);
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {

            /* get current coordinate */
            double currY = this->originY + (y * this->pixelSizeY);
            double currX = this->originX + (x * this->pixelSizeX);

            /* get the elevation value */
            double z = this->data[(y * this->width) + x];
            this->vertex.push_back(rkcommon::math::vec3f((float)currX,(float)z,(float)currY));
            this->color.push_back(rkcommon::math::vec4f(0.9f, 0.5f, 0.5f, 1.0f));

            /* create indices for vertex array */
            if (y+1 < this->height && x+1 < this->width) {
                int tl = (y * this->width) + x;
                int bl = ((y+1) * this->width) + x;
                int br = ((y+1) * this->width) + (x+1);
                int tr = (y * this->width) + (x+1);
                this->index.push_back(rkcommon::math::vec3ui(tl,bl,br)); // bottom left triangle
                this->index.push_back(rkcommon::math::vec3ui(tl,br,tr)); // top right triangle
            }
        }
    }
}
   




FILETYPE DataFile::getFiletype()
{
    std::stringstream ss;
    ss.str(this->filename);
    std::string token;
    char delim = '.';
    // keep the last token after splitting on dots
    while(std::getline(ss, token, delim)) {
    }

    if (token.compare("tiff") == 0 || 
        token.compare("tif") == 0) {
        return TIFF;
    }
    else if(token.compare("nc") == 0) {
        return NETCDF;
    }
    else {
        return UNKNOWN;
    }
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

   //std::cout << this->filename << std::endl;
    ////std::cout << "dimensions: " << this->xDim << ", "
    //                             << this->yDim << ", "
    //                             << this->zDim << std::endl;
   //std::cout << "minimum:    " << this->minVal << std::endl;
   //std::cout << "maximum:    " << this->maxVal << std::endl;
   //std::cout << "mean:       " << this->avgVal << std::endl;
   //std::cout << "std. dev.:  " << this->stdDev << std::endl;
}

}
