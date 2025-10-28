/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef _Writer_hh_
#define _Writer_hh_ 1

#include <iostream>
#include <sstream>
#include <string>

#include "TROOT.h"
#include "TSystem.h"
#include "ChannelID.hh"

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

#define     NCHANNELS 3

using namespace std;


class Writer {
   public:
      Merger*  TheMerger;
      TString  OutputFileName;
      TString  OutputTreeName;

   public:
      Writer( Merger* _merger=nullptr, TString _outputfile="", TString _outputtree="Data_B" );
      ~Writer()    {  clear();   };

      void clear();

      bool  SetOutputFile(TString _filename, TString _treename="Data_B");
      bool  SetMerger(Merger* _merger);
      void  ExtractInfo( RawEvent* _theev , Reader* _thereader);
      bool  WriteFile();

      void Print();
};


Writer::Writer( Merger* _merger, TString _outputfile, TString _outputtree )
{
   clear();
   SetOutputFile( _outputfile, _outputtree );
   SetMerger(_merger);
};


void Writer::clear()
{
   TheMerger=nullptr;
   OutputFileName="";
}


bool  Writer::SetOutputFile(TString _filename, TString _outputtree)
{
   if ( gSystem->IsFileInIncludePath(_filename)) {
      cerr << WARNING << "File found [" << _filename << "]... press a key to overwrite" << ENDC << endl;
      cin.get();
   }
   OutputFileName = _filename;
   OutputTreeName = _outputtree;
   return true;
}

bool  Writer::SetMerger( Merger* _merger)
{
   if ( !_merger ) {
      cerr << WARNING << "Empty Merger pointer" << ENDC << endl;
      return false;
   }
   TheMerger = _merger;
   return true;
}


void  Writer::Print()
{
   cout << endl;
   cout << OKBLUE2 ;
   cout << "Printing Writer: " << endl;
   cout << "  Output file name:      " << OutputFileName << endl;
   cout << "  Output tree name:      " << OutputTreeName << endl;
   cout << "  Merger events fold " << TheMerger->GetNChannels() << " : " << TheMerger->GetNEventsInCoincidence(TheMerger->GetNChannels(), TheMerger->GetNChannels())<< endl;
   cout << "  Merger Input :         " << endl ;
   for(auto& c : TheMerger->GetIDList()) {
      cout << "\t\t\t [ ID" << c.GetID() << ":  ch" << c.GetChannel() << ",bo" << c.GetBoard() << " -> " << c.GetFileName() << " " << c.GetTreeName() << " ] " << endl;
   }
   cout << ENDC << endl;
}



void  Writer::ExtractInfo( RawEvent* _theev , Reader* _thereader)
{
   _theev->Channel     = **(_thereader->ch);
   _theev->Board       = **(_thereader->bo);
   _theev->Energy      = **(_thereader->en);
   _theev->EnergyShort = **(_thereader->es);
   _theev->Flags       = **(_thereader->fl);
   _theev->Timestamp   = **(_thereader->ts);
   if(_thereader->includesamples)   _theev->Samples     = **(_thereader->sa);
}



bool  Writer::WriteFile()
{
   TheMerger->GetNChannels();
   cout << HEADER << "Building file for [" << TheMerger->GetNChannels() << "] channels" << ENDC << std::endl;

   TFile* fout = new TFile( OutputFileName, "RECREATE" );
   TTree *tree = new TTree( OutputTreeName, "Tree from raw data merged" );
   ULong64_t GlobalTimestamp=0;
   RawEvent channel[TheMerger->GetNChannels()];
   tree->Branch("GlobalTimestamp", &GlobalTimestamp);
   // for (int b = 0; b < (int)TheMerger->GetNChannels(); b++) {
   for (auto& c : TheMerger->GetIDList() ) {
       // tree->Branch(Form("Channel_%i", TheMerger->GetChannelList()[b]), &(channel[b]), "Channel/s:Board/s:Energy/s:EnergyShort/s:Flags/i:Timestamp/l");
      TString bname = Form("Channel_%i", c.GetID());
      cout << "Adding output branch " << bname << endl;
      tree->Branch(
                     bname,
                     &(channel[c.GetID()])
                   );
   }

   cout << endl << endl<< endl;
   ULong64_t n=-1;
   cout << endl;
   stringstream ss;
   string usermsg="";
   for(auto& ts_ev : TheMerger->GTS_ME ){
      n++;
      ss.str("");
      ss.clear();
      ss << OKBLUE2 << "Processing TS = [" << std::setw(16) << ts_ev.first << "]\t" << endl << "\t" ;
      /*** Writing events with all channels ***/
      if( ts_ev.second.GetNChannels()!= TheMerger->GetNChannels()){
         ss << ENDC << endl;
         continue;
      }

      /*** Loading variables and writing ***/
      for (auto& c : TheMerger->GetIDList() )    channel[c.GetID()].clear();
      for (auto& c : TheMerger->GetIDList()) {
         int currentchannel = c.GetChannel();
         int currentboard = c.GetBoard();
         ULong64_t currentTS = ts_ev.second.GetTS( c.GetID() );
         // cout << " searching id" <<  c.GetID() << " ts" << currentTS << endl;
         // cout << " asking to reader ch" << TheMerger->Readers[c.GetID()].GetChannel() << " b" << TheMerger->Readers[c.GetID()].GetBoard() << endl;
         // cout << " TheMerger->Readers[c.GetID()].TS_EV size " << TheMerger->Readers[c.GetID()].TS_EV.size() << endl;
         // int asd=0;
         // for(auto& r : TheMerger->Readers[c.GetID()].TS_EV){
         //    // cout << r.first << " " << r.second << endl;
         //    asd++;
         //    if(asd>5)break;
         // }
         map<ULong64_t,Long64_t>::iterator it = TheMerger->Readers[c.GetID()].TS_EV.lower_bound(currentTS);
         if ( it == TheMerger->Readers[c.GetID()].TS_EV.end() ) {
            cerr << FAIL << "Cannot find entry for channel [" << currentchannel << "] and TS [" << currentTS << "]...press a key" << ENDC << endl;
            cin.get();
            return false;
         }
         Long64_t  currentEntry = it->second;
         ss << DARK << "\t CH:[" << std::setw(2) << currentchannel << "] TS:[" << std::setw(16)<< currentTS << "] EV:[" << std::setw(9) << currentEntry << "]" ;
         TheMerger->Readers[c.GetID()].dataReader->SetEntry( currentEntry );
         ExtractInfo( &(channel[c.GetID()]) , &(TheMerger->Readers[c.GetID()]) );
         channel[c.GetID()].Entry = currentEntry;

         if (channel[c.GetID()].Timestamp!=currentTS) { cout << "error " <<channel[c.GetID()].Timestamp << " " <<currentTS<< endl; cin.get();  }
      }
      GlobalTimestamp = ts_ev.second.GetGlobalTS();
      tree->Fill();
      ss << ENDC << endl;

      /*** Print progress ***/
      if( n%10000==0 || n==1 || n>=TheMerger->GTS_ME.size()-2 ){
         //cout << usermsg;  // Output: Hello, world! 42
         ss << HEADER << "Processed " << std::setw(9) << n << "  / " << std::setw(9) << TheMerger->GTS_ME.size() << " = " << std::fixed << std::setprecision(2) << 100.*((double) n)/((double) TheMerger->GTS_ME.size()-1) << "%" << ENDC ;
         usermsg = ss.str();
         std::cout << "\033[F\033[F" << usermsg << flush;
         // cin.get();
      }
   }
   cout << endl;

   tree->Write();
   fout->Close();

   return true;
}



#endif
