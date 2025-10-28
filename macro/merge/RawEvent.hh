/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef _RawEvent_hh_
#define _RawEvent_hh_ 1

#include <TROOT.h>
#include <TSystem.h>
#include <TArrayS.h>

#define	  HEADER 	"\033[95m"
#define    OKBLUE 	"\033[94m"
#define    OKBLUE2 	"\033[1;34m"
#define    OKCYAN 	"\033[96m"
#define    OKGREEN 	"\033[92m"
#define    OKGREEN2 	"\033[1;32m"
#define    WARNING 	"\033[93m"
#define    FAIL 		"\033[91m"
#define    FAIL2 		"\033[1;31m"
#define    ENDC 		"\033[0m"
#define    BOLD 		"\033[1m"
#define    DARK 		"\033[1;30m"
#define    UNDERLINE "\033[4m"

using namespace std;


class RawEvent{
   public:
      Long64_t             Entry       = -1;
      unsigned short       Channel     = -1;
      unsigned short       Board       = -1;
      unsigned short       Energy      = -1;
      unsigned short       EnergyShort = -1;
      unsigned int         Flags       = -1;
      unsigned long long   Timestamp   = 0;
      TArrayS              Samples;

   public:
      RawEvent()     { clear(); };
      ~RawEvent()    {};

      void clear();

};

void RawEvent::clear()
{
   Entry       = -1;
   Channel     = -1;
   Board       = -1;
   Energy      = -1;
   EnergyShort = -1;
   Flags       = -1;
   Timestamp   = 0;
   Samples.Reset();
}


#endif
