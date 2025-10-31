void int_timestamp(const char *file_na, short data_chan=0){
    TH1D *histo = getHistoForChannelFromTree(file_na,data_chan,2000,10,2010);
    TCanvas *c0 = new TCanvas("c0");
    histo->Draw();
}