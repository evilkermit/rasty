// Copyright 2009 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

/* This is a small example tutorial how to use OSPRay in an application.
 *
 * On Linux build it in the build_directory with
 *   g++ ../apps/ospTutorial/ospTutorial.cpp -I ../ospray/include \
 *       -I ../../rkcommon -L . -lospray -Wl,-rpath,. -o ospTutorial
 * On Windows build it in the build_directory\$Configuration with
 *   cl ..\..\apps\ospTutorial\ospTutorial.cpp /EHsc -I ..\..\ospray\include ^
 *      -I ..\.. -I ..\..\..\rkcommon ospray.lib
 * Above commands assume that rkcommon is present in a directory right "next
 * to" the OSPRay directory. If this is not the case, then adjust the include
 * path (alter "-I <path/to/rkcommon>" appropriately).
 */

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
#include "Cbar.h"
#include <rasty.h>
#include <time.h>

#include "ospray/ospray.h"
#include "ospray/ospray_cpp.h"
#include "ospray/ospray_cpp/ext/rkcommon.h"
#include "rkcommon/utility/SaveImage.h"

using namespace rkcommon::math;

int main(int argc, const char **argv)
{

  std::string nc_filename = "../data/LFBB-interp.nc";
  std::string geo_filename = "../data/LFBB.tiff";
  std::string cmap_type = "base";
  rasty::rastyInit(argc, argv);

  rasty::DataFile *ncFile = new rasty::DataFile();
  ncFile->loadFromFile(nc_filename);
  ncFile->loadVariable("Accumulated_Precipitation");
  ncFile->loadTimeStep(1); 

  rasty::Cbar *cbar = new rasty::Cbar(cmap_type);

  rasty::Raster *raster = new rasty::Raster(geo_filename);

  rasty::Renderer *renderer = new rasty::Renderer();
  renderer->setRaster(raster);
  renderer->setCbar(cbar);
  renderer->setData(ncFile);

  rasty::Camera *camera = new rasty::Camera(1024, 1024); // (width, height)
  // camera->setPosition(0,1.5,1.5);
  camera->setPosition(0,.5,.5);
  camera->setView(0,-1,-1);

  renderer->setCamera(camera);
  renderer->addLight();

  std::string outdir = "outfiles/";
  int png_size = std::to_string(ncFile->timeDim).length();
  float avg_time = 0;

  for (int t = 0; t < ncFile->timeDim; t++) {
    std::string png_num = std::to_string(t);
    png_num.insert(png_num.begin(), png_size - png_num.length(), '0');
    std::string png_filename = outdir+"LFBB-"+png_num+".png";

    auto start = std::chrono::high_resolution_clock::now();
    ncFile->loadTimeStep(t);
    renderer->setData(ncFile);
    renderer->renderImage(png_filename);
    
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>( 
            std::chrono::high_resolution_clock::now()-start 
    ).count();
    avg_time = avg_time + (1/(float)(t+1))*(diff - avg_time);

    std::cout <<"["<<t<<"] avg render time = "<<avg_time<<" ms"<< std::endl;

  }

  ospShutdown();
  return 0;
}



