#ifndef SPINNAKER_SDK_SPINCAMERA_H
#define SPINNAKER_SDK_SPINCAMERA_H

#include <string>
#include <iostream>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "SpinnakerSDK_SpinOption.h"
#include "SpinnakerSDK_SpinImage.h"

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
    SpinImage CaptureSingleFrame();
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
    void SetImageDimensions(int, int, int, int);
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