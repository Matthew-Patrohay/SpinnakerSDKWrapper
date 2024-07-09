// Include the Spinnnaker SDK Wrapper header file
#include "../include/SpinnakerSDK_SpinCamera.h"
#include <atomic>
#include <thread>
#include <iostream>

int main() {
    // Create camera objectss
    SpinCamera camera_1;
    SpinCamera camera_2;

    // Initialize the cameras
    camera_1.Initialize(0);
    camera_2.Initialize(1);

    // Set all settings to default values
    camera_1.SetDefaultSettings();
    camera_2.SetDefaultSettings();

    // Create a trigger variable
    std::atomic<bool> trigger(false);

    // Create a variable to store the captured image
    SpinImage capturedImage_1(nullptr);
    SpinImage capturedImage_2(nullptr);

    // Start a thread for each camera to capture a parallel image on a shared trigger
    std::thread capture_thread_1(&SpinCamera::CaptureSingleFrameOnTrigger, &camera_1, std::ref(capturedImage_1), std::ref(trigger), 10000);
    std::thread capture_thread_2(&SpinCamera::CaptureSingleFrameOnTrigger, &camera_2, std::ref(capturedImage_2), std::ref(trigger), 10000);

    // Simulate some other operations in main program
    for (int i = 5; i > 0; i--) {
        std::cout << "Triggering in: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Set the trigger to true
    trigger.store(true);

    // Wait for the capture threads to complete
    capture_thread_1.join();
    capture_thread_2.join();
    std::cout << "Captured Images" << std::endl;

    // Save the image
    capturedImage_1.SaveImage("Photo_on_Trigger_1.png");
    capturedImage_2.SaveImage("Photo_on_Trigger_2.png");

    return 0;
}