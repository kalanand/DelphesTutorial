// ===========================================================================
// 
//       Filename:  DelHATS.cc
// 
//    Description:  An example code for Delphes exercise of JetMET performance
//    1. Basic Jet distribution comparison
//    2. MET and MET resolution 
//    3. Jet response simply using TProfile
//    4. Jet resolution with Jet (Pt>30) for different eta range (0-2.5),(2.5-4)
// 
//        Version:  1.0
//        Created:  02/11/2014 01:44:39 PM
//       Compiler:  g++ -std=c++11
// 
//         Author:  Zhenbin Wu, John Stupak
//        Company:  HATS@LPC
// 
// ===========================================================================

// Classes from STL
#include <cstdlib>
#include <iostream>  
#include <string>
#include <vector>
#include <sstream>

// Classes from ROOT
#include "TH1.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TProfile.h"

// Classes from Delphes
#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTreeReader.h"
#include "ExRootAnalysis/ExRootResult.h"
#include "classes/DelphesClasses.h"

TVector2 CalcMHT(TClonesArray *branchJet, TClonesArray *branchElectron, TClonesArray *branchMuon, TClonesArray *branchPhoton);
std::map<int, int> MatchingJet(TClonesArray *branchGenJet, TClonesArray *branchJet);
bool IsHadronic(TClonesArray *branchParticle);

// ===  FUNCTION  ============================================================
//         Name:  main
//  Description:  Wrap up of Example1.C
// ===========================================================================
int main ( int argc, char *argv[] )
{
  if(argc != 3)
  {
    std::cout << " Usage: DelHATS input_file output_file" << std::endl;
    std::cout << " input_file - input file in ROOT format ('Delphes' tree)," << std::endl;
    std::cout << " output_file - output file in ROOT format" << std::endl;
    return EXIT_FAILURE;
  }

  // Getting the input filename
  const std::string inputFile_name  = argv[1];
  const std::string outputFile_name = argv[2];

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile_name.c_str());

  // Create the output file
  TFile outputfile(outputFile_name.c_str(), "RECREATE");

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *branchJet        = treeReader->UseBranch("Jet");
  TClonesArray *branchGenJet     = treeReader->UseBranch("GenJet");
  TClonesArray *branchElectron   = treeReader->UseBranch("Electron");
  TClonesArray *branchMuon       = treeReader->UseBranch("Muon");
  TClonesArray *branchPhoton     = treeReader->UseBranch("Photon");
  TClonesArray *branchMet        = treeReader->UseBranch("MissingET");
  TClonesArray *branchParticle   = treeReader->UseBranch("Particle");
  
  // Book histograms
//----------------------------------------------------------------------------
//  Example 
//---------------------------------------------------------------------------
  TH1 *histJet1PT = new TH1F("jet1_pt", "jet1 P_{T}", 500, 0.0, 1000);

//----------------------------------------------------------------------------
//  Lepton Efficiency exercise
//----------------------------------------------------------------------------
  //TH1 *histGenLepEta      = new TH1F("histGenLepEta ", "GenLepton Eta", 120, -6, 6);
  //TH1 *histMatchGenLepEta = new TH1F("histMatchGenLepEta", "Matched GenLepton Eta", 120, -6, 6);
  //TH1 *histLepEffEta      = new TH1F("histLepEffEta", "Lepton Efficiency", 120, -6, 6);


