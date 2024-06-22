//=============================================================================
// Copyright (c) 2001-2024 FLIR Systems, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

#ifndef FLIR_SPINNAKER_DEFS_C_H
#define FLIR_SPINNAKER_DEFS_C_H

#include "SpinnakerPlatformC.h"
#include <memory.h>

/**
 * @defgroup SpinnakerDefsC Spinnaker C Definitions
 * @ingroup SpinnakerC
 *
 * Definitions for Spinnaker C API
 *
 * Holds enumerations, typedefs and structures that are used across the
 * Spinnaker C API wrapper.
 */
/**@{*/

typedef uint8_t bool8_t;
static const bool8_t False = 0;
static const bool8_t True = 1;

/**
 * @defgroup SpinnakerHandlesC Spinnaker C Handles
 *
 * Spinnaker C handle definitions
 */
/**@{*/

/**
 * Handle for system functionality. Created by calling
 * spinSystemGetInstance(), which requires a call to
 * spinSystemReleaseInstance() to release.
 */
typedef void* spinSystem;

/**
 * Handle for interface list functionality. Created by calling
 * spinSystemGetInterfaces(), which requires a call to
 * spinInterfaceListClear() to clear, or spinInterfaceListCreateEmpty(),
 * which requires a call to spinInterfaceListDestroy() to destroy.
 */
typedef void* spinInterfaceList;

/**
 * Handle for interface functionality. Created by calling
 * spinInterfaceListGet(), which requires a call to
 * spinInterfaceRelease() to release.
 */
typedef void* spinInterface;

/**
 * Handle for interface functionality. Created by calling
 * spinSystemGetCameras() or spinInterfaceGetCameras(), which require
 * a call to spinCameraListClear() to clear, or
 * spinCameraListCreateEmpty(), which requires a call to
 * spinCameraListDestroy() to destroy.
 */
typedef void* spinCameraList;

/**
 * Handle for camera functionality. Created by calling
 * spinCameraListGet(), which requires a call to spinCameraRelease() to
 * release.
 */
typedef void* spinCamera;

/**
 * Handle for image functionality. Created by calling
 * spinCameraGetNextImage() or spinCameraGetNextImageEx(), which require
 * a call to spinImageRelease() to remove from buffer, or
 * spinImageCreateEmpty(), spinImageCreateEx(), or spinImageCreate(),
 * which require a call to spinImageDestroy() to destroy.
 */
typedef void* spinImage;

/**
 * Handle for image list functionality. Created by calling
 * spinCameraGetNextImageSync(), which require a call to
 * spinImageRelease() to remove from buffer, or
 * spinImageCreateEmpty(), spinImageCreateEx(), or spinImageCreate(),
 * which require a call to spinImageDestroy() to destroy.
 */
typedef void* spinImageList;

/**
 * Handle for image processor functionality. Created by calling
 * spinImageProcessorCreate(), which requires a call to
 * spinImageProcessorDestroy() to destroy.
 */
typedef void* spinImageProcessor;

/**
 * Handle for image statistics functionality. Created by calling
 * spinImageStatisticsCreate(), which requires a call to
 * spinImageStatisticsDestroy() to destroy.
 */
typedef void* spinImageStatistics;

/**
 * Handle for device event handler functionality. Created by calling
 * spinDeviceEventHandlerCreate(), which requires a call to spinDeviceEventHandlerDestroy()
 * to destroy.
 */
typedef void* spinDeviceEventHandler;

/**
 * Handle for image event handler functionality. Created by calling
 * spinImageEventHandlerCreate(), which requires a call to spinImageEventHandlerDestroy()
 * to destroy.
 */
typedef void* spinImageEventHandler;

/**
 * Handle for image list event handler functionality. Created by calling
 * spinImageListEventHandlerCreate(), which requires a call to spinImageListEventHandlerDestroy()
 * to destroy.
 */
typedef void* spinImageListEventHandler;

