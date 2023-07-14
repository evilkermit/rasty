#ifndef RASTY_RENDERER_H
#define RASTY_RENDERER_H

// disable using an X display
#define cimg_display 0
// force libjpeg usage
#define cimg_use_jpeg 1 

#include <rasty.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"

#include <string>
#include <vector>

#include <ospray/ospray.h>

#include "CImg.h"

namespace rasty {

    enum IMAGETYPE {INVALID, PIXMAP, PNG, JPG};

    class Renderer {
        public:
            Renderer();
            ~Renderer();

            void setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
            void setBackgroundColor(std::vector<unsigned char> bgColor);
            void setRaster(Raster *r);
            void setData(DataFile *dataFile);
            void setCbar(Cbar *cbar);
            void addLight();
            void setupWorld();
            void setCamera(Camera *c);
            void setSamples(unsigned int spp);

            void render();
            void renderToBuffer(unsigned char **buffer);
            void renderToJPGObject(std::vector<unsigned char> &jpg, int quality);
            void renderToPNGObject(std::vector<unsigned char> &png);
            void renderImage(std::string imageFilename);

            unsigned char backgroundColor[4];

            OSPRenderer oRenderer;
            OSPFrameBuffer oFrameBuffer;
            OSPGeometricModel oModel;
            OSPGroup oGroup;
            OSPInstance oInstance;
            OSPTexture oTexture;
            OSPWorld oWorld;


            // OSPModel oModel;
            OSPCamera oCamera;
            // OSPGeometry oSurface;
            OSPMaterial oMaterial;

            Camera * rastyCamera;
            Raster * rastyRaster;
            Cbar * cbar;
 
            int cameraWidth;
            int cameraHeight;

            IMAGETYPE getFiletype(std::string filename);
            void saveImage(std::string filename, IMAGETYPE imageType);

            void saveAsPPM(std::string filename);
            void saveAsPNG(std::string filename);
            void saveAsJPG(std::string filename);
            void bufferToPNG(std::vector<unsigned char> &png);

            std::string lastRasterID;
            std::string lastCameraID;
            std::string lastRenderType;
            std::vector<float> lastIsoValues;

            std::vector<OSPLight> lights;
            float lightDirection[3];
            unsigned int samples;

            bool worldIsSetup;
            bool rasterChanged;

            std::vector<rkcommon::math::vec3f> textureData;
    };
}

#endif
