#include "../include/SpinnakerSDK_SpinCamera.h"

using namespace Spinnaker;
using namespace GenApi;

SpinCamera::SpinCamera() : pCam(nullptr), system(nullptr), nodeMap(nullptr) {}

SpinCamera::~SpinCamera() {
    Shutdown();
}

void SpinCamera::Initialize(int camera_index) {
    // Get the instance of the camera
    system = System::GetInstance();
    camList = system->GetCameras();
    if (camList.GetSize() == 0) {
        throw std::runtime_error("[ ERROR ] No cameras found.");
    }
    pCam = camList.GetByIndex(camera_index);

    // Initialize the camera
    pCam->Init();

    // Retrieve and set node map
    nodeMap = &pCam->GetNodeMap();
    if (nodeMap == nullptr) {
        throw std::runtime_error("[ ERROR ] Failed to get node map.");
    }

    // Retrieve and set the TLStreamNodeMap
    streamNodeMap = &pCam->GetTLStreamNodeMap();
    if (streamNodeMap == nullptr) {
        throw std::runtime_error("[ ERROR ] Failed to get stream node map.");
    }
}

void SpinCamera::StartAcquisition() {
    if (!pCam) throw std::runtime_error("[ ERROR ] Unable to start camera Acquisition");
    pCam->BeginAcquisition();
    acquisitionActive = true;
}

void SpinCamera::StopAcquisition() {
    if (!pCam) throw std::runtime_error("[ ERROR ] Unable to end camera Acquisition");
    pCam->EndAcquisition();
    acquisitionActive = false;
}


void SpinCamera::CaptureSingleFrame(SpinImage& capturedImage) {
    // Start acquisition if not already active
    bool startedAcquisition = false;
    if (!acquisitionActive) {
        // Set acquisition mode to single frame
        SetAcquisitionMode(SpinOption::AcquisitionMode::SingleFrame);
        // Set buffer handling mode to NewestOnly
        SetBufferHandlingMode(SpinOption::BufferHandlingMode::NewestOnly);
        StartAcquisition();
        startedAcquisition = true;
    }

    // Capture image from the camera
    if (pCam) {
        Spinnaker::ImagePtr rawImage = pCam->GetNextImage();
        if (rawImage->IsIncomplete()) {
            std::cerr << "[ ERROR ] Image incomplete with image status " << rawImage->GetImageStatus() << std::endl;
        } else {
            capturedImage = SpinImage(rawImage);
        }
    }

    // Stop acquisition if it was started by this function
    if (startedAcquisition) {
        StopAcquisition();
    }
}

// Note that the goal of the CaptureSingleFrameOnTrigger is not to use the 
// built in "Trigger" mechanisms, as that trigger only triggers the start of
// acquisition. To capture a frame immediately available on the trigger, a 
// continuous approach needs to be used.
void SpinCamera::CaptureSingleFrameOnTrigger(SpinImage& capturedImage, std::atomic<bool>& trigger, int polling_delay_us) {
    // Start acquisition if not already active
    bool startedAcquisition = false;
    if (!acquisitionActive) {
        // Set acquisition mode to continuous
        SetAcquisitionMode(SpinOption::AcquisitionMode::Continuous);
        // Set buffer handling mode to NewestOnly
        SetBufferHandlingMode(SpinOption::BufferHandlingMode::NewestOnly);
        StartAcquisition();
        startedAcquisition = true;
    }

    // Wait for the trigger to go high
    while (!trigger.load()) {
        // Polling delay to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::microseconds(polling_delay_us));
    }

    // Capture the time point right after the trigger
    // auto triggerTime = std::chrono::high_resolution_clock::now();

    // Capture and release the image that was being acquired DURING the trigger
    Spinnaker::ImagePtr preTriggerImage;
    if (pCam) {
        preTriggerImage = pCam->GetNextImage();
        if (preTriggerImage->IsIncomplete()) {
            std::cerr << "[ ERROR ] Pre-trigger image incomplete with image status " << preTriggerImage->GetImageStatus() << std::endl;
        }
        // Release the image
        preTriggerImage->Release();
    }

    // Capture the image immediately after the trigger
    // This is the first "Legal" image in a situation where the trigger effectively begins aquisition (starts exposure)
    Spinnaker::ImagePtr postTriggerImage;
    if (pCam) {
        postTriggerImage = pCam->GetNextImage();
        if (postTriggerImage->IsIncomplete()) {
            std::cerr << "[ ERROR ] Post-trigger image incomplete with image status " << postTriggerImage->GetImageStatus() << std::endl;
        } else {
            capturedImage = SpinImage(postTriggerImage);
        }
    }

    // Capture the time point right after the image is captured
    // auto captureTime = std::chrono::high_resolution_clock::now();

    // Calculate and print the duration between the trigger and the image capture
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(captureTime - triggerTime).count();
    // std::cout << "Duration from trigger to image capture: " << duration << " ms" << std::endl;
    // double duration2 = postTriggerImage->GetTimeStamp() - preTriggerImage->GetTimeStamp();
    // std::cout << "Duration between image timestamps: " << duration2/1000000 << " ms" << std::endl;

    // Stop acquisition if it was started by this function
    if (startedAcquisition) {
        StopAcquisition();
    }
}

