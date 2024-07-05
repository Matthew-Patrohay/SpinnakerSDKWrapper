#include "../include/SpinnakerSDK_SpinCamera.h"
#include "../include/SpinnakerSDK_SpinOption.h"
#include "../include/SpinnakerSDK_SpinImage.h"
#include <iostream>

int main() {
    // Create a camera object
    SpinCamera camera;

    // Initialize the camera (index 0)
    camera.Initialize(0);

    // Set all settings to default values
    camera.SetDefaultSettings();

    // Capture an image
    camera.StartAcquisition();
    SpinImage image = camera.CaptureRawImage();
    camera.StopAcquisition();

    // Demosaic the image
    image.Demosaic();

    // Save the image
    image.SaveToFile("captured_image2.png", Spinnaker::ImageFileFormat::SPINNAKER_IMAGE_FILE_FORMAT_PNG);

    // Shutdown the camera
    camera.Shutdown();

    return 0;
}