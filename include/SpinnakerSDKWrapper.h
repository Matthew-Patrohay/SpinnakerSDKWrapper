#ifndef SPINNAKER_SDK_WRAPPER_H
#define SPINNAKER_SDK_WRAPPER_H

#include <unordered_map>
#include <string>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

class SpinnakerSDKWrapper {
public:
    // Available Pixel Data formats
    enum class PixelFormat {
        BayerRG8,    // 8-bit color data
        BayerRG10p,  // 10-bit color data (packed)
        BayerRG12p,  // 12-bit color data (packed)
        BayerRG16,   // 16-bit color data
        Mono8,       // 8-bit greyscale data
        Mono10p,     // 10-bit greyscale data (packed)
        Mono12p,     // 12-bit greyscale data (packed)
        Mono16       // 16-bit greyscale data
    };

    SpinnakerSDKWrapper();
    ~SpinnakerSDKWrapper();

    bool Initialize();
    bool StartAcquisition();
    bool StopAcquisition();
    bool CaptureImage(const std::string& imagePath);
    void Shutdown();
    int SetCameraSettings();
    void SetPixelFormat(PixelFormat format);
    void SetBinning();
    void SetDecimation();
    void SetExposureTime();
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


    // Maps for any user-options
    std::unordered_map<PixelFormat, std::string> pixelFormatMap = {
        {PixelFormat::BayerRG8,   "BayerRG8"},
        {PixelFormat::BayerRG10p, "BayerRG10p"},
        {PixelFormat::BayerRG12p, "BayerRG12p"},
        {PixelFormat::BayerRG16,  "BayerRG16"},
        {PixelFormat::Mono8,      "Mono8"},
        {PixelFormat::Mono10p,    "Mono10p"},
        {PixelFormat::Mono12p,    "Mono12p"},
        {PixelFormat::Mono16,     "Mono16"}
    };
};

#endif // SPINNAKER_SDK_WRAPPER_H