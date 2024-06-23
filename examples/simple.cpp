#include "../include/SpinnakerSDKWrapper.h"
#include "../include/SpinnakerSDKNamespace.h"
#include <iostream>

int main() {
    SpinCamera camera;
    if (camera.Initialize()) {
        camera.SetPixelFormat(SpinOption::PixelFormat::BayerRG8);
        camera.StartAcquisition();
        camera.CaptureImage("example_image.jpg");
        camera.StopAcquisition();
        camera.Shutdown();
    } else {
        std::cout << "Failed to initialize the camera." << std::endl;
    }
    return 0;
}