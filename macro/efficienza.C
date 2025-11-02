void detector_efficieny(const char *name_file,float acq_time, const char *fondo_name_file, float acq_time_fondo, int numBins = 500, double minX = 0, double maxX = 2000){
    TFile *infile_fondo = new TFile(fondo_name_file);
    TTree *intree_fondo = (TTree*)infile_fondo->Get("Data_B");
    TH1F *fondo = new TH1F("fondo","Fondo", numBins, minX, maxX);
    intree_fondo->Project("fondo", "Channel_2.Energy");
    fondo->Scale(acq_time/acq_time_fondo);
    
    TFile *infile = new TFile(name_file);
    TTree *intree = (TTree*)infile->Get("Data_B");
    TH1F *raw_h = new TH1F("raw_h", "Raw Energy", numBins, minX, maxX);
    intree->Project("raw_h", "Channel_2.Energy");
    raw_h->Scale(1);

    TCanvas *canvas = new TCanvas();
    canvas->Divide(2,2);
    canvas->cd(1);
    fondo->Draw("HIST");
    canvas->cd(2);
    raw_h->Draw("HIST");

    // //calibrazione ch0 
    // int nbins = raw_h->GetNbinsX();
    // std::vector<double> new_edges(nbins + 1);
    // double a_0 = 0.7662729177579881;
    // double b_0 = 15.371539025668165;
    // for (int i = 1; i <= nbins + 1; ++i) {
    //     double x_old = raw_h->GetXaxis()->GetBinLowEdge(i);
    //     new_edges[i - 1] = (x_old - b_0)/a_0;
    // }
    // TH1F *h_cal = new TH1F("h_cal", "Istogramma calibrato", nbins, &new_edges[0]);
    // for (int i = 1; i <= nbins; ++i)
    //     h_cal->SetBinContent(i, raw_h->GetBinContent(i));
    // canvas->cd(1);
    // h_cal->Draw();

    // //calibrazione fondo 
    // nbins = fondo->GetNbinsX();
    // std::vector<double> new_edges_fondo(nbins + 1);
    // double a_2 = 0.905704242535311;
    // double b_2 = 13.972140156865908;
    // for (int i = 1; i <= nbins + 1; ++i) {
    //     double x_old = fondo->GetXaxis()->GetBinLowEdge(i);
    //     new_edges_fondo[i - 1] = (x_old - b_2)/a_2;
    // }
    // TH1F *fondo_cal = new TH1F("fondo_cal", "Fondo calibrato", nbins, &new_edges_fondo[0]);
    // for (int i = 1; i <= nbins; ++i)
    //     fondo_cal->SetBinContent(i, fondo->GetBinContent(i));
    // canvas->cd(2);
    // fondo_cal->Draw();


    TH1F *h = (TH1F*) raw_h->Clone();
    h->Add(fondo,-1);
    canvas->cd(3);
    h->Draw("HIST");

    //pick integral
    float range_min = 21;
    float range_max = 184;
    int bin_min = h->FindBin(range_min);
    int bin_max = h->FindBin(range_max);
    double entries_in_range = h->Integral(bin_min, bin_max);
    cout << "Entries between " << range_min << " and " << range_max << ": " << entries_in_range << endl;

    //full integral
    range_min = 201;
    range_max = 300;
    bin_min = h->FindBin(range_min);
    bin_max = h->FindBin(range_max);
    entries_in_range = h->Integral(bin_min, bin_max);
    cout << "Entries between " << range_min << " and " << range_max << ": " << entries_in_range << endl;
}