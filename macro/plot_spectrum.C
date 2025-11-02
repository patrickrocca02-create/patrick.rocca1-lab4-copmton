#include "gethisto.C"

void plot_calibrated_spectrum(const char *file_na, short data_chan=0, const char *title = "",short n_bin = 2000, short xMin=10, short xMax=2010, bool save_image = false){
    TH1D *h = getHistoForChannelFromTree(file_na,data_chan,n_bin,xMin,xMax);
    TCanvas *c0 = new TCanvas("c0");

    //tagger
    double a = 0.7662729177579881;
    double s_a = 0.00019162024784844515;
    double b = 15.371539025668165;
    double s_b = 0.12159773938906922;

    TString title_0 = "Tagger";

    if(data_chan == 1){
        //detector
        a = 2.004180751628301;
        s_a = 0.00017787820790798043;
        b = 21.485695971854078;
        s_b = 0.01473013831804863;
        title_0 = "Detector";
    } else if(data_chan == 2){
        //scatterer
        a = 0.905704242535311;
        s_a = 0.00017303505516316406;
        b = 13.972140156865908;
        s_b = 0.02464179686945202;
        title_0 = "Scatterer";
    }

    // a=1;
    // b=0;
    
    double e_min = (xMin - b)/a;
    double e_max = (xMax - b)/a;


    TString title_1(title);
    title_0 += title_1;
    
    
    gStyle->SetOptStat(0);
 
    TCanvas *c = new TCanvas("c","c",1000,700);

    h->GetXaxis()->SetLimits(e_min,e_max);
    h->SetTitle(title_0);
    h->GetXaxis()->SetTitle("Energy [keV]");
    h->GetYaxis()->SetTitle("Counts");
    h->Draw();

    gStyle->SetTitleY(0.975); 

    TPaveText *pt = new TPaveText(0.74, 0.843, 0.89, 0.883, "NDC");
    pt->SetFillColor(0);      // sfondo trasparente
    pt->SetBorderSize(1);     // bordo
    pt->SetTextAlign(12);     // allineamento
    pt->SetTextFont(42);      // font
    pt->SetTextSize(0.03);      // dimensione del testo
    pt->AddText(Form("Entries = %.0f", h->GetEntries()));
    pt->Draw();

    c->Update();
    if(save_image){
        TString outName = Form("imgs/%s.png", title_0.Data());
        c->SaveAs(outName.Data()); 
    }
}

void plot_spectrum(const char *file_na,const char *title,float acq_time = 1, short n_bin = 2000, int xMin=10, int xMax=2010, bool save_image=false){
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