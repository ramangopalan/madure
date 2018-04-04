// Module for interfacing with platform data

#include "my_basic.h"
#include "platform.h"
#include <string.h>

#define MACRO_NAME( x ) MACRO_AGAIN( x )
#define MACRO_AGAIN( x ) #x

// ****************************************************************************
// Platform module for my-basic.

// v = pd_platform()
int pd_platform(struct mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(MACRO_NAME(MADURE_PLATFORM));
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, MACRO_NAME(MADURE_PLATFORM), len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}

// V = pd_cpu()
int pd_cpu(struct mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(MACRO_NAME(MADURE_CPU));
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, MACRO_NAME(MADURE_CPU), len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}

// v = pd_board()
int pd_board(struct mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(MACRO_NAME(MADURE_BOARD));
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, MACRO_NAME(MADURE_BOARD), len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}
