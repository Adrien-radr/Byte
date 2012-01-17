#ifndef BYTE_DEVICE_HPP
#define BYTE_DEVICE_HPP

#include "common.h"
#include "context.h"
#include "renderer.h"
#include "camera.h"

/// Initialize the engine Device.
/// @return : True if everything went well, false if error;
bool Device_init();

/// Destroy the engine Device and all engine parts
void Device_destroy();

/// Call functions that need to be at the begining of every rendering frame
void Device_beginFrame();

/// Call functions that need to be at the end of every rendering frame
void Device_endFrame();

/// Returns the frame time of the last frame
f32 Device_getFrameTime();

/// Sets the device active camera
void Device_setCamera( Camera *pCamera );

#endif // BYTE_DEVICE
