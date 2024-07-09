// Include the Spinnnaker SDK Wrapper header file
#include "../include/SpinnakerSDK_SpinCamera.h"
#include <atomic>
#include <thread>
#include <iostream>

int main() {
    // Create a camera object
    SpinCamera camera;

    // Initialize the camera (index 0)
    camera.Initialize(0);

    // Set all settings to default values
    camera.SetDefaultSettings();

    // Create a trigger variable
    std::atomic<bool> trigger(false);

    // Create a variable to store the captured image
    SpinImage capturedImage(nullptr);

    // Start a thread to capture an image on trigger
    std::thread capture_thread(&SpinCamera::CaptureSingleFrameOnTrigger, &camera, std::ref(capturedImage), std::ref(trigger), 10000);

    // Simulate some other operations in main program
    for (int i = 5; i > 0; i--) {
        std::cout << "Triggering in: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Set the trigger to true
    trigger.store(true);

    // Wait for the capture thread to complete
    capture_thread.join();
    std::cout << "Captured Image" << std::endl;

    // Save the image
    capturedImage.SaveImage("Photo_on_Trigger.png");

    return 0;
}