#ifndef RASTY_CAMERA_H
#define RASTY_CAMERA_H

#include <rasty.h>
#include <Raster.h>

#include <ospray/ospray.h>
#include "ospray/ospray_cpp/ext/rkcommon.h"

#include <string>

namespace rasty {

    class Camera {
        public:
            Camera(int width, int height);
            ~Camera();

            void setPosition(float x, float y, float z);
            void setUpVector(float x, float y, float z);
            void setOrbitRadius(float radius);
            void setView(float x, float y, float z);
            void setImageSize(int width, int height);
            void setTransform(rkcommon::math::affine3f transform);
            int getImageWidth();
            int getImageHeight();
            // no longer needed as the raster is centered automatically
            void centerView();

            void setRegion(float top, float right, float bottom, float left);

            OSPCamera asOSPRayObject();

            //some sort of setPath function that takes an enum type for the path
            //and a ... for path parameters

            float viewX;
            float viewY;
            float viewZ;

            std::string ID;

        // private:
            float xPos;
            float yPos;
            float zPos;
            float upX;
            float upY;
            float upZ;
            float orbitRadius;
            int imageWidth;
            int imageHeight;
            rkcommon::math::affine3f transform;
            OSPCamera oCamera;

            void updateOSPRayPosition();
    };
}

#endif
