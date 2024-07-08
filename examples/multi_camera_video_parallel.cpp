#include "../include/SpinnakerSDK_SpinCamera.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <thread>

void createVideoFromImages(const std::string&, const std::string&, int);
void captureFrames(SpinCamera&, std::vector<SpinImage>&, int, const std::string&);

namespace fs = std::__fs::filesystem;

int main() {
    // Create folders to store the video frames
    std::string folderName_1 = "Camera_1_parallel_frames";
    std::string folderName_2 = "Camera_2_parallel_frames";
    if (!fs::exists(folderName_1)) {
        fs::create_directory(folderName_1);
    }
    if (!fs::exists(folderName_2)) {
        fs::create_directory(folderName_2);
    }

    // Create camera objects
    SpinCamera camera_1;
    SpinCamera camera_2;

    // Initialize the cameras
    camera_1.Initialize(0);
    camera_2.Initialize(1);

    // Set all settings to default values
    camera_1.SetDefaultSettings();
    camera_2.SetDefaultSettings();

    // Capture all images (video frames)
    int numFrames = 100;
    std::vector<SpinImage> videoFrames_1;
    std::vector<SpinImage> videoFrames_2;

    // Create threads to capture frames simultaneously
    std::thread thread_1(captureFrames, std::ref(camera_1), std::ref(videoFrames_1), numFrames, folderName_1);
    std::thread thread_2(captureFrames, std::ref(camera_2), std::ref(videoFrames_2), numFrames, folderName_2);

    // Wait for both threads to complete
    thread_1.join();
    thread_2.join();

    // Use ffmpeg to create a video from the images
    int fps = 30;
    createVideoFromImages(folderName_1, "../Camera_1_Parallel_Video.mp4", 30);
    createVideoFromImages(folderName_2, "../Camera_2_Parallel_Video.mp4", 30);

    return 0;
}

void captureFrames(SpinCamera& camera, std::vector<SpinImage>& frames, int numFrames, const std::string& folderName) {
    camera.CaptureContinuousFrames(frames, numFrames);
    for (int i = 0; i < frames.size(); ++i) {
        std::ostringstream filename;
        filename << folderName << "/video_frame_" << std::setw(4) << std::setfill('0') << i << ".png";
        frames[i].SaveImage(filename.str());
    }
}

void createVideoFromImages(const std::string& folderPath, const std::string& outputVideoPath, int fps) {
    // Construct the ffmpeg command
    std::string command = "cd " + folderPath + " && ffmpeg -framerate " + std::to_string(fps) + " -i video_frame_%04d.png -c:v libx264 -pix_fmt yuv420p " + outputVideoPath;
    
    // Print the command to verify it's correct
    std::cout << "Running command: " << command << std::endl;

    // Execute the command
    int result = system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Error: ffmpeg command failed with exit code " << result << std::endl;
    } else {
        std::cout << "Video created successfully: " << outputVideoPath << std::endl;
    }
}