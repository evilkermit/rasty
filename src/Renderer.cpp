#include "Camera.h"
#include "Renderer.h"
#include "Raster.h"
#include "DataFile.h"
#include "Cbar.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

#include <stdlib.h>
#include <stdio.h>

#include <ospray/ospray.h>
#include <ospray/ospray_util.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"

#include "lodepng/lodepng.h"
#include "CImg.h"

namespace rasty {

Renderer::Renderer() :
    backgroundColor(), samples(1)
{
    //std::cout<<"[Renderer] Init"<<std::endl;

    this->oRenderer = ospNewRenderer("scivis");

    this->setBackgroundColor(0, 0, 0, 0);
    this->oCamera = NULL;
    this->oModel = NULL;
    this->oGroup = NULL;
    this->oInstance = NULL;
    this->oWorld = NULL;
    // this->oSurface = NULL;
    this->oMaterial = NULL;
    this->rasterChanged = true;
    this->lastRasterID = "unset";
    this->lastCameraID = "unset";
    this->worldIsSetup = false;
}

Renderer::~Renderer()
{

    //std::cout<<"[Renderer] Deleting Renderer"<<std::endl;
    // ospRemoveParam(this->oRenderer, "bgColor");
    // ospRemoveParam(this->oRenderer, "spp");
    // ospRemoveParam(this->oRenderer, "lights");
    // ospRemoveParam(this->oRenderer, "aoSamples");
    // ospRemoveParam(this->oRenderer, "shadowsEnabled");
    // ospRemoveParam(this->oRenderer, "oneSidedLighting");
    // ospRemoveParam(this->oRenderer, "model");
    // ospRemoveParam(this->oRenderer, "camera");
    ospRelease(this->oRenderer);
    // ospRelease(this->oFrameBuffer);

    // ospRelease(this->oCamera);

    ospRelease(this->oModel);

    // ospRemoveParam(this->oMaterial, "Kd");
    // ospRemoveParam(this->oMaterial, "Ks");
    // ospRemoveParam(this->oMaterial, "Ns");
    ospRelease(this->oMaterial);

    // ospRemoveParam(this->oSurface, "isovalues");
    // ospRemoveParam(this->oSurface, "volume");
    // ospRelease(this->oSurface);

    for(int light = 0; light < this->lights.size(); light++) {
        ospRemoveParam(this->lights[light], "angularDiameter");
        // ospRemoveParam(this->lights[light], "direction");
        ospRelease(this->lights[light]);
    }

    ospRelease(this->oInstance);
    ospRelease(this->oGroup);
    ospRelease(this->oWorld);
    //std::cout<<"[Renderer] Deleted Renderer"<<std::endl;

}

void Renderer::setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    //std::cout<<"[Renderer] setBackgroundColor"<<std::endl;

    this->backgroundColor[0] = r;
    this->backgroundColor[1] = g;
    this->backgroundColor[2] = b;
    this->backgroundColor[3] = a;
    float asVec[] = {r/(float)255.0, g/(float)255.0, b/(float)255.0, a/(float)255.0};
    ospSetParam(this->oRenderer, "backgroundColor", OSP_VEC4F, asVec);
    ospCommit(this->oRenderer);
}

void Renderer::setBackgroundColor(std::vector<unsigned char> bgColor)
{
    // if the incoming vector is empty, it's probably from the config
    // just set to black instead
    if(bgColor.empty() || bgColor.size() < 3)
        this->setBackgroundColor(0, 0, 0, 0);
    else if (bgColor.size() == 3)
        this->setBackgroundColor(bgColor[0], bgColor[1], bgColor[2], 255);
    else
        this->setBackgroundColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
}

void Renderer::setRaster(Raster *r)
{
    //std::cout<<"[Renderer] setRaster"<<std::endl;

    if(this->lastRasterID == r->ID && this->lastRenderType == "geometry" && !this->rasterChanged) {
        // this is the same volume as the current model and we previously
        // did a volume render
        //std::cout<<"[Renderer] Same Raster as last time, skipping"<<std::endl;
        return;
    }
    if(this->oModel != NULL) {
        ospRelease(this->oModel);
        ospRelease(this->oMaterial);

        this->oModel = NULL;
        this->oMaterial = NULL;
    }
    this->rastyRaster = r;
    this->lastRasterID = r->ID;
    this->lastRenderType = "geometry";

    this->oMaterial = ospNewMaterial("", "obj");
    ospCommit(this->oMaterial);

    this->oModel = ospNewGeometricModel(r->asOSPRayObject());
    ospSetObject(this->oModel, "material", this->oMaterial);
    ospRelease(this->oMaterial);

    ospCommit(this->oModel);
    this->rasterChanged = false;
}

