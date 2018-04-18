
/**
 * MY-BASIC platform agnostic modules.
 * 
 * All of them are declared here; Platform specific modules are
 * defined in platform/xxx/mybasic_platform.h
 */
#ifndef __MYBASIC_MOD_H__
#define __MYBASIC_MOD_H__

// Helper macros.

#define MYBASIC_LIB_DEFINE(fname, fun)\
  {#fname, fun}

// Platform module.
#define MYBASIC_MOD_PD\
  MYBASIC_LIB_DEFINE(PD_PLATFORM, pd_platform),\
  MYBASIC_LIB_DEFINE(PD_CPU, pd_cpu),\
  MYBASIC_LIB_DEFINE(PD_BOARD, pd_board),

#define MYBASIC_MOD_ELUA\
  MYBASIC_LIB_DEFINE(ELUA_VERSION, elua_version),\
  MYBASIC_LIB_DEFINE(ELUA_SAVE_HISTORY, elua_save_history),\
  MYBASIC_LIB_DEFINE(ELUA_SHELL, elua_shell),

#define MYBASIC_MOD_TERMINAL\
  MYBASIC_LIB_DEFINE(BASICTERM_CLRSCR, basicterm_clrscr),\
  MYBASIC_LIB_DEFINE(BASICTERM_CLREOL, basicterm_clreol),\
  MYBASIC_LIB_DEFINE(BASICTERM_MOVETO,basicterm_moveto),\
  MYBASIC_LIB_DEFINE(BASICTERM_MOVEUP, basicterm_moveup),\
  MYBASIC_LIB_DEFINE(BASICTERM_MOVEDOWN, basicterm_movedown),\
  MYBASIC_LIB_DEFINE(BASICTERM_MOVELEFT, basicterm_moveleft),\
  MYBASIC_LIB_DEFINE(BASICTERM_MOVERIGHT, basicterm_moveright),\
  MYBASIC_LIB_DEFINE(BASICTERM_GETLINES, basicterm_getlines),\
  MYBASIC_LIB_DEFINE(BASICTERM_GETCOLS, basicterm_getcols),\
  MYBASIC_LIB_DEFINE(BASICTERM_PUTS, basicterm_puts),\
  MYBASIC_LIB_DEFINE(BASICTERM_PUTCH, basicterm_putch),\
  MYBASIC_LIB_DEFINE(BASICTERM_DECODE, basicterm_decode),\
  MYBASIC_LIB_DEFINE(BASICTERM_GETCX,basicterm_getcx),\
  MYBASIC_LIB_DEFINE(BASICTERM_GETCY, basicterm_getcy),\
  MYBASIC_LIB_DEFINE(BASICTERM_GETCHAR, basicterm_getchar),
  
#endif
