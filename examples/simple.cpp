#include "../include/SpinnakerSDKWrapper.h"
#include <iostream>

int main() {
    SpinnakerSDKWrapper camera;
    if (camera.Initialize()) {
        camera.SetPixelFormat(SpinnakerSDKWrapper::PixelFormat::BayerRG8); // Correct usage
        camera.StartAcquisition();
        camera.CaptureImage("example_image.jpg");
        camera.StopAcquisition();
        camera.Shutdown();
    } else {
        std::cout << "Failed to initialize the camera." << std::endl;
    }
    return 0;
}