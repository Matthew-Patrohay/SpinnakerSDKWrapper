#ifndef SPINNAKER_SDK_NAMESPACE_H
#define SPINNAKER_SDK_NAMESPACE_H

#include <unordered_map>
#include <string>

namespace SpinnakerSDKNamespace {
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
    const std::unordered_map<PixelFormat, std::string> pixelFormatMap = {
        {PixelFormat::BayerRG8,   "BayerRG8"},
        {PixelFormat::BayerRG10p, "BayerRG10p"},
        {PixelFormat::BayerRG12p, "BayerRG12p"},
        {PixelFormat::BayerRG16,  "BayerRG16"},
        {PixelFormat::Mono8,      "Mono8"},
        {PixelFormat::Mono10p,    "Mono10p"},
        {PixelFormat::Mono12p,    "Mono12p"},
        {PixelFormat::Mono16,     "Mono16"}
    };


    
}

#endif // SPINNAKER_SDK_NAMESPACE_H