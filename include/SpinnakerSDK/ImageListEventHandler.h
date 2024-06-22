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

#ifndef FLIR_SPINNAKER_IMAGE_LIST_EVENT_HANDLER_H
#define FLIR_SPINNAKER_IMAGE_LIST_EVENT_HANDLER_H

#include "Interface/IImageListEventHandler.h"

namespace Spinnaker
{
    /**
     *  @defgroup SpinnakerEventClasses Spinnaker EventHandler Classes
     */

    /**@{*/

    /**
     *  @defgroup ImageListEventHandler_h ImageListEventHandler Class
     */

    /**@{*/

    /**
     * @brief A handler for capturing image list arrival events.
     */

    class SPINNAKER_API ImageListEventHandler : public IImageListEventHandler
    {
      public:
        /**
         * Default Constructor
         */
        ImageListEventHandler();

        /**
         * Virtual Destructor
         */
        virtual ~ImageListEventHandler();

      protected:
        /**
         * Assignment operator.
         */
        ImageListEventHandler& operator=(const ImageListEventHandler&);

        /**
         * Image list event callback
         *
         * @param imageList The ImageList object
         */
        virtual void OnImageListEvent(ImageList imageList) = 0;
    };

    /**@}*/

    /**@}*/
} // namespace Spinnaker

#endif // FLIR_SPINNAKER_IMAGE_LIST_EVENT_HANDLER_H