// Module for interfacing with the I2C interface

#include "platform.h"
#include <string.h>
#include <ctype.h>

/* platform variables */
const int Fast = PLATFORM_I2C_SPEED_FAST;
const int Slow = PLATFORM_I2C_SPEED_SLOW;
const int Transmitter = PLATFORM_I2C_DIRECTION_TRANSMITTER;
const int Receiver = PLATFORM_I2C_DIRECTION_RECEIVER;
