#ifndef	MCP2515_H
#define	MCP2515_H

#include "config.h"
#include "mcp2515_defs.h"

#include <inttypes.h>


typedef struct
{
	uint32_t id;				//!< ID der Nachricht (11 oder 29 Bit)
	uint8_t length;				//!< Anzahl der Datenbytes
	uint8_t data[8];			//!< Die Daten der CAN Nachricht
	
} can_t;

typedef struct
{
	uint32_t id;				//!< ID der Nachricht (11 oder 29 Bit)
	uint32_t mask;				//!< Maske
} can_filter_t;


#define MCP2515_FILTER_EXTENDED(id)	\
		(uint8_t)  ((uint32_t) (id) >> 21), \
		(uint8_t)((((uint32_t) (id) >> 13) & 0xe0) | (1<<3) | \
			(((uint32_t) (id) >> 16) & 0x3)), \
		(uint8_t)  ((uint32_t) (id) >> 8), \
		(uint8_t)  ((uint32_t) (id))


extern can_t InMessage ;
extern can_t OutMessage ;
extern uint8_t message_number;			//!< Running number of the messages
extern uint8_t message_data_counter;	
extern uint8_t message_data_length;		//!< Length of the data-field
extern uint8_t message_data[4];

typedef enum {
    // Bootloader commands
	UPDATE_REQ      = 1,
	IDENTIFY		= 2,
	SET_ADDRESS		= 3,
	DATA			= 4,
	START_APP		= 5,

	// Common Commands
	SEND_STATUS     = 10,
	READ_CONFIG		= 11,
	WRITE_CONFIG	= 12,
	READ_VAR		= 13,
	SET_VAR			= 14,
	START_BOOT		= 15,
	TIME			= 16,
	// Relais commands
	CHANNEL_ON		= 20,
	CHANNEL_OFF     = 21,
	CHANNEL_TOGGLE  = 22,
	SHADE_UP_FULL   = 23,
	SHADE_DOWN_FULL = 24,
	SHADE_UP_SHORT  = 25,
	SHADE_DOWN_SHORT= 26,
	// LED commands
	LED_OFF			= 30,
	LED_ON			= 31,
	SET_TO			= 32,
	HSET_TO			= 33,
	L_AND_S			= 34,
	SET_TO_G1		= 35,
	SET_TO_G2		= 36,
	SET_TO_G3		= 37,
	LOAD_LOW		= 38,
	LOAD_MID1		= 39,
	LOAD_MID2		= 40,
	LOAD_HIGH		= 41,
	START_PROG		= 42,
	STOP_PROG		= 43,
	// Sensor commands
	SET_PIN         = 50,
    LOAD_LED        = 51,
	OUT_LED         = 52,
	START_SENSOR    = 53,
	STOP_SENSOR     = 54,
	// Answer bits
	REQUEST			        = 0x00,
	SUCCESSFULL_RESPONSE	= 0x40,
	ERROR_RESPONSE		    = 0x80,
	WRONG_NUMBER_REPSONSE	= 0xC0,
	NO_MESSAGE		= 0x3f
} tCommand;


typedef enum {
	LISTEN_ONLY_MODE,		//!< der CAN Contoller empf�ngt nur und verh�lt sich v�llig passiv
	LOOPBACK_MODE,			//!< alle Nachrichten direkt auf die Empfangsregister umleiten ohne sie zu senden
	NORMAL_MODE				//!< normaler Modus, CAN Controller ist aktiv
} can_mode_t;


#define	COMMAND_MASK			0x3F
#define	START_OF_MESSAGE_MASK	0x80

#define false (0==1)
#define true (0==0)

#ifndef	MCP2515_CLKOUT_PRESCALER
	#error	MCP2515_CLKOUT_PRESCALER not defined!
#elif MCP2515_CLKOUT_PRESCALER == 0
	#define	CLKOUT_PRESCALER_	0x0
#elif MCP2515_CLKOUT_PRESCALER == 1
	#define	CLKOUT_PRESCALER_	0x4
#elif MCP2515_CLKOUT_PRESCALER == 2
	#define	CLKOUT_PRESCALER_	0x5
#elif MCP2515_CLKOUT_PRESCALER == 4
	#define	CLKOUT_PRESCALER_	0x6
#elif MCP2515_CLKOUT_PRESCALER == 8
	#define	CLKOUT_PRESCALER_	0x7
#else
	#error	invaild value of MCP2515_CLKOUT_PRESCALER
#endif

#if defined (__AVR_AT90PWM3B__)
	#define P_MOSI D,3
	#define P_MISO D,2
	#define P_SCK  D,4
	
	#define PORT_SPI	PORTD
	#define DDR_SPI		DDRD
        #define	MCP2515_CS			D,1
        #define	MCP2515_INT			D,6


#elif defined (__AVR_ATtiny84__)
	#define P_MOSI A,6
	#define P_MISO A,5
	#define P_SCK  A,4
	
	#define PORT_SPI	PORTA
	#define DDR_SPI		DDRA
        #define	MCP2515_CS	A,7
        #define	MCP2515_INT	A,3

#else
	#error	the choosen AVR Type is not yet supported by this library.
#endif


// ----------------------------------------------------------------------------
extern void mcp2515_init(void);

// ----------------------------------------------------------------------------
extern void send_message( uint8_t type, uint8_t length );

// ----------------------------------------------------------------------------
extern uint8_t get_message( void );

extern uint8_t mcp2515_get_message(can_t *msg);

extern uint8_t mcp2515_send_message(const can_t *msg) ;

extern uint8_t mcp2515_read_status(uint8_t) ;


extern int8_t mcp2515_read_register(uint8_t adress);

extern void mcp2515_write_id(const uint32_t *id);

extern void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data) ;

extern uint8_t mcp2515_set_filter(uint8_t number, const can_filter_t *filter) ;

extern __attribute__ ((gnu_inline)) inline void mcp2515_change_operation_mode(uint8_t mode)
{
	mcp2515_bit_modify(CANCTRL, 0xe0, mode);
	while ((mcp2515_read_register(CANSTAT) & 0xe0) != (mode & 0xe0))
		;
}


// ----------------------------------------------------------------------------
extern void mcp2515_write_register( uint8_t adress, uint8_t data );


#endif	// MCP2515_H
