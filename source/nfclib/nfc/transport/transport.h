/* 
 * File:   transport.h
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 2:58 PM
 */

#ifndef TRANSPORT_H
#define	TRANSPORT_H

int openPort(char* comPort);
int wakeupReader();

#endif	/* TRANSPORT_H */

