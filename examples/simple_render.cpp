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

#include "Raster.h"
#include "Camera.h"
#include "DataFile.h"
#include "Renderer.h"
#include <osp_raster.h>


#include "ospray/ospray_cpp.h"
#include "ospray/ospray_cpp/ext/rkcommon.h"
#include "rkcommon/utility/SaveImage.h"

using namespace rkcommon::math;

int main(int argc, const char **argv)
{

  std::cout<< "Starting OSPRay" << std::endl;
  rasty::rastyInit(argc, argv);
  std::string filename = "../data/LFBB.tiff";
  rasty::Raster *raster = new rasty::Raster(filename);
  rasty::Camera *camera = new rasty::Camera(512, 512); // (width, height)
  camera->setPosition(1,-10,-5);
  // camera->setView(0,-1,0);
  // camera->setPosition(0,0,1200);
  // camera->setView(0,0,-1);
  camera->centerView();
  // camera->setUpVector(0, 0, 1);
  
  rasty::Renderer *renderer = new rasty::Renderer();
  renderer->setRaster(raster);
  // renderer->setBackgroundColor(255,255,255,255);
  renderer->setCamera(camera);
  renderer->addLight();
  renderer->renderImage("LFBB.png");
  std::cout << "fully done" << std::endl;

  // image size
  // vec2i imgSize;
  // imgSize.x = 1024; // width
  // imgSize.y = 1024; // height

  // camera
  // vec3f cam_pos{0.f, 0.f, 0.f};
  // vec3f cam_up{0.f, 0.f, 1.f};
  // vec3f cam_view{-1200.f, -1200.f, -500.f};
  // vec3f cam_pos{500.f, 0.f, 2200.f};
  // vec3f cam_up{0.f, 0.f, 1.f};
  // vec3f cam_view{1200.f, 1200.f, -500.f};




  // initialize OSPRay; OSPRay parses (and removes) its commandline parameters,
  // e.g. "--osp:debug"
  // OSPError init_error = ospInit(&argc, argv);
  // if (init_error != OSP_NO_ERROR)
  //   return init_error;

  // use scoped lifetimes of wrappers to release everything before ospShutdown()
    // create and setup camera
    // ospray::cpp::Camera camera("perspective");
    // // camera.setParam("aspect", imgSize.x / (float)imgSize.y);
    // camera.setParam("position", cam_pos);
    // camera.setParam("direction", cam_view);
    // camera.setParam("up", cam_up);
    // // camera.setParam("fovy", 180);
    // camera.commit(); // commit each object to indicate modifications are done

    // create and setup model and mesh
    // ospray::cpp::Geometry mesh("mesh");
    // mesh.setParam("vertex.position", ospray::cpp::CopiedData(vertex));
    // mesh.setParam("vertex.color", ospray::cpp::CopiedData(color));
    // mesh.setParam("index", ospray::cpp::CopiedData(index));
    // mesh.commit();

    // put the mesh into a model
    // ospray::cpp::GeometricModel model(mesh);
    // ospray::cpp::VolumetricModel model(raster->asOSPRayObject());
    // model.commit();


    // put the model into a group (collection of models)
    // ospray::cpp::Group group;
    // group.setParam("geometry", ospray::cpp::CopiedData(raster->asOSPRayObject()));
    // group.commit();

    // // put the group into an instance (give the group a world transform)
    // ospray::cpp::Instance instance(group);
    // instance.commit();

    // // put the instance in the world
    // ospray::cpp::World world;
    // world.setParam("instance", ospray::cpp::CopiedData(instance));

    // // create and setup light for Ambient Occlusion
    // ospray::cpp::Light light("sunSky");
    // light.commit();

    // world.setParam("light", ospray::cpp::CopiedData(light));
    // world.commit();

    // // create renderer, choose Scientific Visualization renderer
    // ospray::cpp::Renderer renderer("scivis");

    // // complete setup of renderer
    // renderer.setParam("aoSamples", 1);
    // renderer.setParam("backgroundColor", vec4f(135,206,235,0.5)); // white, transparent
    // renderer.commit();

    // create and setup framebuffer
    // ospray::cpp::FrameBuffer framebuffer(
    //     imgSize.x, imgSize.y, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
    // framebuffer.clear();

    // // render one frame
    // framebuffer.renderFrame(renderer->oRenderer, camera->asOSPRayObject(), renderer->oWorld);

    // // access framebuffer and write its content as PPM file
    // uint32_t *fb = (uint32_t *)framebuffer.map(OSP_FB_COLOR);
    // rkcommon::utility::writePPM("firstFrameCpp.ppm", imgSize.x, imgSize.y, fb);
    // framebuffer.unmap(fb);
    // std::cout << "rendering initial frame to firstFrameCpp.ppm" << std::endl;

    // // render 10 more frames, which are accumulated to result in a better
    // // converged image
    // for (int frames = 0; frames < 10; frames++){
    //   framebuffer.renderFrame(renderer->oRenderer, camera->asOSPRayObject(), renderer->oWorld);
    //   std::cout<<frames<<std::endl;
    // }

    // fb = (uint32_t *)framebuffer.map(OSP_FB_COLOR);
    // rkcommon::utility::writePPM(
    //     "accumulatedFrameCpp.ppm", imgSize.x, imgSize.y, fb);
    // framebuffer.unmap(fb);
    // std::cout << "rendering 10 accumulated frames to accumulatedFrameCpp.ppm"
    //           << std::endl;

    // ospray::cpp::PickResult res =
    //     framebuffer.pick(renderer->oRenderer, camera->asOSPRayObject(), renderer->oWorld, 0.5f, 0.5f);

    // if (res.hasHit) {
    //   std::cout << "picked geometry [instance: " << res.instance.handle()
    //             << ", model: " << res.model.handle()
    //             << ", primitive: " << res.primID << "]" << std::endl;
    // }
  std::cout << "osp Shutdown" << std::endl;
  ospShutdown();
  std::cout << "done" << std::endl;
  return 0;
}
