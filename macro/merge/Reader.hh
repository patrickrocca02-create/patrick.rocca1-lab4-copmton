/***       PhysicsLaboratoryMacros      ***/
/***       Pablo Aguilera Jorquera      ***/
/***      pablo.aguilera@pd.infn.it     ***/
/***     Paolotti building, room 210    ***/
/***       Last update 31/10/2024       ***/

#ifndef _Reader_hh_
#define _Reader_hh_ 1

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

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


class Reader{
   public:
      TString                                FileName;
      TString                                TreeName;
      int                                    Channel;
      int                                    Board;
      int                                    ID;

      map<ULong64_t,Long64_t>                TS_EV;

      bool                                   includesamples=false;

      TFile*                                 f;
      TTreeReader*                           dataReader;
      TTreeReaderValue<unsigned short>*      ch;
      TTreeReaderValue<unsigned short>*      bo;
      TTreeReaderValue<unsigned short>*      en;
      TTreeReaderValue<unsigned short>*      es;
      TTreeReaderValue<unsigned int>*        fl;
      TTreeReaderValue<unsigned long long>*  ts;
      TTreeReaderValue<TArrayS>*             sa = nullptr;

   public:
      Reader( int _ch, TString _filename="", TString _treename="Data_R" , bool _includesamples=false , int _bo=0, int _id=-1 );
      ~Reader()   { clear(); };

      void        clear();
      bool        SetFile(TString _filename, TString _treename="Data_R");
      bool        SetChannel( int _ch , int _bo=0 )   {  Channel=_ch; Board=_bo; return true; };
      void        SetID( int _id )  {  ID=_id; };

      TString     GetFileName()     {  return FileName;  };
      TString     GetTreeName()     {  return TreeName;  };
      int         GetChannel()      {  return Channel;  };
      int         GetBoard()        {  return Board;  };
      int         GetID()           {  return ID;  };
      void        LoadEventMap(  );

      void        Print();
};


Reader::Reader( int _ch, TString _filename, TString _treename , bool _includesamples, int _bo, int _id)
{
   clear();
   SetFile(_filename,_treename);
   SetChannel(_ch,_bo);
   SetID(_id);
   includesamples = _includesamples;

   f = new TFile( _filename );
   if (!f) {
       std::cerr << "Cannot open file: " << _filename << std::endl;
       return;
   }
   dataReader = new TTreeReader( _treename, f);
   ch = new TTreeReaderValue<unsigned short>          ( *dataReader, "Channel" );
   bo = new TTreeReaderValue<unsigned short>          ( *dataReader, "Board" );
   en = new TTreeReaderValue<unsigned short>          ( *dataReader, "Energy" );
   es = new TTreeReaderValue<unsigned short>          ( *dataReader, "EnergyShort" );
   fl = new TTreeReaderValue<unsigned int>            ( *dataReader, "Flags" );
   ts = new TTreeReaderValue<unsigned long long>      ( *dataReader, "Timestamp" );

   if(includesamples)  sa = new TTreeReaderValue<TArrayS> ( *dataReader, "Samples" );
}


void Reader::clear()
{
   FileName = "";
   TreeName = "";
   Channel = -1;
   Board = -1;
   TS_EV.clear();
   //if(f.IsOpen()) f.Close();
}


bool Reader::SetFile(TString _filename, TString _treename )
{
   if(!gSystem->IsFileInIncludePath(_filename)){
      cerr << FAIL << "File not found [" << _filename << "] (press a key...)" << ENDC << endl;
      cin.get();
      return false;
   }
   FileName = _filename;
   TreeName = _treename;
   return true;
}


void  Reader::Print()
{
   cout << OKBLUE2 ;
   cout << "Printing Reader: " << endl;
   cout << "  ID         :  " << ID << endl;
   cout << "  FileName   :  " << FileName << endl;
   cout << "  TreeName   :  " << TreeName << endl;
   cout << "  Channel    :  [ " << Channel << " ]" << endl ;
   cout << "  Board      :  [ " << Board << " ]" << endl ;
   cout << "  Samples    :  [ ";
   if (includesamples)  cout << "ENABLED ]" << endl;
   else                 cout << "DISABLED ]" << endl;
   cout << ENDC << endl;
}


void  Reader::LoadEventMap( )
{
   TFile* f = new TFile(FileName);
   if ( !f ) {  cerr << FAIL << "Cannot open file: " << FileName << ENDC << endl;  return ;  }
   cout << HEADER << "From File:                         " << FileName << ENDC << endl;
   cout << HEADER << "Preparing event map for channel:   " << Channel << ENDC << endl;

   TTreeReader dataReader( TreeName, f);
   TTreeReaderValue<unsigned short>       ch ( dataReader, "Channel" );
   TTreeReaderValue<unsigned short>       bo ( dataReader, "Board" );
   TTreeReaderValue<unsigned long long>   ts ( dataReader, "Timestamp" );

   Long64_t TotalEntries = dataReader.GetEntries();
   Long64_t counter =0;
   while ( dataReader.Next() ) {
      if( *ch != (unsigned int) Channel ) continue;
      if( *bo != (unsigned int) Board ) continue;
      TS_EV[ *ts ] = dataReader.GetCurrentEntry();
      // if ( TS_EV.size()>10) break;
      if(counter%10000==0) cout << HEADER << "Processed " << std::setw(9) << counter << "  / " << std::setw(9) << TotalEntries << " = " << std::fixed << std::setprecision(2) << 100.*((double) counter)/((double) TotalEntries) << "%" << ENDC << "\r" <<  flush ;
      ++counter;
   }

   delete f;
   cout << HEADER << "Loaded [" << TS_EV.size() << "] in event map"<< ENDC << endl;
}





#endif
