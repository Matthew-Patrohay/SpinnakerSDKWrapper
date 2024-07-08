// Include the Spinnnaker SDK Wrapper header file
#include "../include/SpinnakerSDK_SpinCamera.h"

int main() {
    // Create camera objects
    SpinCamera camera_1;
    SpinCamera camera_2;

    // Initialize the cameras
    camera_1.Initialize(0);
    camera_2.Initialize(1);

    // Set all settings to default values
    camera_1.SetDefaultSettings();
    camera_2.SetDefaultSettings();

    // Capture an image from each camera
        // Note: The two image captures don't occur simultaniously
        // For a parallel-camera example, see multi_camera_video_parallel
    SpinImage image_1 = camera_1.CaptureSingleFrame();
    SpinImage image_2 = camera_2.CaptureSingleFrame();
    image_1.PrintSimpleImageInformation();
    image_2.PrintSimpleImageInformation();

    // Save the images (specify any path and filetype)
    image_1.SaveImage("Camera_1_Photo.png");
    image_2.SaveImage("Camera_2_Photo.png");

    return 0;
}