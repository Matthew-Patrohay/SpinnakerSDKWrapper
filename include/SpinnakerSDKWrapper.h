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
    int SetCameraSettings();
    void SetPixelFormat(SpinOption::PixelFormat);
    void SetBinning(SpinOption::Binning);
    void SetDecimation(SpinOption::Decimation);
    void SetExposureTime(SpinOption::ExposureTime);
    void SetExposureTime(double);
    void SetImageDimensions();
    void SetGainSensitivity();
    void SetGammaCorrection();
    void SetBlackLevel();
    void SetRedBalanceRatio();
    void SetBlueBalanceRatio();

private:
    // Primary Spinnaker-relevant variables
    Spinnaker::CameraPtr pCam;
    Spinnaker::SystemPtr system;
    Spinnaker::CameraList camList;
    Spinnaker::GenApi::INodeMap* nodeMap;
};

#endif // SPINNAKER_SDK_WRAPPER_H