/**
 * Handle for arrival event handler functionality. Created by calling
 * spinArrivalEventCreate(), which requires a call to
 * spinDeviceArrivalEventHandlerDestroy() to destroy.
 */
typedef void* spinDeviceArrivalEventHandler;

/**
 * Handle for removal event handler functionality. Created by calling
 * spinDeviceRemovalEventHandlerCreate(), which requires a call to
 * spinDeviceRemovalEventHandlerDestroy() to destroy.
 */
typedef void* spinDeviceRemovalEventHandler;

/**
 * Handle for interface event handler functionality. Created by calling
 * spinInterfaceEventHandlerCreate(), which requires a call to
 * spinInterfaceEventHandlerDestroy() to destroy.
 */
typedef void* spinInterfaceEventHandler;

/**
 * Handle for logging event handler functionality. Created by calling
 * spinLogEventHandlerCreate(), which requires a call to spinLogEventHandlerDestroy()
 * to destroy.
 */
typedef void* spinLogEventHandler;

/**
 * Handle for logging event data functionality. Received in log event
 * function. No need to release, clear, or destroy.
 */
typedef void* spinLogEventData;

/**
 * Handle for device event data functionality. Received in device event
 * function. No need to release, clear, or destroy.
 */
typedef void* spinDeviceEventData;

/**
 * Handle for video recording functionality. Created by calling
 * spinVideoOpenUncompressed(), spinVideoOpenMJPG(), and
 * spinVideoOpenH264(), which require a call to
 * spinVideoClose() to destroy.
 */
typedef void* spinVideo;

/**@}*/

/**
 * @defgroup SpinnakerFuncSignatureC Spinnaker C Function Signatures
 *
 * Spinnaker C function signature definitions
 */
/**@{*/

/**
 * Function signatures are used to create and trigger callbacks and
 * events.
 */
typedef void (*spinDeviceEventFunction)(const spinDeviceEventData hEventData, const char* pEventName, void* pUserData);

typedef void (*spinImageEventFunction)(const spinImage hImage, void* pUserData);

typedef void (*spinImageListEventFunction)(const spinImageList hImage, void* pUserData);

typedef void (*spinArrivalEventFunction)(const spinCamera hCamera, void* pUserData);

typedef void (*spinRemovalEventFunction)(const spinCamera hCamera, void* pUserData);

typedef void (*spinLogEventFunction)(const spinLogEventData hEventData, void* pUserData);

/**@}*/

/**
 * @defgroup SpinnakerEnumsC Spinnaker C Enumerations
 *
 * Spinnaker C enumumeration definitions
 */
/**@{*/

/**
 * The error codes used in Spinnaker C.  These codes are returned from every
 * function in Spinnaker C.
 */

/** The error codes in the range of -2000 to -2999 are reserved
 * for GenICam related errors.  The error codes in the range of -3000 to -3999
 * are reserved for image processing related errors.
 */
