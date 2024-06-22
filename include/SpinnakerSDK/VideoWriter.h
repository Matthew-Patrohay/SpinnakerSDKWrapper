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

#ifndef FLIR_SPINNAKER_VIDEOWRITER_H
#define FLIR_SPINNAKER_VIDEOWRITER_H

#include "SpinVideoDefs.h"
#include "ImagePtr.h"
#include "ImageProcessor.h"

#include "FFMPEGVideoEncoder.h"


namespace Spinnaker
{
    namespace Video
    {
        class VideoWriter
        {
          public:
            VideoWriter();
            ~VideoWriter();

            void VideoOpen(const char* pszFileName, Spinnaker::Video::AVIOption* pOption);
            void VideoOpen(const char* pszFileName, Spinnaker::Video::MJPGOption* pOption);
            void VideoOpen(const char* pszFileName, Spinnaker::Video::H264Option* pOption);
            void VideoAppend(ImagePtr pImage);
            void VideoClose();
            void VideoOpenSplit();

            void SetMaxFileSize(unsigned int size);

          private:
            char VideoFileNameBase[1024];
            int filenameIncrement;

            /* Pointer to video engine handle. */
            struct SpinVideoHandle* pHandle;

            /* Encoder options. */
            struct SpinVideoEncoderOptions opts;

            /* The max file size in MB */
            unsigned int maxFileSize;
            unsigned int frameCount;
            unsigned int uncompressedFrameSize;
            uint64_t currFileSize;

            bool deferInitialization;

            ImageProcessor processor;

            VideoWriter(const VideoWriter&);
            void RemoveFileExtension(const char* pszFileName);
        };
    } // namespace Video
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_VIDEOWRITER_H