void Renderer::setCbar(Cbar *cbar) {
    this->cbar = cbar;
}


void Renderer::setData(DataFile * dataFile)
{
    // std::cout<<"[Renderer] setData"<<std::endl;
    this->rastyRaster->dataFile->color.clear();
    for (int i = 0; i < dataFile->numValues; i++) {
        this->rastyRaster->dataFile->color.push_back(this->cbar->getColor(dataFile->data[i]));
    }
    this->rastyRaster->setColor();
    this->rasterChanged = true;
}

void Renderer::addLight()
{
    //std::cout<<"[Renderer] addLight"<<std::endl;

    // currently the renderer will hold only one light
    if(this->lights.size() == 0) {
        // create a new directional light
        OSPLight light = ospNewLight("ambient");
        // float direction[] = {0, -1, 1};
        // ospSetVec3f(light, "direction", 0, -1, -1);
        // set the apparent size of the light in degrees
        // 0.53 approximates the Sun
        // ospSetFloat(light, "angularDiameter", 0.53);
        ospCommit(light);
        this->lights.push_back(light);
    }
}


void Renderer::setCamera(Camera *c)
{
    //std::cout<<"[Renderer] setCamera"<<std::endl;

    if(this->lastCameraID == c->ID) {
        // this is the same camera as the current one
        return;
    }
    if(this->oCamera != NULL) {
        //std::cerr << "Camera already set!" << std::endl;
        //return;
        ospRelease(this->oCamera);
        this->oCamera = NULL;
    }

    this->lastCameraID = c->ID;
    this->cameraWidth = c->getImageWidth();
    this->cameraHeight = c->getImageHeight();
    // grab the light direction while we have the pbnj Camera
    this->lightDirection[0] = c->viewX;
    this->lightDirection[1] = c->viewY;
    this->lightDirection[2] = c->viewZ;
    this->oCamera = c->asOSPRayObject();
    this->rastyCamera = c;
}

void Renderer::setSamples(unsigned int spp)
{
    this->samples = spp;
    ospSetInt(this->oRenderer, "spp", spp);
    ospCommit(this->oRenderer);
}

void Renderer::renderImage(std::string imageFilename)
{
    IMAGETYPE imageType = this->getFiletype(imageFilename);
    if(imageType == INVALID) {
        std::cerr << "Invalid image filetype requested!" << std::endl;
        return;
    }

    this->saveImage(imageFilename, imageType);
}

void Renderer::renderToJPGObject(std::vector<unsigned char> &jpg, int quality)
{
    // std::cout << "render to buffer" << std::endl;
    unsigned char *colorBuffer;
    this->renderToBuffer(&colorBuffer);

    // CImg doesn't interlace the channels, we have to work around that 
    // std::cout << "create CImg" << std::endl;
    cimg_library::CImg<unsigned char> img(colorBuffer, 4, this->cameraWidth, 
            this->cameraHeight, 1, false);
    img.permute_axes("yzcx");
    // std::cout << "save jpeg to memory" << std::endl;
    img.save_jpeg_to_memory(jpg, quality);

    // std::cout << "free buffer" << std::endl;
    free(colorBuffer);
}

void Renderer::renderToPNGObject(std::vector<unsigned char> &png)
{
    unsigned char *colorBuffer;
    this->renderToBuffer(&colorBuffer);
    //std::cout<<"[Renderer] renderToPNGObject"<<std::endl;
    unsigned int error = lodepng::encode(png, colorBuffer,
            this->cameraWidth, this->cameraHeight);
    if(error) {
        std::cerr << "ERROR: could not encode PNG, error " << error << ": ";
        std::cerr << lodepng_error_text(error) << std::endl;
    }
    free(colorBuffer);
}

/*
 * Renders the OSPRay buffer to buffer and sets the width and height in 
 * their respective variables.
 */
