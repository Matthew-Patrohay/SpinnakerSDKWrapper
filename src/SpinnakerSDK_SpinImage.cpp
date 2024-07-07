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
    int rowStride = imageWidth * 3;

    unsigned char* pixel = &buffer[y * rowStride + x];
    if (x % 2 == 0 && y % 2 == 0) {
        R = *pixel;
        G = (*(pixel + 1) + *(pixel + rowStride)) / 2;
        B = *(pixel + rowStride + 1);
    } else if (x % 2 == 1 && y % 2 == 0) {
        G = *pixel;
        R = *(pixel - 1);
        B = *(pixel + rowStride);
    } else if (x % 2 == 0 && y % 2 == 1) {
        G = *pixel;
        R = *(pixel + rowStride);
        B = *(pixel - 1);
    } else {
        B = *pixel;
        G = (*(pixel - 1) + *(pixel - rowStride)) / 2;
        R = *(pixel - rowStride - 1);
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