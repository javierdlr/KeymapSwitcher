#include <exec/libraries.h>
#include <libraries/commodities.h>
#include <dos/dos.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/alib_stdio_protos.h>
#include <clib/commodities_protos.h>
#include <clib/dos_protos.h>

struct Library *CxBase;
CxObj *broker;

struct NewBroker newbroker = 
{    
  NB_VERSION,   /* nb_Version - Version of the NewBroker structure */    
  "facticheski test brokera", /* nb_Name - Name Commodities uses to identify this commodity */
  "broker",     /* nb_Title - Title of commodity that appears in CXExchange */    
  "a simple example of a broker",  /* nb_Descr - Description of the commodity */
  0,            /* nb_Unique - Tells CX not to launch another commodity with same name */ 
  0,            /* nb_Flags - Tells CX if this commodity has a window */    
  0,            /* nb_Pri - This commodity's priority */    
  0,            /* nb_Port - MsgPort CX talks to */    
  0             /* nb_ReservedChannel - reserved for later use */
};


void main(void)
{
     CxBase=OpenLibrary("commodities.library",37L);  // open lib     
     broker = CxBroker(&newbroker, NULL);            // cxbroker()    
     ActivateCxObj(broker, 1L);                      // activatecxobj()   

     Delay(10*50);   // delay (test by 'Scout' as was create commoditie

     DeleteCxObj(broker);  // remove commoditie
     CloseLibrary(CxBase); // close lib
} 
