void fit_calibrazione(const char *file_name, int channel, const char *data_label, double xmin, double xmax, double start_mean = 400, double start_p0 = 100) {
    // Apri il file ROOT
    TFile *file = TFile::Open(file_name);

    // Controlla che il file sia stato aperto correttamente
    if (!file || file->IsZombie()) {
        cout << "Errore: impossibile aprire il file!" << endl;
        return;
    }

    cout << "File aperto correttamente!" << endl;

    TDirectory *dir = (TDirectory*)file->Get("Energy");
    if (!dir) {
        cout << "Directory non trovata!" << endl;
        return;
    }

   std::string hist_name = "_R_EnergyCH"+ std::to_string(channel)+"@DT5720B_24269;1";
    TH1D *hist = (TH1D*)dir->Get(hist_name.c_str());
    if (!hist) {
        cout << "Istogramma non trovato!" << endl;
        return;
    }

    TF1 *fitFunc = new TF1("fitFunc",
                           "[0] + [1]*x + [2]*exp(-0.5*((x-[3])/[4])**2)",
                        xmin,xmax);
    fitFunc->SetParNames("p0","p1","Amplitude","Mean","Sigma");
    fitFunc->SetParameters(start_p0, 0, 1000, start_mean, 30);

    // Fai il fit
    hist->Fit("fitFunc", "R", "");

    // Disegna
    fitFunc->SetLineColor(kRed);
    fitFunc->SetLineWidth(2);
    hist->GetXaxis()->SetRangeUser(xmin, xmax);
    hist->Draw();
    fitFunc->Draw("SAME");
    gPad->Update(); 
 
    // Mostra risultati
    cout << "------------------------" << endl;
    cout << "Risultati del fit:" << endl;
    cout << "  p0     = " << fitFunc->GetParameter(0) << endl;
    cout << "  p1     = " << fitFunc->GetParameter(1) << endl;
    cout << "  Amp    = " << fitFunc->GetParameter(2) << endl;
    cout << "  Mean   = " << fitFunc->GetParameter(3) << endl;
    cout << "  Sigma  = " << fitFunc->GetParameter(4) << endl;

    char conferma;
    cout << "Vuoi salvare i risultati del fit in fit_parameters.csv? (s/n): ";
    cin >> conferma;
    conferma = std::tolower(conferma);

    if (conferma != 's') {
        cout << "Salvataggio annullato dall'utente." << endl;
        return;
    }

    //Salva
    ofstream file_fit("fit_parameters.csv", ios::app); // 'app' = append (aggiunge in fondo)
    if (file_fit.is_open()) {
        file_fit << data_label << ",";
        for (int i = 0; i < 5; ++i) {
            file_fit << fitFunc->GetParameter(i) << "," << fitFunc->GetParError(i);
            if (i < 4) file_fit << ",";  // separatore tra colonne
        }
        file_fit << "\n";
        file_fit.close();
        cout << "Parametri salvati in fit_parameters.csv" << endl;
    } else {
        cout << "Errore: impossibile aprire fit_parameters.csv" << endl;
    }
}