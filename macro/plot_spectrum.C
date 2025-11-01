#include "gethisto.C"

void plot_calibrated_spectrum(const char *file_na, short data_chan=0, short n_bin = 2000){
    TH1D *h = getHistoForChannelFromTree(file_na,data_chan,n_bin,0,2000);
    TCanvas *c0 = new TCanvas("c0");

    //tagger
    double a = 0.808094;
    double s_a = 0.0001936;
    double b = -38.2448;
    double s_b = 0.130712;

    double xmin = 0;
    double xmax = 2000;
    double e_min = (xmin - b)/a;
    double e_max = (xmax - b)/a;

    h->GetXaxis()->SetLimits(e_min,e_max);
    h->Draw();
}

void plot_spectrum(const char *file_na,const char *title,float acq_time = 1, short n_bin = 2000, int xMin=10, int xMax=2010, bool save_image=true){
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
    if(save_image){
        TString outName = Form("img_spectra/%s.png", title);
        c0->SaveAs(outName); 
    }
}