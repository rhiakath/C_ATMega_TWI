#ifndef ATMEGATWI_INCLUDED
#define ATMEGATWI_INCLUDED

#include <util/twi.h>
#include <stdint.h>
void TWIInit ( void );
uint8_t TWIStart ( uint8_t In_Address, uint8_t In_Mode );
uint8_t TWIStop ( void );
uint8_t TWIWrite ( uint8_t In_Data );
uint8_t TWIRead ( void );
uint8_t TWIReadNACK ( void );
uint8_t TWIGetLastStatus ( void );
const char *TWIStatusToString ( uint8_t In_Status );
const char *TWCRToString ( void );
#endif