typedef enum _spinError
{
    /**
     * An error code of 0 means that the function has run without error.
     */
    SPINNAKER_ERR_SUCCESS = 0,

    /**
     * The error codes in the range of -1000 to -1999 are reserved for
     * Spinnaker exceptions.
     */
    SPINNAKER_ERR_ERROR = -1001,
    SPINNAKER_ERR_NOT_INITIALIZED = -1002,
    SPINNAKER_ERR_NOT_IMPLEMENTED = -1003,
    SPINNAKER_ERR_RESOURCE_IN_USE = -1004,
    SPINNAKER_ERR_ACCESS_DENIED = -1005,
    SPINNAKER_ERR_INVALID_HANDLE = -1006,
    SPINNAKER_ERR_INVALID_ID = -1007,
    SPINNAKER_ERR_NO_DATA = -1008,
    SPINNAKER_ERR_INVALID_PARAMETER = -1009,
    SPINNAKER_ERR_IO = -1010,
    SPINNAKER_ERR_TIMEOUT = -1011,
    SPINNAKER_ERR_ABORT = -1012,
    SPINNAKER_ERR_INVALID_BUFFER = -1013,
    SPINNAKER_ERR_NOT_AVAILABLE = -1014,
    SPINNAKER_ERR_INVALID_ADDRESS = -1015,
    SPINNAKER_ERR_BUFFER_TOO_SMALL = -1016,
    SPINNAKER_ERR_INVALID_INDEX = -1017,
    SPINNAKER_ERR_PARSING_CHUNK_DATA = -1018,
    SPINNAKER_ERR_INVALID_VALUE = -1019,
    SPINNAKER_ERR_RESOURCE_EXHAUSTED = -1020,
    SPINNAKER_ERR_OUT_OF_MEMORY = -1021,
    SPINNAKER_ERR_BUSY = -1022,

    /**
     * The error codes in the range of -2000 to -2999 are reserved for
     * Gen API related errors.
     */
    SPINNAKER_ERR_GENICAM_INVALID_ARGUMENT = -2001,
    SPINNAKER_ERR_GENICAM_OUT_OF_RANGE = -2002,
    SPINNAKER_ERR_GENICAM_PROPERTY = -2003,
    SPINNAKER_ERR_GENICAM_RUN_TIME = -2004,
    SPINNAKER_ERR_GENICAM_LOGICAL = -2005,
    SPINNAKER_ERR_GENICAM_ACCESS = -2006,
    SPINNAKER_ERR_GENICAM_TIMEOUT = -2007,
    SPINNAKER_ERR_GENICAM_DYNAMIC_CAST = -2008,
    SPINNAKER_ERR_GENICAM_GENERIC = -2009,
    SPINNAKER_ERR_GENICAM_BAD_ALLOCATION = -2010,

    /**
     * The error codes in the range of -3000 to -3999 are reserved for
     * image processing related errors.
     */
    SPINNAKER_ERR_IM_CONVERT = -3001,
    SPINNAKER_ERR_IM_COPY = -3002,
    SPINNAKER_ERR_IM_MALLOC = -3003,
    SPINNAKER_ERR_IM_NOT_SUPPORTED = -3004,
    SPINNAKER_ERR_IM_HISTOGRAM_RANGE = -3005,
    SPINNAKER_ERR_IM_HISTOGRAM_MEAN = -3006,
    SPINNAKER_ERR_IM_MIN_MAX = -3007,
    SPINNAKER_ERR_IM_COLOR_CONVERSION = -3008,

    /**
     * Error codes less than -10000 are reserved for user-defined custom
     * errors.
     */
    SPINNAKER_ERR_CUSTOM_ID = -10000
} spinError;

/**
 * Color processing algorithms. Please refer to our knowledge base at
 * article at
 * https://www.flir.com/support-center/iis/machine-vision/knowledge-base/different-color-processing-algorithms-in-flycapture2
 * for complete details for each algorithm.
 */
typedef enum _spinColorProcessingAlgorithm
{
    /** No color processing. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_NONE,
    /**
     * Fastest but lowest quality. Equivalent to
     * FLYCAPTURE_NEAREST_NEIGHBOR_FAST in FlyCapture.
     */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_NEAREST_NEIGHBOR,
    /**
     * Nearest Neighbor with averaged green pixels. Higher quality but slower
     * compared to nearest neighbor without averaging.
     */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_NEAREST_NEIGHBOR_AVG,
    /** Weighted average of surrounding 4 pixels in a 2x2 neighborhood. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_BILINEAR,
    /** Weights surrounding pixels based on localized edge orientation. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_EDGE_SENSING,
    /** Well-balanced speed and quality. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR,
    /** Multi-threaded with similar results to edge sensing. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_IPP,
    /** Best quality but much faster than rigorous. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_DIRECTIONAL_FILTER,
    /** Slowest but produces good results. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_RIGOROUS,
    /** Weighted pixel average from different directions. */
    SPINNAKER_COLOR_PROCESSING_ALGORITHM_WEIGHTED_DIRECTIONAL_FILTER
} spinColorProcessingAlgorithm;

