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
#include "ArcballCamera.h"
#include <osp_raster.h>
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
  size_t t = 10;
  int png_size = std::to_string(ncFile->timeDim).length();
  float avg_time = 0;
  int n = 1;

  for (int t = 0; t < ncFile->timeDim; t++) {
    std::string png_num = std::to_string(t);
    png_num.insert(png_num.begin(), png_size - png_num.length(), '0');
    std::string png_filename = outdir+"LFBB-"+png_num+".png";

    auto start = std::chrono::high_resolution_clock::now();
    camera->
    ncFile->loadTimeStep(t);
    renderer->setData(ncFile);
    renderer->renderImage(png_filename);
    
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>( 
            std::chrono::high_resolution_clock::now()-start 
    ).count();
    avg_time = avg_time + (1/n)*(diff - avg_time);

    std::cout <<"["<<t<<"] avg render time = "<<avg_time<<" ms"<< std::endl;

  }
  // renderer->renderImage(png_filename);

  ospShutdown();
  return 0;
}
  // camera->setUpVector(1,0,0);
  
  // camera->setTransform(
  // //   affine3f::translate(vec3f(0, 0, 0))+
  //   raster->getCenterTransformation()
  //   );



/*
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#ifdef _WIN32
#define NOMINMAX
#include <conio.h>
#include <malloc.h>
#include <windows.h>
#else
#include <alloca.h>
#endifand

int main(int argc, const char **argv)
{
  // image size
  vec2i imgSize;
  imgSize.x = 1024; // width
  imgSize.y = 768; // height

  // camera
  vec3f cam_pos{2.5f, 5.f, 5.f};
  vec3f cam_up{0.f, 1.f, 0.f};
  vec3f cam_view{-0.5f, -0.5f, -1.f};
      // vec3f(-1, -1, 1), // 0
      // vec3f(-1, 0, 2), // 1
      // vec3f(-1, 1, 1), // 2
      // vec3f(0, -1, 2), // 3
      // vec3f(0, 0, 3), // 4
      // vec3f(0, 1, 2),  // 5
      // vec3f(1, -1, 1), // 6
      // vec3f(1, 0, 2), // 7
      // vec3f(1, 1, 1)  // 8
      // 
      //       vec3f(-1, 1,-1), // 0
      // vec3f(-1, 2, 0), // 1
      // vec3f(-1, 1, 1), // 2
      // vec3f(-1, 1, 2), // 2
      // vec3f(0, 2, -1), // 3
      // vec3f(0, 3, 0), // 4
      // vec3f(0, 2, 1),  // 5
      // vec3f(0, 2, 2),  // 5
      // vec3f(1, 1, -1), // 6
      // vec3f(1, 2, 0), // 7
      // vec3f(1, 1, 1),  // 8
      // vec3f(1, 1, 2), // 8

  // triangle mesh data
  std::vector<vec3f> vertex = {
      vec3f(-1, 1,-1), // 0
      vec3f(-1, 1, 0), // 1
      vec3f(-1, 1, 1), // 2
      vec3f(-1, 1, 2), // 2
      vec3f(0, 2, -1), // 3
      vec3f(0, 2, 0), // 4
      vec3f(0, 2, 1),  // 5
      vec3f(0, 2, 2),  // 5
      vec3f(1, 1, -1), // 6
      vec3f(1, 1, 0), // 7
      vec3f(1, 1, 1),  // 8
      vec3f(1, 1, 2), // 8
};

  std::vector<vec4f> color = {
    vec4f(0.9f, 0.1f, 0.1f, 1.0f),
    vec4f(0.1f, 0.9f, 0.1f, 1.0f),
    vec4f(0.1f, 0.1f, 0.9f, 1.0f),
    vec4f(0.1f, 0.1f, 0.9f, 1.0f),
    vec4f(0.9f, 0.1f, 0.1f, 1.0f),
    vec4f(0.1f, 0.9f, 0.1f, 1.0f),
    vec4f(0.1f, 0.1f, 0.9f, 1.0f),
    vec4f(0.1f, 0.1f, 0.9f, 1.0f),
    vec4f(0.9f, 0.1f, 0.1f, 1.0f),
    vec4f(0.1f, 0.9f, 0.1f, 1.0f),
    vec4f(0.1f, 0.1f, 0.9f, 1.0f),
    vec4f(0.1f, 0.1f, 0.9f, 1.0f)
  };
  int height = 3;
  int width = 4;
  std::vector<vec3ui> index;
  for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (y+1 < height && x+1 < width) {
                int tl = (y * width) + x;
                int bl = ((y+1) * width) + x;
                int br = ((y+1) * width) + (x+1);
                int tr = (y * width) + (x+1);
                index.push_back(rkcommon::math::vec3ui(tl,bl,br)); // bottom left triangle
                index.push_back(rkcommon::math::vec3ui(tl,br,tr)); // top right triangle
                //std::cout<<rkcommon::math::vec3ui(tl,bl,br)<<std::endl;
                //std::cout<<rkcommon::math::vec3ui(tl,br,tr)<<std::endl;
            }
        }
    }
//   std::vector<vec3ui> index = {
//     vec3ui(0, 1, 4), 
//     vec3ui(0, 3, 4), 
//     vec3ui(1, 2, 5), 
//     vec3ui(1, 4, 5),
//     vec3ui(3, 4, 7),
//     vec3ui(3, 6, 7),
//     vec3ui(4, 5, 8),
//     vec3ui(4, 7, 8)
// } ;  

#ifdef _WIN32
  bool waitForKey = false;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    // detect standalone console: cursor at (0,0)?
    waitForKey = csbi.dwCursorPosition.X == 0 && csbi.dwCursorPosition.Y == 0;
  }
#endif

  // initialize OSPRay; OSPRay parses (and removes) its commandline parameters,
  // e.g. "--osp:debug"
  OSPError init_error = ospInit(&argc, argv);
  if (init_error != OSP_NO_ERROR)
    return init_error;

  // use scoped lifetimes of wrappers to release everything before ospShutdown()
  {
    // create and setup camera
    ospray::cpp::Camera camera("perspective");
    camera.setParam("aspect", imgSize.x / (float)imgSize.y);
    camera.setParam("position", cam_pos);
    camera.setParam("direction", cam_view);
    camera.setParam("up", cam_up);
    camera.commit(); // commit each object to indicate modifications are done

    // create and setup model and mesh
    ospray::cpp::Geometry mesh("mesh");
    mesh.setParam("vertex.position", ospray::cpp::CopiedData(vertex));
    mesh.setParam("vertex.color", ospray::cpp::CopiedData(color));
    mesh.setParam("index", ospray::cpp::CopiedData(index));
    mesh.commit();

    // put the mesh into a model
    ospray::cpp::GeometricModel model(mesh);
    model.commit();

    // put the model into a group (collection of models)
    ospray::cpp::Group group;
    group.setParam("geometry", ospray::cpp::CopiedData(model));
    group.commit();

    // put the group into an instance (give the group a world transform)
    ospray::cpp::Instance instance(group);
    // instance.setParam("transform", ospray::cpp::CopiedData(
    //                                    affine3f::translate(vec3f(-10.f, -1.f, 0.f))));
    instance.commit();

    // put the instance in the world
    ospray::cpp::World world;
    world.setParam("instance", ospray::cpp::CopiedData(instance));

    // create and setup light for Ambient Occlusion
    ospray::cpp::Light light("sunSky");
    light.setParam("direction", vec3f(0.f, -1.f, -1.f));
    light.commit();

    world.setParam("light", ospray::cpp::CopiedData(light));
    world.commit();

    // create renderer, choose Scientific Visualization renderer
    ospray::cpp::Renderer renderer("scivis");

    // complete setup of renderer
    renderer.setParam("aoSamples", 1);
    renderer.setParam("backgroundColor", 1.0f); // white, transparent
    renderer.commit();

    // create and setup framebuffer
    ospray::cpp::FrameBuffer framebuffer(
        imgSize.x, imgSize.y, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
    framebuffer.clear();

    // render one frame
    framebuffer.renderFrame(renderer, camera, world);

    // access framebuffer and write its content as PPM file
    uint32_t *fb = (uint32_t *)framebuffer.map(OSP_FB_COLOR);
    rkcommon::utility::writePPM("firstFrameCpp.ppm", imgSize.x, imgSize.y, fb);
    framebuffer.unmap(fb);
   //std::cout << "rendering initial frame to firstFrameCpp.ppm" << std::endl;

    // render 10 more frames, which are accumulated to result in a better
    // converged image
    for (int frames = 0; frames < 10; frames++)
      framebuffer.renderFrame(renderer, camera, world);

    fb = (uint32_t *)framebuffer.map(OSP_FB_COLOR);
    rkcommon::utility::writePPM(
        "accumulatedFrameCpp.ppm", imgSize.x, imgSize.y, fb);
    framebuffer.unmap(fb);
   //std::cout << "rendering 10 accumulated frames to accumulatedFrameCpp.ppm"
              << std::endl;

    ospray::cpp::PickResult res =
        framebuffer.pick(renderer, camera, world, 0.5f, 0.5f);

    if (res.hasHit) {
     //std::cout << "picked geometry [instance: " << res.instance.handle()
                << ", model: " << res.model.handle()
                << ", primitive: " << res.primID << "]" << std::endl;
    }
  }

  ospShutdown();

#ifdef _WIN32
  if (waitForKey) {
    printf("\n\tpress any key to exit");
    _getch();
  }
#endif

  return 0;
}
*/