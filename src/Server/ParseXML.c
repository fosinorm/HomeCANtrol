#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <expat.h>
#include <string.h>
#include <locale.h>
#include "XMLConfig.h"

#define BUFF_SIZE 10240

struct Node *Haus ;
struct Node *Current ;

int MakroNummer;
int Depth;

struct TypSel Types[] = {
  {"Haus",N_STRUCTURE},
  {"Home",N_STRUCTURE},
  {"Etage",N_STRUCTURE},
  {"Floor",N_STRUCTURE},
  {"Zimmer",N_STRUCTURE},
  {"Room",N_STRUCTURE},
  {"Schalter",N_ONOFF},
  {"Switch",N_ONOFF},
  {"Rollo",N_SHADE},
  {"Shade",N_SHADE},
  {"Adresse",N_ADRESS},
  {"Adress",N_ADRESS},
  {"Sensor",N_SENSOR},
  {"Aktion",N_ACTION},
  {"Action",N_ACTION},
  {"Makro",N_MACRO},
  {"Macro",N_MACRO},
  {"Warte",N_DELAY},
  {"Wait",N_DELAY},
  {"Timer",N_TIMER},
  {"Rufe",N_CALL},
  {"Call",N_CALL},
  {"Gleichzeitig",N_TASK},
  {"Task",N_TASK},
  {"Immer",N_ALWAYS},
  {"Always",N_ALWAYS},
  {"Aktiv",N_ACTIVE},
  {"Active",N_ACTIVE},
  {"Start",N_STARTUP},
  {"Startup",N_STARTUP},
  {"Wenn",N_IF},
  {"If",N_IF},
  {"Variable",N_VAR},
  {"Var",N_VAR},
  {"Setze",N_SET},
  {"Set",N_SET},
  {"Solange",N_REPEAT},
  {"While",N_REPEAT},
  {"Sprache",N_LANGUAGE},
  {"Language",N_LANGUAGE},
  {"Einfach",S_SIMPLE},
  {"Simple",S_SIMPLE},
  {"KurzLang",S_SHORTLONG},
  {"ShortLong",S_SHORTLONG},
  {"Rollo_KL",S_SHADE_SHORTLONG},
  {"Shade_SL",S_SHADE_SHORTLONG},
  {"Rollo_Einfach",S_SHADE_SIMPLE},
  {"Shade_Simple",S_SHADE_SIMPLE},
  {"Monoflop",S_MONO},
  {"R_Monoflop",S_RETMONO},
  {"An",A_ON},
  {"On",A_ON},
  {"Aus",A_OFF},
  {"Off",A_OFF},
  {"Toggle",A_TOGGLE},
  {"Hoch",A_SHADE_UP_FULL},
  {"Up",A_SHADE_UP_FULL},
  {"KurzHoch",A_SHADE_UP_SHORT},
  {"ShortUp",A_SHADE_UP_SHORT},
  {"Runter",A_SHADE_DOWN_FULL},
  {"Down",A_SHADE_DOWN_FULL},
  {"KurzRunter",A_SHADE_DOWN_SHORT},
  {"ShortDown",A_SHADE_DOWN_SHORT},
  {"Wert",A_SEND_VAL},
  {"Value",A_SEND_VAL},
  {"Herzschlag",A_HEARTBEAT},
  {"Heartbeat",A_HEARTBEAT},
  {NULL,0} 
} ;

NodeType FillType(const char *This)
{
  int i ;
  for (i=0;Types[i].Name!=NULL;i++) {
    if (strcmp(This,Types[i].Name)==0) break ;
  }
  return (Types[i].Type) ;
}

int ParseError ;

