#include "gethisto.C"

void plot_calibrated_spectrum(const char *file_na, short data_chan=0, short n_bin = 2000){
    TH1D *h = getHistoForChannelFromTree(file_na,data_chan,n_bin,10,2010);
    TCanvas *c0 = new TCanvas("c0");

    double a = 1.305848972754931;
    double s_a = 0.003410386945931957;
    double b = -20.50011964584811;
    double s_b = 2.4810877440445904;

    double xmin = 10;
    double xmax = 2010;
    double e_min = a * xmin + b;
    double e_max = a * xmax + b;

    TH1D *hE = new TH1D("hE", "Spectrum in Energy", n_bin, e_min, e_max);

    for (int i = 1; i <= n_bin; i++) {
        // hE->SetBinContent(i, h->GetBinContent(i)*a+b);
        hE->SetBinContent(i, h->GetBinContent(i));
    }

    hE->GetXaxis()->SetTitle("Energy [keV]");
    hE->Draw();
}

void plot_spectrum(const char *file_na,const char *title,float acq_time = 1, short n_bin = 2000, int xMin=10, int xMax=2010){
    TCanvas *c0 = new TCanvas("c0");
    c0->Divide(2,2);
    for(int i=0; i<=2; i++){
        c0->cd(i+1);
        TH1D *h = getHistoForChannelFromTree(file_na,i,n_bin,xMin,xMax);
        h->Scale(1/acq_time);
        h->SetTitle(title);
        h->Draw("H");
    }

    c0->Update();  
    TString outName = Form("img_spectra/%s.png", title);
    c0->SaveAs(outName); 
}