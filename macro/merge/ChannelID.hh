/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef _ChannelID_hh_
#define _ChannelID_hh_ 1

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


class ChannelID{
   public:
      int                  ID          = -1;
      int                  Channel     = -1;
      int                  Board       = -1;
      TString              FileName;
      TString              TreeName;

   public:
      ChannelID(int _Channel=-1, int _Board=-1, TString _FileName="", TString _TreeName="", int _ID=-1);
      ~ChannelID()    {};

      int         GetID()        { return ID; }
      int         GetChannel()   { return Channel; }
      int         GetBoard()     { return Board; }
      TString     GetFileName()  { return FileName; }
      TString     GetTreeName()  { return TreeName; }
      void        Print();

      void clear();

};


ChannelID::ChannelID( int _Channel, int _Board, TString _FileName, TString _TreeName, int _ID )
{
   clear();
   Channel = _Channel;
   Board = _Board;
   FileName = _FileName;
   TreeName = _TreeName;
   ID = _ID;
}


void ChannelID::clear()
{
   ID          = -1;
   Channel     = -1;
   Board       = -1;
   FileName    = "";
   TreeName    = "";
}


void  ChannelID::Print()
{
   cout << OKBLUE2 ;
   cout << "Printing ChannelID: " << endl;
   cout << "  ID         :  " << ID << endl;
   cout << "  Channel    :  [ " << Channel << " ]" << endl ;
   cout << "  Board      :  [ " << Board << " ]" << endl ;
   cout << "  FileName   :  " << FileName << endl;
   cout << "  TreeName   :  " << TreeName << endl;
   cout << ENDC << endl;
}



#endif
