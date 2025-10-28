/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef _Merger_hh_
#define _Merger_hh_ 1

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

#include    "RawEvent.hh"
#include    "MergedEvent.hh"
#include    "Reader.hh"
#include    "ChannelID.hh"

using namespace std;


class Merger{
   public:
      map<ULong64_t,MergedEvent> GTS_ME;

      // vector<TString>            FileNames;
      // vector<TString>            TreeNames;
      vector<ChannelID>          ChannelList;
      vector<Reader>             Readers;

      bool                       includesamples;

   public:
      Merger( bool _includesamples=false );
      ~Merger()    {};

      void                 clear();
      bool                 AddChannel( int channel, TString _filename, TString _treename="Data_R" , int _board=0 );
      bool                 AddChannels( TString _channels, TString _filename, TString _treename="Data_R" );
      bool                 ReadTS();

      vector<TString>      GetFileNames()    ;
      vector<TString>      GetTreeNames()    ;
      int                  GetNChannels()    {  return ChannelList.size();    };
      vector<ChannelID>    GetIDList()  {  return ChannelList;  };
      TString              GetIDstring();

      Reader*              GetReader( int _ID) {   for(auto& r : Readers) if( r.GetID()==_ID){ return &r;}   return nullptr; }

      ULong64_t            GetNEventsInCoincidence(int _foldmin, int _foldmax);
      bool                 ApplyMergeWindow(ULong64_t _MergeWindowSize=1000);

      bool                 LoadTSEventNumberMap();

      void                 Print();
};


Merger::Merger( bool _includesamples )
{
   clear();
   includesamples=_includesamples;
}


void Merger::clear()
{
   GTS_ME.clear();
   ChannelList.clear();
   // FileNames.clear();
   // TreeNames.clear();
}

TString              Merger::GetIDstring()
{
   TString s="";
   for(auto& c : ChannelList){
      // s+=Form("ch%ibo%i ",c.GetChannel(),c.GetBoard());
      s+=Form("id%i ",c.GetID());
   }
   // cout << " kkkk " << s << endl;
   return s;
}


vector<TString>      Merger::GetFileNames()
{
   vector<TString> res;
   for(auto& c : ChannelList) res.push_back(c.GetFileName());
   return res;
}


vector<TString>      Merger::GetTreeNames()
{
   vector<TString> res;
   for(auto& c : ChannelList) res.push_back(c.GetTreeName());
   return res;
}


bool  Merger::AddChannel( int _channel, TString _filename, TString _treename, int _board )
{
   if(!gSystem->IsFileInIncludePath(_filename)){
      cerr << FAIL << "File not found [" << _filename << "] (press a key...)" << ENDC << endl;
      cin.get();
      return false;
   } else {
      cout << DARK << "Adding Channel " << _channel << endl;
   }

   ChannelID newchannel ( _channel, _board, _filename, _treename , ChannelList.size() );
   newchannel.Print();
   // FileNames.push_back( _filename);
   // TreeNames.push_back( _treename);

   Readers.push_back( Reader( _channel, _filename, _treename , includesamples , _board , newchannel.GetID() ) );
   ChannelList.push_back( newchannel );

   GetReader(newchannel.GetID())->LoadEventMap();
   GetReader(newchannel.GetID())->Print();


   return true;
}


bool  Merger::AddChannels( TString _channels, TString _filename, TString _treename )
{
   if(!gSystem->IsFileInIncludePath(_filename)){
      cerr << FAIL << "File not found [" << _filename << "] (press a key...)" << ENDC << endl;
      cin.get();
      return false;
   }

   TString ch_list = _channels+" ";
   ch_list.ToUpper();
   for(int b=0; b<10 ; b++){
      for(int i=0; i<16; i++){
         TString the_ch = Form("CH%i ",i);
         if( ch_list.Contains(the_ch) && b==0 ){
            AddChannel(i,_filename,_treename,b);
            continue;
         }
         TString the_ch_bo = Form("CH%iBO%i ",i,b);
         if ( ch_list.Contains(the_ch_bo) ){
            AddChannel(i,_filename,_treename, b);
         }
      }
   }

   return true;
}


void  Merger::Print()
{
   cout << OKBLUE2 ;
   cout << "Printing Merger: " << endl;
   cout << "  Merged events: " << GTS_ME.size()<< endl;
   cout << "  N. Channels:   [ " << ChannelList.size() << " ]" << endl ;
   cout << "  ChannelList:    " << endl ;
   for(int i=0; i<(int)ChannelList.size(); i++) {
      cout << " \t\t ID=" << ChannelList[i].GetID()  << " [ ch" << ChannelList[i].GetChannel() << ",bo" << ChannelList[i].GetBoard() ;
      cout << " -> " << ChannelList[i].GetFileName() << " " << ChannelList[i].GetTreeName() << " Traces:";
      if ( GetReader(ChannelList[i].GetID())->includesamples ) cout << "ENABLED" ;
      else              cout << "DISABLED" ;
      cout << " ] " << endl;
   }
   cout << ENDC << endl;
}


