// Common code for all backends

#include "platform.h"
#include "platform_conf.h"
#include "type.h"
#include "genstd.h"
#include "common.h"
#include "buf.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "math.h"
#include "mybasic_adc.h"
#include "term.h"
#include "xmodem.h"
#include "mybasic_int.h"
#include "sermux.h"

// [TODO] the new builder should automatically do this
#if defined( BUILD_LUA_INT_HANDLERS ) || defined( BUILD_C_INT_HANDLERS )
#define BUILD_INT_HANDLERS

#ifndef INT_TMR_MATCH
#define INT_TMR_MATCH         PICOC_INT_INVALID_INTERRUPT
#endif

extern const picoc_int_descriptor picoc_int_table[ INT_PICOC_LAST ];

#endif // #if defined( BUILD_PICOC_INT_HANDLERS ) || defined( BUILD_C_INT_HANDLERS )

// [TODO] the new builder should automatically do this
#ifndef VTMR_NUM_TIMERS
#define VTMR_NUM_TIMERS       0
#endif // #ifndef VTMR_NUM_TIMERS

// [TODO] the new builder should automatically do this
#ifndef CON_BUF_SIZE
#define CON_BUF_SIZE          0
#endif // #ifndef CON_BUF_SIZE

// [TODO] the new builder should automatically do this
#ifndef SERMUX_FLOW_TYPE
#define SERMUX_FLOW_TYPE      PLATFORM_UART_FLOW_NONE
#endif

// [TODO] the new builder should automatically do this
#ifndef CON_FLOW_TYPE
#define CON_FLOW_TYPE        PLATFORM_UART_FLOW_NONE
#endif

// ****************************************************************************
// XMODEM support code

#ifdef BUILD_XMODEM

static void xmodem_send( u8 data )
{
  platform_uart_send( CON_UART_ID, data );
}

static int xmodem_recv( u32 timeout )
{
  return platform_uart_recv( CON_UART_ID, CON_TIMER_ID, timeout );
}

#endif // #ifdef BUILD_XMODEM

// ****************************************************************************
// Terminal support code

#ifdef BUILD_TERM

#define TERM_TIMEOUT    100000 

static void term_out( u8 data )
{
  platform_uart_send( CON_UART_ID, data );
}

static int term_in( int mode )
{
  if( mode == TERM_INPUT_DONT_WAIT )
    return platform_uart_recv( CON_UART_ID, CON_TIMER_ID, 0 );
  else
    return platform_uart_recv( CON_UART_ID, CON_TIMER_ID, PLATFORM_UART_INFINITE_TIMEOUT );
}

static int term_translate( int data )
{
  int c;
  
  if( isprint( data ) )
    return data;
  else if( data == 0x1B ) // escape sequence
  {
    // If we don't get a second char, we got a simple "ESC", so return KC_ESC
    // If we get a second char it must be '[', the next one is relevant for us
    if( platform_uart_recv( CON_UART_ID, CON_TIMER_ID, TERM_TIMEOUT ) == -1 )
      return KC_ESC;
    if( ( c = platform_uart_recv( CON_UART_ID, CON_TIMER_ID, TERM_TIMEOUT ) ) == -1 )
      return KC_UNKNOWN;
    if( c >= 0x41 && c <= 0x44 )
      switch( c )
      {
        case 0x41:
          return KC_UP;
        case 0x42:
          return KC_DOWN;
        case 0x43:
          return KC_RIGHT;
        case 0x44:
          return KC_LEFT;               
      }
    else if( c > 48 && c < 55 )
    {
      // Extended sequence: read another byte
      if( platform_uart_recv( CON_UART_ID, CON_TIMER_ID, TERM_TIMEOUT ) != 126 )
        return KC_UNKNOWN;
      switch( c )
      {
        case 49:
          return KC_HOME;
        case 52:
          return KC_END;
        case 53:
          return KC_PAGEUP;
        case 54:
          return KC_PAGEDOWN;  
      }
    }
  }
  else if( data == 0x0D )
  {
    // CR/LF sequence, read the second char (LF) if applicable
    platform_uart_recv( CON_UART_ID, CON_TIMER_ID, TERM_TIMEOUT );
    return KC_ENTER;
  }
  else
  {
    switch( data )
    {
      case 0x09:
        return KC_TAB;
      case 0x7F:
//        return KC_DEL; // bogdanm: some terminal emulators (for example screen) return 0x7F for BACKSPACE :(
      case 0x08:
        return KC_BACKSPACE;
      case 26:
        return KC_CTRL_Z;
      case 1:
        return KC_CTRL_A;
      case 5:
        return KC_CTRL_E;
      case 3:
        return KC_CTRL_C;
      case 20:
        return KC_CTRL_T;
      case 21:
        return KC_CTRL_U;
      case 11:
        return KC_CTRL_K; 
    }
  }
  return KC_UNKNOWN;
}

