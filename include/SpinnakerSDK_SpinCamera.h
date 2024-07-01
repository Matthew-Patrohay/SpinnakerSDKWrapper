#ifndef SPINNAKER_SDK_SPINCAMERA_H
#define SPINNAKER_SDK_SPINCAMERA_H

#include <string>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "SpinnakerSDK_SpinOption.h"

class SpinCamera {
public:
    SpinCamera();
    ~SpinCamera();

    void Initialize(int camera_index);
    void StartAcquisition();
    void StopAcquisition();
    Spinnaker::ImagePtr CaptureRawImage();
    void Shutdown();
    void SetDefaultSettings();
    void SetPixelFormat(SpinOption::PixelFormat);
    void SetBinning(SpinOption::Binning);
    void SetDecimation(SpinOption::Decimation);
    void SetExposureTime(SpinOption::ExposureTime);
    void SetExposureTime(double); 
    void SetImageDimensions(SpinOption::ImageDimensions);  // Standard (centered) options
    void SetImageDimensions(int, int, int, int);           // Custom image positioning
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
};

#endif // SPINNAKER_SDK_SPINCAMERA_H