ULong64_t  Merger::GetNEventsInCoincidence(int _foldmin, int _foldmax)
{
   ULong64_t n=0;
   for(auto& gts_me : GTS_ME){
      int nchannels = gts_me.second.GetNChannels();
      if( nchannels<_foldmin) continue;
      if( nchannels>_foldmax) continue;
      n++;
   }
   return n;
}


bool  Merger::ReadTS()
{
   for( auto& c : ChannelList ){
      cout << DARK << "Reading TS for channel [" << c.GetChannel() << "] board [" << c.GetBoard() << "]   inner ID [" << c.GetID() << "]" << ENDC << endl;
      Long64_t TotalEntries = GetReader(c.GetID())->TS_EV.size();
      Long64_t counter =0;
      GetReader(c.GetID())->Print();
      auto it = GetReader(c.GetID())->TS_EV.begin();
// cin.get();
      while (it != GetReader(c.GetID())->TS_EV.end()) {
         if (GTS_ME.count(it->first) == 0) {
            MergedEvent new_ev;
            new_ev.Add( c.GetID(), it->first );
            new_ev.SetGlobalTS( it->first );
            GTS_ME.emplace( it->first, std::move(new_ev) );
            // GTS_ME[it->first] = new_ev;
            // cout << "adding" << endl;
         } else {
            auto stored_ev = GTS_ME.lower_bound( it->first );
            stored_ev->second.Add( c.GetID(), it->first );
         }
         // it = GetReader(c.GetID())->TS_EV.erase(it);
         ++it;
         if( counter%10000==0 || counter>=(int) (GetReader(c.GetID())->TS_EV).size()-3 )
            cout << HEADER << "Processed " << std::setw(9) << counter << "  / " << std::setw(9) << TotalEntries << " = " << std::fixed << std::setprecision(2) << 100.*((double) counter)/((double) TotalEntries) << "%" << ENDC << "\r" <<  flush ;
         ++counter;
      }
      cout << endl;
   }
   return true;
}


bool  Merger::ApplyMergeWindow(ULong64_t _MergeWindowSize)
{
   cout << OKGREEN2 << "Applying Merge Window: " <<_MergeWindowSize << ENDC<< endl;
   ULong64_t N_merged = 0;
   ULong64_t counter = 0;

   bool one_erased = false;
   for (auto gts_me = GTS_ME.begin(); gts_me != GTS_ME.end(); ++gts_me) {
      if(one_erased && gts_me!=GTS_ME.begin()) { --gts_me; counter--; }
      counter++;
      one_erased=false;
      /*** discard channels with all channels present ***/
      if(gts_me->second.GetNChannels()==((int)ChannelList.size())){
         // cout << OKGREEN2 << "good event " << gts_me->first << ", all channels present " << gts_me->second.GetNChannels()<< "="<< NChannels << ", skipping" << ENDC << endl;
         continue;
      }
      /*** search the closest channel ***/
      map<ULong64_t,MergedEvent>::iterator closest_ev = GTS_ME.lower_bound(gts_me->first+1) ;
      if ( ! (closest_ev != GTS_ME.end()) ) {
         // cout << DARK << "No event found for " << gts_me->first+1 << ", skipping" << ENDC << endl;
         continue;
      }
      /*** check that it's inside the coincidence window ***/
      if ( closest_ev->first > gts_me->first+_MergeWindowSize ){
         // cout << DARK << "outside coincidence window " << closest_ev->first << ">" <<  gts_me->first << "+" << _MergeWindowSize << ", skipping" << ENDC << endl;
         continue;
      }
      /*** check that the closest event is not complete ***/
      if( closest_ev->second.GetNChannels()==((int)ChannelList.size()) ){
         // cout << DARK << "closest_ev " << closest_ev->first << " has already all channels, cannot merge " << ENDC << endl;
         continue;
      }
      bool status = gts_me->second.MergeWith( closest_ev->second );
      if( status ) {
         // cout << DARK << "Merged succesfully" << ENDC << endl;
         GTS_ME.erase(closest_ev);
         one_erased=true;
         N_merged++;
      } else {
         // cout << FAIL << "attempted to merge but failed" << ENDC << endl;
      }

      if( counter%10000==0 || counter>=GTS_ME.size()-2 ){
         cout << HEADER << "Processed " << std::setw(9) << counter << "  / " << std::setw(9) << GTS_ME.size() << " = " << std::fixed << std::setprecision(2) << 100.*((double) counter)/((double) GTS_ME.size()-1) << "%" << ENDC << "\r"<< flush ;
      }
   }
   cout << endl;
   cout << OKGREEN2 << "N. Merged events: " <<N_merged << ENDC<< endl;
   return true;
}



#endif