//----------------------------------------------------------------------------
//  JEtMET execise
//----------------------------------------------------------------------------
  TH1 *histNJets = new TH1F("NJets", "NJets;NO. of Jets(P_{T} > 30GeV);Events", 20, 0.0, 20);
  TH1 *histJetPT = new TH1F("jet_pt", "jet P_{T};jet P_{T};NO. of Jets", 400, 0.0, 400);
  TH1 *histJetEta = new TH1F("jet_eta", "jet Eta;jet #eta;NO. of Jets", 120, -6, 6);

  //  MET and MET resolution
  TH1 *histMET = new TH1F("MET", "MET;#slash{E}_{T};Events", 100, 0.0, 300);
  TH1 *histMET_X = new TH1F("MET_X", "MET_X;#slash{E}_{T} P_{x};Events", 200, -200.0, 200.0);
  TH1 *histMET_Y = new TH1F("MET_Y", "MET_Y;#slash{E}_{T} P_{Y};Events", 200, -200.0, 200.0);

  // MHT and MHT resolution
  TH1 *histMHT = new TH1F("MHT", "MHT;#slash{H}_{T};Events", 100, 0.0, 300);
  TH1 *histMHT_X = new TH1F("MHT_X", "MHT_X;#slash{H}_{T};Events", 200, -200.0, 200.0);
  TH1 *histMHT_Y = new TH1F("MHT_Y", "MHT_Y;#slash{H}_{T};Events", 200, -200.0, 200.0);

  // Jet response: approximate by TProfile
  TProfile *histJetResPT = new TProfile("histJetResPT ", 
      "Jet response as a fucntion of GenJet Pt;GenJet P_{T};<RecoJet/GenJet P_{T}>", 100, 0, 500);
  TProfile *histJetResEta = new TProfile("histJetResEta ", 
      "Jet response as a function of GenJet Eta;GenJet #eta;<RecoJet/GenJet P_{T}>", 100, -5, 5);

  // Jet resolution for GenJet Eta (0, 2.5),(2.5, 4) and Pt > 50;
  TH1 *histJetEta1 = new TH1F("histJetEta1 ", "Jet Resolution with eta (0, 2.5);RecoJet/GenJet (GenJet 0 < |#eta| < 2.5, P_{T} > 50);Jets", 200, 0, 3);
  TH1 *histJetEta2 = new TH1F("histJetEta2 ", "Jet Resolution with eta (2.5, 4);RecoJet/GenJet (GenJet 2.5 < |#eta| < 4, P_{T} > 50);Jets", 200, 0, 3);

  std::map<std::string, TH1*> JetPTScale;
  double eta[4] ={0, 2.5, 4, 5 };
  double Pt[11] ={30, 50, 70, 100, 150, 200, 300, 500, 700, 900, 1200};
  for (int i = 1; i < 4; ++i)
  {
    for (int j = 1; j < 11; ++j)
    {
      std::stringstream ss;
      ss <<"JetPTScale" << i <<"-"<<j;
      std::string name = ss.str();
      ss << ";Reco Jet / Gen Jet " << "(GenJet " << eta[i-1] << 
        " < |#eta| < " << eta[i] << ", P_{T} > " << Pt[j-1] << " )" << ";Jets" ;
      JetPTScale[name] = new TH1F(name.c_str(), ss.str().c_str(), 200, 0, 3);
    }
  }

//----------------------------------------------------------------------------
//  Bonus for jets:
//----------------------------------------------------------------------------
  // Jet Efficiency 
  TH1 *histGenJetEta      = new TH1F("histGenJetEta ", "GenJet Eta;GenJet #eta;NO. of GenJet", 120, -6, 6);
  TH1 *histMatchGenJetEta = new TH1F("histMatchGenJetEta", "Matched GenJet Eta;GenJet #eta;NO. of GenJet with RecoJet matched", 120, -6, 6);
  TH1 *histJetEffEta      = new TH1F("histJetEffEta", "Jet Efficiency;GenJet #eta;Jet Efficiency", 120, -6, 6);
  
  // Jet Energy Correction
  TProfile *histJECPT = new TProfile("histJECPT ", 
      "Jet Energy Correction as a fucntion of RecoJet Pt;RecoJet P_{T};<GenJet/RecoJet P_{T}>", 100, 0, 500);
  TProfile *histJECEta = new TProfile("histJECEta ", 
      "Jet Energy Correction as a function of RecoJet Eta;RecoJet #eta;<GenJet/RecoJet P_{T}>", 100, -5, 5);
  
