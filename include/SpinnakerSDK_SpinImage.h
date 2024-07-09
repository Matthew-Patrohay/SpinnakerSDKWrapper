#ifndef SPINNAKER_SDK_SPINIMAGE_H
#define SPINNAKER_SDK_SPINIMAGE_H

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "SpinnakerSDK_SpinOption.h"
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

class SpinImage {
public:
    SpinImage(Spinnaker::ImagePtr rawImage);
    ~SpinImage();

    void PrintAllImageInformation();
    void PrintSimpleImageInformation();
    void Demosaic();
    void SaveImage(const std::string& filename, Spinnaker::ImageFileFormat format = Spinnaker::ImageFileFormat::SPINNAKER_IMAGE_FILE_FORMAT_FROM_FILE_EXT);
    void DrawRedSquare(int x, int y, int squareSize);
    void GetPixelRGB(int x, int y, unsigned char& R, unsigned char& G, unsigned char& B);
    void CalculateAverageColor(int x, int y, int width, int height, unsigned char& R, unsigned char& G, unsigned char& B);

private:
    Spinnaker::ImagePtr rawImage;
    Spinnaker::ImagePtr demosaicedImage;
    Spinnaker::ImageProcessor imageProcessor;
    int imageWidth;
    int imageHeight;
    std::vector<unsigned char> imageData; // Local copy of image data
};

#endif // SPINNAKER_SDK_SPINIMAGE_H