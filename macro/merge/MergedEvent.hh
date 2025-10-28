/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef _MergedEvent_hh_
#define _MergedEvent_hh_ 1

#include <iostream>
#include <iomanip>

#include "TROOT.h"
#include "TSystem.h"

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


class MergedEvent{
   public:

      ULong64_t                     Global_TS;
      vector<pair<int,ULong64_t>>   ID_TS;
      bool                          IsMerged;

   public:
      MergedEvent()     { clear(); };
      ~MergedEvent()    {};

      void clear();
      bool Add( int _ch, ULong64_t _ts);
      void SetGlobalTS( ULong64_t _ts )   {  Global_TS = _ts;  }
      ULong64_t GetGlobalTS( )            {  return Global_TS;  }
      void Print(TString _channel_list="");

      int            GetNChannels()       {  return (int) ID_TS.size(); };
      vector<int>    GetIDList() const;
      ULong64_t      GetTS( int _ch) const;
      bool           ContainsID( int _id) const;
      bool           MergeWith( const MergedEvent& _otherev );

      MergedEvent&   operator=( MergedEvent other)
                     {
                        Global_TS = other.Global_TS;
                        ID_TS = other.ID_TS;
                        return *this;
                      }
};

void MergedEvent::clear()
{
   ID_TS.clear();
   Global_TS = 0;
   IsMerged=false;
}


bool MergedEvent::Add( int _id, ULong64_t _ts)
{
   /*** check that the channel was not already readed ***/
   for(auto& id_ts : ID_TS){
      if( id_ts.first == _id ){
         cerr << FAIL << "ID " << _id << " already in MergedEvent, cannot add" << ENDC << endl;
         return false;
      }
   }
   /*** Channel not readed before, storing ***/
   ID_TS.push_back( make_pair(_id, _ts) );
   return true;
}


vector<int> MergedEvent::GetIDList() const
{
   std::vector<int> list;
   for(int id=0; id<20 ; id++){
      for( auto& id_ts : ID_TS){
         if (id_ts.first!=id) continue;
         list.push_back(id);
      }
   }
   return list;
}


ULong64_t  MergedEvent::GetTS( int _id) const
{
   for( auto& id_ts : ID_TS){
      if (id_ts.first!=_id) continue;
      return id_ts.second;
   }
   cerr << FAIL << "Cannot find ID [" << _id << "] in MergedEvent with GlobalTS [" << Global_TS << "]" << ENDC << endl;
   return 0;
}


bool MergedEvent::ContainsID( int _id) const
{
   for(auto& id_ts : ID_TS){
      if( id_ts.first == _id) return true;
   }
   return false;
}


bool MergedEvent::MergeWith( const MergedEvent& _otherev )
{
   /*** check that the other event do not contains my channels ***/
   for(auto ch : GetIDList()){
      if ( _otherev.ContainsID(ch) ) return false;
   }
   /*** check that I do not contain the other event channels ***/
   for(auto ch : _otherev.GetIDList()){
      if ( ContainsID(ch) ) return false;
   }
   /*** copy the info ***/
   for(auto ch : _otherev.GetIDList()){
      bool status = Add( ch , _otherev.GetTS(ch) );
      if (!status) return false;
   }
   IsMerged=true;
   return true;
}


void MergedEvent::Print(TString _id_list)
{
   TString CL = " "+_id_list+" ";
   CL.ToUpper();
   cout << BOLD << UNDERLINE << "GlobalTS" << ENDC << ":[" << BOLD << setw(16) << Global_TS << "]\t\t" << ENDC;
   if (IsMerged) cout << UNDERLINE;
   for( int id=0; id<100; id++){
      // cout << "searching " <<  Form("ID%i ",id) << endl;
      if ( !CL.Contains( Form("ID%i ",id) ) ) {
         // cout << DARK << "channel " << ch << "not in list" <<ENDC << endl;
         continue;
      }
      if ( !ContainsID(id) ) {
         cout << Form("[%16s]","-") ;
      } else {
         cout << Form("[%16llu]", GetTS(id))  ;
      }
      cout << "\t";
   }
   cout << ENDC << endl;
}



#endif
