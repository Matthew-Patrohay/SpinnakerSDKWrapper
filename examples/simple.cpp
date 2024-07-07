// Include the Spinnnaker SDK Wrapper header file
#include "../include/SpinnakerSDK_SpinCamera.h"

int main() {
    // Create a camera object
    SpinCamera camera;

    // Initialize the camera (index 0)
    camera.Initialize(0);

    // Set all settings to default values
    camera.SetDefaultSettings();

    // Capture an image
    SpinImage image = camera.CaptureRawImage();

    // Save the image (specify any path and filetype)
    image.SaveImage("Simple_image.png");

    return 0;
}