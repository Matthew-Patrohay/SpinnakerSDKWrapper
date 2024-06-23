#ifndef SPINNAKER_SDK_NAMESPACE_H
#define SPINNAKER_SDK_NAMESPACE_H

#include <unordered_map>
#include <string>

namespace SpinOptions {
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

    // Available pixel binning formats
    // Pixel binning is the process of combining the charge from adjacent pixels into a single pixel. 
    // This effectively reduces the resolution of the sensor but increases the signal-to-noise ratio 
    // and sensitivity of the image. This requires more compute power than decimation
    enum class Binning {
        NoBinning,         // No binning (1x1 binning), the full sensor resolution is used.
        TwoByTwoBinning,   // Combine every 2x2 block of pixels into one (reduces resolution by a factor of 4).
        FourByFourBinning  // Combine every 4x4 block of pixels into one (reduces resolution by a factor of 16).
    };

    // Available pixel decimation formats
    // Pixel decimation reduces the resolution of the sensor by discarding pixels at regular intervals.
    // This effectively reduces the data rate and the resolution without increasing the signal-to-noise ratio.
    enum class Decimation {
        NoDecimation,         // No decimation (1x1), the full sensor resolution is used.
        TwoByTwoDecimation,   // Discard every other pixel in both horizontal and vertical directions.
        FourByFourDecimation  // Discard three out of every four pixels in both horizontal and vertical directions.
    };


    // Available standard preset options for exposure time
    // NOTE: You may also set it to any integer/double (custom)
    enum class ExposureTime {
        Auto,                 // Automatically set by the camera
        MinimumValue,         // The minimum allowable exposure time (4 us) - Max framerate: 250,000 fps
        MaximumValue,         // The maximum allowable exposure time (30 s) - Max framerate: 0.033 fps
        Preset_10us,          // 10 microseconds - Max framerate: 100,000 fps
        Preset_20us,          // 20 microseconds - Max framerate: 50,000 fps
        Preset_50us,          // 50 microseconds - Max framerate: 20,000 fps
        Preset_100us,         // 100 microseconds - Max framerate: 10,000 fps
        Preset_200us,         // 200 microseconds - Max framerate: 5,000 fps
        Preset_500us,         // 500 microseconds - Max framerate: 2,000 fps
        Preset_1ms,           // 1 millisecond (1000 microseconds) - Max framerate: 1,000 fps
        Preset_2ms,           // 2 milliseconds (2000 microseconds) - Max framerate: 500 fps
        Preset_5ms,           // 5 milliseconds (5000 microseconds) - Max framerate: 200 fps
        Preset_10ms,          // 10 milliseconds - Max framerate: 100 fps
        Preset_20ms,          // 20 milliseconds - Max framerate: 50 fps
        Preset_50ms,          // 50 milliseconds - Max framerate: 20 fps
        Preset_100ms,         // 100 milliseconds - Max framerate: 10 fps
        Preset_200ms,         // 200 milliseconds - Max framerate: 5 fps
        Preset_500ms,         // 500 milliseconds - Max framerate: 2 fps
        Preset_1s,            // 1 second - Max framerate: 1 fps
        Preset_2s,            // 2 seconds - Max framerate: 0.5 fps
        Preset_5s,            // 5 seconds - Max framerate: 0.2 fps
        Preset_10s,           // 10 seconds - Max framerate: 0.1 fps
        Preset_20s,           // 20 seconds - Max framerate: 0.05 fps
        Shutter_1_1000,       // 1/1000 seconds (1000 microseconds) - Max framerate: 1000 fps
        Shutter_1_500,        // 1/500 seconds (2000 microseconds) - Max framerate: 500 fps
        Shutter_1_250,        // 1/250 seconds (4000 microseconds) - Max framerate: 250 fps
        Shutter_1_125,        // 1/125 seconds (8000 microseconds) - Max framerate: 125 fps
        Shutter_1_60,         // 1/60 seconds (16667 microseconds) - Max framerate: 60 fps
        Shutter_1_30,         // 1/30 seconds (33333 microseconds) - Max framerate: 30 fps
        Shutter_1_15,         // 1/15 seconds (66667 microseconds) - Max framerate: 15 fps
        Shutter_1_8,          // 1/8 seconds (125000 microseconds) - Max framerate: 8 fps
        Shutter_1_4,          // 1/4 seconds (250000 microseconds) - Max framerate: 4 fps
        Shutter_1_2,          // 1/2 seconds (500000 microseconds) - Max framerate: 2 fps
        Shutter_1_1           // 1 second (1000000 microseconds) - Max framerate: 1 fps
    };

}

#endif // SPINNAKER_SDK_NAMESPACE_H