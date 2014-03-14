/* 
 * File:   debug.c
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 2:20 PM
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <debug.h>


void fatal_error (int res, char* file, int line){
    printf("Error %d in: %s:%d", res, file, line);
    exit(1);
}

const char *
log_priority_to_str(const int priority)
{
  switch (priority) {
    case NFC_LOG_PRIORITY_ERROR:
      return  "error";
    case NFC_LOG_PRIORITY_INFO:
      return  "info";
    case NFC_LOG_PRIORITY_DEBUG:
      return  "debug";
    default:
      break;
  }
  return "unknown";
}

void
log_put(const uint8_t group, const char *category, const uint8_t priority, const char *format, ...)
{
      va_list va;
      va_start(va, format);
      printf("%s\t%s\t", log_priority_to_str(priority), category);
      vfprintf(stdout, format, va);
      printf("\n");
      va_end(va);
      
      fflush(stdout);
}