#endif // #ifdef BUILD_TERM


// *****************************************************************************
// std functions and platform initialization

static void uart_send( int fd, char c )
{
  fd = fd;
  platform_uart_send( CON_UART_ID, c );
}

static int uart_recv( s32 to )
{
  return platform_uart_recv( CON_UART_ID, CON_TIMER_ID, to );
}

void cmn_platform_init()
{
#ifdef BUILD_INT_HANDLERS
  platform_int_init();
#endif

#ifdef BUILD_SERMUX
  unsigned i;
  unsigned bufsizes[] = SERMUX_BUFFER_SIZES;  

  // Setup the serial multiplexer
  platform_uart_setup( SERMUX_PHYS_ID, SERMUX_PHYS_SPEED, 8, PLATFORM_UART_PARITY_NONE, PLATFORM_UART_STOPBITS_1 );
  platform_uart_set_flow_control( SERMUX_PHYS_ID, SERMUX_FLOW_TYPE );
  cmn_uart_setup_sermux();

  // Set buffers for all virtual UARTs 
  for( i = 0; i < sizeof( bufsizes ) / sizeof( unsigned ); i ++ )
    platform_uart_set_buffer( i + SERMUX_SERVICE_ID_FIRST, bufsizes[ i ] );
#endif // #ifdef BUILD_SERMUX

#if defined( CON_UART_ID ) && CON_UART_ID < SERMUX_SERVICE_ID_FIRST
  // Setup console UART
  platform_uart_setup( CON_UART_ID, CON_UART_SPEED, 8, PLATFORM_UART_PARITY_NONE, PLATFORM_UART_STOPBITS_1 );  
  platform_uart_set_flow_control( CON_UART_ID, CON_FLOW_TYPE );
  platform_uart_set_buffer( CON_UART_ID, CON_BUF_SIZE );
#endif // #if defined( CON_UART_ID ) && CON_UART_ID < SERMUX_SERVICE_ID_FIRST

  // Set the send/recv functions                          
  std_set_send_func( uart_send );
  std_set_get_func( uart_recv );  

#ifdef BUILD_XMODEM  
  // Initialize XMODEM
  xmodem_init( xmodem_send, xmodem_recv );    
#endif

#ifdef BUILD_TERM  
  // Initialize terminal
  term_init( TERM_LINES, TERM_COLS, term_out, term_in, term_translate );
#endif
}

// ****************************************************************************
// PIO functions

int platform_pio_has_port( unsigned port )
{
  return port < NUM_PIO;
}

const char* platform_pio_get_prefix( unsigned port )
{
  static char c[ 3 ];
  
  sprintf( c, "P%c", ( char )( port + PIO_PREFIX ) );
  return c;
}

int platform_pio_has_pin( unsigned port, unsigned pin )
{
#if defined( PIO_PINS_PER_PORT )
  return port < NUM_PIO && pin < PIO_PINS_PER_PORT;
#elif defined( PIO_PIN_ARRAY )
  const u8 pio_port_pins[] = PIO_PIN_ARRAY;
  return port < NUM_PIO && pin < pio_port_pins[ port ];
#else
  #error "You must define either PIO_PINS_PER_PORT of PIO_PIN_ARRAY in platform_conf.h"
#endif
}

// ****************************************************************************
// CAN functions

int platform_can_exists( unsigned id )
{
  return id < NUM_CAN;
}

// ****************************************************************************
// SPI functions


int platform_spi_exists( unsigned id )
{
  return id < NUM_SPI;
}

// ****************************************************************************
// PWM functions

int platform_pwm_exists( unsigned id )
{
  return id < NUM_PWM;
}

// ****************************************************************************
// CPU functions

u32 platform_cpu_get_frequency()
{
  return CPU_FREQUENCY;
}

// ****************************************************************************
// ADC functions

int platform_adc_exists( unsigned id )
{
  return id < NUM_ADC;
}

#ifdef BUILD_ADC

