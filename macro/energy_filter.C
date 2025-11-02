#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <iostream>
#include <cmath>

void filter_energy_sum() {
    // === Parametri ===
    const char* infile_name = "merged_data/energia_0deg.root";
    const char* outfile_name = "energia_filtrata.root";
    double target_sum = 500.0;
    double tolerance = 10.0;

    // === Apri file di input ===
    TFile *infile = new TFile(infile_name, "READ");
    // === Leggi il TTree ===
    TTree *intree = (TTree*)infile->Get("Data_B");
    
    // === Variabili per leggere i rami ===
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

    TCanvas *canvas = new TCanvas();
    canvas->Divide(2,1);

    TH1F *h = new TH1F("h", "Energy", numBins, minX, maxX);
    intree->Project("h", 2);
    TH1F *filtered_h = new TH1F("filtered_h", "Filtered Energy", numBins, minX, maxX);
    outtree->Project("filtered_h", 2);
}