//----------------------------------------------------------------------------
//   Loop over all events
//----------------------------------------------------------------------------
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
  
    if (entry % 500 == 0)
      std::cout << "--------------------" << entry << std::endl;

    // If event contains at least 1 jet
    if(branchJet->GetEntries() > 0)
    {
      // Take first jet
      Jet *jet = (Jet*) branchJet->At(0);
      
      // Plot jet transverse momentum
      histJet1PT->Fill(jet->PT);
      
      // Print jet transverse momentum
      //std::cout << jet->PT << std::endl;
    }

//----------------------------------------------------------------------------
//  For MET and MHT
//----------------------------------------------------------------------------
    if (IsHadronic(branchParticle)) // Only plot MET for Hadronic TTbar decay
    {
      MissingET *MET = (MissingET*)branchMet->At(0);
      histMET->Fill(MET->MET);
      histMET_X->Fill(MET->P4().Px());
      histMET_Y->Fill(MET->P4().Py());

      TVector2 MHT = CalcMHT(branchJet, branchElectron, branchMuon, branchPhoton);
      histMHT->Fill(MHT.Mod());
      histMHT_X->Fill(MHT.Px());
      histMHT_Y->Fill(MHT.Py());
    }

//----------------------------------------------------------------------------
//  Jet distribution
//----------------------------------------------------------------------------
    int jetcount=0;
    for (int i = 0; i < branchJet->GetEntries(); ++i)
    {
      Jet *jet = (Jet*)branchJet->At(i);
      if (jet->PT > 30)
      {
        histJetPT->Fill(jet->PT);
        histJetEta->Fill(jet->Eta);
        jetcount++;
      }
    }
    histNJets->Fill(jetcount);

//----------------------------------------------------------------------------
//  Find the matching index
//----------------------------------------------------------------------------
    std::map<int, int> MatchIdx = MatchingJet(branchGenJet, branchJet);
    for (int i = 0; i < branchGenJet->GetEntries(); ++i)
    {
      if (MatchIdx.find(i) == MatchIdx.end() ) continue;
      Jet* gjet = (Jet*)branchGenJet->At(i);
      // Bonus: Jet Efficiency
      histGenJetEta->Fill(gjet->Eta);

      if (MatchIdx[i] == -1) continue;
      // Bonus: Jet Efficiency
      histMatchGenJetEta->Fill(gjet->Eta);

      Jet *jet = (Jet*) branchJet->At(MatchIdx[i]);

      histJetResPT->Fill(gjet->PT, jet->PT/gjet->PT);
      histJetResEta->Fill(gjet->Eta, jet->PT/gjet->PT);

      // Bonus: Jet Energy Correction
      // :WARNING:02/16/2014 07:31:47 PM:benwu: Delphes has certain Jet PT
      // threshold in the generation step. This would cause the matching bias
      // Here we select high Pt jets for the jet energy correction
      // More accurate way should obtain from the PT and Eta range plots
      if (jet->PT > 30)
      {
        histJECPT->Fill(jet->PT, gjet->PT/jet->PT);
        histJECEta->Fill(jet->Eta, gjet->PT/jet->PT);
      }

      for (int j = 1; j < 4; ++j)
      {
        if (fabs(gjet->Eta) < eta[j-1] || fabs(gjet->Eta) > eta[j]) continue;
        for (int k = 1; k < 11; ++k)
        {
          if (gjet->PT > Pt[k-1])
          {
            std::stringstream ss;
            ss <<"JetPTScale" << j <<"-"<<k;
            JetPTScale[ss.str()]->Fill(jet->PT/gjet->PT);
          }
        }
      }
    }

  } // End of looping event

  histJetEffEta = (TH1*)histMatchGenJetEta->Clone("histJetEffEta");
  histJetEffEta->SetTitle("Jet Efficiency;GenJet #eta;Jet Efficiency");
  histJetEffEta->Divide(histGenJetEta);

  // Saving resulting histograms
  histJet1PT->Write();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Lepton Efficiency ~~~~~
  //histGenLepEta->Write();
  //histMatchGenLepEta->Write();
  //histLepEffEta->Write();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ JetMet Exercise ~~~~~
  histNJets->Write();
  histJetPT->Write();
  histJetEta->Write();
  histMET->Write();
  histMET_X->Write();
  histMET_Y->Write();
  histMHT->Write();
  histMHT_X->Write();
  histMHT_Y->Write();
  histJetResPT->Write();
  histJetResEta->Write();
  //histJetEta1->Write();
  //histJetEta2->Write();
  for(std::map<std::string, TH1*>::iterator it=JetPTScale.begin();
    it!=JetPTScale.end(); it++)
  {
    it->second->Write();
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ JetMET Bonus ~~~~~
  histGenJetEta->Write();
  histMatchGenJetEta->Write();
  histJetEffEta->Write();
  histJECPT->Write();
  histJECEta->Write();

  outputfile.Close();
  return EXIT_SUCCESS;
}				// ----------  end of function main  ----------



