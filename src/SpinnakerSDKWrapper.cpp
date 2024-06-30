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

void SpinCamera::SetDefaultSettings() {
    SetPixelFormat(SpinOption::PixelFormat::BayerRG8);
    SetBinning(SpinOption::Binning::NoBinning);
    SetDecimation(SpinOption::Decimation::NoDecimation);
    SetExposureTime(SpinOption::ExposureTime::Shutter_1_1000);
    SetImageDimensions(SpinOption::ImageDimensions::Preset_1440x1080);
    SetGainSensitivity(SpinOption::GainSensitivity::Preset_18dB);
    SetGammaCorrection(SpinOption::GammaCorrection::Preset_1_00);
    SetBlackLevel(SpinOption::BlackLevel::Preset_0_50);
    SetRedBalanceRatio(SpinOption::RedBalanceRatio::Preset_1_00);
    SetBlueBalanceRatio(SpinOption::BlueBalanceRatio::Preset_1_00);
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

    // Set exposure mode to auto or manual based on user option
    CEnumerationPtr ptrExposureAuto = nodeMap->GetNode("ExposureAuto");
    if (!IsReadable(ptrExposureAuto) || !IsWritable(ptrExposureAuto)) {
        std::cout << "[ WARNING ] Unable to set exposure mode" << std::endl;
        return;
    }

    if (user_option == SpinOption::ExposureTime::Auto) {
        // Attempt to enable automatic exposure
        CEnumEntryPtr ptrExposureAutoOn = ptrExposureAuto->GetEntryByName("Continuous");
        if (IsReadable(ptrExposureAutoOn) && IsWritable(ptrExposureAutoOn)) {
            ptrExposureAuto->SetIntValue(ptrExposureAutoOn->GetValue());
            std::cout << "Auto Exposure Enabled" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to enable automatic exposure" << std::endl;
        }
        return;
    } else {
        // Attempt to disable automatic exposure
        CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
        if (IsReadable(ptrExposureAutoOff) && IsWritable(ptrExposureAutoOff)) {
            ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
            std::cout << "Manual Exposure Enabled (Automatic exposure disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic exposure (enable manual exposure)" << std::endl;
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

void SpinCamera::SetImageDimensions(SpinOption::ImageDimensions user_option) {
    // All legal options
    const std::unordered_map<SpinOption::ImageDimensions, std::pair<int, int>> ImageDimensions_legal = {
        {SpinOption::ImageDimensions::Preset_1440x1080, {1440, 1080}},
        {SpinOption::ImageDimensions::Preset_1080x1440, {1080, 1440}},
        {SpinOption::ImageDimensions::Preset_1280x960,  {1280, 960}},
        {SpinOption::ImageDimensions::Preset_960x1280,  {960, 1280}},
        {SpinOption::ImageDimensions::Preset_720x540,   {720, 540}},
        {SpinOption::ImageDimensions::Preset_540x720,   {540, 720}},
        {SpinOption::ImageDimensions::Preset_640x480,   {640, 480}},
        {SpinOption::ImageDimensions::Preset_480x640,   {480, 640}},
        {SpinOption::ImageDimensions::Preset_320x240,   {320, 240}},
        {SpinOption::ImageDimensions::Preset_240x320,   {240, 320}}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected image dimensions from the map
    auto option = ImageDimensions_legal.find(user_option);
    if (option == ImageDimensions_legal.end()) {
        std::cout << "[ WARNING ] Invalid image dimensions option." << std::endl;
        return;
    }
    const std::pair<int, int>& dimensions = option->second;

    // Get width and height
    const int& width = dimensions.first;
    const int& height = dimensions.second;
    int finalWidth = width;
    int finalHeight = height;

    // Apply user-selected width
    CIntegerPtr ptrWidth = nodeMap->GetNode("Width");
    if (IsAvailable(ptrWidth) && IsWritable(ptrWidth)) {
        const int widthMax = static_cast<int>(ptrWidth->GetMax());
        const int widthMin = static_cast<int>(ptrWidth->GetMin());

        if (width > widthMax) {
            finalWidth = widthMax;
            std::cout << "[ NOTE ] Selected width exceeds maximum, setting to max allowable: " << finalWidth << "." << std::endl;
        } else if (width < widthMin) {
            finalWidth = widthMin;
            std::cout << "[ NOTE ] Selected width is below minimum, setting to min allowable: " << finalWidth << "." << std::endl;
        }

        ptrWidth->SetValue(finalWidth);
        std::cout << "Width set to " << finalWidth << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Width setting not available." << std::endl;
    }

    // Apply user-selected height
    CIntegerPtr ptrHeight = nodeMap->GetNode("Height");
    if (IsAvailable(ptrHeight) && IsWritable(ptrHeight)) {
        const int heightMax = static_cast<int>(ptrHeight->GetMax());
        const int heightMin = static_cast<int>(ptrHeight->GetMin());

        if (height > heightMax) {
            finalHeight = heightMax;
            std::cout << "[ NOTE ] Selected height exceeds maximum, setting to max allowable: " << finalHeight << "." << std::endl;
        } else if (height < heightMin) {
            finalHeight = heightMin;
            std::cout << "[ NOTE ] Selected height is below minimum, setting to min allowable: " << finalHeight << "." << std::endl;
        }

        ptrHeight->SetValue(finalHeight);
        std::cout << "Height set to " << finalHeight << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Height setting not available." << std::endl;
    }

    // Calculate and set width offset
    CIntegerPtr ptrWidthOffset = nodeMap->GetNode("OffsetX");
    if (IsAvailable(ptrWidthOffset) && IsWritable(ptrWidthOffset)) {
        int maxWidth = static_cast<int>(ptrWidth->GetMax());
        int offsetX = (maxWidth - finalWidth) / 2;

        ptrWidthOffset->SetValue(offsetX);
        std::cout << "Width offset set to " << offsetX << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Width offset setting not available." << std::endl;
    }

    // Calculate and set height offset
    CIntegerPtr ptrHeightOffset = nodeMap->GetNode("OffsetY");
    if (IsAvailable(ptrHeightOffset) && IsWritable(ptrHeightOffset)) {
        int maxHeight = static_cast<int>(ptrHeight->GetMax());
        int offsetY = (maxHeight - finalHeight) / 2;

        ptrHeightOffset->SetValue(offsetY);
        std::cout << "Height offset set to " << offsetY << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Height offset setting not available." << std::endl;
    }
}

void SpinCamera::SetImageDimensions(int user_width, int user_height, int user_width_offset, int user_height_offset) {
    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get width and height
    const int& width = user_width;
    const int& height = user_height;
    int finalWidth = width;
    int finalHeight = height;

    // Apply user-selected width
    CIntegerPtr ptrWidth = nodeMap->GetNode("Width");
    if (IsAvailable(ptrWidth) && IsWritable(ptrWidth)) {
        const int widthMax = static_cast<int>(ptrWidth->GetMax());
        const int widthMin = static_cast<int>(ptrWidth->GetMin());

        if (width > widthMax) {
            finalWidth = widthMax;
            std::cout << "[ NOTE ] Selected width exceeds maximum, setting to max allowable: " << finalWidth << "." << std::endl;
        } else if (width < widthMin) {
            finalWidth = widthMin;
            std::cout << "[ NOTE ] Selected width is below minimum, setting to min allowable: " << finalWidth << "." << std::endl;
        }

        ptrWidth->SetValue(finalWidth);
        std::cout << "Width set to " << finalWidth << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Width setting not available." << std::endl;
    }

    // Apply user-selected height
    CIntegerPtr ptrHeight = nodeMap->GetNode("Height");
    if (IsAvailable(ptrHeight) && IsWritable(ptrHeight)) {
        const int heightMax = static_cast<int>(ptrHeight->GetMax());
        const int heightMin = static_cast<int>(ptrHeight->GetMin());

        if (height > heightMax) {
            finalHeight = heightMax;
            std::cout << "[ NOTE ] Selected height exceeds maximum, setting to max allowable: " << finalHeight << "." << std::endl;
        } else if (height < heightMin) {
            finalHeight = heightMin;
            std::cout << "[ NOTE ] Selected height is below minimum, setting to min allowable: " << finalHeight << "." << std::endl;
        }

        ptrHeight->SetValue(finalHeight);
        std::cout << "Height set to " << finalHeight << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Height setting not available." << std::endl;
    }

    // Calculate and set width offset
    CIntegerPtr ptrWidthOffset = nodeMap->GetNode("OffsetX");
    if (IsAvailable(ptrWidthOffset) && IsWritable(ptrWidthOffset)) {
        const int sensorWidth = static_cast<int>(ptrWidth->GetMax());
        int maxWidthOffset = (sensorWidth - finalWidth);
        int minWidthOffset = 0;

        if (user_width_offset > maxWidthOffset) {
            user_width_offset = maxWidthOffset;
            std::cout << "[ NOTE ] Selected width offset exceeds maximum, setting to max allowable: " << user_width_offset << "." << std::endl;
        } else if (user_width_offset < minWidthOffset) {
            user_width_offset = minWidthOffset;
            std::cout << "[ NOTE ] Selected width offset is below minimum, setting to min allowable: " << user_width_offset << "." << std::endl;
        }

        ptrWidthOffset->SetValue(user_width_offset);
        std::cout << "Width offset set to " << user_width_offset << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Width offset setting not available." << std::endl;
    }

    // Calculate and set height offset
    CIntegerPtr ptrHeightOffset = nodeMap->GetNode("OffsetY");
    if (IsAvailable(ptrHeightOffset) && IsWritable(ptrHeightOffset)) {
        const int sensorHeight = static_cast<int>(ptrHeight->GetMax());
        int maxHeightOffset = (sensorHeight - finalHeight);
        int minHeightOffset = 0;

        if (user_height_offset > maxHeightOffset) {
            user_height_offset = maxHeightOffset;
            std::cout << "[ NOTE ] Selected height offset exceeds maximum, setting to max allowable: " << user_height_offset << "." << std::endl;
        } else if (user_height_offset < minHeightOffset) {
            user_height_offset = minHeightOffset;
            std::cout << "[ NOTE ] Selected height offset is below minimum, setting to min allowable: " << user_height_offset << "." << std::endl;
        }

        ptrHeightOffset->SetValue(user_height_offset);
        std::cout << "Height offset set to " << user_height_offset << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Height offset setting not available." << std::endl;
    }
}

void SpinCamera::SetGainSensitivity(SpinOption::GainSensitivity user_option) {
    // All legal options
    const std::unordered_map<SpinOption::GainSensitivity, float> GainSensitivity_legal = {
        {SpinOption::GainSensitivity::Auto, 0.0f},
        {SpinOption::GainSensitivity::Preset_0dB, 0.0f},
        {SpinOption::GainSensitivity::Preset_3dB, 3.0f},
        {SpinOption::GainSensitivity::Preset_6dB, 6.0f},
        {SpinOption::GainSensitivity::Preset_9dB, 9.0f},
        {SpinOption::GainSensitivity::Preset_12dB, 12.0f},
        {SpinOption::GainSensitivity::Preset_15dB, 15.0f},
        {SpinOption::GainSensitivity::Preset_18dB, 18.0f},
        {SpinOption::GainSensitivity::Preset_21dB, 21.0f},
        {SpinOption::GainSensitivity::Preset_24dB, 24.0f},
        {SpinOption::GainSensitivity::Preset_27dB, 27.0f},
        {SpinOption::GainSensitivity::Preset_30dB, 30.0f},
        {SpinOption::GainSensitivity::Preset_33dB, 33.0f},
        {SpinOption::GainSensitivity::Preset_36dB, 36.0f},
        {SpinOption::GainSensitivity::Preset_39dB, 39.0f},
        {SpinOption::GainSensitivity::Preset_42dB, 42.0f},
        {SpinOption::GainSensitivity::ISO_100, 0.0f},
        {SpinOption::GainSensitivity::ISO_200, 6.0f},
        {SpinOption::GainSensitivity::ISO_400, 12.0f},
        {SpinOption::GainSensitivity::ISO_800, 18.0f},
        {SpinOption::GainSensitivity::ISO_1600, 24.0f},
        {SpinOption::GainSensitivity::ISO_3200, 30.0f},
        {SpinOption::GainSensitivity::ISO_6400, 36.0f},
        {SpinOption::GainSensitivity::ISO_12800, 42.0f}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected gain sensitivity value from the map
    auto option = GainSensitivity_legal.find(user_option);
    if (option == GainSensitivity_legal.end()) {
        std::cout << "[ WARNING ] Invalid gain sensitivity option." << std::endl;
        return;
    }
    const float& gainSensitivity = option->second;

    // Set gain mode to auto or manual based on user option
    CEnumerationPtr ptrGainAuto = nodeMap->GetNode("GainAuto");
    if (!IsReadable(ptrGainAuto) || !IsWritable(ptrGainAuto)) {
        std::cout << "[ WARNING ] Unable to set gain mode" << std::endl;
        return;
    }

    if (user_option == SpinOption::GainSensitivity::Auto) {
        // Attempt to enable automatic gain
        CEnumEntryPtr ptrGainAutoOn = ptrGainAuto->GetEntryByName("Continuous");
        if (IsReadable(ptrGainAutoOn) && IsWritable(ptrGainAutoOn)) {
            ptrGainAuto->SetIntValue(ptrGainAutoOn->GetValue());
            std::cout << "Auto Gain Enabled" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to enable automatic gain" << std::endl;
        }
        return;
    } else {
        // Attempt to disable automatic gain
        CEnumEntryPtr ptrGainAutoOff = ptrGainAuto->GetEntryByName("Off");
        if (IsReadable(ptrGainAutoOff) && IsWritable(ptrGainAutoOff)) {
            ptrGainAuto->SetIntValue(ptrGainAutoOff->GetValue());
            std::cout << "Manual Gain Enabled (Automatic gain disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic gain (enable manual gain)" << std::endl;
        }
    }


    // Apply user-selected gain sensitivity
    CFloatPtr ptrGain = nodeMap->GetNode("Gain");
    if (IsAvailable(ptrGain) && IsWritable(ptrGain)) {
        const float gainMax = static_cast<float>(ptrGain->GetMax());
        const float gainMin = static_cast<float>(ptrGain->GetMin());
        float finalGainSensitivity = gainSensitivity;

        if (gainSensitivity > gainMax) {
            finalGainSensitivity = gainMax;
            std::cout << "[ NOTE ] Selected gain sensitivity exceeds maximum, setting to max allowable: " << finalGainSensitivity << std::endl;
        } else if (gainSensitivity < gainMin) {
            finalGainSensitivity = gainMin;
            std::cout << "[ NOTE ] Selected gain sensitivity is below minimum, setting to min allowable: " << finalGainSensitivity << std::endl;
        }

        ptrGain->SetValue(finalGainSensitivity);
        std::cout << "Gain sensitivity set to " << finalGainSensitivity << std::endl;
    } else {
        std::cout << "[ WARNING ] Gain sensitivity setting not available" << std::endl;
    }
}

void SpinCamera::SetGainSensitivity(float user_gain_sensitivity) {
    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure automatic gain is off to allow manual setting
    CEnumerationPtr ptrGainAuto = nodeMap->GetNode("GainAuto");
    if (IsReadable(ptrGainAuto) && IsWritable(ptrGainAuto)) {
        CEnumEntryPtr ptrGainAutoOff = ptrGainAuto->GetEntryByName("Off");
        if (IsReadable(ptrGainAutoOff)) {
            ptrGainAuto->SetIntValue(ptrGainAutoOff->GetValue());
            std::cout << "Manual Gain Enabled (Automatic gain disabled)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to disable automatic gain" << std::endl;
        return;
    }

    // Apply user-selected gain sensitivity
    CFloatPtr ptrGain = nodeMap->GetNode("Gain");
    if (IsAvailable(ptrGain) && IsWritable(ptrGain)) {
        const float gainMax = static_cast<float>(ptrGain->GetMax());
        const float gainMin = static_cast<float>(ptrGain->GetMin());
        float finalGainSensitivity = user_gain_sensitivity;

        if (user_gain_sensitivity > gainMax) {
            finalGainSensitivity = gainMax;
            std::cout << "[ NOTE ] Selected gain sensitivity exceeds maximum, setting to max allowable: " << finalGainSensitivity << std::endl;
        } else if (user_gain_sensitivity < gainMin) {
            finalGainSensitivity = gainMin;
            std::cout << "[ NOTE ] Selected gain sensitivity is below minimum, setting to min allowable: " << finalGainSensitivity << std::endl;
        }

        ptrGain->SetValue(finalGainSensitivity);
        std::cout << "Gain sensitivity set to " << finalGainSensitivity << std::endl;
    } else {
        std::cout << "[ WARNING ] Gain sensitivity setting not available" << std::endl;
    }
}

void SpinCamera::SetGammaCorrection(SpinOption::GammaCorrection user_option) {
    // All legal options for GammaCorrection
    const std::unordered_map<SpinOption::GammaCorrection, float> GammaCorrection_legal = {
        {SpinOption::GammaCorrection::Disable, 0.0f},
        {SpinOption::GammaCorrection::Auto, 0.0f},
        {SpinOption::GammaCorrection::Preset_0_00, 0.00f},
        {SpinOption::GammaCorrection::Preset_0_25, 0.25f},
        {SpinOption::GammaCorrection::Preset_0_50, 0.50f},
        {SpinOption::GammaCorrection::Preset_0_75, 0.75f},
        {SpinOption::GammaCorrection::Preset_1_00, 1.00f},
        {SpinOption::GammaCorrection::Preset_1_25, 1.25f},
        {SpinOption::GammaCorrection::Preset_1_50, 1.50f},
        {SpinOption::GammaCorrection::Preset_1_75, 1.75f},
        {SpinOption::GammaCorrection::Preset_2_00, 2.00f},
        {SpinOption::GammaCorrection::Preset_2_25, 2.25f},
        {SpinOption::GammaCorrection::Preset_2_50, 2.50f},
        {SpinOption::GammaCorrection::Preset_2_75, 2.75f},
        {SpinOption::GammaCorrection::Preset_3_00, 3.00f}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected gamma correction value from the map
    auto option = GammaCorrection_legal.find(user_option);
    if (option == GammaCorrection_legal.end()) {
        std::cout << "[ WARNING ] Invalid gamma correction option." << std::endl;
        return;
    }
    const float& gammaValue = option->second;

    // Either enable or disable gamma based on user input
    CBooleanPtr ptrGammaEnabled = nodeMap->GetNode("GammaEnable");
    if (IsAvailable(ptrGammaEnabled) && IsWritable(ptrGammaEnabled)) {
        if (user_option == SpinOption::GammaCorrection::Disable) {
            ptrGammaEnabled->SetValue(false);
            std::cout << "Gamma disabled" << std::endl;
            return;
        } else {
            ptrGammaEnabled->SetValue(true);
            std::cout << "Gamma enabled" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to enable/disable gamma correction" << std::endl;
    }

    // Set gamma mode to auto or manual based on user option
    CEnumerationPtr ptrGammaAuto = nodeMap->GetNode("GammaAuto");
    if (!IsReadable(ptrGammaAuto) || !IsWritable(ptrGammaAuto)) {
        std::cout << "[ WARNING ] Unable to set gamma mode" << std::endl;
        return;
    }

    if (user_option == SpinOption::GammaCorrection::Auto) {
        // Attempt to enable automatic gamma
        CEnumEntryPtr ptrGammaAutoOn = ptrGammaAuto->GetEntryByName("Continuous");
        if (IsReadable(ptrGammaAutoOn) && IsWritable(ptrGammaAutoOn)) {
            ptrGammaAuto->SetIntValue(ptrGammaAutoOn->GetValue());
            std::cout << "Auto Gamma Enabled" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to enable automatic gamma" << std::endl;
        }
        return;
    } else {
        // Attempt to disable automatic gamma
        CEnumEntryPtr ptrGammaAutoOff = ptrGammaAuto->GetEntryByName("Off");
        if (IsReadable(ptrGammaAutoOff) && IsWritable(ptrGammaAutoOff)) {
            ptrGammaAuto->SetIntValue(ptrGammaAutoOff->GetValue());
            std::cout << "Manual Gamma Enabled (Automatic gamma disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic gamma (enable manual gamma)" << std::endl;
        }
    }

    // Apply user-selected gamma correction
    CFloatPtr ptrGamma = nodeMap->GetNode("Gamma");
    if (IsAvailable(ptrGamma) && IsWritable(ptrGamma)) {
        const float gammaMax = static_cast<float>(ptrGamma->GetMax());
        const float gammaMin = static_cast<float>(ptrGamma->GetMin());
        float finalGammaValue = gammaValue;

        if (gammaValue > gammaMax) {
            finalGammaValue = gammaMax;
            std::cout << "[ NOTE ] Selected gamma value exceeds maximum, setting to max allowable: " << finalGammaValue << std::endl;
        } else if (gammaValue < gammaMin) {
            finalGammaValue = gammaMin;
            std::cout << "[ NOTE ] Selected gamma value is below minimum, setting to min allowable: " << finalGammaValue << std::endl;
        }

        ptrGamma->SetValue(finalGammaValue);
        std::cout << "Gamma correction set to " << finalGammaValue << std::endl;
    } else {
        std::cout << "[ WARNING ] Gamma correction setting not available" << std::endl;
    }
}

void SpinCamera::SetGammaCorrection(float user_gamma_value) {
    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Enable gamma
    CBooleanPtr ptrGammaEnabled = nodeMap->GetNode("GammaEnable");
    if (IsAvailable(ptrGammaEnabled) && IsWritable(ptrGammaEnabled)) {
        ptrGammaEnabled->SetValue(true);
        std::cout << "Gamma enabled" << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to enable gamma correction" << std::endl;
        return;
    }
    
    // Ensure automatic gamma is off to allow manual setting
    CEnumerationPtr ptrGammaAuto = nodeMap->GetNode("GammaAuto");
    if (IsReadable(ptrGammaAuto) && IsWritable(ptrGammaAuto)) {
        CEnumEntryPtr ptrGammaAutoOff = ptrGammaAuto->GetEntryByName("Off");
        if (IsReadable(ptrGammaAutoOff) && IsWritable(ptrGammaAutoOff)) {
            ptrGammaAuto->SetIntValue(ptrGammaAutoOff->GetValue());
            std::cout << "Manual Gamma Enabled (Automatic gamma disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic gamma (enable manual gamma)" << std::endl;
            return;
        }
    } else {
        std::cout << "[ WARNING ] Unable to disable automatic gamma" << std::endl;
        return;
    }

    // Apply user-selected gamma correction
    CFloatPtr ptrGamma = nodeMap->GetNode("Gamma");
    if (IsAvailable(ptrGamma) && IsWritable(ptrGamma)) {
        const float gammaMax = static_cast<float>(ptrGamma->GetMax());
        const float gammaMin = static_cast<float>(ptrGamma->GetMin());
        float finalGammaValue = user_gamma_value;

        if (user_gamma_value > gammaMax) {
            finalGammaValue = gammaMax;
            std::cout << "[ NOTE ] Selected gamma value exceeds maximum, setting to max allowable: " << finalGammaValue << std::endl;
        } else if (user_gamma_value < gammaMin) {
            finalGammaValue = gammaMin;
            std::cout << "[ NOTE ] Selected gamma value is below minimum, setting to min allowable: " << finalGammaValue << std::endl;
        }

        ptrGamma->SetValue(finalGammaValue);
        std::cout << "Gamma correction set to " << finalGammaValue << std::endl;
    } else {
        std::cout << "[ WARNING ] Gamma correction setting not available" << std::endl;
    }
}

void SpinCamera::SetBlackLevel(SpinOption::BlackLevel user_option) {
    // All legal options for BlackLevel
    const std::unordered_map<SpinOption::BlackLevel, float> BlackLevel_legal = {
        {SpinOption::BlackLevel::Auto, 0.0f},
        {SpinOption::BlackLevel::Preset_0_00, 0.00f},
        {SpinOption::BlackLevel::Preset_0_25, 0.25f},
        {SpinOption::BlackLevel::Preset_0_50, 0.50f},
        {SpinOption::BlackLevel::Preset_0_75, 0.75f},
        {SpinOption::BlackLevel::Preset_1_00, 1.00f},
        {SpinOption::BlackLevel::Preset_1_25, 1.25f},
        {SpinOption::BlackLevel::Preset_1_50, 1.50f},
        {SpinOption::BlackLevel::Preset_1_75, 1.75f},
        {SpinOption::BlackLevel::Preset_2_00, 2.00f},
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Get the selected black level value from the map
    auto option = BlackLevel_legal.find(user_option);
    if (option == BlackLevel_legal.end()) {
        std::cout << "[ WARNING ] Invalid black level option." << std::endl;
        return;
    }
    const float& blackLevelValue = option->second;

    // Set black level mode to auto or manual based on user option
    CEnumerationPtr ptrBlackLevelAuto = nodeMap->GetNode("BlackLevelAuto");
    if (!IsReadable(ptrBlackLevelAuto) || !IsWritable(ptrBlackLevelAuto)) {
        std::cout << "[ WARNING ] Unable to set black level mode" << std::endl;
        return;
    }

    if (user_option == SpinOption::BlackLevel::Auto) {
        // Attempt to enable automatic black level
        CEnumEntryPtr ptrBlackLevelAutoOn = ptrBlackLevelAuto->GetEntryByName("Continuous");
        if (IsReadable(ptrBlackLevelAutoOn) && IsWritable(ptrBlackLevelAutoOn)) {
            ptrBlackLevelAuto->SetIntValue(ptrBlackLevelAutoOn->GetValue());
            std::cout << "Auto Black Level Enabled" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to enable automatic black level" << std::endl;
        }
        return;
    } else {
        // Attempt to disable automatic black level
        CEnumEntryPtr ptrBlackLevelAutoOff = ptrBlackLevelAuto->GetEntryByName("Off");
        if (IsReadable(ptrBlackLevelAutoOff) && IsWritable(ptrBlackLevelAutoOff)) {
            ptrBlackLevelAuto->SetIntValue(ptrBlackLevelAutoOff->GetValue());
            std::cout << "Manual Black Level Enabled (Automatic black level disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic black level (enable manual black level)" << std::endl;
        }
    }

    // Apply user-selected black level correction
    CFloatPtr ptrBlackLevel = nodeMap->GetNode("BlackLevel");
    if (IsAvailable(ptrBlackLevel) && IsWritable(ptrBlackLevel)) {
        const float blackLevelMax = static_cast<float>(ptrBlackLevel->GetMax());
        const float blackLevelMin = static_cast<float>(ptrBlackLevel->GetMin());
        float finalBlackLevelValue = blackLevelValue;

        if (blackLevelValue > blackLevelMax) {
            finalBlackLevelValue = blackLevelMax;
            std::cout << "[ NOTE ] Selected black level value exceeds maximum, setting to max allowable: " << finalBlackLevelValue << std::endl;
        } else if (blackLevelValue < blackLevelMin) {
            finalBlackLevelValue = blackLevelMin;
            std::cout << "[ NOTE ] Selected black level value is below minimum, setting to min allowable: " << finalBlackLevelValue << std::endl;
        }

        ptrBlackLevel->SetValue(finalBlackLevelValue);
        std::cout << "Black level correction set to " << finalBlackLevelValue << std::endl;
    } else {
        std::cout << "[ WARNING ] Black level correction setting not available" << std::endl;
    }
}

void SpinCamera::SetBlackLevel(float user_black_level_value) {
    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Enable black level
    CBooleanPtr ptrBlackLevelEnabled = nodeMap->GetNode("BlackLevelEnable");
    if (IsAvailable(ptrBlackLevelEnabled) && IsWritable(ptrBlackLevelEnabled)) {
        ptrBlackLevelEnabled->SetValue(true);
        std::cout << "Black level correction enabled" << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to enable black level correction" << std::endl;
        return;
    }
    
    // Ensure automatic black level is off to allow manual setting
    CEnumerationPtr ptrBlackLevelAuto = nodeMap->GetNode("BlackLevelAuto");
    if (IsReadable(ptrBlackLevelAuto) && IsWritable(ptrBlackLevelAuto)) {
        CEnumEntryPtr ptrBlackLevelAutoOff = ptrBlackLevelAuto->GetEntryByName("Off");
        if (IsReadable(ptrBlackLevelAutoOff) && IsWritable(ptrBlackLevelAutoOff)) {
            ptrBlackLevelAuto->SetIntValue(ptrBlackLevelAutoOff->GetValue());
            std::cout << "Manual Black Level Enabled (Automatic black level disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic black level (enable manual black level)" << std::endl;
            return;
        }
    } else {
        std::cout << "[ WARNING ] Unable to disable automatic black level" << std::endl;
        return;
    }

    // Apply user-selected black level correction
    CFloatPtr ptrBlackLevel = nodeMap->GetNode("BlackLevel");
    if (IsAvailable(ptrBlackLevel) && IsWritable(ptrBlackLevel)) {
        const float blackLevelMax = static_cast<float>(ptrBlackLevel->GetMax());
        const float blackLevelMin = static_cast<float>(ptrBlackLevel->GetMin());
        float finalBlackLevelValue = user_black_level_value;

        if (user_black_level_value > blackLevelMax) {
            finalBlackLevelValue = blackLevelMax;
            std::cout << "[ NOTE ] Selected black level value exceeds maximum, setting to max allowable: " << finalBlackLevelValue << std::endl;
        } else if (user_black_level_value < blackLevelMin) {
            finalBlackLevelValue = blackLevelMin;
            std::cout << "[ NOTE ] Selected black level value is below minimum, setting to min allowable: " << finalBlackLevelValue << std::endl;
        }

        ptrBlackLevel->SetValue(finalBlackLevelValue);
        std::cout << "Black level correction set to " << finalBlackLevelValue << std::endl;
    } else {
        std::cout << "[ WARNING ] Black level correction setting not available" << std::endl;
    }
}

void SpinCamera::SetRedBalanceRatio(SpinOption::RedBalanceRatio user_option) {
    // All legal options for RedBalanceRatio
    const std::unordered_map<SpinOption::RedBalanceRatio, float> RedBalanceRatio_legal = {
        {SpinOption::RedBalanceRatio::Auto, 0.0f},
        {SpinOption::RedBalanceRatio::Preset_0_00, 0.00f},
        {SpinOption::RedBalanceRatio::Preset_0_25, 0.25f},
        {SpinOption::RedBalanceRatio::Preset_0_50, 0.50f},
        {SpinOption::RedBalanceRatio::Preset_0_75, 0.75f},
        {SpinOption::RedBalanceRatio::Preset_1_00, 1.00f},
        {SpinOption::RedBalanceRatio::Preset_1_25, 1.25f},
        {SpinOption::RedBalanceRatio::Preset_1_50, 1.50f},
        {SpinOption::RedBalanceRatio::Preset_1_75, 1.75f},
        {SpinOption::RedBalanceRatio::Preset_2_00, 2.00f},
        {SpinOption::RedBalanceRatio::Preset_2_25, 2.25f},
        {SpinOption::RedBalanceRatio::Preset_2_50, 2.50f},
        {SpinOption::RedBalanceRatio::Preset_2_75, 2.75f},
        {SpinOption::RedBalanceRatio::Preset_3_00, 3.00f}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure automatic white balance is off to allow manual setting
    CEnumerationPtr ptrWhiteBalanceAuto = nodeMap->GetNode("BalanceWhiteAuto");
    if (IsReadable(ptrWhiteBalanceAuto) && IsWritable(ptrWhiteBalanceAuto)) {
        CEnumEntryPtr ptrWhiteBalanceAutoOff = ptrWhiteBalanceAuto->GetEntryByName("Off");
        if (IsReadable(ptrWhiteBalanceAutoOff) && IsWritable(ptrWhiteBalanceAutoOff)) {
            ptrWhiteBalanceAuto->SetIntValue(ptrWhiteBalanceAutoOff->GetValue());
            std::cout << "Manual White Balance Enabled (Automatic white balance disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic white balance" << std::endl;
            return;
        }
    } else {
        std::cout << "[ WARNING ] Unable to disable automatic white balance" << std::endl;
        return;
    }

    // Set balance ratio selector to red
    CEnumerationPtr ptrBalanceRatioSelector = nodeMap->GetNode("BalanceRatioSelector");
    if (IsWritable(ptrBalanceRatioSelector)) {
        CEnumEntryPtr ptrRed = ptrBalanceRatioSelector->GetEntryByName("Red");
        if (IsReadable(ptrRed)) {
            ptrBalanceRatioSelector->SetIntValue(ptrRed->GetValue());
        } else {
            std::cout << "[ WARNING ] Unable to find or access BalanceRatioSelector Red" << std::endl;
            return;
        }

        // Get the selected red balance ratio value from the map
        auto option = RedBalanceRatio_legal.find(user_option);
        if (option == RedBalanceRatio_legal.end()) {
            std::cout << "[ WARNING ] Invalid red balance ratio option." << std::endl;
            return;
        }
        const float& redBalanceValue = option->second;

        // Apply user-selected red balance ratio
        CFloatPtr ptrRedBalance = nodeMap->GetNode("BalanceRatio");
        if (IsAvailable(ptrRedBalance) && IsWritable(ptrRedBalance)) {
            ptrRedBalance->SetValue(redBalanceValue);
            std::cout << "Red balance ratio set to " << redBalanceValue << std::endl;
        } else {
            std::cout << "[ WARNING ] Red balance ratio setting not available" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] BalanceRatioSelector not available" << std::endl;
    }
}


void SpinCamera::SetBlueBalanceRatio(SpinOption::BlueBalanceRatio user_option) {
    // All legal options for BlueBalanceRatio
    const std::unordered_map<SpinOption::BlueBalanceRatio, float> BlueBalanceRatio_legal = {
        {SpinOption::BlueBalanceRatio::Auto, 0.0f},
        {SpinOption::BlueBalanceRatio::Preset_0_00, 0.00f},
        {SpinOption::BlueBalanceRatio::Preset_0_25, 0.25f},
        {SpinOption::BlueBalanceRatio::Preset_0_50, 0.50f},
        {SpinOption::BlueBalanceRatio::Preset_0_75, 0.75f},
        {SpinOption::BlueBalanceRatio::Preset_1_00, 1.00f},
        {SpinOption::BlueBalanceRatio::Preset_1_25, 1.25f},
        {SpinOption::BlueBalanceRatio::Preset_1_50, 1.50f},
        {SpinOption::BlueBalanceRatio::Preset_1_75, 1.75f},
        {SpinOption::BlueBalanceRatio::Preset_2_00, 2.00f},
        {SpinOption::BlueBalanceRatio::Preset_2_25, 2.25f},
        {SpinOption::BlueBalanceRatio::Preset_2_50, 2.50f},
        {SpinOption::BlueBalanceRatio::Preset_2_75, 2.75f},
        {SpinOption::BlueBalanceRatio::Preset_3_00, 3.00f}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure automatic white balance is off to allow manual setting
    CEnumerationPtr ptrWhiteBalanceAuto = nodeMap->GetNode("BalanceWhiteAuto");
    if (IsReadable(ptrWhiteBalanceAuto) && IsWritable(ptrWhiteBalanceAuto)) {
        CEnumEntryPtr ptrWhiteBalanceAutoOff = ptrWhiteBalanceAuto->GetEntryByName("Off");
        if (IsReadable(ptrWhiteBalanceAutoOff) && IsWritable(ptrWhiteBalanceAutoOff)) {
            ptrWhiteBalanceAuto->SetIntValue(ptrWhiteBalanceAutoOff->GetValue());
            std::cout << "Manual White Balance Enabled (Automatic white balance disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic white balance" << std::endl;
            return;
        }
    } else {
        std::cout << "[ WARNING ] Unable to disable automatic white balance" << std::endl;
        return;
    }

    // Set balance ratio selector to blue
    CEnumerationPtr ptrBalanceRatioSelector = nodeMap->GetNode("BalanceRatioSelector");
    if (IsWritable(ptrBalanceRatioSelector)) {
        CEnumEntryPtr ptrBlue = ptrBalanceRatioSelector->GetEntryByName("Blue");
        if (IsReadable(ptrBlue)) {
            ptrBalanceRatioSelector->SetIntValue(ptrBlue->GetValue());
        } else {
            std::cout << "[ WARNING ] Unable to find or access BalanceRatioSelector Blue" << std::endl;
            return;
        }

        // Get the selected blue balance ratio value from the map
        auto option = BlueBalanceRatio_legal.find(user_option);
        if (option == BlueBalanceRatio_legal.end()) {
            std::cout << "[ WARNING ] Invalid blue balance ratio option." << std::endl;
            return;
        }
        const float& blueBalanceValue = option->second;

        // Apply user-selected blue balance ratio
        CFloatPtr ptrBlueBalance = nodeMap->GetNode("BalanceRatio");
        if (IsAvailable(ptrBlueBalance) && IsWritable(ptrBlueBalance)) {
            ptrBlueBalance->SetValue(blueBalanceValue);
            std::cout << "Blue balance ratio set to " << blueBalanceValue << std::endl;
        } else {
            std::cout << "[ WARNING ] Blue balance ratio setting not available" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] BalanceRatioSelector not available" << std::endl;
    }
}