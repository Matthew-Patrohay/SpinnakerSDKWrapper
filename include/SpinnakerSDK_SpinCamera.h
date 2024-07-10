#ifndef SPINNAKER_SDK_SPINCAMERA_H
#define SPINNAKER_SDK_SPINCAMERA_H

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "SpinnakerSDK_SpinOption.h"
#include "SpinnakerSDK_SpinImage.h"
#include <string>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <unordered_map>

class SpinCamera {
public:
    SpinCamera();
    ~SpinCamera();

    // Camera setup and information
    void Initialize(int camera_index);
    void Shutdown();
    void SetDefaultSettings();
    void SetAutoSettings();
    void PrintSettings();

    // Aquisition and Capture
    void StartAcquisition();
    void StopAcquisition();
    void CaptureSingleFrame(SpinImage&);
    void CaptureSingleFrameOnTrigger(SpinImage&, std::atomic<bool>&, int);
    void CaptureContinuousFrames(std::vector<SpinImage>&, int);
    

    // Setting Camera Settings
    void SetAcquisitionMode(SpinOption::AcquisitionMode);
    void SetBufferHandlingMode(SpinOption::BufferHandlingMode);
    void SetPixelFormat(SpinOption::PixelFormat);
    void SetBinning(SpinOption::Binning);
    void SetDecimation(SpinOption::Decimation);
    void SetExposureTime(SpinOption::ExposureTime);
    void SetExposureTime(double); 
    void SetImageDimensions(SpinOption::ImageDimensions);
    void SetImageDimensions(int user_width, int user_height, int user_width_offset=-1, int user_height_offset=-1);
    void SetGainSensitivity(SpinOption::GainSensitivity);
    void SetGainSensitivity(float);
    void SetGammaCorrection(SpinOption::GammaCorrection);
    void SetGammaCorrection(float);
    void SetBlackLevel(SpinOption::BlackLevel);
    void SetBlackLevel(float);
    void SetRedBalanceRatio(SpinOption::RedBalanceRatio);
    void SetRedBalanceRatio(float);
    void SetBlueBalanceRatio(SpinOption::BlueBalanceRatio);
    void SetBlueBalanceRatio(float);

private:
    // Primary Spinnaker-relevant variables
    Spinnaker::CameraPtr pCam;
    Spinnaker::SystemPtr system;
    Spinnaker::CameraList camList;
    Spinnaker::GenApi::INodeMap* nodeMap;
    Spinnaker::GenApi::INodeMap* streamNodeMap;

    // Status for if the camera aquisition is currently active
    bool acquisitionActive = false;
};

#endif // SPINNAKER_SDK_SPINCAMERA_H