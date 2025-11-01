struct FitResult {
    double mean;
    double sigma;
    double mean_err;
    double sigma_err;
};

FitResult filter_data(const char *name_file,float acq_time, const char *fondo_name_file, float acq_time_fondo, int numBins = 500, double minX = 0, double maxX = 2000, double lowThr = 300, double highThr = 700, float c=1) {
    // //calibrated format: in alternatvia all'effettiva calibrazione si può tenere tutto in u.a. e convertire solo al momento del plot
    // TF1 *calib = new TF1("calib", Form("%f*x + %f", m, q));
    // h->GetXaxis()->SetTitle("Energy [keV]");
    // h->GetXaxis()->SetFunction(calib);
    
    TFile *infile = new TFile(name_file);
    TTree *intree = (TTree*)infile->Get("Data_B");

    //data filter
    TString condition = Form("Channel_0.Energy > %f && Channel_0.Energy < %f", lowThr, highThr);
    TH1F *filtered_h = new TH1F("filtered_h", "Filtered Energy", numBins, minX, maxX);
    intree->Project("filtered_h", "Channel_2.Energy", condition);

    //fondo
    TFile *infile_fondo = new TFile(fondo_name_file);
    TTree *intree_fondo = (TTree*)infile_fondo->Get("Data_B");
    TH1F *fondo = new TH1F("fondo","Fondo", numBins, minX, maxX);
    intree_fondo->Project("fondo", "Channel_2.Energy");
    fondo->Scale(1/acq_time_fondo);

    TH1F *raw_h = new TH1F("raw_h", "Raw Energy", numBins, minX, maxX);
    intree->Project("raw_h", "Channel_2.Energy");
    filtered_h->Scale(raw_h->GetEntries()/filtered_h->GetEntries());
    
    // filtered_h->Add(fondo,-1);
    
    TCanvas *canvas = new TCanvas();
    canvas->Divide(2,2);
    canvas->cd(1);
    fondo->Draw("HIST");
    canvas->cd(2);
    raw_h->Draw("HIST");
    canvas->cd(3);
    filtered_h->Draw("HIST");

    //pick and FWHM finder
    int maxBin = filtered_h->GetMaximumBin();       
    double xPeak = filtered_h->GetBinCenter(maxBin);
    double yPeak = filtered_h->GetBinContent(maxBin);

    double yHalf = yPeak / 2.0;
    int binLeft = maxBin, binRight = maxBin;
    while (binLeft > 1 && filtered_h->GetBinContent(binLeft) > yHalf) binLeft--;
    while (binRight < filtered_h->GetNbinsX() && filtered_h->GetBinContent(binRight) > yHalf) binRight++;

    double xLeft = filtered_h->GetBinCenter(binLeft);
    double xRight = filtered_h->GetBinCenter(binRight);
    double fwhm = xRight - xLeft;

    //fit
    double sigmaInit = fwhm / 2.355;
    TF1 *fGaus = new TF1("fGaus", "gaus", xPeak - fwhm*c, xPeak + fwhm*c);
    fGaus->SetParameters(yPeak, xPeak, sigmaInit);

    filtered_h->Fit(fGaus, "RQ");
    canvas->cd(4); 
    filtered_h->Draw("HIST");
    fGaus->Draw("SAME");

    FitResult res;
    res.mean = fGaus->GetParameter(1);
    res.sigma = fGaus->GetParameter(2);
    res.mean_err = fGaus->GetParError(1);
    res.sigma_err = fGaus->GetParError(2);
    return res;
}
