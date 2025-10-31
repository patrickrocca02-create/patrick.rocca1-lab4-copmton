void prova_fondo(const char *name_file,float acq_time, const char *fondo_name_file, float acq_time_fondo, int numBins = 500, double minX = 0, double maxX = 2000, double lowThr = 300, double highThr = 700, float c=1) {
    TFile *infile = new TFile(name_file);
    TTree *intree = (TTree*)infile->Get("Data_B");

    //fondo
    TFile *infile_fondo = new TFile(fondo_name_file);
    TTree *intree_fondo = (TTree*)infile_fondo->Get("Data_B");
    TH1F *fondo = new TH1F("fondo","Fondo", numBins, minX, maxX);
    intree_fondo->Project("fondo", "Channel_2.Energy");
    
    TH1F *raw_h = new TH1F("raw_h", "Raw Energy", numBins, minX, maxX);
    intree->Project("raw_h", "Channel_2.Energy");

    float rate = raw_h->GetEntries()/acq_time;
    float rate_fondo = fondo->GetEntries()/acq_time_fondo;
    // fondo->Scale(acq_time/acq_time_fondo);
    

    TCanvas *canvas = new TCanvas();
    canvas->Divide(2,2);
    canvas->cd(1);
    fondo->Draw("HIST");
    canvas->cd(2);
    raw_h->Draw("HIST");


    TH1F *h = (TH1F*) raw_h->Clone();
    h->Add(fondo,-1);
    canvas->cd(3);
    h->Draw("HIST");
}