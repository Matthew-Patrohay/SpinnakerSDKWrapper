#include "../include/SpinnakerSDK_SpinCamera.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <vector>

void createVideoFromImages(const std::string&, const std::string&, int);
namespace fs = std::__fs::filesystem;

int main() {
    // Create a folder named "video_frames" if it does not exist
    std::string folderName = "video_frames";
    if (!fs::exists(folderName)) {
        fs::create_directory(folderName);
    }

    // Create a camera object
    SpinCamera camera;

    // Initialize the camera (index 0)
    camera.Initialize(0);

    // Set all settings to default values
    camera.SetDefaultSettings();

    // Capture all images (video frames)
    int numFrames = 100;
    std::vector<SpinImage> videoFrames;
    camera.CaptureContinuousFrames(videoFrames, numFrames);

    // Process video frames (e.g., save them to files)
    for (int i = 0; i < videoFrames.size(); ++i) {
        std::ostringstream filename;
        filename << folderName << "/video_frame_" << std::setw(4) << std::setfill('0') << i << ".png";
        videoFrames[i].SaveImage(filename.str());
    }

    // Use ffmpeg to create a video from the images
    int fps = 30;
    std::string folderPath = "./video_frames";
    std::string outputVideoPath = "../simple_video.mp4";
    createVideoFromImages(folderPath, outputVideoPath, fps);

    return 0;
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