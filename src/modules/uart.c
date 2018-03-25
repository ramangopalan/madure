// Module for interfacing with UART

#include "platform.h"
#include "common.h"
#include "sermux.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "platform_conf.h"

/* max size for uart recv buffer */
#define MYBASIC_UART_BUF (100U)

/* Modes for the UART read function */
enum {
	UART_READ_MODE_LINE,
  	UART_READ_MODE_NUMBER,
  	UART_READ_MODE_SPACE,
  	UART_READ_MODE_MAXSIZE
};

/* platform variables */
const int ParEven = PLATFORM_UART_PARITY_EVEN;
const int ParOdd = PLATFORM_UART_PARITY_ODD;
const int ParNone = PLATFORM_UART_PARITY_NONE;
const int Stop_1 = PLATFORM_UART_STOPBITS_1;
const int Stop_1_5 = PLATFORM_UART_STOPBITS_1_5;
const int Stop_2 = PLATFORM_UART_STOPBITS_2;
const int NoTime = 0;
const int InfTime = PLATFORM_UART_INFINITE_TIMEOUT;
const int FlowNone = PLATFORM_UART_FLOW_NONE;
const int FlowRts = PLATFORM_UART_FLOW_RTS;
const int FlowCts = PLATFORM_UART_FLOW_CTS;
