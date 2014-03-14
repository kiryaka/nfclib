/* 
 * File:   nfcOpen.c
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 2:55 PM
 */


#define Diagnose 0x00
#define GetFirmwareVersion 0x02
#define GetGeneralStatus 0x04
#define ReadRegister 0x06
#define WriteRegister 0x08
#define ReadGPIO 0x0C
#define WriteGPIO 0x0E
#define SetSerialBaudRate 0x10
#define SetParameters 0x12
#define SAMConfiguration 0x14
#define PowerDown 0x16
#define AlparCommandForTDA 0x18

typedef enum {
  PSM_NORMAL = 0x01,
  PSM_VIRTUAL_CARD = 0x02,
  PSM_WIRED_CARD = 0x03,
  PSM_DUAL_CARD = 0x04
} pn532_sam_mode;

#include "transport/transport.h"
#include "communication.h"
#include <defines.h>
#include <stdint.h>
#include <stddef.h>


int res;

int nfcOpen(char* comPort){
    DW( openPort(comPort));
    DW( diagnose());
    return NFC_SUCCESS;
}

int nfcClose(){
    DW(closePort());
    return NFC_SUCCESS;
}

int diagnose(void){
  
  const uint8_t abtCmd[] = { Diagnose, 0x00, 'n', 'f', 'c', 'l', 'i', 'b' };
  const uint8_t abtExpectedRx[] = { 0x00, 'n', 'f', 'c', 'l', 'i', 'b' };
  
  uint8_t abtRx[sizeof(abtExpectedRx)];
  size_t szRx = sizeof(abtRx);
  int res = 0;

  DW(   wakeupReader()    );
  DW(   doSAMConfiguration(PSM_NORMAL, 500)  );
  DW(   transceive(abtCmd, sizeof(abtCmd), abtRx, szRx, 500)  );
  
  szRx = (size_t) res;
  
  if ((sizeof(abtExpectedRx) == szRx) && (0 == memcmp(abtRx, abtExpectedRx, sizeof(abtExpectedRx))))
    return NFC_SUCCESS;

  return NFC_EIO;
}

int doSAMConfiguration(const pn532_sam_mode sam_mode, int timeout) {
  uint8_t abtCmd[] = { SAMConfiguration, sam_mode, 0x00, 0x00 };
  size_t szCmd = sizeof(abtCmd);
  
  DWR(   transceive(abtCmd, szCmd, NULL, 0, -1)   );
}

int powerDown() {
  uint8_t  abtCmd[] = { PowerDown, 0xf0 };
  DWR(   transceive(abtCmd, sizeof(abtCmd), NULL, 0, -1)   );
}
