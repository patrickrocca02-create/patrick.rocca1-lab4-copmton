struct FitResult {
    double mean;
    double sigma;
    double mean_err;
    double sigma_err;
};

FitResult filter_data(const char *name_file,float acq_time, const char *fondo_name_file, float acq_time_fondo, int numBins = 500, double minX = 0, double maxX = 2000, double lowThr = 300, double highThr = 700, float c=1,int chan=2) {
    // //calibrated format: in alternatvia all'effettiva calibrazione si può tenere tutto in u.a. e convertire solo al momento del plot
    // TF1 *calib = new TF1("calib", Form("%f*x + %f", m, q));
    // h->GetXaxis()->SetTitle("Energy [keV]");
    // h->GetXaxis()->SetFunction(calib);
    TFile *infile = new TFile(name_file);
    TTree *intree = (TTree*)infile->Get("Data_B");
    TString channel = Form("Channel_%i.Energy", chan);


    double target_sum = 500.0;
    double tolerance = 10.0;

    double E1 = 0, E2 = 0;
    intree->SetBranchAddress("Channel_1.Energy", &E1);
    intree->SetBranchAddress("Channel_2.Energy", &E2);

    // === Crea file di output ===
    TFile *outfile = new TFile(outfile_name, "RECREATE");
    TTree *outtree = intree->CloneTree(0); // stesso schema, ma vuoto

    Long64_t nentries = intree->GetEntries();
    std::cout << "Numero totale di eventi: " << nentries << std::endl;

    // === Loop sugli eventi ===
    for (Long64_t i = 0; i < nentries; ++i) {
        intree->GetEntry(i);
        double sum = E1 + E2;

        if (std::abs(sum - target_sum) < tolerance) {
            outtree->Fill();
        }
    }

    // === Salva file ===
    outfile->Write();
    outfile->Close();
    infile->Close();

    std::cout << "File salvato come '" << outfile_name << "' con eventi filtrati.\n";
    return 0;



    //data filter
    TString condition = Form("Channel_0.Energy > %f && Channel_0.Energy < %f", lowThr, highThr);
    TH1F *filtered_h = new TH1F("filtered_h", "Filtered Energy", numBins, minX, maxX);
    intree->Project("filtered_h", channel, condition);

     //fondo
    TFile *infile_fondo = new TFile(fondo_name_file);
    TTree *intree_fondo = (TTree*)infile_fondo->Get("Data_B");
    TH1F *fondo = new TH1F("fondo","Fondo", numBins, minX, maxX);
    intree_fondo->Project("fondo", channel);
    fondo->Scale(1/acq_time_fondo);

    TH1F *raw_h = new TH1F("raw_h", "Raw Energy", numBins, minX, maxX);
    intree->Project("raw_h", channel);
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

    if(chan==1){
        double sogliaX = 950;  
        int nBins = filtered_h->GetNbinsX();
        maxBin = -1;
        yPeak = -1;

        for (int i = nBins; i > 0; i--) {
            double val = filtered_h->GetBinContent(i);
            double x = filtered_h->GetBinCenter(i);
            if (x < sogliaX && val > yPeak) {
                yPeak = val;
                xPeak = x;
                maxBin = i;
            }
        }
    }

    cout << "xPeak: " << xPeak << ", yPeak: " << yPeak << endl;

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

    canvas->Modified();
    canvas->Update();
    canvas->Draw();

    FitResult res;
    res.mean = fGaus->GetParameter(1);
    res.sigma = fGaus->GetParameter(2);
    res.mean_err = fGaus->GetParError(1);
    res.sigma_err = fGaus->GetParError(2);
    return res;
}

