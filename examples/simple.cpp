#include "../include/SpinnakerSDKWrapper.h"
#include "../include/SpinnakerSDKNamespace.h"
#include <iostream>

int main() {
    SpinCamera camera;
    if (!camera.Initialize()) {
        std::cout << "Failed to initialize the camera. Exiting with Failure" << std::endl;
        return 1; // Return failure
    }

    camera.SetPixelFormat(SpinOption::PixelFormat::BayerRG8);
    camera.StartAcquisition();
    camera.CaptureImage("example_image.jpg");
    camera.StopAcquisition();
    camera.Shutdown();

    return 0;
}