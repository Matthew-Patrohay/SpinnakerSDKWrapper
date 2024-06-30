#ifndef SPINNAKER_SDK_WRAPPER_H
#define SPINNAKER_SDK_WRAPPER_H

#include <string>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "SpinnakerSDKNamespace.h"

class SpinCamera {
public:
    SpinCamera();
    ~SpinCamera();

    bool Initialize();
    bool StartAcquisition();
    bool StopAcquisition();
    bool CaptureImage(const std::string& imagePath);
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

#endif // SPINNAKER_SDK_WRAPPER_H