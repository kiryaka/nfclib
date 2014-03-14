/* 
 * File:   transport.c
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 1:17 PM
 */

#include "uart.h"
#include "transport.h"
#include <defines.h>
#include <stdbool.h>
#include <stdint.h>

#include <fcntl.h>
#include <stdio.h>
#include <termios.h>

serial_port sp;
int res;

#define LOG_GROUP    NFC_LOG_GROUP_COM
#define LOG_CATEGORY "libnfc.bus"

const uint8_t pn53x_ack_frame[] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
const uint8_t pn53x_nack_frame[] = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 };
static const uint8_t pn53x_error_frame[] = { 0x00, 0x00, 0xff, 0x01, 0xff, 0x7f, 0x81, 0x00 };


int openPort(char* comPort){
    sp = uart_open(comPort);
    uart_flush_input(sp, true);
    uart_set_speed(sp, PN532_DEFAULT_SPEED);
    return NFC_SUCCESS;
} 

int closePort(void){
    if (sp == null) return 1;
    uart_close(sp);
    return NFC_SUCCESS;
} 

int wakeupReader() {
    const uint8_t wakeup_preamble[] = { 0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    DW(   uart_send           (sp, wakeup_preamble, sizeof(wakeup_preamble), 0)                  );
    return NFC_SUCCESS;
}

int sendCommand(const uint8_t *pbtTx, const size_t szTx, int timeout) {
    
  uart_flush_input(sp, false);
  
  uint8_t abtRxBuf[PN53x_ACK_FRAME__LEN];
  
  DW(   uart_send       (sp, pbtTx, szTx, timeout)                      );
  DW(   uart_receive    (sp, abtRxBuf, sizeof(abtRxBuf), 0, timeout)    );
  DW(   checkAckFrame   (abtRxBuf, sizeof(abtRxBuf))               );
 

  return NFC_SUCCESS;
}

int receiveResult(uint8_t *pbtData, const size_t szDataLen, int timeout){
  uint8_t  abtRxBuf[5];
  size_t len;
  void *abort_p = NULL;

  DW(   uart_receive    (sp, abtRxBuf, 5, abort_p, timeout)    );

  

  const uint8_t pn53x_preamble[3] = { 0x00, 0x00, 0xff };
  if (0 != (memcmp(abtRxBuf, pn53x_preamble, 3))) {
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Frame preamble+start code mismatch");
    goto error;
  }

  if ((0x01 == abtRxBuf[3]) && (0xff == abtRxBuf[4])) {
    // Error frame
    uart_receive(sp, abtRxBuf, 3, 0, timeout);
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Application level error detected");

    goto error;
  } else if ((0xff == abtRxBuf[3]) && (0xff == abtRxBuf[4])) {
    // Extended frame
    res = uart_receive(sp, abtRxBuf, 3, 0, timeout);
    if (res != 0) {
      log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Unable to receive data. (RX)");
      goto error;
    }

    len = (abtRxBuf[0] << 8) + abtRxBuf[1] - 2;
    if (((abtRxBuf[0] + abtRxBuf[1] + abtRxBuf[2]) % 256) != 0) {
      log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Length checksum mismatch");
      goto error;
    }
  } else {
    // Normal frame
    if (256 != (abtRxBuf[3] + abtRxBuf[4])) {
      // TODO: Retry
      log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Length checksum mismatch");
      goto error;
    }

    // abtRxBuf[3] (LEN) include TFI + (CC+1)
    len = abtRxBuf[3] - 2;
  }

  if (len > szDataLen) {
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "Unable to receive data: buffer too small. (szDataLen: %lu, len: %lu)", szDataLen, len);
    goto error;
  }

  // TFI + PD0 (CC+1)
  res = uart_receive(sp, abtRxBuf, 2, 0, timeout);
  if (res != 0) {
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Unable to receive data. (RX)");
    goto error;
  }

  if (abtRxBuf[0] != 0xD5) {
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "TFI Mismatch");

    goto error;
  }

//  if (abtRxBuf[1] != CHIP_DATA(pnd)->last_command + 1) {
//    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Command Code verification failed");
//    goto error;
//  }

  if (len) {
    res = uart_receive(sp, pbtData, len, 0, timeout);
    if (res != 0) {
      log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Unable to receive data. (RX)");
      goto error;
    }
  }

  res = uart_receive(sp, abtRxBuf, 2, 0, timeout);
  if (res != 0) {
    log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Unable to receive data. (RX)");
    goto error;
  }

  return len;
error:
  uart_flush_input(sp, true);
  return NFC_EIO;  
}

int checkAckFrame(const uint8_t *pbtRxFrame, const size_t szRxFrameLen) {
  if (szRxFrameLen >= sizeof(pn53x_ack_frame)) {
    if (0 == memcmp(pbtRxFrame, pn53x_ack_frame, sizeof(pn53x_ack_frame))) {
      log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_DEBUG, "%s", "PN53x ACKed");
      return NFC_SUCCESS;
    }
  }
  log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "%s", "Unexpected PN53x reply!");
  return NFC_EIO;
}