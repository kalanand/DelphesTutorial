// ===========================================================================
// 
//       Filename:  DelHATS.cc
// 
//    Description:  An example code for Delphes exercise for lepton efficiency
//    comparison between PhaseI and PhaseII 0PU samples
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


  template <class T>
std::map<int, int> MatchingLepton(TClonesArray *branchParticle, TClonesArray *branchLep, int PID);

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
  TH1 *histGenLepEta      = new TH1F("histGenLepEta ", "GenLepton Eta;GenLepton #eta;NO. of GenLepton", 120, -6, 6);
  TH1 *histMatchGenLepEta = new TH1F("histMatchGenLepEta", "Matched GenLepton Eta;GenLepton #eta;NO. of GenLepton with RecoLepton matched", 120, -6, 6);
  TH1 *histLepEffEta      = new TH1F("histLepEffEta", "Lepton Efficiency;GenLepton #eta;Lepton Efficiency", 120, -6, 6);


//----------------------------------------------------------------------------
//  JEtMET execise
//----------------------------------------------------------------------------
  //TH1 *histNJets = new TH1F("NJets", "NJets;NO. of Jets(P_{T} > 30GeV);Events", 20, 0.0, 20);
  //TH1 *histJetPT = new TH1F("jet_pt", "jet P_{T};jet P_{T};NO. of Jets", 400, 0.0, 400);
  //TH1 *histJetEta = new TH1F("jet_eta", "jet Eta;jet #eta;NO. of Jets", 120, -6, 6);

  ////  MET and MET resolution
  //TH1 *histMET = new TH1F("MET", "MET;#slash{E}_{T};Events", 100, 0.0, 300);
  //TH1 *histMET_X = new TH1F("MET_X", "MET_X;#slash{E}_{T} P_{x};Events", 200, -200.0, 200.0);
  //TH1 *histMET_Y = new TH1F("MET_Y", "MET_Y;#slash{E}_{T} P_{Y};Events", 200, -200.0, 200.0);

  //// MHT and MHT resolution
  //TH1 *histMHT = new TH1F("MHT", "MHT;#slash{H}_{T};Events", 100, 0.0, 300);
  //TH1 *histMHT_X = new TH1F("MHT_X", "MHT_X;#slash{H}_{T};Events", 200, -200.0, 200.0);
  //TH1 *histMHT_Y = new TH1F("MHT_Y", "MHT_Y;#slash{H}_{T};Events", 200, -200.0, 200.0);

  //// Jet response: approximate by TProfile
  //TProfile *histJetResPT = new TProfile("histJetResPT ", 
      //"Jet response as a fucntion of GenJet Pt;GenJet P_{T};<RecoJet/GenJet P_{T}>", 100, 0, 500);
  //TProfile *histJetResEta = new TProfile("histJetResEta ", 
      //"Jet response as a function of GenJet Eta;GenJet #eta;<RecoJet/GenJet P_{T}>", 100, -5, 5);

  //// Jet resolution for GenJet Eta (0, 2.5),(2.5, 4) and Pt > 50;
  //TH1 *histJetEta1 = new TH1F("histJetEta1 ", "Jet Resolution with eta (0, 2.5);RecoJet/GenJet (GenJet 0 < |#eta| < 2.5, P_{T} > 50);Jets", 200, 0, 3);
  //TH1 *histJetEta2 = new TH1F("histJetEta2 ", "Jet Resolution with eta (2.5, 4);RecoJet/GenJet (GenJet 2.5 < |#eta| < 4, P_{T} > 50);Jets", 200, 0, 3);

//----------------------------------------------------------------------------
//  Bonus for jets:
//----------------------------------------------------------------------------
  //// Jet Efficiency 
  //TH1 *histGenJetEta      = new TH1F("histGenJetEta ", "GenJet Eta;GenJet #eta;NO. of GenJet", 120, -6, 6);
  //TH1 *histMatchGenJetEta = new TH1F("histMatchGenJetEta", "Matched GenJet Eta;GenJet #eta;NO. of GenJet with RecoJet matched", 120, -6, 6);
  //TH1 *histJetEffEta      = new TH1F("histJetEffEta", "Jet Efficiency;GenJet #eta;Jet Efficiency", 120, -6, 6);
  
  //// Jet Energy Correction
  //TProfile *histJECPT = new TProfile("histJECPT ", 
      //"Jet Energy Correction as a fucntion of RecoJet Pt;RecoJet P_{T};<GenJet/RecoJet P_{T}>", 100, 0, 500);
  //TProfile *histJECEta = new TProfile("histJECEta ", 
      //"Jet Energy Correction as a function of RecoJet Eta;RecoJet #eta;<GenJet/RecoJet P_{T}>", 100, -5, 5);
  