void Renderer::renderToBuffer(unsigned char **buffer)
{   
    // std::cout<<"[Renderer] renderToBuffer"<<std::endl;
    this->render();


    // std::cout << "get frame buffer" << std::endl;
    int width = this->cameraWidth;
    int height = this->cameraHeight;
    uint32_t *colorBuffer = (uint32_t *)ospMapFrameBuffer(this->oFrameBuffer,
            OSP_FB_COLOR);
    
    *buffer = (unsigned char *) malloc(4 * width * height);
    
    for(int j = 0; j < height; j++) {
        unsigned char *rowIn = (unsigned char*)&colorBuffer[(height-1-j)*width];
        for(int i = 0; i < width; i++) {
            int index = j * width + i;
            // composite rowIn RGB with background color
            float a = rowIn[4*i + 3] / 255.0;
            float r = rowIn[4*i + 0] * a,
                  g = rowIn[4*i + 1] * a,
                  b = rowIn[4*i + 2] * a;
            float abg = this->backgroundColor[3] / 255.0;
            float rbg = this->backgroundColor[0] * abg,
                  gbg = this->backgroundColor[1] * abg,
                  bbg = this->backgroundColor[2] * abg;

            (*buffer)[4*index + 0] = (unsigned char) (r + rbg * (1 - a));
            (*buffer)[4*index + 1] = (unsigned char) (g + gbg * (1 - a));
            (*buffer)[4*index + 2] = (unsigned char) (b + bbg * (1 - a));
            (*buffer)[4*index + 3] = (unsigned char) 255 * (a + abg * (1 - a));
            // //std::cout<< (int)(*buffer)[4*index + 0] << " " << (int)(*buffer)[4*index + 1] << " " << (int)(*buffer)[4*index + 2] << " " << (int)(*buffer)[4*index + 3] << std::endl;
        }
    }

    //std::cout<<"[Renderer] unmap fram buffer"<<std::endl;
    ospUnmapFrameBuffer(colorBuffer, this->oFrameBuffer);
    ospRelease(this->oFrameBuffer);
}

void Renderer::setupWorld() 
{
    //check if everything is ready for rendering
    bool exit = false;
    if(this->oModel == NULL) {
        std::cerr << "No model set to render!" << std::endl;
        exit = true;
    }
    if(this->oCamera == NULL) {
        std::cerr << "No camera set to render with!" << std::endl;
        exit = true;
    }
    if(exit)
        return;

    // std::cout << "[Renderer] creating group (should only see this once)" << std::endl;
    this->oGroup = ospNewGroup();
    ospSetObjectAsData(this->oGroup, "geometry", OSP_GEOMETRIC_MODEL, this->oModel);
    ospCommit(this->oGroup);

    // std::cout << "[Renderer] creating instance (should only see this once)" << std::endl;
    this->oInstance = ospNewInstance(this->oGroup);
    ospSetParam(this->oInstance, "transform", OSP_AFFINE3F, this->rastyRaster->getCenterTransformation());
    ospCommit(this->oInstance);

    // std::cout << "[Renderer] creating world (should only see this once)" << std::endl;
    this->oWorld = ospNewWorld();
    ospSetObjectAsData(this->oWorld, "instance", OSP_INSTANCE, this->oInstance);
    ospCommit(this->oWorld);

    std::cout << "[Renderer] create lights" << std::endl;
    if(this->lights.size() == 1) {

        //if there was a light, set its direction based on the camera
        //and add it to the renderer
        ospSetParam(this->lights[0], "direction", OSP_VEC3F, this->lightDirection);
        ospCommit(this->lights[0]);
        // OSPData lightDataArray = ospNewSharedData(this->lights.data(), OSP_LIGHT, this->lights.size());
        // ospCommit(lightDataArray);
        ospSetObjectAsData(this->oWorld, "light", OSP_LIGHT, this->lights[0]);
        unsigned int aoSamples = std::max(this->samples/8, (unsigned int) 1);
        ospSetInt(this->oRenderer, "aoSamples", aoSamples);
        ospSetBool(this->oRenderer, "shadows", 0);
        ospSetBool(this->oRenderer, "visibleLights", 0);
    }
    std::cout << "[Renderer] commit world" << std::endl;
    ospCommit(this->oWorld);

    std::cout << "[Renderer] set commit renderer" << std::endl;
    ospCommit(this->oRenderer);
    this->worldIsSetup = true;
}


