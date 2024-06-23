#include "../include/SpinnakerSDKWrapper.h"
#include <iostream>
#include <unordered_map>

using namespace Spinnaker;
using namespace GenApi;
using namespace std;

SpinnakerSDKWrapper::SpinnakerSDKWrapper() : pCam(nullptr), system(nullptr), nodeMap(nullptr) {}

SpinnakerSDKWrapper::~SpinnakerSDKWrapper() {
    Shutdown();
}

bool SpinnakerSDKWrapper::Initialize() {
    system = System::GetInstance();
    camList = system->GetCameras();
    if (camList.GetSize() == 0) {
        cout << "No cameras found." << endl;
        return false;
    }
    pCam = camList.GetByIndex(0);
    nodeMap = &pCam->GetNodeMap();
    return true;
}

bool SpinnakerSDKWrapper::StartAcquisition() {
    if (!pCam) return false;
    pCam->BeginAcquisition();
    return true;
}

bool SpinnakerSDKWrapper::StopAcquisition() {
    if (!pCam) return false;
    pCam->EndAcquisition();
    return true;
}

bool SpinnakerSDKWrapper::CaptureImage(const std::string& imagePath) {
    if (!pCam) return false;

    try {
        ImagePtr pResultImage = pCam->GetNextImage();
        if (pResultImage->IsIncomplete()) {
            cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << endl;
            return false;
        } else {
            pResultImage->Save(imagePath.c_str());
            cout << "Image saved at " << imagePath << endl;
            pResultImage->Release();
        }
    } catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        return false;
    }
    return true;
}

void SpinnakerSDKWrapper::Shutdown() {
    if (pCam) {
        pCam->DeInit();
        pCam = nullptr;
    }
    camList.Clear();
    if (system) {
        system->ReleaseInstance();
        system = nullptr;
    }
}

int SpinnakerSDKWrapper::SetCameraSettings() {
    if (!nodeMap) return -1;

    SetPixelFormat(PixelFormat::BayerRG8); // Default format for testing
    SetBinning();
    SetDecimation();
    SetExposureTime();
    SetImageDimensions();
    SetGainSensitivity();
    SetGammaCorrection();
    SetBlackLevel();
    SetRedBalanceRatio();
    SetBlueBalanceRatio();

    cout << endl;
    return 0;
}

void SpinnakerSDKWrapper::SetPixelFormat(PixelFormat format) {
    // Ensure nodemap exists
    if (!nodeMap) return;

    // Ensure Pixel Format is available to be written to
    CEnumerationPtr ptrPixelFormat = nodeMap->GetNode("PixelFormat");
    if (!IsAvailable(ptrPixelFormat) || !IsWritable(ptrPixelFormat)) {
        std::cout << "[ WARNING ] Unable to set pixel format (node retrieval)" << std::endl;
        return;
    }

    // Get the format string from the map
    auto option = pixelFormatMap.find(format);
    if (option == pixelFormatMap.end()) {
        std::cout << "[ WARNING ] Invalid pixel format" << std::endl;
        return;
    }
    const std::string& formatStr = option->second;

    // Apply user selected format
    CEnumEntryPtr ptrPixelFormatEntry = ptrPixelFormat->GetEntryByName(formatStr.c_str());
    if (!IsReadable(ptrPixelFormatEntry)) {
        std::cout << "[ WARNING ] Unable to set pixel format (entry retrieval)" << std::endl;
    } else {
        ptrPixelFormat->SetIntValue(ptrPixelFormatEntry->GetValue());
        std::cout << "Pixel format set to " << formatStr << std::endl;
    }
}

void SpinnakerSDKWrapper::SetBinning() {
    // Your implementation for setting binning
}

void SpinnakerSDKWrapper::SetDecimation() {
    // Your implementation for setting decimation
}

void SpinnakerSDKWrapper::SetExposureTime() {
    // Your implementation for setting exposure time
}

void SpinnakerSDKWrapper::SetImageDimensions() {
    // Your implementation for setting image dimensions
}

void SpinnakerSDKWrapper::SetGainSensitivity() {
    // Your implementation for setting gain sensitivity
}

void SpinnakerSDKWrapper::SetGammaCorrection() {
    // Your implementation for setting gamma correction
}

void SpinnakerSDKWrapper::SetBlackLevel() {
    // Your implementation for setting black level
}

void SpinnakerSDKWrapper::SetRedBalanceRatio() {
    // Your implementation for setting red balance ratio
}

void SpinnakerSDKWrapper::SetBlueBalanceRatio() {
    // Your implementation for setting blue balance ratio
}