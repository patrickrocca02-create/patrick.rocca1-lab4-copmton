#include "gethisto.C"

void plot_spectrum(const char *file_na, short data_chan=0, short n_bin = 2000){
    TH1D *h = getHistoForChannelFromTree(file_na,data_chan,n_bin,10,2010);
    TCanvas *c0 = new TCanvas("c0");

    h->Draw();
}