u32 platform_adc_op( unsigned id, int op, u32 data )
{  
  picoc_adc_ch_state *s = adc_get_ch_state( id );
  picoc_adc_dev_state *d = adc_get_dev_state( 0 );
  u32 res = 0;

  switch( op )
  {
    case PLATFORM_ADC_GET_MAXVAL:
      res = pow( 2, ADC_BIT_RESOLUTION ) - 1;
      break;

    case PLATFORM_ADC_SET_SMOOTHING:
      res = adc_update_smoothing( id, ( u8 )intlog2( ( unsigned ) data ) );
      break;
      
    case PLATFORM_ADC_SET_BLOCKING:
      s->blocking = data;
      break;
      
    case PLATFORM_ADC_IS_DONE:
      res = ( s->op_pending == 0 );
      break;
    
    case PLATFORM_ADC_OP_SET_TIMER:
      if ( d->timer_id != data )
        d->running = 0;
      platform_adc_stop( id );
      d->timer_id = data;
      break;
    
    case PLATFORM_ADC_OP_SET_CLOCK:
      res = platform_adc_setclock( id, data );
      break;
      
    case PLATFORM_ADC_SET_FREERUNNING:
      s->freerunning = data;
      break;
  }
  return res;
}
#endif // #ifdef BUILD_ADC

// ****************************************************************************
// Allocator support

#define MIN_ALIGN         8
#define MIN_ALIGN_SHIFT   3

extern char end[];

void* platform_get_first_free_ram( unsigned id )
{
  void* mstart[] = MEM_START_ADDRESS;
  u32 p;

  if( id >= sizeof( mstart ) / sizeof( void* ) )
    return NULL;
  p = ( u32 )mstart[ id ];
  if( p & ( MIN_ALIGN - 1 ) )
    p = ( ( p >> MIN_ALIGN_SHIFT ) + 1 ) << MIN_ALIGN_SHIFT;
  return ( void* )p;
}

void* platform_get_last_free_ram( unsigned id )
{
  void* mend[] = MEM_END_ADDRESS;
  u32 p;

  if( id >= sizeof( mend ) / sizeof( void* ) )
    return NULL;
  p = ( u32 )mend[ id ];
  if( p & ( MIN_ALIGN - 1 ) )
    p = ( ( p >> MIN_ALIGN_SHIFT ) - 1 ) << MIN_ALIGN_SHIFT;
  return ( void* )p;
}

// I2C support
int platform_i2c_exists( unsigned id )
{
#ifndef NUM_I2C
  return 0;
#else
  return id < NUM_I2C;
#endif
}

// ****************************************************************************
// Interrupt support
#ifdef BUILD_INT_HANDLERS

int platform_cpu_set_interrupt( picoc_int_id id, picoc_int_resnum resnum, int status )
{
  picoc_int_p_set_status ps;

  if( id < PICOC_INT_FIRST_ID || id > INT_PICOC_LAST )
    return PLATFORM_INT_INVALID;
  if( ( ps = picoc_int_table[ id - PICOC_INT_FIRST_ID ].int_set_status ) == NULL )
    return PLATFORM_INT_NOT_HANDLED;
  if( id == INT_TMR_MATCH )
    return cmn_tmr_int_set_status( resnum, status );
  return ps( resnum, status );
}

int platform_cpu_get_interrupt( picoc_int_id id, picoc_int_resnum resnum )
{
  picoc_int_p_get_status pg;

  if( id < PICOC_INT_FIRST_ID || id > INT_PICOC_LAST )
    return PLATFORM_INT_INVALID;
  if( ( pg = picoc_int_table[ id - PICOC_INT_FIRST_ID ].int_get_status ) == NULL )
    return PLATFORM_INT_NOT_HANDLED;
  if( id == INT_TMR_MATCH )
    return cmn_tmr_int_get_status( resnum );
  return pg( resnum );
}

int platform_cpu_get_interrupt_flag( picoc_int_id id, picoc_int_resnum resnum, int clear )
{
  picoc_int_p_get_flag pf;

  if( id < PICOC_INT_FIRST_ID || id > INT_PICOC_LAST )
    return PLATFORM_INT_INVALID;
  if( ( pf = picoc_int_table[ id - PICOC_INT_FIRST_ID ].int_get_flag ) == NULL )
    return PLATFORM_INT_NOT_HANDLED;
  if( id == INT_TMR_MATCH )
    return cmn_tmr_int_get_flag( resnum, clear );
  return pf( resnum, clear );
}

// Common interrupt handling
void cmn_int_handler( picoc_int_id id, picoc_int_resnum resnum )
{
  //picoc_int_add( id, resnum );
#ifdef BUILD_C_INT_HANDLERS
  picoc_int_c_handler phnd = picoc_int_get_c_handler( id );
  if( phnd )
    phnd( resnum );
#endif
}

#endif // #ifdef BUILD_INT_HANDLERS

// ****************************************************************************
// Misc support

unsigned int intlog2( unsigned int v )
{
  unsigned r = 0;

  while (v >>= 1)
  {
    r++;
  }
  return r;
}

