// Module for interfacing with PIO

#include "platform.h"
#include "platform_conf.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* PIO public constants */
#define PIO_DIR_OUTPUT      0
#define PIO_DIR_INPUT       1

/* PIO private constants */
#define PIO_PORT_OP         0
#define PIO_PIN_OP          1
