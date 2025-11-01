#include "gethisto.C"

struct PeakInfo {
    double mean;
    double sigma;
};

std::vector<PeakInfo> peaksearch(const char *file_na, short data_chan=0, bool verbose=true, TH1D **h_na_arg=nullptr, int nBin=2000, int xMin=10, int xMax=2010, float peak_min_ratio=0.18) {

	TH1D *h_na = getHistoForChannelFromTree(file_na,data_chan,nBin,xMin,xMax);
	TH1D *h_peaks = (TH1D*) h_na->Clone();

	if(h_na_arg) *h_na_arg = h_na;
	
	TCanvas *c0 = new TCanvas("c0");
	if (verbose){
		c0->Divide(2,1); // to draw more histos on a Canvas (see slides)
		c0->cd(1);
		h_na->Draw();
		c0->cd(2);
		h_peaks->Draw();
	}

	h_na->SetTitle("Measured spectrum");
	h_peaks->SetTitle("Peak search");

	TSpectrum *s = new TSpectrum(3);
	int nPeaks;
	double *xPeaks;
	int sigma = 40;	// minimum ratio between a peak and the main peak

	nPeaks = s->Search(h_peaks,sigma,"",peak_min_ratio);

	xPeaks = s->GetPositionX();
	if (verbose) 
		for (int p = 0; p<nPeaks; p++) {
			cout << "Peak #" << p << " @ channel " << xPeaks[p] << endl;	
		}
	
	
	if(verbose){
		//Print the result as png image
		c0->Print(Form("peaks_sigma%i.png",sigma));
	}

	std::vector<PeakInfo> peaks;

	for (int p = 0; p < nPeaks; p++) {
		int binCenter = h_na->FindBin(xPeaks[p]);
		double maxVal = h_na->GetBinContent(binCenter);
		double halfMax = maxVal / 2.0;

		int binLeft = binCenter;
		while (binLeft > 1 && h_na->GetBinContent(binLeft) > halfMax)
			binLeft--;

		int binRight = binCenter;
		while (binRight < h_na->GetNbinsX() && h_na->GetBinContent(binRight) > halfMax)
			binRight++;

		double fwhm = h_na->GetBinCenter(binRight) - h_na->GetBinCenter(binLeft);
		double sigma = fwhm / 2.355;

		peaks.push_back({xPeaks[p], sigma});

		if (verbose)
			cout << Form("Peak #%d: mean=%.1f sigma≈%.1f\n", p, xPeaks[p], sigma);
	}


	return peaks;
}

void peaksearch(const char *file_na, const char *file_bg, short data_chan=0, short bg_chan=2) {

	//Create the canvas
	TCanvas *c0 = new TCanvas("c0");
	c0->Divide(2,2); // to draw more histos on a Canvas (see slides)

	TH1D *h_na = getHistoForChannelFromTree(file_na,data_chan,2000,10,2010);
	TH1D *h_bg = getHistoForChannelFromTree(file_bg,bg_chan,2000,10,2010);
	//bg normalization
	h_bg->Scale(h_na->Integral()/h_bg->Integral());

	TH1D *h_subtr = (TH1D*) h_na->Clone();
	h_subtr->Add(h_bg,-1);
	TH1D *h_peaks = (TH1D*) h_subtr->Clone();

	h_na->SetTitle("Measured spectrum");
	h_bg->SetTitle("Measure of background");
	h_subtr->SetTitle("Subtracted background");
	h_peaks->SetTitle("Peak search");

	c0->cd(1);
	h_na->Draw();
	c0->cd(2);
	h_bg->Draw();
	c0->cd(3);
	h_subtr->Draw();
	c0->cd(4);
	h_peaks->Draw();

	TSpectrum *s = new TSpectrum(3);
	int nPeaks;
	double *xPeaks;
	int sigma = 40;
	double minratio = 0.32;	// minimum ratio between a peak and the main peak

	nPeaks = s->Search(h_peaks,sigma,"",minratio);

	xPeaks = s->GetPositionX();
	for (int p = 0; p<nPeaks; p++) {
		cout << "Peak #" << p << " @ channel " << xPeaks[p] << endl;	
	}
	
	//Print the result as png image
	c0->Print(Form("peaks_sigma%i.png",sigma));

}