void SpinCamera::CaptureContinuousFrames(std::vector<SpinImage>& frames, int numFrames) {
    // Ensure frames vector is empty
    frames.clear();

    // Set the buffer count mode to manual
    CEnumerationPtr ptrStreamBufferCountMode = streamNodeMap->GetNode("StreamBufferCountMode");
    if (IsWritable(ptrStreamBufferCountMode)) {
        CEnumEntryPtr ptrStreamBufferCountModeManual = ptrStreamBufferCountMode->GetEntryByName("Manual");
        if (IsReadable(ptrStreamBufferCountModeManual)) {
            ptrStreamBufferCountMode->SetIntValue(ptrStreamBufferCountModeManual->GetValue());
            std::cout << "Stream Buffer Count Mode set to manual" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to read buffer count mode manual entry." << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to set buffer count mode to manual." << std::endl;
    }

    // Set the buffer count to a reasonable value
    int64_t bufferCount = 100;
    CIntegerPtr ptrBufferCount = streamNodeMap->GetNode("StreamBufferCountManual");
    if (IsWritable(ptrBufferCount)) {
        ptrBufferCount->SetValue(bufferCount);
        std::cout << "Buffer count set to: " << bufferCount << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to set buffer count." << std::endl;
    }

    // Start acquisition if not already active
    bool startedAcquisition = false;
    if (!acquisitionActive) {
        // Set acquisition mode to continuous
        SetAcquisitionMode(SpinOption::AcquisitionMode::Continuous);
        // Set buffer handling mode to NewestFirst
        SetBufferHandlingMode(SpinOption::BufferHandlingMode::OldestFirst);
        StartAcquisition();
        startedAcquisition = true;
    }

    // Capture the specified number of frames
    for (int i = 1; i <= numFrames; ++i) {
        if (pCam) {
            Spinnaker::ImagePtr rawImage = pCam->GetNextImage(1000);
            if (rawImage->IsIncomplete()) {
                std::cerr << "[ ERROR ] Image incomplete with image status " << rawImage->GetImageStatus() << std::endl;
            } else {
                frames.emplace_back(rawImage);
                // frames[i].PrintAllImageInformation();
                std::cout << "Image number " << i << " complete" << std::endl;
                // Release image
                rawImage->Release();
            }
        }
    }

    // Retrieve and print the number of lost frames
    CIntegerPtr ptrLostFrameCount = streamNodeMap->GetNode("StreamLostFrameCount");
    if (IsReadable(ptrLostFrameCount)) {
        int64_t lostFrameCount = ptrLostFrameCount->GetValue();
        std::cout << "Number of lost frames: " << lostFrameCount << std::endl;
    } else {
        std::cout << "[ WARNING ] Unable to retrieve lost frame count." << std::endl;
    }

    // Stop acquisition if it was started by this function
    if (startedAcquisition) {
        StopAcquisition();
    }
}

void SpinCamera::Shutdown() {
    // Ensure not aquiring
    if (acquisitionActive) {
        StopAcquisition();
    }
    
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
    SetExposureTime(SpinOption::ExposureTime::Shutter_1_30);
    SetImageDimensions(SpinOption::ImageDimensions::Preset_1440x1080);
    SetGainSensitivity(SpinOption::GainSensitivity::Preset_24dB);
    SetGammaCorrection(SpinOption::GammaCorrection::Preset_0_75);
    SetBlackLevel(SpinOption::BlackLevel::Preset_0_25);
    SetRedBalanceRatio(SpinOption::RedBalanceRatio::Preset_1_25);
    SetBlueBalanceRatio(SpinOption::BlueBalanceRatio::Preset_2_25);
}

void SpinCamera::SetAutoSettings() {
    SetPixelFormat(SpinOption::PixelFormat::BayerRG8);
    SetBinning(SpinOption::Binning::NoBinning);
    SetDecimation(SpinOption::Decimation::NoDecimation);
    SetExposureTime(SpinOption::ExposureTime::Auto);
    SetImageDimensions(SpinOption::ImageDimensions::Preset_1440x1080);
    SetGainSensitivity(SpinOption::GainSensitivity::Auto);
    SetGammaCorrection(SpinOption::GammaCorrection::Disable);
    SetBlackLevel(SpinOption::BlackLevel::Auto);
    SetRedBalanceRatio(SpinOption::RedBalanceRatio::Auto);
    SetBlueBalanceRatio(SpinOption::BlueBalanceRatio::Auto);
}

void SpinCamera::PrintSettings() {
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    std::cout << "===== Printing Settings =====" << std::endl;

    try {
        // Pixel Format
        CEnumerationPtr ptrPixelFormat = nodeMap->GetNode("PixelFormat");
        if (IsReadable(ptrPixelFormat)) {
            CEnumEntryPtr ptrPixelFormatEntry = ptrPixelFormat->GetCurrentEntry();
            std::string pixelFormat = std::string(ptrPixelFormatEntry->GetSymbolic().c_str());
            std::cout << "Pixel Format: " << pixelFormat << std::endl;
        } else {
            std::cout << "[ WARNING ] Pixel format not readable." << std::endl;
        }

        // Binning
        CIntegerPtr ptrBinningHorizontal = nodeMap->GetNode("BinningHorizontal");
        CIntegerPtr ptrBinningVertical = nodeMap->GetNode("BinningVertical");
        if (IsReadable(ptrBinningHorizontal) && IsReadable(ptrBinningVertical)) {
            int binningHorizontal = ptrBinningHorizontal->GetValue();
            int binningVertical = ptrBinningVertical->GetValue();
            std::cout << "Binning (Horizontal x Vertical): " << binningHorizontal << " x " << binningVertical << std::endl;
        } else {
            std::cout << "[ WARNING ] Binning values not readable." << std::endl;
        }

        // Decimation
        CIntegerPtr ptrDecimationHorizontal = nodeMap->GetNode("DecimationHorizontal");
        CIntegerPtr ptrDecimationVertical = nodeMap->GetNode("DecimationVertical");
        if (IsReadable(ptrDecimationHorizontal) && IsReadable(ptrDecimationVertical)) {
            int decimationHorizontal = ptrDecimationHorizontal->GetValue();
            int decimationVertical = ptrDecimationVertical->GetValue();
            std::cout << "Decimation (Horizontal x Vertical): " << decimationHorizontal << " x " << decimationVertical << std::endl;
        } else {
            std::cout << "[ WARNING ] Decimation values not readable." << std::endl;
        }

        // Exposure Time
        CEnumerationPtr ptrExposureAuto = nodeMap->GetNode("ExposureAuto");
        if (IsReadable(ptrExposureAuto)) {
            CEnumEntryPtr ptrExposureAutoEntry = ptrExposureAuto->GetCurrentEntry();
            std::string exposureAuto = std::string(ptrExposureAutoEntry->GetSymbolic().c_str());
            std::cout << "Exposure Auto: " << exposureAuto << std::endl;
        }
        CFloatPtr ptrExposureTime = nodeMap->GetNode("ExposureTime");
        if (IsReadable(ptrExposureTime)) {
            double exposureTime = ptrExposureTime->GetValue();
            std::cout << "Exposure Time: " << exposureTime << " microseconds" << std::endl;
        } else {
            std::cout << "[ WARNING ] Exposure time not readable." << std::endl;
        }

        // Image Dimensions
        CIntegerPtr ptrWidth = nodeMap->GetNode("Width");
        CIntegerPtr ptrHeight = nodeMap->GetNode("Height");
        if (IsReadable(ptrWidth) && IsReadable(ptrHeight)) {
            int width = ptrWidth->GetValue();
            int height = ptrHeight->GetValue();
            std::cout << "Image Dimensions (Width x Height): " << width << " x " << height << std::endl;
        } else {
            std::cout << "[ WARNING ] Image dimensions not readable." << std::endl;
        }

        // Gain Sensitivity
        CEnumerationPtr ptrGainAuto = nodeMap->GetNode("GainAuto");
        if (IsReadable(ptrGainAuto)) {
            CEnumEntryPtr ptrGainAutoEntry = ptrGainAuto->GetCurrentEntry();
            std::string gainAuto = std::string(ptrGainAutoEntry->GetSymbolic().c_str());
            std::cout << "Gain Auto: " << gainAuto << std::endl;
        }
        CFloatPtr ptrGain = nodeMap->GetNode("Gain");
        if (IsReadable(ptrGain)) {
            float gain = ptrGain->GetValue();
            std::cout << "Gain Sensitivity: " << gain << " dB" << std::endl;
        } else {
            std::cout << "[ WARNING ] Gain sensitivity not readable." << std::endl;
        }

        // Gamma Correction
        CBooleanPtr ptrGammaEnabled = nodeMap->GetNode("GammaEnable");
        if (IsReadable(ptrGammaEnabled)) {
            bool gammaEnabled = ptrGammaEnabled->GetValue();
            std::cout << "Gamma Enabled: " << (gammaEnabled ? "True" : "False") << std::endl;
        }
        CFloatPtr ptrGamma = nodeMap->GetNode("Gamma");
        if (IsReadable(ptrGamma)) {
            float gamma = ptrGamma->GetValue();
            std::cout << "Gamma Correction: " << gamma << std::endl;
        } else {
            std::cout << "[ WARNING ] Gamma correction not readable." << std::endl;
        }

        // Black Level
        CEnumerationPtr ptrBlackLevelAuto = nodeMap->GetNode("BlackLevelAuto");
        if (IsReadable(ptrBlackLevelAuto)) {
            CEnumEntryPtr ptrBlackLevelAutoEntry = ptrBlackLevelAuto->GetCurrentEntry();
            std::string blackLevelAuto = std::string(ptrBlackLevelAutoEntry->GetSymbolic().c_str());
            std::cout << "Black Level Auto: " << blackLevelAuto << std::endl;
        }
        CFloatPtr ptrBlackLevel = nodeMap->GetNode("BlackLevel");
        if (IsReadable(ptrBlackLevel)) {
            float blackLevel = ptrBlackLevel->GetValue();
            std::cout << "Black Level: " << blackLevel << std::endl;
        } else {
            std::cout << "[ WARNING ] Black level not readable." << std::endl;
        }

        // White Balance Ratios
        CEnumerationPtr ptrBalanceWhiteAuto = nodeMap->GetNode("BalanceWhiteAuto");
        if (IsReadable(ptrBalanceWhiteAuto)) {
            CEnumEntryPtr ptrBalanceWhiteAutoEntry = ptrBalanceWhiteAuto->GetCurrentEntry();
            std::string balanceWhiteAuto = std::string(ptrBalanceWhiteAutoEntry->GetSymbolic().c_str());
            std::cout << "Balance White Auto: " << balanceWhiteAuto << std::endl;
        }

        CEnumerationPtr ptrBalanceRatioSelector = nodeMap->GetNode("BalanceRatioSelector");
        if (IsWritable(ptrBalanceRatioSelector)) {
            // Red Balance Ratio
            CEnumEntryPtr ptrRed = ptrBalanceRatioSelector->GetEntryByName("Red");
            if (IsReadable(ptrRed)) {
                ptrBalanceRatioSelector->SetIntValue(ptrRed->GetValue());
                CFloatPtr ptrRedBalance = nodeMap->GetNode("BalanceRatio");
                if (IsReadable(ptrRedBalance)) {
                    float redBalance = ptrRedBalance->GetValue();
                    std::cout << "Red Balance Ratio: " << redBalance << std::endl;
                }
            }
            // Blue Balance Ratio
            CEnumEntryPtr ptrBlue = ptrBalanceRatioSelector->GetEntryByName("Blue");
            if (IsReadable(ptrBlue)) {
                ptrBalanceRatioSelector->SetIntValue(ptrBlue->GetValue());
                CFloatPtr ptrBlueBalance = nodeMap->GetNode("BalanceRatio");
                if (IsReadable(ptrBlueBalance)) {
                    float blueBalance = ptrBlueBalance->GetValue();
                    std::cout << "Blue Balance Ratio: " << blueBalance << std::endl;
                }
            }
        } else {
            std::cout << "[ WARNING ] White balance ratios not readable." << std::endl;
        }


    } catch (const Spinnaker::Exception& e) {
        std::cout << "[ ERROR ] Exception caught while reading settings: " << e.what() << std::endl;
    }

    std::cout << "=============================" << std::endl;

}

void SpinCamera::SetAcquisitionMode(SpinOption::AcquisitionMode mode) {

    // All legal options
    const std::unordered_map<SpinOption::AcquisitionMode, std::string> AcquisitionMode_legal = {
        {SpinOption::AcquisitionMode::Continuous, "Continuous"},
        {SpinOption::AcquisitionMode::SingleFrame, "SingleFrame"},
        {SpinOption::AcquisitionMode::MultiFrame, "MultiFrame"}
    };

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure Acquisition Mode is available to be written to
    CEnumerationPtr ptrAcquisitionMode = nodeMap->GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
        std::cout << "[ WARNING ] Unable to set acquisition mode (node retrieval)." << std::endl;
        return;
    }

    // Get the mode string from the map
    auto option = AcquisitionMode_legal.find(mode);
    if (option == AcquisitionMode_legal.end()) {
        std::cout << "[ WARNING ] Invalid acquisition mode." << std::endl;
        return;
    }
    const std::string& modeStr = option->second;

    // Apply user selected mode
    CEnumEntryPtr ptrAcquisitionModeEntry = ptrAcquisitionMode->GetEntryByName(modeStr.c_str());
    if (!IsReadable(ptrAcquisitionModeEntry)) {
        std::cout << "[ WARNING ] Unable to set acquisition mode (entry retrieval)." << std::endl;
    } else {
        try {
            ptrAcquisitionMode->SetIntValue(ptrAcquisitionModeEntry->GetValue());
            std::cout << "Acquisition mode set to " << modeStr << std::endl;
        } catch (const Spinnaker::Exception& e) {
            std::cout << "[ ERROR ] Exception caught while setting acquisition mode: " << e.what() << std::endl;
        }
    }
}

