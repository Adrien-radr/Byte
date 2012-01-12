#ifndef BYTE_DEVICE_HPP
#define BYTE_DEVICE_HPP

#include "common.h"

bool Device_init();
void Device_destroy();


void Device_beginFrame();
void Device_endFrame();

#endif // BYTE_DEVICE