/**
 * Channels that allow statistics to be calculated.
 */
typedef enum _spinStatisticsChannel
{
    SPINNAKER_STATISTICS_CHANNEL_GREY,
    SPINNAKER_STATISTICS_CHANNEL_RED,
    SPINNAKER_STATISTICS_CHANNEL_GREEN,
    SPINNAKER_STATISTICS_CHANNEL_BLUE,
    SPINNAKER_STATISTICS_CHANNEL_HUE,
    SPINNAKER_STATISTICS_CHANNEL_SATURATION,
    SPINNAKER_STATISTICS_CHANNEL_LIGHTNESS,
    SPINNAKER_STATISTICS_CHANNEL_NUM_CHANNELS
} spinStatisticsChannel;

/** File formats to be used for saving images to disk. */
typedef enum _spinImageFileFormat
{
    SPINNAKER_IMAGE_FILE_FORMAT_FROM_FILE_EXT = -1, /**< Determine file format from file extension. */
    SPINNAKER_IMAGE_FILE_FORMAT_PGM,                /**< Portable gray map. */
    SPINNAKER_IMAGE_FILE_FORMAT_PPM,                /**< Portable pixmap. */
    SPINNAKER_IMAGE_FILE_FORMAT_BMP,                /**< Bitmap. */
    SPINNAKER_IMAGE_FILE_FORMAT_JPEG,               /**< JPEG. */
    SPINNAKER_IMAGE_FILE_FORMAT_JPEG2000,           /**< JPEG 2000. */
    SPINNAKER_IMAGE_FILE_FORMAT_TIFF,               /**< Tagged image file format. */
    SPINNAKER_IMAGE_FILE_FORMAT_PNG,                /**< Portable network graphics. */
    SPINNAKER_IMAGE_FILE_FORMAT_RAW,                /**< Raw data. */
    SPINNAKER_IMAGE_FILE_FORMAT_FORCE_32BITS = 0x7FFFFFFF
} spinImageFileFormat;

/**
 * This enum represents the namespace in which the TL specific pixel format
 * resides. This enum is returned from a captured image when calling
 * spinImageGetTLPixelFormatNamespace().  It can be used to interpret the raw
 * pixel format returned from spinImageGetTLPixelFormat().
 *
 * @see spinImageGetTLPixelFormat()
 *
 * @see spinImageGetTLPixelFormatNamespace()
 */
typedef enum _spinTLPixelFormatNamespace
{
    SPINNAKER_TLPIXELFORMAT_NAMESPACE_UNKNOWN = 0,    /* GenTL v1.2 */
    SPINNAKER_TLPIXELFORMAT_NAMESPACE_GEV = 1,        /* GenTL v1.2 */
    SPINNAKER_TLPIXELFORMAT_NAMESPACE_IIDC = 2,       /* GenTL v1.2 */
    SPINNAKER_TLPIXELFORMAT_NAMESPACE_PFNC_16BIT = 3, /* GenTL v1.4 */
    SPINNAKER_TLPIXELFORMAT_NAMESPACE_PFNC_32BIT = 4, /* GenTL v1.4 */

    SPINNAKER_PIXELFORMAT_NAMESPACE_CUSTOM_ID = 1000
} spinTLPixelFormatNamespace;

