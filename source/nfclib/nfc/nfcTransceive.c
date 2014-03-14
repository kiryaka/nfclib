/* 
 * File:   nfcTransceive.c
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 14, 2014, 10:13 AM
 */

#include "transport/transport.h"
#include <stdint.h>
#include <stddef.h>
#include <defines.h>
#include <string.h>


#define LOG_GROUP    NFC_LOG_GROUP_COM
#define LOG_CATEGORY "nfclib.uart"

#define DEFAULT_TIMEOUT 350

int res;

int transceive(const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t szRxLen, int timeout) {
    
  if (timeout == -1) timeout = DEFAULT_TIMEOUT;
  
  uint8_t  abtFrame[PN532_BUFFER_LEN] = { 0x00, 0x00, 0xff };       // Every packet must start with "00 00 ff"
  uint8_t  abtRx[PN53x_EXTENDED_FRAME__DATA_MAX_LEN];
  size_t  szRx = sizeof(abtRx);
  size_t szFrame = 0;
  
  if (szRxLen == 0 || !pbtRx) {
    pbtRx = abtRx;
  } else {
    szRx = szRxLen;
  }
  
  DW(   buildFrame      (abtFrame, &szFrame, pbtTx, szTx)   );
  DW(   sendCommand     (abtFrame, szFrame, timeout)        );
  DW(   receiveResult   (pbtRx, szRx, timeout       )       );

  return res;
}

// SHOUL GO TO PROTOCOL
int
buildFrame(uint8_t *pbtFrame, size_t *pszFrame, const uint8_t *pbtData, const size_t szData)
{
  size_t szPos;
  if (szData <= PN53x_NORMAL_FRAME__DATA_MAX_LEN) {
    // LEN - Packet length = data length (len) + checksum (1) + end of stream marker (1)
    pbtFrame[3] = szData + 1;
    // LCS - Packet length checksum
    pbtFrame[4] = 256 - (szData + 1);
    // TFI
    pbtFrame[5] = 0xD4;
    // DATA - Copy the PN53X command into the packet buffer
    memcpy(pbtFrame + 6, pbtData, szData);

    // DCS - Calculate data payload checksum
    uint8_t btDCS = (256 - 0xD4);
    
    for (szPos = 0; szPos < szData; szPos++) {
      btDCS -= pbtData[szPos];
    }
    pbtFrame[6 + szData] = btDCS;

    // 0x00 - End of stream marker
    pbtFrame[szData + 7] = 0x00;

    (*pszFrame) = szData + PN53x_NORMAL_FRAME__OVERHEAD;
  } else if (szData <= PN53x_EXTENDED_FRAME__DATA_MAX_LEN) {
    // Extended frame marker
    pbtFrame[3] = 0xff;
    pbtFrame[4] = 0xff;
    // LENm
    pbtFrame[5] = (szData + 1) >> 8;
    // LENl
    pbtFrame[6] = (szData + 1) & 0xff;
    // LCS
    pbtFrame[7] = 256 - ((pbtFrame[5] + pbtFrame[6]) & 0xff);
    // TFI
    pbtFrame[8] = 0xD4;
    // DATA - Copy the PN53X command into the packet buffer
    memcpy(pbtFrame + 9, pbtData, szData);

    // DCS - Calculate data payload checksum
    uint8_t btDCS = (256 - 0xD4);
    for (szPos = 0; szPos < szData; szPos++) {
      btDCS -= pbtData[szPos];
    }
    pbtFrame[9 + szData] = btDCS;

    // 0x00 - End of stream marker
    pbtFrame[szData + 10] = 0x00;

    (*pszFrame) = szData + PN53x_EXTENDED_FRAME__OVERHEAD;
  } else {
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "We can't send more than %d bytes in a raw (requested: %ld)", PN53x_EXTENDED_FRAME__DATA_MAX_LEN, szData);
    return NFC_ECHIP;
  }
  return NFC_SUCCESS;
}