void angular_dependence_graph(){
    std::vector<FitResult> results_1;
    std::vector<FitResult> results_2;

    //tresholds sul tagger per il detector (0.inf,0.sup,1.inf,1.sup,...) 
    double thr_det[10] = {378,442,386,442,386,444,378,442,386,446};

    double angles[5] = {0,30,50,60,90};
    int acq_times[5] = {30,30,20,30,30}; 

    TString bg_file_name = "merged_data/fondo_coinc.root";
    int bg_acq_time = 20;

    double x_peaks_1[5];
    double s_x_peaks_1[5];
    for(int i=0; i<5; i++){
        TString file_name = Form("merged_data/energia_%gdeg.root", angles[i]);
        cout << file_name <<"\n";
        results_1.push_back(
            filter_data(file_name.Data(), acq_times[i], bg_file_name.Data(), bg_acq_time,200,0,1400,thr_det[2*i],thr_det[2*i+1],0.7,1)
        );
        x_peaks_1[i] = results_1[i].mean;
        s_x_peaks_1[i] = results_1[i].sigma_err;
    }

    double x_peaks_2[5];
    double s_x_peaks_2[5];
    for(int i=0; i<5; i++){
        TString file_name = Form("merged_data/energia_%gdeg.root", angles[i]);
        cout << file_name <<"\n";
        results_2.push_back(
            filter_data(file_name.Data(), acq_times[i], bg_file_name.Data(), bg_acq_time,200,0,1400,400,425,0.7,2)
        );
        x_peaks_2[i] = results_2[i].mean;
        s_x_peaks_2[i] = results_2[i].sigma_err;
    }

    double sum[5];
    double s_sum[5];
    for(int i=0; i<5; i++){
        sum[i] = results_1[i].mean + results_2[i].mean;
        s_sum[i] = sqrt(results_1[i].sigma_err*results_1[i].sigma_err + results_2[i].sigma_err*results_2[i].sigma_err);
    }

    //calibration
    // double a = 1.1088657489201306;
    // double s_a = 0.003410386945931957;
    // double b = -16.57114900852498;
    // double s_b = 2.4810877440445904;
    // for(int i=0; i<5; i++){
    //     x_peaks_2[i] = a*x_peaks_2[i]+b;
    //     s_x_peaks_2[i] = sqrt(pow(x_peaks_2[i] * s_a, 2) + pow(s_b, 2) + pow(a*s_x_peaks_2[i],2));
    // }

    // Grafico scatterer
    auto gr1 = new TGraphErrors(5, angles, x_peaks_1, nullptr, s_x_peaks_1);
    gr1->SetTitle("Angular dependence of scattered energy;Angle [deg];Peak mean [keV]");
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerColor(kRed);
    gr1->SetLineColor(kRed);
    gr1->SetLineWidth(2);

    // Grafico detector
    auto gr2 = new TGraphErrors(5, angles, x_peaks_2, nullptr, s_x_peaks_2);
    gr2->SetMarkerStyle(21);
    gr2->SetMarkerColor(kBlue);
    gr2->SetLineColor(kBlue);
    gr2->SetLineWidth(2);

    // Somma
    auto gr3 = new TGraphErrors(5, angles, sum, nullptr, s_sum);
    gr3->SetMarkerStyle(22);
    gr3->SetMarkerColor(kGreen);
    gr3->SetLineColor(kGreen);
    gr3->SetLineWidth(2);

    // Canvas e disegno
    auto c = new TCanvas("c", "Angular Dependence", 800, 600);
    gr1->Draw("AP");    // Disegna il primo grafico con assi
    gr1->GetYaxis()->SetRangeUser(0, 900);
    gr2->Draw("P SAME"); // Disegna il secondo grafico sopra al primo
    gr3->Draw("P SAME"); 

    // Aggiungi una legenda per distinguerli
    auto legend = new TLegend(0.6, 0.7, 0.88, 0.88);
    legend->AddEntry(gr1, "Electron", "lp");
    legend->AddEntry(gr2, "Scatterer gamma", "lp");
    legend->AddEntry(gr3, "Sum", "lp");
    legend->Draw();
//     gr->Fit(f_model, "E");
//     f_model->SetLineColor(kRed);
//     f_model->Draw("SAME");

//     cout << "Fit result: a = " << f_model->GetParameter(0)
//         << " ± " << f_model->GetParError(0) << endl;
}