/** Status of images returned from spinImageGetStatus() call. */
typedef enum _spinImageStatus
{
    SPINNAKER_IMAGE_STATUS_UNKNOWN_ERROR = -1,   /**< Image has an unknown error. */
    SPINNAKER_IMAGE_STATUS_NO_ERROR = 0,         /**< Image is returned from GetNextImage() call without any errors. */
    SPINNAKER_IMAGE_STATUS_CRC_CHECK_FAILED = 1, /**< Image failed CRC check. */
    SPINNAKER_IMAGE_STATUS_DATA_OVERFLOW = 2,    /**< Received more data than the size of the image. */
    SPINNAKER_IMAGE_STATUS_MISSING_PACKETS =
        3, /**< Image has missing packets. Potential fixes include enabling
           jumbo packets and adjusting packet size/delay. For more information see
           https://www.flir.com/support-center/iis/machine-vision/application-note/troubleshooting-image-consistency-errors/
         */
    SPINNAKER_IMAGE_STATUS_LEADER_BUFFER_SIZE_INCONSISTENT =
        4, /**< Image leader is incomplete. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_TRAILER_BUFFER_SIZE_INCONSISTENT =
        5, /**< Image trailer is incomplete. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_PACKETID_INCONSISTENT =
        6, /**< Image has an inconsistent packet id. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_MISSING_LEADER =
        7, /**< Image leader is missing. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_MISSING_TRAILER =
        8, /**< Image trailer is missing. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_DATA_INCOMPLETE =
        9, /**< Image data is incomplete. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_INFO_INCONSISTENT =
        10, /**< Image info is corrupted. Could be caused by missing packet(s). See link above.*/
    SPINNAKER_IMAGE_STATUS_CHUNK_DATA_INVALID = 11, /**< Image chunk data is invalid */
    SPINNAKER_IMAGE_STATUS_NO_SYSTEM_RESOURCES = 12 /**< Image cannot be processed due to lack of system
                                   resources. */
} spinImageStatus;

/** log levels */
typedef enum _spinLogLevel
{
    SPINNAKER_LOG_LEVEL_OFF = -1,     // Logging is off.
    SPINNAKER_LOG_LEVEL_FATAL = 0,    // Failures that are non-recoverable without user intervention.
    SPINNAKER_LOG_LEVEL_ALERT = 100,  // Not used by Spinnaker.
    SPINNAKER_LOG_LEVEL_CRIT = 200,   // Not used by Spinnaker.
    SPINNAKER_LOG_LEVEL_ERROR = 300,  // Failures that may or may not be recoverable without user
                                      // intervention (use case dependent).
    SPINNAKER_LOG_LEVEL_WARN = 400,   // Failures that are recoverable without user intervention.
    SPINNAKER_LOG_LEVEL_NOTICE = 500, // Events such as camera arrival and removal, initialization
                                      // and deinitialization, starting and stopping image acquisition,
                                      // and feature modification.
    SPINNAKER_LOG_LEVEL_INFO = 600,   // Information about recurring events that are generated regularly
                                      // such as information on individual images.
    SPINNAKER_LOG_LEVEL_DEBUG = 700,  // Information that can be used to troubleshoot the system.
    SPINNAKER_LOG_LEVEL_NOTSET = 800  // Logs everything.
} spinnakerLogLevel;

/* Enumeration of TLType dependent payload types. Introduced in GenTL v1.2 */
typedef enum _spinTLPayloadType
{
    SPINNAKER_TLPAYLOAD_TYPE_UNKNOWN = 0,         /* GenTL v1.2 */
    SPINNAKER_TLPAYLOAD_TYPE_IMAGE = 1,           /* GenTL v1.2 */
    SPINNAKER_TLPAYLOAD_TYPE_RAW_DATA = 2,        /* GenTL v1.2 */
    SPINNAKER_TLPAYLOAD_TYPE_FILE = 3,            /* GenTL v1.2 */
    SPINNAKER_TLPAYLOAD_TYPE_CHUNK_DATA = 4,      /* GenTL v1.2, Deprecated in GenTL 1.5*/
    SPINNAKER_TLPAYLOAD_TYPE_JPEG = 5,            /* GenTL v1.4 */
    SPINNAKER_TLPAYLOAD_TYPE_JPEG2000 = 6,        /* GenTL v1.4 */
    SPINNAKER_TLPAYLOAD_TYPE_H264 = 7,            /* GenTL v1.4 */
    SPINNAKER_TLPAYLOAD_TYPE_CHUNK_ONLY = 8,      /* GenTL v1.4 */
    SPINNAKER_TLPAYLOAD_TYPE_DEVICE_SPECIFIC = 9, /* GenTL v1.4 */
    SPINNAKER_TLPAYLOAD_TYPE_MULTI_PART = 10,     /* GenTL v1.5 */

    SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID = 1000, /* Starting value for GenTL Producer custom IDs*/
    SPINNAKER_TLPAYLOAD_TYPE_LOSSLESS_COMPRESSED = SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID + 1,
    SPINNAKER_TLPAYLOAD_TYPE_LOSSY_COMPRESSED = SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID + 2,
    SPINNAKER_TLPAYLOAD_TYPE_JPEG_LOSSLESS_COMPRESSED = SPINNAKER_TLPAYLOAD_TYPE_CUSTOM_ID + 3
} spinTLPayloadType;

