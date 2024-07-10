#include "../include/SpinnakerSDK_SpinImage.h"

SpinImage::SpinImage(Spinnaker::ImagePtr rawImage) : rawImage(rawImage), demosaicedImage(nullptr) {
    if (rawImage) {
        imageWidth = rawImage->GetWidth();
        imageHeight = rawImage->GetHeight();
        size_t imageSize = rawImage->GetBufferSize();
        imageData.resize(imageSize);
        memcpy(imageData.data(), rawImage->GetData(), imageSize);
    } else {
        imageWidth = 0;
        imageHeight = 0;
    }
}

SpinImage::~SpinImage() {
    // Destructor
}

// Convert nanoseconds to a more readable format (hh:mm:ss.xxxxxxxxx)
std::string ConvertTimestampToReadableFormat(uint64_t timestamp) {
    // Convert timestamp to total seconds
    double totalSeconds = static_cast<double>(timestamp) / 1e9;

    // Calculate hours, minutes, seconds
    uint64_t hours = static_cast<uint64_t>(totalSeconds) / 3600;
    uint64_t minutes = (static_cast<uint64_t>(totalSeconds) % 3600) / 60;
    double seconds = totalSeconds - (hours * 3600) - (minutes * 60);

    // Create a formatted string
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":"
       << std::setw(2) << std::setfill('0') << minutes << ":"
       << std::fixed << std::setprecision(9) << seconds;
    return ss.str();
}

void SpinImage::PrintAllImageInformation() {
    if (!rawImage) {
        std::cerr << "Raw image is invalid." << std::endl;
        return;
    }
    uint64_t timestamp = rawImage->GetTimeStamp();

    std::cout << "\nImage Information:" << std::endl;
    std::cout << "Timestamp: " << timestamp << " ns (" << ConvertTimestampToReadableFormat(timestamp) << ")" << std::endl;
    std::cout << "Image ID: " << rawImage->GetID() << std::endl;
    std::cout << "Stream Index: " << rawImage->GetStreamIndex() << std::endl;
    std::cout << "Width: " << rawImage->GetWidth() << std::endl;
    std::cout << "Height: " << rawImage->GetHeight() << std::endl;
    std::cout << "Pixel Format: " << rawImage->GetPixelFormatName() << std::endl;
    std::cout << "Image Size: " << rawImage->GetImageSize() << " bytes" << std::endl;
    std::cout << "Stride: " << rawImage->GetStride() << " bytes" << std::endl;
    std::cout << "Frame ID: " << rawImage->GetFrameID() << std::endl;
    std::cout << "Bits Per Pixel: " << rawImage->GetBitsPerPixel() << std::endl;
    std::cout << "Number of Channels: " << rawImage->GetNumChannels() << std::endl;
    std::cout << "X Offset: " << rawImage->GetXOffset() << std::endl;
    std::cout << "Y Offset: " << rawImage->GetYOffset() << std::endl;
    std::cout << "X Padding: " << rawImage->GetXPadding() << std::endl;
    std::cout << "Y Padding: " << rawImage->GetYPadding() << std::endl;
    std::cout << "Payload Type: " << rawImage->GetPayloadType() << std::endl;
    std::cout << "Transport Layer Payload Type: " << rawImage->GetTLPayloadType() << std::endl;
    std::cout << "Transport Layer Pixel Format: " << rawImage->GetTLPixelFormat() << std::endl;
    std::cout << "Transport Layer Pixel Format Namespace: " << rawImage->GetTLPixelFormatNamespace() << std::endl;
    std::cout << "Pixel Format Integer Type: " << rawImage->GetPixelFormatIntType() << std::endl;
    std::cout << "Is Incomplete: " << (rawImage->IsIncomplete() ? "Yes" : "No") << std::endl;
    std::cout << "Valid Payload Size: " << rawImage->GetValidPayloadSize() << std::endl;
    std::cout << "Chunk Layout ID: " << rawImage->GetChunkLayoutId() << std::endl;
    std::cout << "Data Absolute Max: " << rawImage->GetDataAbsoluteMax() << std::endl;
    std::cout << "Data Absolute Min: " << rawImage->GetDataAbsoluteMin() << std::endl;
}

void SpinImage::PrintSimpleImageInformation() {
    if (!rawImage) {
        std::cerr << "Raw image is invalid." << std::endl;
        return;
    }

    uint64_t timestamp = rawImage->GetTimeStamp();

    std::cout << "\nImage Information:" << std::endl;
    std::cout << "Timestamp: " << timestamp << "ns (" << ConvertTimestampToReadableFormat(timestamp) << ")" << std::endl;
    std::cout << "Image ID: " << rawImage->GetID() << std::endl;
    std::cout << "Stream Index: " << rawImage->GetStreamIndex() << std::endl;
    std::cout << "Frame ID: " << rawImage->GetFrameID() << std::endl;
}

