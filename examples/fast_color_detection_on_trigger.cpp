// Face a camera at a Rubiks Cube face and determine the colors!
// Note: You may need to shine a flashlight at the rubiks cube to let in more light

// Include the Spinnnaker SDK Wrapper header file
#include "../include/SpinnakerSDK_SpinCamera.h"
#include <atomic>
#include <thread>
#include <iostream>
#include <cmath>
#include <iostream>

struct Color {
    std::string name;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

// Function to calculate the Euclidean distance between two colors
double colorDistance(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2) {
    return std::sqrt((r2 - r1) * (r2 - r1) + (g2 - g1) * (g2 - g1) + (b2 - b1) * (b2 - b1));
}

void printClosestColor(unsigned char avgR, unsigned char avgG, unsigned char avgB) {
    // Define the reference colors
    Color colors[] = {
        {"Red", 190, 0, 0},
        {"Orange", 188, 105, 0},
        {"Blue", 0, 0, 180},
        {"Green", 0, 180, 0},
        {"White", 175, 175, 175},
        {"Yellow", 175, 175, 0}
    };

    Color closestColor = colors[0];
    double minDistance = colorDistance(avgR, avgG, avgB, colors[0].r, colors[0].g, colors[0].b);

    // Find the closest color
    for (int i = 1; i < sizeof(colors) / sizeof(colors[0]); i++) {
        double distance = colorDistance(avgR, avgG, avgB, colors[i].r, colors[i].g, colors[i].b);
        if (distance < minDistance) {
            minDistance = distance;
            closestColor = colors[i];
        }
    }

    // Additional check for orange and yellow
    if (closestColor.name == "Orange" || closestColor.name == "Yellow") {
        if (avgR > avgG) {
            closestColor.name = "Orange";
        } else {
            closestColor.name = "Yellow";
        }
    }

    std::cout << "Closest color to (" << (int)avgR << ", " << (int)avgG << ", " << (int)avgB << ") is " << closestColor.name << std::endl;
}

int main() {
    // Create a camera object
    SpinCamera camera;

    // Initialize the camera (index 0)
    camera.Initialize(0);

    // Set all settings to default values
    camera.SetDefaultSettings();
    camera.SetImageDimensions(200, 200);
    camera.SetExposureTime(SpinOption::ExposureTime::Preset_1ms);
    camera.SetGainSensitivity(14);
    camera.SetGammaCorrection(0.6);
    camera.SetBufferHandlingMode(SpinOption::BufferHandlingMode::NewestOnly);
    camera.SetAcquisitionMode(SpinOption::AcquisitionMode::Continuous);

    // Manually start aquisition to save compute time
    camera.StartAcquisition();

    // Create a trigger variable
    std::atomic<bool> trigger(false);

    // Create a variable to store the captured image
    SpinImage capturedImage(nullptr);

    // Start a thread to capture an image on trigger
    std::thread capture_thread(&SpinCamera::CaptureSingleFrameOnTrigger, &camera, std::ref(capturedImage), std::ref(trigger), 500);

    // Simulate some other operations in main program
    for (int i = 5; i > 0; i--) {
        std::cout << "Triggering in: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Set the trigger to true
    trigger.store(true);

    // Start a timer as soon as the trigger goes high
    auto triggerTime = std::chrono::high_resolution_clock::now();

    // Wait for the capture thread to complete
    capture_thread.join();
    std::cout << "Captured Image" << std::endl;

    auto imageTime = std::chrono::high_resolution_clock::now();

    // Calculate the average color of specific regions
    for (int x_loc = 45; x_loc < 200; x_loc=x_loc+55) {
        for (int y_loc = 45; y_loc < 200; y_loc=y_loc+55) {
            int sample_square_size = 5;
            unsigned char avgR, avgG, avgB;
            capturedImage.CalculateAverageColor(x_loc, y_loc, sample_square_size, sample_square_size, avgR, avgG, avgB);
            printClosestColor(avgR, avgG, avgB);
            capturedImage.DrawRedSquare(x_loc, y_loc, sample_square_size);
        }
    }

    // Capture the time point right after the colors are processed
    auto computeDone = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(imageTime - triggerTime).count();
    std::cout << "Duration from trigger to image capture: " << duration << " us (" << duration/1000 << "ms)" << std::endl;
    duration = std::chrono::duration_cast<std::chrono::microseconds>(computeDone - triggerTime).count();
    std::cout << "Duration from trigger to compute done: " << duration << " us (" << duration/1000 << "ms)" << std::endl;

    // End aquisition
    camera.StopAcquisition();

    // Save the image
    capturedImage.SaveImage("Color_Detection_on_Trigger.png");

    return 0;
}