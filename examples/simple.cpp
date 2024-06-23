#include "../include/SpinnakerSDKWrapper.h"
#include "../include/SpinnakerSDKNamespace.h"
#include <iostream>

using namespace SpinnakerSDKNamespace;

int main() {
    SpinnakerSDKWrapper camera;
    if (camera.Initialize()) {
        camera.SetPixelFormat(PixelFormat::BayerRG8);
        camera.StartAcquisition();
        camera.CaptureImage("example_image.jpg");
        camera.StopAcquisition();
        camera.Shutdown();
    } else {
        std::cout << "Failed to initialize the camera." << std::endl;
    }
    return 0;
}