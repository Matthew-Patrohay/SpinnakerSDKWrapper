#include "../include/SpinnakerSDKWrapper.h"
#include <iostream>
#include <unordered_map>

using namespace Spinnaker;
using namespace GenApi;

SpinCamera::SpinCamera() : pCam(nullptr), system(nullptr), nodeMap(nullptr) {}

SpinCamera::~SpinCamera() {
    Shutdown();
}

bool SpinCamera::Initialize() {
    system = System::GetInstance();
    camList = system->GetCameras();
    if (camList.GetSize() == 0) {
        std::cout << "No cameras found." << std::endl;
        return false;
    }
    pCam = camList.GetByIndex(0);
    nodeMap = &pCam->GetNodeMap();
    return true;
}

bool SpinCamera::StartAcquisition() {
    if (!pCam) return false;
    pCam->BeginAcquisition();
    return true;
}

bool SpinCamera::StopAcquisition() {
    if (!pCam) return false;
    pCam->EndAcquisition();
    return true;
}

bool SpinCamera::CaptureImage(const std::string& imagePath) {
    if (!pCam) return false;

    try {
        ImagePtr pResultImage = pCam->GetNextImage();
        if (pResultImage->IsIncomplete()) {
            std::cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << std::endl;
            return false;
        } else {
            pResultImage->Save(imagePath.c_str());
            std::cout << "Image saved at " << imagePath << std::endl;
            pResultImage->Release();
        }
    } catch (Spinnaker::Exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void SpinCamera::Shutdown() {
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

int SpinCamera::SetCameraSettings() {
    if (!nodeMap) return -1;

    SetPixelFormat(SpinOption::PixelFormat::BayerRG8);   // Default format for testing
    SetBinning(SpinOption::Binning::NoBinning);          // Default to no binning
    SetDecimation(SpinOption::Decimation::NoDecimation); // Default to no decimation
    SetExposureTime(SpinOption::ExposureTime::Shutter_1_1000);
    SetImageDimensions();
    SetGainSensitivity();
    SetGammaCorrection();
    SetBlackLevel();
    SetRedBalanceRatio();
    SetBlueBalanceRatio();

    std::cout << std::endl;
    return 0;
}


void SpinCamera::SetPixelFormat(SpinOption::PixelFormat format) {

    // All legal options
    const std::unordered_map<SpinOption::PixelFormat, std::string> PixelFormat_legal = {
        {SpinOption::PixelFormat::BayerRG8,   "BayerRG8"},
        {SpinOption::PixelFormat::BayerRG10p, "BayerRG10p"},
        {SpinOption::PixelFormat::BayerRG12p, "BayerRG12p"},
        {SpinOption::PixelFormat::BayerRG16,  "BayerRG16"},
        {SpinOption::PixelFormat::Mono8,      "Mono8"},
        {SpinOption::PixelFormat::Mono10p,    "Mono10p"},
        {SpinOption::PixelFormat::Mono12p,    "Mono12p"},
        {SpinOption::PixelFormat::Mono16,     "Mono16"}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure Pixel Format is available to be written to
    CEnumerationPtr ptrPixelFormat = nodeMap->GetNode("PixelFormat");
    if (!IsAvailable(ptrPixelFormat) || !IsWritable(ptrPixelFormat)) {
        std::cout << "[ WARNING ] Unable to set pixel format (node retrieval)." << std::endl;
        return;
    }

    // Get the format string from the map
    auto option = PixelFormat_legal.find(format);
    if (option == PixelFormat_legal.end()) {
        std::cout << "[ WARNING ] Invalid pixel format." << std::endl;
        return;
    }
    const std::string& formatStr = option->second;

    // Apply user selected format
    CEnumEntryPtr ptrPixelFormatEntry = ptrPixelFormat->GetEntryByName(formatStr.c_str());
    if (!IsReadable(ptrPixelFormatEntry)) {
        std::cout << "[ WARNING ] Unable to set pixel format (entry retrieval)." << std::endl;
    } else {
        try {
            ptrPixelFormat->SetIntValue(ptrPixelFormatEntry->GetValue());
            std::cout << "Pixel format set to " << formatStr << std::endl;
        } catch (const Spinnaker::Exception& e) {
            std::cout << "[ ERROR ] Exception caught while setting pixel format: " << e.what() << std::endl;
        }
    }
}

void SpinCamera::SetBinning(SpinOption::Binning user_option) {

    // All legal options
    const std::unordered_map<SpinOption::Binning, int> Binning_legal = {
        {SpinOption::Binning::NoBinning,         1},
        {SpinOption::Binning::TwoByTwoBinning,   2},
        {SpinOption::Binning::FourByFourBinning, 4},
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected binning value from the map
    auto option = Binning_legal.find(user_option);
    if (option == Binning_legal.end()) {
        std::cout << "[ WARNING ] Invalid pixel format." << std::endl;
        return;
    }
    const int& selected_value = option->second;

    // Apply user-selected value
    CIntegerPtr ptrBinningHorizontalValue = nodeMap->GetNode("BinningHorizontal");
    CIntegerPtr ptrBinningVerticalValue = nodeMap->GetNode("BinningVertical");
    if (IsAvailable(ptrBinningHorizontalValue) && IsWritable(ptrBinningHorizontalValue)) {
        ptrBinningHorizontalValue->SetValue(selected_value);
        std::cout << "Binning horizontal set to: " << selected_value << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to set BinningHorizontal (node retrieval)." << std::endl;
    }
    if (IsAvailable(ptrBinningVerticalValue) && IsWritable(ptrBinningVerticalValue)) {
        ptrBinningVerticalValue->SetValue(selected_value);
        std::cout << "Binning vertical set to: " << selected_value << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to set BinningVertical (node retrieval)." << std::endl;
    }
}

void SpinCamera::SetDecimation(SpinOption::Decimation user_option) {

    // All legal options
    const std::unordered_map<SpinOption::Decimation, int> Decimation_legal = {
        {SpinOption::Decimation::NoDecimation,       1},
        {SpinOption::Decimation::TwoByTwoDecimation, 2},
        {SpinOption::Decimation::FourByFourDecimation, 4},
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected decimation value from the map
    auto option = Decimation_legal.find(user_option);
    if (option == Decimation_legal.end()) {
        std::cout << "[ WARNING ] Invalid decimation option." << std::endl;
        return;
    }
    const int& decimation = option->second;

    // Apply user-selected value
    CIntegerPtr ptrDecimationHorizontal = nodeMap->GetNode("DecimationHorizontal");
    CIntegerPtr ptrDecimationVertical = nodeMap->GetNode("DecimationVertical");
    if (IsAvailable(ptrDecimationHorizontal) && IsWritable(ptrDecimationHorizontal)) {
        ptrDecimationHorizontal->SetValue(decimation);
        std::cout << "Decimation horizontal set to: " << decimation << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to set DecimationHorizontal (node retrieval)." << std::endl;
    }
    if (IsAvailable(ptrDecimationVertical) && IsWritable(ptrDecimationVertical)) {
        ptrDecimationVertical->SetValue(decimation);
        std::cout << "Decimation vertical set to: " << decimation << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to set DecimationVertical (node retrieval)." << std::endl;
    }
}

void SpinCamera::SetExposureTime(SpinOption::ExposureTime user_option) {
    // All legal options
    const std::unordered_map<SpinOption::ExposureTime, double> ExposureTime_legal = {
        {SpinOption::ExposureTime::Auto,           0},
        {SpinOption::ExposureTime::MinimumValue,   INT_MIN},
        {SpinOption::ExposureTime::MaximumValue,   INT_MAX},
        {SpinOption::ExposureTime::Preset_10us,    10},
        {SpinOption::ExposureTime::Preset_20us,    20},
        {SpinOption::ExposureTime::Preset_50us,    50},
        {SpinOption::ExposureTime::Preset_100us,   100},
        {SpinOption::ExposureTime::Preset_200us,   200},
        {SpinOption::ExposureTime::Preset_500us,   500},
        {SpinOption::ExposureTime::Preset_1ms,     1000},
        {SpinOption::ExposureTime::Preset_2ms,     2000},
        {SpinOption::ExposureTime::Preset_5ms,     5000},
        {SpinOption::ExposureTime::Preset_10ms,    10000},
        {SpinOption::ExposureTime::Preset_20ms,    20000},
        {SpinOption::ExposureTime::Preset_50ms,    50000},
        {SpinOption::ExposureTime::Preset_100ms,   100000},
        {SpinOption::ExposureTime::Preset_200ms,   200000},
        {SpinOption::ExposureTime::Preset_500ms,   500000},
        {SpinOption::ExposureTime::Preset_1s,      1000000},
        {SpinOption::ExposureTime::Preset_2s,      2000000},
        {SpinOption::ExposureTime::Preset_5s,      5000000},
        {SpinOption::ExposureTime::Preset_10s,     10000000},
        {SpinOption::ExposureTime::Preset_20s,     20000000},
        {SpinOption::ExposureTime::Shutter_1_1000, 1000},
        {SpinOption::ExposureTime::Shutter_1_500,  2000},
        {SpinOption::ExposureTime::Shutter_1_250,  4000},
        {SpinOption::ExposureTime::Shutter_1_125,  8000},
        {SpinOption::ExposureTime::Shutter_1_60,   16667},
        {SpinOption::ExposureTime::Shutter_1_30,   33333},
        {SpinOption::ExposureTime::Shutter_1_15,   66667},
        {SpinOption::ExposureTime::Shutter_1_8,    125000},
        {SpinOption::ExposureTime::Shutter_1_4,    250000},
        {SpinOption::ExposureTime::Shutter_1_2,    500000},
        {SpinOption::ExposureTime::Shutter_1_1,    1000000}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected exposure time value from the map
    auto option = ExposureTime_legal.find(user_option);
    if (option == ExposureTime_legal.end()) {
        std::cout << "[ WARNING ] Invalid exposure time option." << std::endl;
        return;
    }
    const double& exposureTime = option->second;

    // If auto mode is selected, then set to auto mode and return, otherwise, make sure its manual mode
    CEnumerationPtr ptrExposureAuto = nodeMap->GetNode("ExposureAuto");
    if (!IsReadable(ptrExposureAuto) || !IsWritable(ptrExposureAuto)) {
        std::cout << "[ WARNING ] Unable to set exposure" << std::endl;
    }
    if (user_option == SpinOption::ExposureTime::Auto) {
        CEnumEntryPtr ptrExposureAutoOn = ptrExposureAuto->GetEntryByName("On");
        if (IsReadable(ptrExposureAutoOn)) {
            ptrExposureAuto->SetIntValue(ptrExposureAutoOn->GetValue());
            std::cout << "Auto Exposure Enabled" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to enable automatic exposure" << std::endl;
        }
        return;
    } else {
        CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
        if (IsReadable(ptrExposureAutoOff)) {
            ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
            std::cout << "Manual Exposure Enabled (Automatic exposure disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to enable manual exposure (disable automatic exposure)" << std::endl;
        }
    }

    // Apply user-selected exposure time
    CFloatPtr ptrExposureTime = nodeMap->GetNode("ExposureTime");
    if (IsAvailable(ptrExposureTime) && IsWritable(ptrExposureTime)) {
        double finalExposureTime = exposureTime;

        // Ensure exposure time is within allowable range
        const double exposureTimeMax = ptrExposureTime->GetMax();
        const double exposureTimeMin = ptrExposureTime->GetMin();
        if (exposureTime > exposureTimeMax) {
            finalExposureTime = exposureTimeMax;
            std::cout << "[ NOTE ] Selected exposure time exceeds maximum, setting to max allowable: " << finalExposureTime << " microseconds." << std::endl;
        } else if (exposureTime < exposureTimeMin) {
            finalExposureTime = exposureTimeMin;
            std::cout << "[ NOTE ] Selected exposure time is below minimum, setting to min allowable: " << finalExposureTime << " microseconds." << std::endl;
        } else {
            finalExposureTime = exposureTime;
        }

        // Perform the actual value set
        ptrExposureTime->SetValue(finalExposureTime);
        std::cout << "Exposure time set to " << finalExposureTime << " microseconds." << std::endl;
    } else {
        std::cout << "[ WARNING ] Exposure time setting not available." << std::endl;
    }
}

void SpinCamera::SetExposureTime(double user_exposure_time) {
    
    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure automatic exposure is off to allow manual setting
    CEnumerationPtr ptrExposureAuto = nodeMap->GetNode("ExposureAuto");
    if (IsReadable(ptrExposureAuto) && IsWritable(ptrExposureAuto)) {
        CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
        if (IsReadable(ptrExposureAutoOff)) {
            ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
            std::cout << "Manual Exposure Enabled (Automatic exposure disabled)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to disable automatic exposure" << std::endl;
        return;
    }
    
    // Apply user-selected exposure time
    CFloatPtr ptrExposureTime = nodeMap->GetNode("ExposureTime");
    if (IsAvailable(ptrExposureTime) && IsWritable(ptrExposureTime)) {
        const double exposureTimeMax = ptrExposureTime->GetMax();
        const double exposureTimeMin = ptrExposureTime->GetMin();
        if (user_exposure_time > exposureTimeMax) {
            user_exposure_time = exposureTimeMax;
            std::cout << "[ NOTE ] Provided exposure time exceeds maximum limit, setting to max value of " << exposureTimeMax << " microseconds." << std::endl;
        }
        if (user_exposure_time < exposureTimeMin) {
            user_exposure_time = exposureTimeMin;
            std::cout << "[ NOTE ] Provided exposure time exceeds minimum limit, setting to min value of " << exposureTimeMin << " microseconds." << std::endl;
        }
        ptrExposureTime->SetValue(user_exposure_time);
        std::cout << "Exposure time set to " << user_exposure_time << " microseconds." << std::endl;
    } else {
        std::cout << "[ WARNING ] Exposure time setting not available." << std::endl;
    }
}

void SpinCamera::SetImageDimensions() {
    // Your implementation for setting image dimensions
}

void SpinCamera::SetGainSensitivity() {
    // Your implementation for setting gain sensitivity
}

void SpinCamera::SetGammaCorrection() {
    // Your implementation for setting gamma correction
}

void SpinCamera::SetBlackLevel() {
    // Your implementation for setting black level
}

void SpinCamera::SetRedBalanceRatio() {
    // Your implementation for setting red balance ratio
}

void SpinCamera::SetBlueBalanceRatio() {
    // Your implementation for setting blue balance ratio
}