void SpinImage::Demosaic() {
    if (!rawImage || imageData.empty()) {
        std::cerr << "Raw image is invalid or empty." << std::endl;
        return;
    }

    // Determine the pixel format of the image being demosaiced
    Spinnaker::PixelFormatEnums pixelFormat = static_cast<Spinnaker::PixelFormatEnums>(rawImage->GetPixelFormat());

    // Create a copy of the image to process
    Spinnaker::ImagePtr imageCopy = Spinnaker::Image::Create(imageWidth, imageHeight, 0, 0, rawImage->GetPixelFormat(), imageData.data());

    // Approriately process (demoasaic) the image
    switch (pixelFormat) {
        case Spinnaker::PixelFormatEnums::PixelFormat_BayerRG8:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_RGB8);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_BayerRG10p:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_RGB10p);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_BayerRG12p:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_RGB12p);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_BayerRG16:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_RGB16);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_Mono8:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_Mono8);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_Mono10p:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_Mono10p);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_Mono12p:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_Mono12p);
            break;
        case Spinnaker::PixelFormatEnums::PixelFormat_Mono16:
            demosaicedImage = imageProcessor.Convert(imageCopy, Spinnaker::PixelFormatEnums::PixelFormat_Mono16);
            break;
        default:
            std::cerr << "Unsupported pixel format for demosaicing." << std::endl;
            return;
    }
}

void SpinImage::SaveImage(const std::string& filename, Spinnaker::ImageFileFormat format) {
    if (demosaicedImage) {
        demosaicedImage->Save(filename.c_str(), format);
    } else {
        Demosaic();
        SaveImage(filename, format);
    }
}

void SpinImage::DrawRedSquare(int x, int y, int squareSize) {
    if (!demosaicedImage) {
        Demosaic();
    }
    unsigned char* imgData = static_cast<unsigned char*>(demosaicedImage->GetData());

    for (int i = -squareSize; i <= squareSize; ++i) {
        for (int j = -squareSize; j <= squareSize; ++j) {
            if (i == -squareSize || i == squareSize || j == -squareSize || j == squareSize) {
                int currentX = x + i;
                int currentY = y + j;
                if (currentX >= 0 && currentX < imageWidth && currentY >= 0 && currentY < imageHeight) {
                    int index = (currentY * imageWidth + currentX) * 3;
                    imgData[index] = 255;     // R
                    imgData[index + 1] = 0;   // G
                    imgData[index + 2] = 0;   // B
                }
            }
        }
    }
}

void SpinImage::GetPixelRGB(int x, int y, unsigned char& R, unsigned char& G, unsigned char& B) {
    unsigned char* buffer = imageData.data();
    int rowStride = imageWidth;

    // Determine the Bayer pattern based on the position
    if (x % 2 == 0 && y % 2 == 0) { // Red pixel
        R = buffer[y * rowStride + x];
        G = (buffer[y * rowStride + (x + 1)] + buffer[(y + 1) * rowStride + x]) / 2;
        B = buffer[(y + 1) * rowStride + (x + 1)];
    } else if (x % 2 == 1 && y % 2 == 0) { // Green pixel on Red row
        G = buffer[y * rowStride + x];
        R = buffer[y * rowStride + (x - 1)];
        B = buffer[(y + 1) * rowStride + x];
    } else if (x % 2 == 0 && y % 2 == 1) { // Green pixel on Blue row
        G = buffer[y * rowStride + x];
        R = buffer[(y - 1) * rowStride + x];
        B = buffer[y * rowStride + (x + 1)];
    } else { // Blue pixel
        B = buffer[y * rowStride + x];
        G = (buffer[y * rowStride + (x - 1)] + buffer[(y - 1) * rowStride + x]) / 2;
        R = buffer[(y - 1) * rowStride + (x - 1)];
    }
}

void SpinImage::CalculateAverageColor(int x, int y, int width, int height, unsigned char& R, unsigned char& G, unsigned char& B) {
    unsigned long long totalR = 0, totalG = 0, totalB = 0;
    int count = 0;

    for (int i = x; i < x + width; ++i) {
        for (int j = y; j < y + height; ++j) {
            unsigned char r, g, b;
            GetPixelRGB(i, j, r, g, b);
            totalR += r;
            totalG += g;
            totalB += b;
            count++;
        }
    }

    R = static_cast<unsigned char>(totalR / count);
    G = static_cast<unsigned char>(totalG / count);
    B = static_cast<unsigned char>(totalB / count);
}