/* 
 * File:   debug.h
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 1:47 PM
 */

#ifndef DEBUG_H
#define	DEBUG_H

#include <stdint.h>

#define NFC_LOG_PRIORITY_NONE   0
#define NFC_LOG_PRIORITY_ERROR  1
#define NFC_LOG_PRIORITY_INFO   2
#define NFC_LOG_PRIORITY_DEBUG  3

#define NFC_LOG_GROUP_GENERAL   1
#define NFC_LOG_GROUP_CONFIG    2
#define NFC_LOG_GROUP_CHIP      3
#define NFC_LOG_GROUP_DRIVER    4
#define NFC_LOG_GROUP_COM       5
#define NFC_LOG_GROUP_LIBUSB    6

void fatal_error (int res, char* file, int line);

#define DW(arg) if ( (res = arg) < 0) fatal_error (res, __FILE__, __LINE__)
#define DWR(arg) if ( (res = arg) < 0) fatal_error (res, __FILE__, __LINE__); else return res
//#define log_put(...)
void log_put(const uint8_t group, const char *category, const uint8_t priority, const char *format, ...);

#endif //DEBUG_H


