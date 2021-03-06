#include <iostream>
#include <string>
using namespace std;
#include <cmath>
#include <vector>

void persistence (TString file)
{
  //Show behaviour of waveform over time as well as average waveform
  //Outputs multiple plots

  //Load file and tree
  TFile *F = new TFile(file);
  TTree *T = (TTree *)F->Get("waveformTree");

  //Variables
  Double_t polarity = 0;
  Double_t baseline = 0;
  Double_t temp = 0;
  Int_t counter = 0;
  vector<UShort_t> *waveform = 0;
  vector<UShort_t> readWaveform;
  const Double_t binSize = 4.0; //Time length of each bin, ns

  //Tree branches
  T->SetBranchAddress("waveform", &waveform);
  T->SetBranchAddress("polarity", &polarity);
  T->SetBranchAddress("baseline", &baseline);
  T->GetEntry(0);
  readWaveform = *waveform;

  //Settings for histogram. Needed to get first waveform
  const Int_t waveformStart = 0;
  const Int_t waveformEnd = readWaveform.size()*binSize;
  const Int_t waveformLength = (waveformEnd-waveformStart)/binSize;

  const Int_t waveformLower = -50;
  const Int_t waveformUpper = 2000;
  const Int_t waveformAmplitude = waveformUpper-waveformLower; //for persistence plot, need to know height of waveform

  TH2D* persistHist = new TH2D("Persistence", "Persistence", waveformLength, waveformStart, waveformEnd, waveformAmplitude, waveformLower, waveformUpper);

  //TProfile shows average waveform
  TProfile *hprof = new TProfile("Average Waveform", "Average Waveform", waveformLength, waveformStart, waveformEnd);

  const Int_t numberEntries = T->GetEntries();
  for (Int_t eventNumber = 0; eventNumber < numberEntries; eventNumber++)
  {
    temp = 0;
    if (eventNumber % 1000 == 0)
    {
      printf("Currently on event %d of %d\n", eventNumber, numberEntries);
    }
    T->GetEntry(eventNumber);
    readWaveform = *waveform;
    for (Int_t i = 0; i < readWaveform.size(); i++)
    {
      temp = (readWaveform[i]-baseline*1.0)*polarity*1.0;
      persistHist->Fill(i*binSize, temp);
      hprof->Fill(i*binSize, temp);
    }
  }

  //Draw both plots
  TCanvas *c1 = new TCanvas("c1", "", 1000, 600);
  TCanvas *c2 = new TCanvas("c2", "", 1000, 600);
  
  c1->cd();
  gStyle->SetOptStat(0);
  c1->SetLogz();
  persistHist->GetXaxis()->SetTitle("Time(ns)");
  persistHist->Draw("colz");

  c2->cd();
  gStyle->SetOptStat(0);
  hprof->GetXaxis()->SetTitle("Time(ns)");
  hprof->Draw();
}