void Renderer::render()
{
    bool exit = false;
    if(this->worldIsSetup == false) {
        std::cerr << "World is not setup!" << std::endl;
        exit = true;
    }

    if(exit)
        return;
    // std::cout << "[Renderer] render" << std::endl;

    //set up framebuffer
    // std::cout << "[Renderer] set up framebuffer" << std::endl;
    this->cameraWidth = this->rastyCamera->getImageWidth();
    this->cameraHeight = this->rastyCamera->getImageHeight();

    //this framebuffer will be released after a single frame
    // std::cout << "[Renderer] create framebuffer" << std::endl;
    this->oFrameBuffer = ospNewFrameBuffer(this->cameraWidth, this->cameraHeight, OSP_FB_SRGBA,
                                           OSP_FB_COLOR | OSP_FB_ACCUM);
    // std::cout << "[Renderer] reset accumulation framebuffer" << std::endl;
    ospResetAccumulation(this->oFrameBuffer);

    // std::cout << "[Renderer] render frame" << std::endl;
    for (int frames = 0; frames < 10; frames++){
        ospRenderFrameBlocking(this->oFrameBuffer, this->oRenderer, this->oCamera, this->oWorld);
    }
//    std::cout << "[Renderer] done rendering" << std::endl;
}

IMAGETYPE Renderer::getFiletype(std::string filename)
{
    std::stringstream ss;
    ss.str(filename);
    std::string token;
    char delim = '.';
    while(std::getline(ss, token, delim)) {
    }

    if(token.compare("ppm") == 0) {
        return PIXMAP;
    }
    else if(token.compare("png") == 0) {
        return PNG;
    }
    else if(token.compare("jpg") == 0 || token.compare("jpeg") == 0) {
        return JPG;
    }
    else {
        return INVALID;
    }
}

void Renderer::saveImage(std::string filename, IMAGETYPE imageType)
{
    if(imageType == PIXMAP)
        this->saveAsPPM(filename);
    else if(imageType == PNG)
        this->saveAsPNG(filename);
    else if (imageType == JPG)
        this->saveAsJPG(filename);
}

void Renderer::saveAsPPM(std::string filename)
{
    this->render();
    int width = this->cameraWidth, height = this->cameraHeight;
    uint32_t *colorBuffer = (uint32_t *)ospMapFrameBuffer(this->oFrameBuffer,
            OSP_FB_COLOR);
    //do a binary file so the PPM isn't quite so large
    FILE *file = fopen(filename.c_str(), "wb");
    unsigned char *rowOut = (unsigned char *)malloc(3*width);
    fprintf(file, "P6\n%i %i\n255\n", width, height);

    //the OSPRay framebuffer uses RGBA, but PPM only supports RGB
    for(int j = 0; j < height; j++) {
        unsigned char *rowIn = (unsigned char*)&colorBuffer[(height-1-j)*width];
        for(int i = 0; i < width; i++) {
            // composite rowIn RGB with background color
            unsigned char r = rowIn[4*i + 0],
                          g = rowIn[4*i + 1],
                          b = rowIn[4*i + 2];
            float a = rowIn[4*i + 3] / 255.0;
            rowOut[3*i + 0] = (unsigned char) r * a + 
                this->backgroundColor[0] * (1.0-a);
            rowOut[3*i + 1] = (unsigned char) g * a + 
                this->backgroundColor[1] * (1.0-a);
            rowOut[3*i + 2] = (unsigned char) b * a + 
                this->backgroundColor[2] * (1.0-a);
        }
        fwrite(rowOut, 3*width, sizeof(char), file);
    }

    fprintf(file, "\n");
    fclose(file);

    //unmap and release so OSPRay will deallocate the memory
    //used by the framebuffer
    ospUnmapFrameBuffer(colorBuffer, this->oFrameBuffer);
    ospRelease(this->oFrameBuffer);
}

void Renderer::saveAsPNG(std::string filename)
{
    std::vector<unsigned char> converted_image;
    this->renderToPNGObject(converted_image);
    //write to file
    lodepng::save_file(converted_image, filename.c_str());
}

void Renderer::saveAsJPG(std::string filename)
{
    unsigned char *colorBuffer;
    this->renderToBuffer(&colorBuffer);

    // CImg doesn't interlace the channels, we have to work around that 
    cimg_library::CImg<unsigned char> img(colorBuffer, 4, this->cameraWidth, 
            this->cameraHeight, 1, false);
    img.permute_axes("yzcx");

    img.save(filename.c_str());
}

}
