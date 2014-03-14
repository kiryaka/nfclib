/* 
 * File:   defines.h
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 1:27 PM
 */

#include <debug.h>

#ifndef DEFINES_H
#define	DEFINES_H

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef null
#define null 0
#endif

#define NFC_SUCCESS 0
#define NFC_EIO     -1
#define NFC_ECHIP   -90

#define PN532_DEFAULT_SPEED                     115200
#define PN53x_NORMAL_FRAME__DATA_MAX_LEN        254
#define PN53x_NORMAL_FRAME__OVERHEAD            8
#define PN53x_EXTENDED_FRAME__DATA_MAX_LEN      264
#define PN53x_EXTENDED_FRAME__OVERHEAD          11
#define PN53x_ACK_FRAME__LEN                    6
#define PN532_BUFFER_LEN (PN53x_EXTENDED_FRAME__DATA_MAX_LEN + PN53x_EXTENDED_FRAME__OVERHEAD)

#endif //DEFINES_H