/** Compression method to use for encoding TIFF images. */
typedef enum _spinTIFFCompressionMethod
{
    SPINNAKER_TIFF_COMPRESS_METHOD_NONE = 1,      // Save without any compression.
    SPINNAKER_TIFF_COMPRESS_METHOD_PACKBITS,      // Save using PACKBITS compression.
    SPINNAKER_TIFF_COMPRESS_METHOD_DEFLATE,       // Save using DEFLATE compression (ZLIB compression).
    SPINNAKER_TIFF_COMPRESS_METHOD_ADOBE_DEFLATE, // Save using ADOBE DEFLATE compression

    // Save using CCITT Group 3 fax encoding. This is only valid for
    // 1-bit images only. Default to LZW for other bit depths.
    //
    SPINNAKER_TIFF_COMPRESS_METHOD_CCITTFAX3,

    // Save using CCITT Group 4 fax encoding. This is only valid for
    // 1-bit images only. Default to LZW for other bit depths.

    SPINNAKER_TIFF_COMPRESS_METHOD_CCITTFAX4,
    SPINNAKER_TIFF_COMPRESS_METHOD_LZW, //< Save using LZW compression.

    //
    // Save using JPEG compression. This is only valid for 8-bit
    // greyscale and 24-bit only. Default to LZW for other bit depths.
    //
    SPINNAKER_TIFF_COMPRESS_METHOD_JPG
} spinTIFFCompressionMethod;

/**
 * Possible Status Codes Returned from Action Command.
 */
typedef enum _spinActionCommandStatus
{
    /** The device acknowledged the command. */
    SPINNAKER_ACTION_COMMAND_STATUS_OK = 0,

    /* The device is not synchronized to a master clock to be used as time reference. Typically used when scheduled
    action commands cannot be scheduled for a future time since the reference time coming from IEEE 1588 is not
    locked. */
    SPINNAKER_ACTION_COMMAND_STATUS_NO_REF_TIME = 0x8013,

    /* Returned when the scheduled action commands queue is full and the device cannot accept the additional request. */
    SPINNAKER_ACTION_COMMAND_STATUS_OVERFLOW = 0x8015,

    /* The requested scheduled action command was requested at a point in time that is in the past. */
    SPINNAKER_ACTION_COMMAND_STATUS_ACTION_LATE = 0x8016,

    /* Generic Error. Try enabling the Extended Status Code 2.0 bit on gvcp configuration register in order to receive
    more meaningful/detailed acknowledge messages from the device. */
    SPINNAKER_ACTION_COMMAND_STATUS_ERROR = 0x8FFF
} spinActionCommandStatus;

/**@}*/

/**
 * @defgroup SpinnakerStructC Spinnaker C Structures
 *
 * Spinnaker C structure definitions
 */
/**@{*/

/**
 * Options for saving PNG images. Used in saving PNG images
 * with a call to spinImageSavePng().
 */
