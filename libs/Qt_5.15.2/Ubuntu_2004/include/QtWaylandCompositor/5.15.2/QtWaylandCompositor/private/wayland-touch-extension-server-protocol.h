/* Generated by wayland-scanner 1.18.0 */

#ifndef TOUCH_EXTENSION_SERVER_PROTOCOL_H
#define TOUCH_EXTENSION_SERVER_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "wayland-server-core.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct wl_client;
struct wl_resource;

/**
 * @page page_touch_extension The touch_extension protocol
 * @section page_ifaces_touch_extension Interfaces
 * - @subpage page_iface_qt_touch_extension - 
 * @section page_copyright_touch_extension Copyright
 * <pre>
 *
 * Copyright (C) 2015 The Qt Company Ltd.
 * Contact: http://www.qt.io/licensing/
 *
 * This file is part of the plugins of the Qt Toolkit.
 *
 * $QT_BEGIN_LICENSE:BSD$
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of The Qt Company Ltd nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 *
 * $QT_END_LICENSE$
 * </pre>
 */
struct qt_touch_extension;

/**
 * @page page_iface_qt_touch_extension qt_touch_extension
 * @section page_iface_qt_touch_extension_api API
 * See @ref iface_qt_touch_extension.
 */
/**
 * @defgroup iface_qt_touch_extension The qt_touch_extension interface
 */
extern const struct wl_interface qt_touch_extension_interface;

#ifndef QT_TOUCH_EXTENSION_FLAGS_ENUM
#define QT_TOUCH_EXTENSION_FLAGS_ENUM
enum qt_touch_extension_flags {
	QT_TOUCH_EXTENSION_FLAGS_MOUSE_FROM_TOUCH = 0x1,
};
#endif /* QT_TOUCH_EXTENSION_FLAGS_ENUM */

/**
 * @ingroup iface_qt_touch_extension
 * @struct qt_touch_extension_interface
 */
struct qt_touch_extension_interface {
	/**
	 */
	void (*dummy)(struct wl_client *client,
		      struct wl_resource *resource);
};

#define QT_TOUCH_EXTENSION_TOUCH 0
#define QT_TOUCH_EXTENSION_CONFIGURE 1

/**
 * @ingroup iface_qt_touch_extension
 */
#define QT_TOUCH_EXTENSION_TOUCH_SINCE_VERSION 1
/**
 * @ingroup iface_qt_touch_extension
 */
#define QT_TOUCH_EXTENSION_CONFIGURE_SINCE_VERSION 1

/**
 * @ingroup iface_qt_touch_extension
 */
#define QT_TOUCH_EXTENSION_DUMMY_SINCE_VERSION 1

/**
 * @ingroup iface_qt_touch_extension
 * Sends an touch event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
qt_touch_extension_send_touch(struct wl_resource *resource_, uint32_t time, uint32_t id, uint32_t state, int32_t x, int32_t y, int32_t normalized_x, int32_t normalized_y, int32_t width, int32_t height, uint32_t pressure, int32_t velocity_x, int32_t velocity_y, uint32_t flags, struct wl_array *rawdata)
{
	wl_resource_post_event(resource_, QT_TOUCH_EXTENSION_TOUCH, time, id, state, x, y, normalized_x, normalized_y, width, height, pressure, velocity_x, velocity_y, flags, rawdata);
}

/**
 * @ingroup iface_qt_touch_extension
 * Sends an configure event to the client owning the resource.
 * @param resource_ The client's resource
 */
static inline void
qt_touch_extension_send_configure(struct wl_resource *resource_, uint32_t flags)
{
	wl_resource_post_event(resource_, QT_TOUCH_EXTENSION_CONFIGURE, flags);
}

#ifdef  __cplusplus
}
#endif

#endif