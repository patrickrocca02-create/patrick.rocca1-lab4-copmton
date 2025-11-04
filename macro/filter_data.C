struct FitResult {
    double mean;
    double sigma;
    double mean_err;
    double sigma_err;
};

FitResult filter_data(const char *name_file,float acq_time, const char *fondo_name_file, float acq_time_fondo, int numBins = 500, double minX = 0, double maxX = 2000, double lowThr = 300, double highThr = 700, double energy_sum = 511, double energy_window = 20, float c=1,int chan=2) {
    // //calibrated format: in alternatvia all'effettiva calibrazione si può tenere tutto in u.a. e convertire solo al momento del plot
    // TF1 *calib = new TF1("calib", Form("%f*x + %f", m, q));
    // h->GetXaxis()->SetTitle("Energy [keV]");
    // h->GetXaxis()->SetFunction(calib);
    TFile *infile = new TFile(name_file);
    TTree *intree = (TTree*)infile->Get("Data_B");
    TString channel = Form("Channel_%i.Energy", chan);

    //data filter
    double a_1 = 2.004180751628301;
    double a_2 = 0.905704242535311;
    double b_1 = 21.485695971854078;
    double b_2 = 13.972140156865908;
    TString condition = Form(
        "Channel_0.Energy > %f && Channel_0.Energy < %f && "
        "((Channel_1.Energy - %f)/%f + (Channel_2.Energy - %f)/%f) > %f && "
        "((Channel_1.Energy - %f)/%f + (Channel_2.Energy - %f)/%f) < %f",
        lowThr, highThr,
        b_1, a_1, b_2, a_2, energy_sum - energy_window,
        b_1, a_1, b_2, a_2, energy_sum + energy_window
    );
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

void angular_dependence_graph(double energy_sum = 511, double energy_window = 10){
    std::vector<FitResult> results_1;
    std::vector<FitResult> results_2;

    //tresholds sul tagger per il detector (0.inf,0.sup,1.inf,1.sup,...) 
    double thr_det[10] = {378,442,386,442,386,444,378,442,386,446};

    double angles[5] = {0,30,50,60,90};
    int acq_times[5] = {30,30,20,30,30}; 

    double a_1 = 2.004180751628301;
    double s_a_1 = 0.00017787820790798043;
    double a_2 = 0.905704242535311;
    double s_a_2 = 0.00017303505516316406;
    double b_1 = 21.485695971854078;
    double s_b_1 = 0.01473013831804863;
    double b_2 = 13.972140156865908;
    double s_b_2 = 0.02464179686945202;

    TString bg_file_name = "merged_data/fondo_coinc.root";
    int bg_acq_time = 20;

    double x_peaks_1[5];
    double s_x_peaks_1[5];
    for(int i=0; i<5; i++){
        TString file_name = Form("merged_data/energia_%gdeg.root", angles[i]);
        cout << file_name <<"\n";
        results_1.push_back(
            filter_data(file_name.Data(), acq_times[i], bg_file_name.Data(), bg_acq_time,30,0,1400,thr_det[2*i],thr_det[2*i+1],energy_sum,energy_window,.7,1)
        );
        x_peaks_1[i] = (results_1[i].mean-b_1)/a_1;
        s_x_peaks_1[i] = sqrt( 1/(a_1*a_1)*results_1[i].sigma_err*results_1[i].sigma_err + 1/(a_1*a_1)*s_b_1*s_b_1 + pow((x_peaks_1[i]-b_1)/(a_1*a_1),2)*s_a_1*s_a_1);
    }

    double x_peaks_2[5];
    double s_x_peaks_2[5];
    for(int i=0; i<5; i++){
        TString file_name = Form("merged_data/energia_%gdeg.root", angles[i]);
        cout << file_name <<"\n";
        results_2.push_back(
            filter_data(file_name.Data(), acq_times[i], bg_file_name.Data(), bg_acq_time,100,0,1400,thr_det[2*i],thr_det[2*i+1],energy_sum,energy_window,0.7,2)
        );
        x_peaks_2[i] = (results_2[i].mean-b_2)/a_2;
        s_x_peaks_2[i] = sqrt( 1/(a_2*a_2)*results_2[i].sigma_err*results_2[i].sigma_err + 1/(a_2*a_2)*s_b_2*s_b_2 + pow((x_peaks_2[i]-b_2)/(a_2*a_2),2)*s_a_2*s_a_2);
    }

    double sum[5];
    double s_sum[5];
    for(int i=0; i<5; i++){
        sum[i] = x_peaks_1[i] + x_peaks_2[i];
        s_sum[i] = sqrt(s_x_peaks_1[i]*s_x_peaks_1[i] + s_x_peaks_2[i]*s_x_peaks_2[i]);
    }

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


    TF1 *f_model = new TF1("f_model", "511/(2 - cos(x*TMath::Pi()/180.))", 0, 90);
    f_model->SetLineColor(kMagenta);
    f_model->SetLineWidth(2);
    f_model->Draw("SAME");

    TF1 *f_model1 = new TF1("f_model1", "511-511/(2 - cos(x*TMath::Pi()/180.))", 0, 90);
    f_model->SetLineColor(kBlue);
    f_model->SetLineWidth(2);
    f_model1->Draw("SAME");

    TF1 *f_model2 = new TF1("f_model2", "[0]*x+[1]", 0, 90);
    f_model2->SetLineColor(kGreen);
    f_model2->SetLineWidth(2);
    f_model2->SetParameter(0,0);

    gr3->Fit(f_model2);
 
    f_model2->Draw("SAME");

    cout << "Fit result: a = " << f_model->GetParameter(0)
        << " ± " << f_model->GetParError(0) << endl;
}


// void filter_data_and_fit(const char *name_file,float acq_time, int numBins = 500, double minX = 0, double maxX = 2000, double lowThr = 300, double highThr = 700, double energy_sum = 511, double energy_window = 20, float c=1)