// ===  FUNCTION  ============================================================
//         Name:  IsHadronic
//  Description:  Add to more box
// ===========================================================================
bool IsHadronic(TClonesArray *branchParticle)
{
  int GenSize = branchParticle->GetEntries(); 
  int lepcount = 0;
  for (int i = 0; i < GenSize; ++i)
  {
    GenParticle *p = (GenParticle*) branchParticle->At(i);
    if (p->Status != 3 ) // Skipping stable lepton, which might come from showing
      continue;
    if ( (p->M1 != -1 && fabs(((GenParticle*)branchParticle->At(p->M1))->PID) != 24) && 
        (p->M2 != -1 && fabs(((GenParticle*)branchParticle->At(p->M2))->PID) != 24 ))
        continue;  //Making sure the lepton from W decay 
    if (std::abs(p->PID) == 11 || std::abs(p->PID) == 13 || std::abs(p->PID) == 15) 
    {
      lepcount++;
    }
  }

  return lepcount == 0;
}       // -----  end of function IsHadronic  -----

// ===  FUNCTION  ============================================================
//         Name:  CalcMHT
//  Description:  
// ===========================================================================
TVector2 CalcMHT(TClonesArray *branchJet, TClonesArray *branchElectron, TClonesArray *branchMuon, TClonesArray *branchPhoton)
{
  TLorentzVector P4Sum(0.0, 0.0, 0.0, 0.0);
  // Summing up jets with PT > 30 and |Eta|< 5
  for (int i = 0; i < branchJet->GetEntriesFast(); ++i)
  {
    Jet *jet = (Jet*)branchJet->At(i);
    if (jet->PT > 30 && fabs(jet->Eta) < 5.0)
    {
      P4Sum += jet->P4();
    }
  }

  // Summing up Electron
  for (int i = 0; i < branchElectron->GetEntriesFast(); ++i)
  {
    P4Sum += ((Electron*)branchElectron->At(i))->P4();
  }
  // Summing up Muon
  for (int i = 0; i < branchMuon->GetEntriesFast(); ++i)
  {
    P4Sum += ((Muon*)branchMuon->At(i))->P4();
  }

  // Summing up Photon
  for (int i = 0; i < branchPhoton->GetEntriesFast(); ++i)
  {
    P4Sum += ((Photon*)branchPhoton->At(i))->P4();
  }

  TVector2 MHT(-1 * P4Sum.Px(), -1*P4Sum.Py());
  return MHT;
}       // -----  end of function CalcMHT  -----


std::map<int, int> MatchingJet(TClonesArray *branchGenJet, TClonesArray *branchJet)
{
  //Mapping the GenParticle index with Lepton index
  std::map<int, int> MatchIdx;

  for (int i = 0; i < branchGenJet->GetEntries(); ++i)
  {
    Jet* gjet = (Jet*)branchGenJet->At(i);
    MatchIdx[i] = -1;
    for (int j = 0; j < branchJet->GetEntries(); ++j)
    {
      Jet* jet = (Jet*)branchJet->At(j);
      if (jet->P4().DeltaR(gjet->P4()) < 0.3)
      {
        MatchIdx[i]=j;
        break;
      }
    }
  }

  return MatchIdx;
}       // -----  end of function MatchingLepton  -----

