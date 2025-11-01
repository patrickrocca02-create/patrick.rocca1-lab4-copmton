void calibrazione_tre_fit() {
    // --- File CSV ---
    TString file_name = "fit_parameters.csv";
    ifstream file(file_name.Data());
    if (!file.is_open()) {
        cout << "Errore: impossibile aprire il file " << file_name << endl;
        return;
    }

    // --- Salta intestazione ---
    string header;
    getline(file, header);

    // --- Vettori per categorie ---
    std::vector<double> mean_tagger, smean_tagger;
    std::vector<double> mean_detector, smean_detector;
    std::vector<double> mean_scatterer, smean_scatterer;

    // --- Lettura file ---
    string label;
    double dummy; // variabili temporanee per ignorare le altre colonne
    double mean, s_mean;

    while (true) {
        string line;
        if (!getline(file, line)) break; // fine file

        if (line.empty()) continue;

        std::stringstream ss(line);
        string token;

        getline(ss, label, ','); // data_label

        // salta 6 colonne (p0,s_p0,p1,s_p1,amplitude,s_amplitude)
        for (int i = 0; i < 6; ++i) getline(ss, token, ',');

        // leggi mean e s_mean
        getline(ss, token, ','); mean = stod(token);
        getline(ss, token, ','); s_mean = stod(token);

        // ignora sigma e s_sigma
        // (non servono)

        // --- Smista nei vettori giusti ---
        if (label == "tagger") {
            mean_tagger.push_back(mean);
            smean_tagger.push_back(s_mean);
        } else if (label == "detector") {
            mean_detector.push_back(mean);
            smean_detector.push_back(s_mean);
        } else if (label == "scatterer") {
            mean_scatterer.push_back(mean);
            smean_scatterer.push_back(s_mean);
        }
    }

    file.close();

    // --- Valori di calibrazione noti (esempio) ---
    // Devi sostituire questi valori con le energie note corrispondenti
    double energy[2] = {551, 1275}; // esempio per 2 punti per gruppo

    // --- Crea TGraphErrors per ciascun gruppo ---
    auto gr_tagger = new TGraphErrors(mean_tagger.size(), energy, &mean_tagger[0], nullptr, &smean_tagger[0]);
    auto gr_detector = new TGraphErrors(mean_detector.size(), energy, &mean_detector[0], nullptr, &smean_detector[0]);
    auto gr_scatterer = new TGraphErrors(mean_scatterer.size(), energy, &mean_scatterer[0], nullptr, &smean_scatterer[0]);

    gr_tagger->SetTitle("Calibration curves;Known energy [keV];Measured mean [channels]");
    gr_tagger->SetMarkerStyle(20);
    gr_tagger->SetMarkerColor(kRed);
    gr_detector->SetMarkerStyle(21);
    gr_detector->SetMarkerColor(kBlue);
    gr_scatterer->SetMarkerStyle(22);
    gr_scatterer->SetMarkerColor(kGreen+2);

    // --- Definizione funzione di fit lineare ---
    TF1 *f_tagger = new TF1("f_tagger", "[0]*x + [1]", 0, 1500);
    TF1 *f_detector = new TF1("f_detector", "[0]*x + [1]", 0, 1500);
    TF1 *f_scatterer = new TF1("f_scatterer", "[0]*x + [1]", 0, 1500);

    // --- Canvas unico per confronto ---
    auto c = new TCanvas("c", "Calibration Fits", 900, 700);
    gr_tagger->Draw("AP");
    gr_detector->Draw("P SAME");
    gr_scatterer->Draw("P SAME");

    // --- Fit ---
    gr_tagger->Fit(f_tagger, "Q");
    gr_detector->Fit(f_detector, "Q");
    gr_scatterer->Fit(f_scatterer, "Q");

    // --- Stili fit ---
    f_tagger->SetLineColor(kRed);
    f_detector->SetLineColor(kBlue);
    f_scatterer->SetLineColor(kGreen+2);

    f_tagger->Draw("SAME");
    gr_tagger->GetYaxis()->SetRangeUser(0, 3000);
    f_detector->Draw("SAME");
    f_scatterer->Draw("SAME");

    // --- Legenda ---
    auto legend = new TLegend(0.6, 0.7, 0.88, 0.88);
    legend->AddEntry(gr_tagger, "Tagger", "lp");
    legend->AddEntry(gr_detector, "Detector", "lp");
    legend->AddEntry(gr_scatterer, "Scatterer", "lp");
    legend->Draw();

    // --- Stampa risultati ---
    cout << "\n=== Fit results ===" << endl;
    cout << "Tagger:    a = " << f_tagger->GetParameter(0) << " ± " << f_tagger->GetParError(0)
         << ", b = " << f_tagger->GetParameter(1) << " ± " << f_tagger->GetParError(1) << endl;
    cout << "Detector:  a = " << f_detector->GetParameter(0) << " ± " << f_detector->GetParError(0)
         << ", b = " << f_detector->GetParameter(1) << " ± " << f_detector->GetParError(1) << endl;
    cout << "Scatterer: a = " << f_scatterer->GetParameter(0) << " ± " << f_scatterer->GetParError(0)
         << ", b = " << f_scatterer->GetParameter(1) << " ± " << f_scatterer->GetParError(1) << endl;
}
