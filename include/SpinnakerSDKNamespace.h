#ifndef SPINNAKER_SDK_NAMESPACE_H
#define SPINNAKER_SDK_NAMESPACE_H

#include <unordered_map>
#include <string>

namespace SpinOption {
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

    // Available standard preset options for ImageDimensions (Pixels)
    // NOTE: You may also set it to any integer/double (custom)
    enum class ImageDimensions {
        Preset_1440x1080,  // Width: 1440  -  Height: 1080
        Preset_1080x1440,  // Width: 1080  -  Height: 1440
        Preset_1280x960,   // Width: 1280  -  Height: 960
        Preset_960x1280,   // Width: 960   -  Height: 1280
        Preset_720x540,    // Width: 720   -  Height: 540
        Preset_540x720,    // Width: 540   -  Height: 720
        Preset_640x480,    // Width: 640   -  Height: 480
        Preset_480x640,    // Width: 480   -  Height: 640
        Preset_320x240,    // Width: 320   -  Height: 240
        Preset_240x320     // Width: 240   -  Height: 320
    };

    // Available standard preset options for GainSensitivity
    // Gain sensitivity controls the amplification of the image sensor's signal, effectively increasing the sensor's sensitivity to light
    // This is similar to ISO in photography, which adjusts the sensor's sensitivity, but Gain and ISO measure slightly different aspects
    // While ISO is a standardized scale used in photography, Gain is measured in decibels (dB) and provides a continuous adjustment more commonly used in video and industrial imaging
    // Higher Gain values increase image brightness but may also introduce noise, similar to higher ISO settings
    // General conversion: A 6 dB increase in Gain approximately doubles the ISO sensitivity, with a typical starting base ISO of 100
    // NOTE: Custom Gain values can also be set using any float value
    enum class GainSensitivity {
        Auto,         // Gain auto mode
        Preset_0dB,   // Approx. ISO 100
        Preset_3dB,   // Intermediate value between ISO 100 and ISO 200
        Preset_6dB,   // Approx. ISO 200
        Preset_9dB,   // Intermediate value between ISO 200 and ISO 400
        Preset_12dB,  // Approx. ISO 400
        Preset_15dB,  // Intermediate value between ISO 400 and ISO 800
        Preset_18dB,  // Approx. ISO 800
        Preset_21dB,  // Intermediate value between ISO 800 and ISO 1600
        Preset_24dB,  // Approx. ISO 1600
        Preset_27dB,  // Intermediate value between ISO 1600 and ISO 3200
        Preset_30dB,  // Approx. ISO 3200
        Preset_33dB,  // Intermediate value between ISO 3200 and ISO 6400
        Preset_36dB,  // Approx. ISO 6400
        Preset_39dB,  // Intermediate value between ISO 6400 and ISO 12800
        Preset_42dB,  // Approx. ISO 12800

        // Not as accurate presets, but maybe more commonly used phrasing
        ISO_100,     // Approx. 0dB
        ISO_200,     // Approx. 6dB
        ISO_400,     // Approx. 12dB
        ISO_800,     // Approx. 18dB
        ISO_1600,    // Approx. 24dB
        ISO_3200,    // Approx. 30dB
        ISO_6400,    // Approx. 36dB
        ISO_12800    // Approx. 42dB
    };

    // Available standard preset options for GammaCorrection
    // Gamma correction adjusts the brightness of the image, correcting the non-linear relationship between pixel values and their actual displayed intensity.
    // This can enhance image quality by making details in shadows and highlights more visible; affecting contrast and brightness
    enum class GammaCorrection {
        Disable,       // Gamma correction is disabled
        Preset_0_00,   // Gamma 0.00 (Values < 1 darken the image, making shadows more pronounced)
        Preset_0_25,   // Gamma 0.25 (Values < 1 darken the image, making shadows more pronounced)
        Preset_0_50,   // Gamma 0.50 (Values < 1 darken the image, making shadows more pronounced)
        Preset_0_75,   // Gamma 0.75 (Values < 1 darken the image, making shadows more pronounced)
        Preset_1_00,   // Gamma 1.00 (neutral, no correction)
        Preset_1_25,   // Gamma 1.25 (Values > 1 brighten the image, making highlights more pronounced)
        Preset_1_50,   // Gamma 1.50 (Values > 1 brighten the image, making highlights more pronounced) 
        Preset_1_75,   // Gamma 1.75 (Values > 1 brighten the image, making highlights more pronounced)
        Preset_2_00,   // Gamma 2.00 (Values > 1 brighten the image, making highlights more pronounced)
        Preset_2_25,   // Gamma 2.25 (Values > 1 brighten the image, making highlights more pronounced)
        Preset_2_50,   // Gamma 2.50 (Values > 1 brighten the image, making highlights more pronounced)
        Preset_2_75,   // Gamma 2.75 (Values > 1 brighten the image, making highlights more pronounced)
        Preset_3_00    // Gamma 3.00 (Values > 1 brighten the image, making highlights more pronounced)
    };


    // Available standard preset options for BlackLevel
    // Black level adjusts the baseline brightness of the image, affecting the darkest parts of the image.
    // This can enhance image quality by making details in dark areas more visible.
    enum class BlackLevel {
        Auto,          // Black level auto mode
        Preset_0_00,   // Black level 0.00 (No adjustment)
        Preset_0_25,   // Black level 0.25
        Preset_0_50,   // Black level 0.50
        Preset_0_75,   // Black level 0.75
        Preset_1_00,   // Black level 1.00
        Preset_1_25,   // Black level 1.25
        Preset_1_50,   // Black level 1.50
        Preset_1_75,   // Black level 1.75
        Preset_2_00,   // Black level 2.00
    };

    // Available standard preset options for RedBalanceRatio
    // Red balance adjusts the intensity of the red channel in the image.
    enum class RedBalanceRatio {
        Auto,            // Automatic white balance (balances blue and red)
        Preset_0_00,     // Red Balance 0.00
        Preset_0_25,     // Red Balance 0.25
        Preset_0_50,     // Red Balance 0.50
        Preset_0_75,     // Red Balance 0.75
        Preset_1_00,     // Red Balance 1.00
        Preset_1_25,     // Red Balance 1.25
        Preset_1_50,     // Red Balance 1.50
        Preset_1_75,     // Red Balance 1.75
        Preset_2_00,     // Red Balance 2.00
        Preset_2_25,     // Red Balance 2.25
        Preset_2_50,     // Red Balance 2.50
        Preset_2_75,     // Red Balance 2.75
        Preset_3_00      // Red Balance 3.00
    };

    // Available standard preset options for BlueBalanceRatio
    // Blue balance adjusts the intensity of the blue channel in the image.
    enum class BlueBalanceRatio {
        Auto,            // Automatic white balance (balances blue and red)
        Preset_0_00,     // Blue Balance 0.00
        Preset_0_25,     // Blue Balance 0.25
        Preset_0_50,     // Blue Balance 0.50
        Preset_0_75,     // Blue Balance 0.75
        Preset_1_00,     // Blue Balance 1.00
        Preset_1_25,     // Blue Balance 1.25
        Preset_1_50,     // Blue Balance 1.50
        Preset_1_75,     // Blue Balance 1.75
        Preset_2_00,     // Blue Balance 2.00
        Preset_2_25,     // Blue Balance 2.25
        Preset_2_50,     // Blue Balance 2.50
        Preset_2_75,     // Blue Balance 2.75
        Preset_3_00      // Blue Balance 3.00
    };

}

#endif // SPINNAKER_SDK_NAMESPACE_H
