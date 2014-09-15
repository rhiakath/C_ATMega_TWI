#include "ATMega_TWI.h"
#include <avr/io.h>
#include <util/twi.h>
#include <Platform.h>
#include <string.h>
#include <stdio.h>
// #include <C_Logger/Logger.h>
uint8_t LastTWIStatus = 0;

#define SCL_CLOCK  400000L

static uint8_t TWIGetStatus ( void )
    {
    LastTWIStatus = TWSR & 0xF8;
    return LastTWIStatus;
    }

void TWIInit ( void )
    {
    TWSR = 0x00; // Prescaler a 0
    TWBR = ( ( F_CPU/SCL_CLOCK )-16 ) /2;
    TWCR = 0;
    }

uint8_t TWIStart ( uint8_t In_Address, uint8_t In_Mode )
    {
    //LOG_MSG ( "Mode %d %02X", In_Mode, TWCR );
    TWCR = 0;
    TWCR = BIT_VALUE ( TWINT ) | BIT_VALUE ( TWSTA ) | BIT_VALUE ( TWEN );
    //LOG_MSG ( "Mode %d %02X", In_Mode, TWCR );
    while ( IS_BIT_ENABLED ( TWCR, TWINT ) == 0 ); // Espera por interrupt

    uint8_t Status = TWIGetStatus();
    //LOG_MSG ( "Status is %02X ( %s )", LastTWIStatus, TWIStatusToString ( LastTWIStatus ) );
    if ( ( Status != TW_START ) && ( Status != TW_REP_START ) ) return 1;

    TWDR = ( In_Address << 1 ) | In_Mode; // Escreve endereço slave
    TWCR = BIT_VALUE ( TWINT ) | BIT_VALUE ( TWEN );
    while ( IS_BIT_ENABLED ( TWCR, TWINT ) == 0 ); // Espera por interrupt

    Status = TWIGetStatus();
    //LOG_MSG ( "Status is %02X ( %s )", LastTWIStatus, TWIStatusToString ( LastTWIStatus ) );
    if ( ( Status != TW_MT_SLA_ACK ) && ( Status!= TW_MR_SLA_ACK ) ) return 2;
    return 0;
    }

uint8_t TWIStop ( void )
    {
    //LOG_MSG ( "" );
    TWCR = 0;
    TWCR = BIT_VALUE ( TWINT ) | BIT_VALUE ( TWSTO ) | BIT_VALUE ( TWEN );
//     while ( IS_BIT_ENABLED ( TWCR, TWINT ) == 0 ); // Espera por interrupt
    TWIGetStatus();
    //LOG_MSG ( "Status is %02X ( %s ) TWCR %02X", LastTWIStatus, TWIStatusToString ( LastTWIStatus ), TWCR );

    return 0;
    }

uint8_t TWIWrite ( uint8_t In_Data )
    {
    //LOG_MSG ( "" );

    TWDR = In_Data;
    TWCR = BIT_VALUE ( TWINT ) | BIT_VALUE ( TWEN );
    while ( IS_BIT_ENABLED ( TWCR, TWINT ) == 0 );
    uint8_t Status = TWIGetStatus();
    //LOG_MSG ( "Status is %02X ( %s )", LastTWIStatus, TWIStatusToString ( LastTWIStatus ) );
    if ( Status != TW_MT_DATA_ACK ) return 1;
    return 0;
    }

uint8_t TWIRead ( void )
    {
    //LOG_MSG ( "" );
    TWCR = BIT_VALUE ( TWINT ) | BIT_VALUE ( TWEN ) | BIT_VALUE ( TWEA );
    while ( IS_BIT_ENABLED ( TWCR, TWINT ) == 0 );
    TWIGetStatus();
    //LOG_MSG ( "Status is %02X ( %s ) TWCR %02X", LastTWIStatus, TWIStatusToString ( LastTWIStatus ), TWCR );
    return TWDR;
    }

uint8_t TWIReadNACK ( void )
    {
    //LOG_MSG ( "" );
    TWCR = BIT_VALUE ( TWINT ) | BIT_VALUE ( TWEN );
    while ( IS_BIT_ENABLED ( TWCR, TWINT ) == 0 );
    TWIGetStatus();
    //LOG_MSG ( "Status is %02X ( %s )", LastTWIStatus, TWIStatusToString ( LastTWIStatus ) );
    return TWDR;
    }

uint8_t TWIGetLastStatus ( void )
    {
    return LastTWIStatus;
    }

const char* TWIStatusToString ( uint8_t In_Status )
    {
    static char DefaultMessage[30];
#define CASE(x) case x: return #x;
    switch ( In_Status )
        {
            CASE ( TW_START );
            CASE ( TW_REP_START );
            CASE ( TW_MT_SLA_ACK );
            CASE ( TW_MT_SLA_NACK );
            CASE ( TW_MT_DATA_ACK );
            CASE ( TW_MT_DATA_NACK );
            CASE ( TW_MR_SLA_ACK );
            CASE ( TW_MR_SLA_NACK );
            CASE ( TW_MR_DATA_ACK );
            CASE ( TW_MR_DATA_NACK );
            CASE ( TW_ST_SLA_ACK );
//             CASE ( TW_ST_SLA_NACK );
            CASE ( TW_ST_DATA_ACK );
            CASE ( TW_ST_DATA_NACK );
            CASE ( TW_SR_SLA_ACK );
//             CASE ( TW_SR_SLA_NACK );
            CASE ( TW_SR_DATA_ACK );
            CASE ( TW_SR_DATA_NACK );
            CASE ( TW_BUS_ERROR );
            CASE ( TW_NO_INFO );
        default:
            snprintf ( DefaultMessage, sizeof ( DefaultMessage ), "Unknown %02X status", In_Status );
            return DefaultMessage;
        }
    }

const char *TWCRToString ( void )
    {
    static char FullString[50];
    FullString[0] = 0;
    static char Names[8][6]= {"TWINT", "TWEA ", "TWSTA", "TWSTO", "TWWC ", "TWEN ", "    ", "TWIE " };
    int cont;
    for ( cont = 7; cont >= 0; --cont )
        {
        if ( IS_BIT_ENABLED ( TWCR, cont ) )
            strcat ( FullString, Names[cont] );
        strcat ( FullString, " " );
        }
    return FullString;
    }
