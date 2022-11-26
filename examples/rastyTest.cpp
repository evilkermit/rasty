#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <alloca.h>
#include <vector>
#include <string>
#include <chrono>

#include "Raster.h"
#include "Camera.h"
#include "DataFile.h"
#include "Renderer.h"
#include "Configuration.h"
#include "Cbar.h"
#include <rasty.h>
#include <time.h>
#include "rapidjson/document.h"

#include "ospray/ospray.h"
#include "ospray/ospray_cpp.h"
#include "ospray/ospray_cpp/ext/rkcommon.h"
#include "rkcommon/utility/SaveImage.h"

using namespace rkcommon::math;

int main(int argc, const char **argv)
{
    rasty::ConfigReader *reader = new rasty::ConfigReader();
    rapidjson::Document json;
    reader->parseConfigFile("/home/hp/github/research/geofabric/examples/smnp/config/LFBB.json", json);
    rasty::Configuration *config = new rasty::Configuration(json);

    rasty::rastyInit(argc, argv);

    rasty::DataFile *ncFile = new rasty::DataFile();
    ncFile->loadFromFile(config->dataFilename);
    ncFile->loadVariable(config->dataVariable);
    ncFile->loadTimeStep(1);

    std::cout<<"cbar"<<std::endl;
    rasty::Cbar *cbar = new rasty::Cbar(config->colorMap);

    std::cout<<"raster"<<std::endl;
    rasty::Raster *raster = new rasty::Raster(config->geoFilename);

    std::cout<<"renderer"<<std::endl;
    rasty::Renderer *renderer = new rasty::Renderer();
    renderer->setRaster(raster);
    renderer->setCbar(cbar);
    renderer->setData(ncFile);

    std::cout<<"camera"<<std::endl;
    rasty::Camera *camera = new rasty::Camera(config->imageWidth, config->imageHeight); 
    camera->setPosition(config->cameraX,config->cameraY,config->cameraZ);
    camera->setUpVector(config->cameraUpX,config->cameraUpY,config->cameraUpZ);
    camera->setView(0,-1,-1);

    renderer->setCamera(camera);
    renderer->addLight();


    ncFile->loadTimeStep(1);
    renderer->setData(ncFile);
    renderer->renderImage(config->imageFilename);


  ospShutdown();
  return 0;
}



