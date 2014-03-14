/* 
 * File:   communication.h
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 14, 2014, 11:15 AM
 */

#ifndef COMMUNICATION_H
#define	COMMUNICATION_H

#include <stddef.h>
#include <stdint.h>

int nfcOpen(char* comPort);
int nfcClose();
int transceive(const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t szRxLen, int timeout);

#endif	/* COMMUNICATION_H */