//----------------------------------------------------------------------------
//   Loop over all events
//----------------------------------------------------------------------------
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
  
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

    if (entry % 500 == 0)
      std::cout << "--------------------" << entry << std::endl;
//----------------------------------------------------------------------------
//  Lepton Efficiency Exercise
//----------------------------------------------------------------------------
    std::map<int, int> MatchIdx = MatchingLepton<Muon>(branchParticle, branchMuon, 13);
    
    for(std::map<int, int>::iterator it=MatchIdx.begin();
      it!=MatchIdx.end(); it++)
    {
      GenParticle *gen = (GenParticle*) branchParticle->At(it->first);
      histGenLepEta->Fill(gen->Eta);
      if (it->second != -1)
      {
        histMatchGenLepEta->Fill(gen->Eta);
      }
    }

  } // End of looping events

  histLepEffEta = (TH1*)histMatchGenLepEta->Clone("histLepEffEta");
  histLepEffEta->SetTitle("Lepton Efficiency;GenLepton #eta;Lepton Efficiency");
  histLepEffEta->Divide(histGenLepEta);

  // Saving resulting histograms
  histJet1PT->Write();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Lepton Efficiency ~~~~~
  histGenLepEta->Write();
  histMatchGenLepEta->Write();
  histLepEffEta->Write();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ JetMet Exercise ~~~~~
  //histNJets->Write();
  //histJetPT->Write();
  //histJetEta->Write();
  //histMET->Write();
  //histMET_X->Write();
  //histMET_Y->Write();
  //histMHT->Write();
  //histMHT_X->Write();
  //histMHT_Y->Write();
  //histJetResPT->Write();
  //histJetResEta->Write();
  //histJetEta1->Write();
  //histJetEta2->Write();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ JetMET Bonus ~~~~~
  //histGenJetEta->Write();
  //histMatchGenJetEta->Write();
  //histJetEffEta->Write();
  //histJECPT->Write();
  //histJECEta->Write();

  outputfile.Close();
  return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

// ===  FUNCTION  ============================================================
//         Name:  MatchingLepton
//  Description:  A template fucntion for matching reco lepton with the
//  GenParticles specified by the PID (ele 11, muon 13, tau 15). Return a
//  mapping of the index of GenParticle to reco lepton
// ===========================================================================
  template <class T>
std::map<int, int> MatchingLepton(TClonesArray *branchParticle, TClonesArray *branchLep, int PID)
{
  //Mapping the GenParticle index with Lepton index
  std::map<int, int> MatchIdx;

//----------------------------------------------------------------------------
//  Getting the Gen Lepton
//----------------------------------------------------------------------------
  int GenSize = branchParticle->GetEntries(); 
  for (int i = 0; i < GenSize; ++i)
  {
    GenParticle *p = (GenParticle*) branchParticle->At(i);
    if (p->Status != 3 ) // Skipping stable lepton, which might come from showing
      continue;
    if ( (p->M1 != -1 && fabs(((GenParticle*)branchParticle->At(p->M1))->PID) != 24) && 
        (p->M2 != -1 && fabs(((GenParticle*)branchParticle->At(p->M2))->PID) != 24 ))
        continue;  //Making sure the lepton from W decay 
    if (std::fabs(p->PID) == PID) //Matched to the wanted lepton
    {
      MatchIdx[i] = -1;
    }
  }

//----------------------------------------------------------------------------
//  Getting the matched lepton, simply take deltaR < 0.3 as matched
//----------------------------------------------------------------------------
  for (int i = 0; i < branchLep->GetEntries(); ++i)
  {
    T *lep = (T*)branchLep->At(i);
    for(std::map<int, int>::iterator it=MatchIdx.begin();
      it!=MatchIdx.end(); it++)
    {
      GenParticle *p = (GenParticle*) branchParticle->At(it->first);
      if (p->P4().DeltaR(lep->P4())<0.3)
      {
        it->second = i;
        break;
      }
    }
  }

  return MatchIdx;
}       // -----  end of function MatchingLepton  -----