void XMLCALL start(void *data, const char *el, const char **attr) 
{
  int i,Time;
  struct Node *This ;


  if (Current==NULL) {
    Current = CreateNode() ;
    Haus = Current ;
  } else {
    Current = NewChild (Current) ;
  } ;

  strncpy (Current->TypeDef,el,NAMELEN) ;
  Current->Type = FillType (Current->TypeDef) ;


  
  for (i = 0; attr[i]; i += 2) {
    if (strcmp(attr[i],"name")==0) {
      strncpy(Current->Name,attr[i+1],NAMELEN) ;
      continue; 
    } ;
    switch (Current->Type) {
    case N_ADRESS:
      if ((strcmp(attr[i],"linie")==0)||(strcmp(attr[i],"line")==0)) {
	sscanf(attr[i+1],"%d",&(Current->Data.Adresse.Linie)) ;
      } ;
      if ((strcmp(attr[i],"knoten")==0)||(strcmp(attr[i],"node")==0)) {
	sscanf(attr[i+1],"%d",&(Current->Data.Adresse.Knoten)) ;
      } ;
      if (strcmp(attr[i],"port")==0) {
	sscanf(attr[i+1],"%d",&(Current->Data.Adresse.Port)) ;
      } ;
      break ;
    case N_ACTION:
      if ((strcmp(attr[i],"kommando")==0)||(strcmp(attr[i],"command")==0)) {
	Current->Data.Aktion.Type = FillType(attr[i+1]) ;
      }
      if ((strcmp(attr[i],"objekt")==0)||(strcmp(attr[i],"object")==0)) {
	strncpy(Current->Data.Aktion.UnitName,attr[i+1],NAMELEN*4) ;
      } ;
      if ((strcmp(attr[i],"autonom")==0)||(strcmp(attr[i],"standalone")==0)) {
	sscanf(attr[i+1],"%d",&(Current->Data.Aktion.StandAlone)) ;
      } ;
      break ;
    case N_SENSOR:
      if ((strcmp(attr[i],"typ")==0)||(strcmp(attr[i],"type")==0)) {
	Current->Data.SensorTyp = FillType(attr[i+1]);
      } ;
      break ;
    case N_DELAY:
      if ((strcmp(attr[i],"zeit")==0)||(strcmp(attr[i],"time")==0)) {
	sscanf (attr[i+1],"%d",&Time) ;
	Current->Data.Time.tv_sec = Time/1000 ;
	Current->Data.Time.tv_usec = Time%1000 * 1000 ;
      } ;
      break ;
    case N_TIMER:
      if ((strcmp(attr[i],"zeit")==0)||(strcmp(attr[i],"time")==0)) {
	strncpy(Current->Data.Wert.UnitName,attr[i+1],NAMELEN*4) ;
      } ;
      break ;
    case N_CALL:
    case N_TASK:
      if ((strcmp(attr[i],"makro")==0)||(strcmp(attr[i],"macro")==0)) {
	strncpy(Current->Data.UnitName,attr[i+1],NAMELEN*4) ;
      } ;
      break ;
    case N_IF:
    case N_SET:
    case N_REPEAT:
      if ((strcmp(attr[i],"objekt")==0)||(strcmp(attr[i],"object")==0)) {
	strncpy(Current->Data.Wert.UnitName,attr[i+1],NAMELEN*4) ;
      } ;
      if ((strcmp(attr[i],"wert")==0)||(strcmp(attr[i],"value")==0)) {
	sscanf(attr[i+1],"%d",&(Current->Data.Wert.Wert)) ;
      } ;
      if ((strcmp(attr[i],"vergleich")==0)||(strcmp(attr[i],"comparison")==0)) {
	switch (attr[i+1][0]) {
	case '<':
	  Current->Data.Wert.Vergleich = -1 ;
	  break ;
	case '>':
	  Current->Data.Wert.Vergleich = 1 ;
	  break ;
	case '=':
	default:
	  Current->Data.Wert.Vergleich = 0 ;
	  break ;
	} ;
      } ;
      break ;
    case N_VAR:
      if ((strcmp(attr[i],"wert")==0)||(strcmp(attr[i],"value")==0)) {
	sscanf(attr[i+1],"%d",&(Current->Value)) ;
      } ;
      break ;
    case N_LANGUAGE:
      if ((strcmp(attr[i],"ist")==0)||(strcmp(attr[i],"is")==0)) {
	switch (attr[i+1][0]) {
	case 'd':
	  setlocale(LC_TIME,"de_DE") ;
	  break ;
	default:
	  break ;
	} ;
      } ;
      break ;
    default:
      break ;
    } ;

  }
  // Ueberpruefen auf Semantik
  // Namensgleichheit
  for (This=Current->Prev;This!=NULL;This=This->Prev) {
    if ((strlen(Current->Name)>0)&&(strcmp(This->Name,Current->Name)==0)) {
      // Name wurde schon verwendet
      ParseError = 1 ;
      fprintf (stderr,"Name doppelt verwendet: ") ;
      for (This=Current;This!=NULL;This=This->Parent) fprintf (stderr,"%s ",This->Name) ;
      fprintf (stderr,"\n") ;
      break ;
    }
  } ;
  // Einzelelemente
  if (Current->Type==N_ADRESS) {
    if ((This=FindNodeAdress(Haus,Current->Data.Adresse.Linie,Current->Data.Adresse.Knoten,Current->Data.Adresse.Port,Current))!=NULL) {
      ParseError = 1 ;
      fprintf (stderr,"Adresse wurde doppelt verwendet: Linie %d, Knoten %d, Port %d\n",
	       Current->Data.Adresse.Linie,Current->Data.Adresse.Knoten,Current->Data.Adresse.Port) ;
      fprintf (stderr,"Erste Verwendung: ") ;
      for (;This!=NULL;This=This->Parent) fprintf (stderr,"%s ",This->Name) ;
      fprintf (stderr,"\nZweite Verwendung: ") ;
      for (This=Current;This!=NULL;This=This->Parent) fprintf (stderr,"%s ",This->Name) ;
      fprintf (stderr,"\n") ;
    } ;
  } ;

  // Element nachbearbeiten
  // Makros nummerieren 
  if (Current->Type==N_MACRO) {
    This = NewChild(Current) ;
    This->Type = N_ADRESS ;
    This->Data.Adresse.Linie = 0 ;
    This->Data.Adresse.Knoten = (MakroNummer>>8)+50 ;
    This->Data.Adresse.Port = (MakroNummer&0xFF) ;
    MakroNummer++ ;
  } ;

  Depth++;
}  /* End of start handler */

