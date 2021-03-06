/* Network.h: Deklarationen fuer die CAN-Kommunikation ueber Ethernet 
   Es werden jeweils die max. moeglichen 8 Byte als UDP-Paket an die Broadcast-Adresse gesendet, das
   Routing uebernehmen die Ethernet-CAN-Umsetzer

   ToDo: Port und Broadcast-Adresse konfigurierbar (bzw. autokonfigurierend) machen
*/

#define CANBUFLEN 20
#define CAN_PORT "13247"                 // Port-Nummer der CAN-Kommunikation
#define CAN_PORT_NUM 13247
#define WS_PORT "13248"                 // Port-Nummer der Websocket-Kommunikation
#define WS_PORT_NUM 13248
#define CAN_BROADCAST "192.168.69.255"

typedef unsigned long ULONG ;
typedef unsigned short USHORT ;
#ifndef TRUE
#define TRUE (0==0)
#endif
#ifndef FALSE
#define FALSE (0==1)
#endif

/* Websocket-Elemente */

enum web_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_CONTROL,

	/* always last */
	DEMO_PROTOCOL_COUNT
};

/* Globale Variablen */

extern int RecSockFD;    // Empfangs-File-Descriptor
extern int SendSockFD;   // Sende-File-Descriptor
extern struct addrinfo *servinfo, *SendInfo ;

extern struct libwebsocket_protocols web_protocols[] ;
extern struct libwebsocket_context *web_context;

/* CAN-Funktionen */

/* Die 29-Bit CAN ID aus den Adress-Informationen zusammensetzen */

ULONG BuildCANId (char Prio, char Repeat, char FromLine, USHORT FromAdd, char ToLine, USHORT ToAdd, char Group) ;

/* Ueberpruefungsfunktion, ob die Adresse Ziel der CAN-ID war */

int MatchAddress (ULONG CANId, char ToLine, USHORT ToAdd) ;

/* Ueberpruefungsfunktion, ob die Adresse Quelle der CAN-ID war */
int MatchSrcAddress (ULONG CANId, char FromLine, USHORT FromAdd);

/* Abspeichern der Quell-Adresse aus einer CAN-ID */

void GetSourceAddress (ULONG CANId, char *FromLine, USHORT *FromAdd) ;

/* Abspeichern der Ziel-Adresse aus einer CAN-ID */

void GetDestinationAddress (ULONG CANId, char *ToLine, USHORT *ToAdd) ;

/* Netzwerk-Funktionen */

/* Netzwerk-Interface initialisieren */

int InitNetwork(void) ;

/* CAN Message empfangen (wartet so lange, bis eine kommt) */

int ReceiveCANMessage (ULONG *CANID, char *Len, unsigned char *Data);

/* CAN-Nachricht senden */

int SendCANMessage (ULONG CANID, char Len, unsigned char *Data);

/* Auf Netzwerk warten */
int CheckNetwork(int sd,int * error,int timeOut) ;

/* Aufraeum-Funktion */

void CloseNetwork (void) ;


/* Sende eine Aktion auf den CAN-Bus */

void SendAction (struct Node *Action) ;

int InitWebsocket(void) ;