typedef struct _spinPNGOption
{
    /** Whether to save the PNG as interlaced. */
    bool8_t interlaced;
    /** Compression level (0-9). 0 is no compression, 9 is best compression. */
    unsigned int compressionLevel;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinPNGOption;

/**
 * Options for saving PPM images. Used in saving PPM images
 * with a call to spinImageSavePpm().
 */
typedef struct _spinPPMOption
{
    /** Whether to save the PPM as a binary file. */
    bool8_t binaryFile;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinPPMOption;

/** Options for saving PGM images. */
typedef struct _spinPGMOption
{
    /** Whether to save the PPM as a binary file. */
    bool8_t binaryFile;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinPGMOption;

/**
 * Options for saving TIFF images. Used in saving PPM images
 * with a call to spinImageSaveTiff().
 */
typedef struct _spinTIFFOption
{
    /** Compression method to use for encoding TIFF images. */
    spinTIFFCompressionMethod compression;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinTIFFOption;

/**
 * Options for saving JPEG images. Used in saving PPM images
 * with a call to spinImageSaveJpeg().
 */
typedef struct _spinJPEGOption
{
    /** Whether to save as a progressive JPEG file. */
    bool8_t progressive;
    /**
     * JPEG image quality in range (0-100).
     * - 100 - Superb quality.
     * - 75  - Good quality.
     * - 50  - Normal quality.
     * - 10  - Poor quality.
     */
    unsigned int quality;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinJPEGOption;

/**
 * Options for saving JPEG 2000 images. Used in saving PPM images
 * with a call to spinImageSaveJpg2().
 */
typedef struct _spinJPG2Option
{
    /** JPEG saving quality in range (1-512). */
    unsigned int quality;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinJPG2Option;

/**
 * Options for saving BMP images. Used in saving PPM images
 * with a call to spinImageSaveBmp().
 */
typedef struct _spinBMPOption
{
    bool8_t indexedColor_8bit;
    /** Reserved for future use. */
    unsigned int reserved[16];
} spinBMPOption;

/**
 * Options for saving MJPG videos. Used in saving MJPG videos
 * with a call to spinAVIRecorderOpenMJPG().
 */
typedef struct _spinMJPGOption
{
    /** Frame rate of the stream */
    float frameRate;

    /** Image quality (1-100) */
    unsigned int quality;

    /** Width of source image */
    unsigned int width;

    /** Height of source image */
    unsigned int height;

    unsigned int reserved[192];
} spinMJPGOption;

/**
 * Options for saving H264 videos. Used in saving H264 videos
 * with a call to spinAVIRecorderOpenH264().
 */
typedef struct _spinH264Option
{
    /** Frame rate of the stream */
    float frameRate;

    /** Width of source image */
    unsigned int width;

    /** Height of source image */
    unsigned int height;

    /** Bitrate to encode at */
    unsigned int bitrate;

    /** Reserved for future use */
    unsigned int reserved[256];

    /** Switch to using mp4 file format */
    bool8_t useMP4;

    /** Constant Rate Factor to use */
    /* Crf values range from 1 to 51 */
    unsigned int crf;
} spinH264Option;

/**
 * Options for saving uncompressed videos. Used in saving AVI videos
 * with a call to spinAVIRecorderOpenUncompressed().
 */
typedef struct _spinAVIOption
{
    /** Frame rate of the stream */
    float frameRate;

    /** Width of source image */
    unsigned int width;

    /** Height of source image */
    unsigned int height;

    unsigned int reserved[192];
} spinAVIOption;

/**
 * Provides easier access to the current version of Spinnaker.
 */
typedef struct _spinLibraryVersion
{
    /** Major version of the library **/
    unsigned int major;

    /** Minor version of the library **/
    unsigned int minor;

    /** Version type of the library **/
    unsigned int type;

    /** Build number of the library **/
    unsigned int build;
} spinLibraryVersion;

/**
 * Action Command Result.
 */
typedef struct _actionCommandResult
{
    /* IP Address of device that responded to Action Command */
    unsigned int DeviceAddress;

    /* Action Command status return from device */
    spinActionCommandStatus Status;
} actionCommandResult;

/**@}*/

/**@}*/

#endif // FLIR_SPINNAKER_DEFS_C_H