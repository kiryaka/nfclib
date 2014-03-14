/* 
 * File:   transport.c
 * Author: Kirill Butin <kiryaka@gmail.com>
 * Company MicroStrategy
 * 
 * Created on March 13, 2014, 1:17 PM
 */

#include <buildoptions.h>
#include <nfclib.h>
#include <defines.h>

int main(int argc, char **argv){
    int res;
    
    DW(     nfcOpen("/dev/ttyUSB0") );
    DW(     nfcClose()              );
    
    
    return NFC_SUCCESS;
}