void XMLCALL end(void *data, const char *el) 
{
  Current = Current->Parent ;
  Depth--;
}  /* End of end handler */

void UpdateActions (struct Node *Root)
{
  struct Node *This ;
  
  for (This=Root;This!=NULL;This=This->Next) {
    if (This->Type==N_ACTION) {
      This->Data.Aktion.Unit = FindNode(Haus->Child,This->Data.Aktion.UnitName) ;
      if (This->Data.Aktion.Unit==NULL) {
	ParseError = 1 ;
	fprintf (stderr,"Konnte Aktor %s nicht finden\n",This->Data.Aktion.UnitName) ;
      } ;
    } ;
    // Rekursiv alle updaten
    UpdateActions(This->Child) ;
  } ;
}

/* Parse a document from the open file descriptor 'fd' until the parse
   is complete (the document has been completely parsed, or there's
   been an error), or the parse is stopped.  Return non-zero when
   the parse is merely suspended.
*/
int parse_xml(XML_Parser p, int fd)
{
  for (;;) {
    int bytes_read;
    enum XML_Status status;
    
    void *buff = XML_GetBuffer(p, BUFF_SIZE);
    if (buff == NULL) {
      /* handle error... */
      ParseError = 1 ;
      fprintf (stderr,"No Buffer\n"); 
      return 0;
    }
    bytes_read = read(fd, buff, BUFF_SIZE);
    if (bytes_read < 0) {
      /* handle error... */
      ParseError = 1 ;
      fprintf (stderr,"File Error\n"); 
      return 0;
    }
    status = XML_ParseBuffer(p, bytes_read, bytes_read == 0);
    switch (status) {
    case XML_STATUS_ERROR:
      /* handle error... */
      ParseError = 1 ;
      fprintf (stderr,"Parse Error: %s at Line %d, Column %d \n",XML_ErrorString(XML_GetErrorCode(p)),(int)XML_GetCurrentLineNumber(p),(int)XML_GetCurrentColumnNumber(p)); 
      return 0;
    case XML_STATUS_SUSPENDED:
      return 1;
    case XML_STATUS_OK:
      break ;
    }
    if (bytes_read == 0)
      return 0;
  }
}

int ReadConfig(void) 
{
  int InFile ;
  XML_Parser p;
  
  ParseError = 0 ;

  p = XML_ParserCreate(NULL);
  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    return(1);
  }
  
  InFile = open("NodeConf/Config.xml",O_RDONLY) ;
  if (InFile==-1) {
    fprintf(stderr,"Konnte Datei nicht oeffnen") ;
    return(1) ;
  }
 
  Current = NULL; 
  
  XML_SetElementHandler(p, start, end);
  
  parse_xml(p,InFile) ;

  UpdateActions(Haus->Child) ;

  XML_ParserFree(p);
  close(InFile) ;

  return (ParseError) ;
} 