void SpinCamera::SetBufferHandlingMode(SpinOption::BufferHandlingMode mode) {
    // All legal options
    const std::unordered_map<SpinOption::BufferHandlingMode, std::string> BufferHandlingMode_legal = {
        {SpinOption::BufferHandlingMode::OldestFirst, "OldestFirst"},
        {SpinOption::BufferHandlingMode::OldestFirstOverwrite, "OldestFirstOverwrite"},
        {SpinOption::BufferHandlingMode::NewestOnly, "NewestOnly"},
        {SpinOption::BufferHandlingMode::NewestFirst, "NewestFirst"}
    };

    // Ensure TLStreamNodeMap exists
    if (!streamNodeMap) {
        std::cout << "[ WARNING ] Stream node map is not initialized." << std::endl;
        return;
    }

    // Ensure Buffer Handling Mode is available to be written to
    CEnumerationPtr ptrBufferHandlingMode = streamNodeMap->GetNode("StreamBufferHandlingMode");
    if (!IsAvailable(ptrBufferHandlingMode) || !IsWritable(ptrBufferHandlingMode)) {
        std::cout << "[ WARNING ] Unable to set buffer handling mode (node retrieval)." << std::endl;
        return;
    }

    // Get the mode string from the map
    auto option = BufferHandlingMode_legal.find(mode);
    if (option == BufferHandlingMode_legal.end()) {
        std::cout << "[ WARNING ] Invalid buffer handling mode." << std::endl;
        return;
    }
    const std::string& modeStr = option->second;

    // Apply user selected mode
    CEnumEntryPtr ptrBufferHandlingModeEntry = ptrBufferHandlingMode->GetEntryByName(modeStr.c_str());
    if (!IsReadable(ptrBufferHandlingModeEntry)) {
        std::cout << "[ WARNING ] Unable to set buffer handling mode (entry retrieval)." << std::endl;
    } else {
        try {
            ptrBufferHandlingMode->SetIntValue(ptrBufferHandlingModeEntry->GetValue());
            std::cout << "Buffer handling mode set to " << modeStr << std::endl;
        } catch (const Spinnaker::Exception& e) {
            std::cout << "[ ERROR ] Exception caught while setting buffer handling mode: " << e.what() << std::endl;
        }
    }

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
    if (IsReadable(ptrExposureAuto) && IsWritable(ptrExposureAuto)) {
        if (user_option == SpinOption::ExposureTime::Auto) {
            // Attempt to enable automatic exposure
            CEnumEntryPtr ptrExposureAutoOn = ptrExposureAuto->GetEntryByName("Continuous");
            if (IsReadable(ptrExposureAutoOn)) {
                ptrExposureAuto->SetIntValue(ptrExposureAutoOn->GetValue());
                std::cout << "Auto Exposure Enabled" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to enable automatic exposure" << std::endl;
            }
            // Return since auto mode was selected
            return;
        } else {
            // Attempt to disable automatic exposure
            CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
            if (IsReadable(ptrExposureAutoOff)) {
                ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
                std::cout << "Manual Exposure Enabled (Automatic exposure disabled)" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to disable automatic exposure (enable manual exposure)" << std::endl;
            }
        }
    } else {
        std::cout << "[ WARNING ] Unable to set exposure mode" << std::endl;
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
        // Attempt to disable automatic exposure
        CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
        if (IsReadable(ptrExposureAutoOff)) {
            ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
            std::cout << "Manual Exposure Enabled (Automatic exposure disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic exposure (enable manual exposure)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to set exposure mode" << std::endl;
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

    CIntegerPtr ptrWidth = nodeMap->GetNode("Width");
    CIntegerPtr ptrHeight = nodeMap->GetNode("Height");
    CIntegerPtr ptrWidthOffset = nodeMap->GetNode("OffsetX");
    CIntegerPtr ptrHeightOffset = nodeMap->GetNode("OffsetY");

    // Retrieve node pointers for sensor width and height
    CIntegerPtr ptrSensorWidth = nodeMap->GetNode("SensorWidth");
    CIntegerPtr ptrSensorHeight = nodeMap->GetNode("SensorHeight");

    // If the sensor width and height nodes are available, use them
    int sensorWidth = 0;
    int sensorHeight = 0;
    if (IsAvailable(ptrSensorWidth) && IsReadable(ptrSensorWidth)) {
        sensorWidth = static_cast<int>(ptrSensorWidth->GetValue());
    } else if (IsAvailable(ptrWidth) && IsReadable(ptrWidth)) {
        // Fallback to using the max value of the Width node if SensorWidth is not available
        sensorWidth = static_cast<int>(ptrWidth->GetMax());
    }

    if (IsAvailable(ptrSensorHeight) && IsReadable(ptrSensorHeight)) {
        sensorHeight = static_cast<int>(ptrSensorHeight->GetValue());
    } else if (IsAvailable(ptrHeight) && IsReadable(ptrHeight)) {
        // Fallback to using the max value of the Height node if SensorHeight is not available
        sensorHeight = static_cast<int>(ptrHeight->GetMax());
    }

    // Apply user-selected width
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
    if (IsAvailable(ptrWidthOffset) && IsWritable(ptrWidthOffset)) {
        int offsetX = (sensorWidth - finalWidth) / 2;

        ptrWidthOffset->SetValue(offsetX);
        std::cout << "Width offset set to " << offsetX << "." << std::endl;
    } else {
        std::cout << "[ WARNING ] Width offset setting not available." << std::endl;
    }

    // Calculate and set height offset
    if (IsAvailable(ptrHeightOffset) && IsWritable(ptrHeightOffset)) {
        int offsetY = (sensorHeight - finalHeight) / 2;

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

    CIntegerPtr ptrWidth = nodeMap->GetNode("Width");
    CIntegerPtr ptrHeight = nodeMap->GetNode("Height");
    CIntegerPtr ptrWidthOffset = nodeMap->GetNode("OffsetX");
    CIntegerPtr ptrHeightOffset = nodeMap->GetNode("OffsetY");

    // Retrieve node pointers for sensor width and height
    CIntegerPtr ptrSensorWidth = nodeMap->GetNode("SensorWidth");
    CIntegerPtr ptrSensorHeight = nodeMap->GetNode("SensorHeight");

    // If the sensor width and height nodes are available, use them
    int sensorWidth = 0;
    int sensorHeight = 0;
    if (IsAvailable(ptrSensorWidth) && IsReadable(ptrSensorWidth)) {
        sensorWidth = static_cast<int>(ptrSensorWidth->GetValue());
    } else if (IsAvailable(ptrWidth) && IsReadable(ptrWidth)) {
        // Fallback to using the max value of the Width node if SensorWidth is not available
        sensorWidth = static_cast<int>(ptrWidth->GetMax());
    }

    if (IsAvailable(ptrSensorHeight) && IsReadable(ptrSensorHeight)) {
        sensorHeight = static_cast<int>(ptrSensorHeight->GetValue());
    } else if (IsAvailable(ptrHeight) && IsReadable(ptrHeight)) {
        // Fallback to using the max value of the Height node if SensorHeight is not available
        sensorHeight = static_cast<int>(ptrHeight->GetMax());
    }

    // Apply user-selected width
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
    if (IsAvailable(ptrWidthOffset) && IsWritable(ptrWidthOffset)) {
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
    if (IsAvailable(ptrHeightOffset) && IsWritable(ptrHeightOffset)) {
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
    if (IsReadable(ptrGainAuto) && IsWritable(ptrGainAuto)) {
        if (user_option == SpinOption::GainSensitivity::Auto) {
            // Attempt to enable automatic gain
            CEnumEntryPtr ptrGainAutoOn = ptrGainAuto->GetEntryByName("Continuous");
            if (IsReadable(ptrGainAutoOn)) {
                ptrGainAuto->SetIntValue(ptrGainAutoOn->GetValue());
                std::cout << "Auto Gain Enabled" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to enable automatic gain" << std::endl;
            }
            // Return since auto mode was selected
            return;
        } else {
            // Attempt to disable automatic gain
            CEnumEntryPtr ptrGainAutoOff = ptrGainAuto->GetEntryByName("Off");
            if (IsReadable(ptrGainAutoOff)) {
                ptrGainAuto->SetIntValue(ptrGainAutoOff->GetValue());
                std::cout << "Manual Gain Enabled (Automatic gain disabled)" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to disable automatic gain (enable manual gain)" << std::endl;
            }
        }
    } else {
        std::cout << "[ WARNING ] Unable to set gain mode" << std::endl;
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
        // Attempt to disable automatic gain
        CEnumEntryPtr ptrGainAutoOff = ptrGainAuto->GetEntryByName("Off");
        if (IsReadable(ptrGainAutoOff)) {
            ptrGainAuto->SetIntValue(ptrGainAutoOff->GetValue());
            std::cout << "Manual Gain Enabled (Automatic gain disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic gain (enable manual gain)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to set gain mode" << std::endl;
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
    
    // Set black level mode to auto or manual based on user option
    CEnumerationPtr ptrBlackLevelAuto = nodeMap->GetNode("BlackLevelAuto");
    if (IsReadable(ptrBlackLevelAuto) && IsWritable(ptrBlackLevelAuto)) {
        // Attempt to disable automatic black level
        CEnumEntryPtr ptrBlackLevelAutoOff = ptrBlackLevelAuto->GetEntryByName("Off");
        if (IsReadable(ptrBlackLevelAutoOff)) {
            ptrBlackLevelAuto->SetIntValue(ptrBlackLevelAutoOff->GetValue());
            std::cout << "Manual Black Level Enabled (Automatic black level disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic black level (enable manual black level)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to set black level mode" << std::endl;
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

    // Set white balance mode to auto or manual based on user option
    CEnumerationPtr ptrBalanceWhiteAuto = nodeMap->GetNode("BalanceWhiteAuto");
    if (IsReadable(ptrBalanceWhiteAuto) && IsWritable(ptrBalanceWhiteAuto)) {
        if (user_option == SpinOption::RedBalanceRatio::Auto) {
            // Attempt to enable automatic white balance
            CEnumEntryPtr ptrBalanceWhiteAutoOn = ptrBalanceWhiteAuto->GetEntryByName("Continuous");
            if (IsReadable(ptrBalanceWhiteAutoOn)) {
                ptrBalanceWhiteAuto->SetIntValue(ptrBalanceWhiteAutoOn->GetValue());
                std::cout << "Auto White Balance Enabled" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to enable automatic white balance" << std::endl;
            }
            return;
        } else {
            // Attempt to disable automatic white balance
            CEnumEntryPtr ptrBalanceWhiteAutoOff = ptrBalanceWhiteAuto->GetEntryByName("Off");
            if (IsReadable(ptrBalanceWhiteAutoOff)) {
                ptrBalanceWhiteAuto->SetIntValue(ptrBalanceWhiteAutoOff->GetValue());
                std::cout << "Manual White Balance Enabled (Automatic white balance disabled)" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to disable automatic white balance (enable manual white balance)" << std::endl;
            }
        }
    } else {
        std::cout << "[ WARNING ] Unable to set white balance mode" << std::endl;
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

void SpinCamera::SetRedBalanceRatio(float user_option) {
    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure automatic white balance is off to allow manual setting
    CEnumerationPtr ptrBalanceWhiteAuto = nodeMap->GetNode("BalanceWhiteAuto");
    if (IsReadable(ptrBalanceWhiteAuto) && IsWritable(ptrBalanceWhiteAuto)) {
        // Attempt to disable automatic white balance
        CEnumEntryPtr ptrBalanceWhiteAutoOff = ptrBalanceWhiteAuto->GetEntryByName("Off");
        if (IsReadable(ptrBalanceWhiteAutoOff)) {
            ptrBalanceWhiteAuto->SetIntValue(ptrBalanceWhiteAutoOff->GetValue());
            std::cout << "Manual White Balance Enabled (Automatic white balance disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic white balance (enable manual white balance)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to set white balance mode" << std::endl;
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

        // Get the selected red balance ratio
        const float& redBalanceValue = user_option;

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

    // Set white balance mode to auto or manual based on user option
    CEnumerationPtr ptrBalanceWhiteAuto = nodeMap->GetNode("BalanceWhiteAuto");
    if (IsReadable(ptrBalanceWhiteAuto) && IsWritable(ptrBalanceWhiteAuto)) {
        if (user_option == SpinOption::BlueBalanceRatio::Auto) {
            // Attempt to enable automatic white balance
            CEnumEntryPtr ptrBalanceWhiteAutoOn = ptrBalanceWhiteAuto->GetEntryByName("Continuous");
            if (IsReadable(ptrBalanceWhiteAutoOn)) {
                ptrBalanceWhiteAuto->SetIntValue(ptrBalanceWhiteAutoOn->GetValue());
                std::cout << "Auto White Balance Enabled" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to enable automatic white balance" << std::endl;
            }
            return;
        } else {
            // Attempt to disable automatic white balance
            CEnumEntryPtr ptrBalanceWhiteAutoOff = ptrBalanceWhiteAuto->GetEntryByName("Off");
            if (IsReadable(ptrBalanceWhiteAutoOff)) {
                ptrBalanceWhiteAuto->SetIntValue(ptrBalanceWhiteAutoOff->GetValue());
                std::cout << "Manual White Balance Enabled (Automatic white balance disabled)" << std::endl;
            } else {
                std::cout << "[ WARNING ] Unable to disable automatic white balance (enable manual white balance)" << std::endl;
            }
        }
    } else {
        std::cout << "[ WARNING ] Unable to set white balance mode" << std::endl;
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

void SpinCamera::SetBlueBalanceRatio(float user_option) {

    // Ensure nodemap exists
    if (!nodeMap) {
        std::cout << "[ WARNING ] Node map is not initialized." << std::endl;
        return;
    }

    // Ensure automatic white balance is off to allow manual setting
    CEnumerationPtr ptrBalanceWhiteAuto = nodeMap->GetNode("BalanceWhiteAuto");
    if (IsReadable(ptrBalanceWhiteAuto) && IsWritable(ptrBalanceWhiteAuto)) {
        // Attempt to disable automatic white balance
        CEnumEntryPtr ptrBalanceWhiteAutoOff = ptrBalanceWhiteAuto->GetEntryByName("Off");
        if (IsReadable(ptrBalanceWhiteAutoOff)) {
            ptrBalanceWhiteAuto->SetIntValue(ptrBalanceWhiteAutoOff->GetValue());
            std::cout << "Manual White Balance Enabled (Automatic white balance disabled)" << std::endl;
        } else {
            std::cout << "[ WARNING ] Unable to disable automatic white balance (enable manual white balance)" << std::endl;
        }
    } else {
        std::cout << "[ WARNING ] Unable to set white balance mode" << std::endl;
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
        const float& blueBalanceValue = user_option;

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
