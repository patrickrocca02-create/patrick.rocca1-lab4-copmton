/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef __Merge_C_
#define __Merge_C_ 1

#include <iostream>
#include <map>

#include "RawEvent.hh"
#include "MergedEvent.hh"
#include "Merger.hh"
#include "Writer.hh"

#define _verbose_ 1

using namespace std;



void Merge( TString _RawFileName="DataR_run_TAC30ns.root", TString _treename="Data_R", TString _Channel_list ="ch0 ch1 ch2 ch3" , TString _MergedFileName="merged.root" , ULong64_t _MergeWindowSize=4000 , bool _includesamples=false)
{
   int n=0, nprint=20;

   cout << endl << WARNING << "==================== CREATING MERGER OBJECT ====================" << ENDC << endl;
   Merger merger( _includesamples );
   merger.AddChannels( _Channel_list, _RawFileName, _treename );
   merger.Print();
   TString chIDS = merger.GetIDstring();

for(auto& r : merger.Readers ){
   n=0;
   cout << "................printing " << nprint << " events of next channel................" << endl;
   for(auto& ts_ev : r.TS_EV ){
      if( n<nprint  ){
         cout << "ch:["  << std::setw(2) << r.GetChannel() << "] bo:[" << r.GetBoard() << "] ";
         cout << " TS:[" << std::setw(15)<< ts_ev.first << "]  EV:[" << ts_ev.second << "]" << endl;
      }
      n++;
   }
}

   cout << endl << WARNING << "==================== READING TIMESTAMPS ====================" << ENDC << endl;
   merger.ReadTS();
   merger.Print();
   for(int i=1; i<=(int )merger.GetNChannels(); i++){      cout << "N. Events fold " << i << " = " << merger.GetNEventsInCoincidence(i,i) << endl;   }
   n=0;
   nprint=20;
   cout << endl << WARNING << "==================== SHOWING FIRST AND LAST " << nprint << " MERGED EVENTS ====================" << ENDC << endl;
   for(auto& ts_ev : merger.GTS_ME ){
      if( n<nprint || (unsigned long long ) n>=merger.GTS_ME.size()-nprint  )    ts_ev.second.Print(chIDS);
      else if ( n==nprint )                            cout << "\t................................." << endl;
      n++;
   }

   cout << endl << WARNING << "==================== MERGING EVENTS WITHIN COINCIDENCE WINDOW ====================" << ENDC << endl;
   merger.ApplyMergeWindow(_MergeWindowSize);
   merger.Print();
   for(int i=1; i<=merger.GetNChannels(); i++){      cout << "N. Events fold " << i << " = " << merger.GetNEventsInCoincidence(i,i) << endl;   }

   n=0;
   cout << endl << WARNING << "==================== SHOWING FIRST AND LAST " << nprint << " MERGED EVENTS ====================" << ENDC << endl;
   for(auto& ts_ev : merger.GTS_ME ){
      if( n<nprint || (unsigned long long ) n>=merger.GTS_ME.size()-nprint  )    ts_ev.second.Print(chIDS);
      else if ( n==nprint )                            cout << "\t................................." << endl;
      n++;
   }


   cout << endl << WARNING << "==================== CREATING WRITER OBJECT ====================" << ENDC << endl;
   Writer writer( &merger, _MergedFileName );
   writer.Print();

   cout << endl << WARNING << "==================== WRITING OUTPUT FILE FROM MERGED EVENTS ====================" << ENDC << endl;
   writer.WriteFile();

   cout << endl << WARNING << "==================== MERGE ENDED ====================" << ENDC << endl;
}


#endif