void peaksearch(TH1* histo) { //Example only for the lesson 

	//Create the canvas
	TCanvas *c0 = new TCanvas("c0");
	c0->Divide(2,2); // to draw more histos on a Canvas (see slides)

    TH1D *h_na = (TH1D*)histo;
	TH1D *h_bg = (TH1D*) h_na->ShowBackground(22);
	TH1D *h_subtr = (TH1D*) h_na->Clone();
	h_subtr->Add(h_bg,-1);
	TH1D *h_peaks = (TH1D*) h_subtr->Clone();

	h_na->SetTitle("Measured spectrum");
	h_bg->SetTitle("Measure of background");
	h_subtr->SetTitle("Subtracted background");
	h_peaks->SetTitle("Peak search");

	c0->cd(1);
	h_na->Draw();
	c0->cd(2);
	h_bg->Draw();
	c0->cd(3);
	h_subtr->Draw();
	c0->cd(4);
	h_peaks->Draw();

	TSpectrum *s = new TSpectrum(3);
	int nPeaks;
	double *xPeaks;
	int sigma = 40;
	double minratio = 0.2;	// minimum ratio between a peak and the main peak

	nPeaks = s->Search(h_peaks,sigma,"",minratio);

	xPeaks = s->GetPositionX();
	for (int p = 0; p<nPeaks; p++) {
		cout << "Peak #" << p << " @ channel " << xPeaks[p] << endl;	
	}
	
	//Print the result as png image
	c0->Print(Form("peaks_sigma%i.png",sigma));

}

void multifit(const char *file_na, const char *data_label = "detector", short data_chan=0, short n_sigma_range=3, int nBin=2000, int xMin=10, int xMax=2010, float peak_min_ratio=0.18){

	TH1D *histo =nullptr;
	auto peaks = peaksearch(file_na, data_chan, true, &histo, nBin, xMin, xMax, peak_min_ratio);
    int nPeaks = peaks.size();
    if (nPeaks == 0) {
        cout << "Nessun picco trovato!\n";
        return;
    }

	TCanvas *c1 = new TCanvas("c1", "Multi Fit", 800, 600);
	histo->Draw();

    for (int i = 0; i < nPeaks; i++) {	
        double mean_guess  = peaks[i].mean;
        double sigma_guess = peaks[i].sigma;  
        double xmin = mean_guess - n_sigma_range*sigma_guess;
        double xmax = mean_guess + n_sigma_range*sigma_guess;

        // Fit function: fondo lineare + gaussiana
        TF1 *fitFunc = new TF1(Form("fitFunc_%d", i),
                               "[0] + [1]*x + [2]*exp(-0.5*((x-[3])/[4])**2)",
                               xmin, xmax);
        fitFunc->SetParName(0, "p0");
        fitFunc->SetParName(1, "p1");
        fitFunc->SetParName(2, "A");
        fitFunc->SetParName(3, "mean");
        fitFunc->SetParName(4, "sigma");

        fitFunc->SetParameters(0, 0, 1000, mean_guess, sigma_guess);

        histo->Fit(fitFunc, "R+");

        fitFunc->SetLineColor(i+2);  // kRed=2, kBlue=4, etc.
        fitFunc->SetLineWidth(2);
        fitFunc->Draw("SAME");

        cout << Form("Fit #%d: mean=%.2f sigma=%.2f\n",
                     i, fitFunc->GetParameter(3), fitFunc->GetParameter(4));

		char conferma;
		cout << "Vuoi salvare i risultati del fit in fit_parameters.csv? (s/n): ";
		cin >> conferma;
		conferma = std::tolower(conferma);

		if (conferma != 's') {
			cout << "Salvataggio annullato dall'utente." << endl;
			continue;
		}

    	gPad->Update();

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
}
