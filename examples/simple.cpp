#include "../include/SpinnakerSDKWrapper.h"
#include "../include/SpinnakerSDKNamespace.h"
#include <iostream>

int main() {
    // Create a camera object
    SpinCamera camera;

    // Initialize the camera (index 0)
    camera.Initialize(0);

    // Set all settings to default
    camera.SetDefaultSettings();

    // Capture an image
    camera.StartAcquisition();
    Spinnaker::ImagePtr image = camera.CaptureRawImage();
    camera.StopAcquisition();

    return 0;
}