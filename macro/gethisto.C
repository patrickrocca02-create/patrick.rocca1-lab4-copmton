//Digitizer data from the LAB

struct slimport_data_t {

    Long64_t   Entry;
    UShort_t   Channel;
    UShort_t   Board;
    UShort_t   Energy;
    UShort_t   EnergyShort;
    UInt_t     Flags;
    ULong64_t  Timestamp;

};

TH1D* getHistoFromTree(const char *name_file, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("Data_B");
	TBranch *inbranch = intree->GetBranch("Channel_0");
	intree->SetBranchAddress("Channel_0.Energy", &indata.Energy);
	TH1D *h_spectrum = new TH1D("h_spectrum","Total spectrum",numBins,minX,maxX);
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		h_spectrum->Fill(indata.Energy);
	}
	// return
	return h_spectrum;
}

TH1D* getHistoForChannelFromTree(const char *name_file, short chan, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("Data_B");
	TBranch *inbranch = intree->GetBranch(Form("Channel_%d",chan));
	intree->SetBranchAddress(Form("Channel_%d.Energy",chan), &indata.Energy);
	TH1D *h_spectrum = new TH1D("h_spectrum","Total spectrum",numBins,minX,maxX);
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		h_spectrum->Fill(indata.Energy);
	}
	// return
	return h_spectrum;
}


TH1D* getHistoWithFilter(const char *name_file, int numBins, double minX, double maxX, double lowThr = 0, double highThr = 999999) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("Data_B");
	TBranch *inbranch = intree->GetBranch("Channel_0");
	intree->SetBranchAddress("Channel_0.Energy", &indata.Energy);
	TH1D *h_spectrum = new TH1D("h_spectrum","Total spectrum",numBins,minX,maxX);
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		if (indata.Energy>lowThr && indata.Energy<highThr) {
			h_spectrum->Fill(indata.Energy);
		}
	}
	// return
	return h_spectrum;
}

