// ===========================================================================
// 
//       Filename:  DelHATS.cc
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  02/11/2014 01:44:39 PM
//       Compiler:  g++ -std=c++11
// 
//         Author:  Zhenbin Wu (benwu)
//          Email:  benwu@fnal.gov
//        Company:  Baylor University, CDF@FNAL, CMS@LPC
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
#include "TRint.h"
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
  const std::string inputFile  = argv[1];
  const std::string outputFile_name  = argv[2];


  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile.c_str());

  // Create the output file
  TFile outputfile(outputFile_name.c_str(), "RECREATE");

  // Keep the root window open
  //TRint *app = new TRint("DelHATS", &argc, argv);
  
  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *branchEvent      = treeReader->UseBranch("Event");
  TClonesArray *branchJet        = treeReader->UseBranch("Jet");
  TClonesArray *branchGenJet     = treeReader->UseBranch("GenJet");
  TClonesArray *branchCAJet      = treeReader->UseBranch("CAJet");
  TClonesArray *branchElectron   = treeReader->UseBranch("Electron");
  TClonesArray *branchMuon       = treeReader->UseBranch("Muon");
  TClonesArray *branchPhoton     = treeReader->UseBranch("Photon");
  TClonesArray *branchMet        = treeReader->UseBranch("MissingET");
  TClonesArray *branchHt         = treeReader->UseBranch("ScalarHT");
  TClonesArray *branchParticle   = treeReader->UseBranch("Particle");
  
  // Book histograms
  TH1 *histJetPT = new TH1F("jet_pt", "jet P_{T}", 500, 0.0, 1000);
  TH1 *histJetEta = new TH1F("jet_eta", "jet Eta", 120, -6, 6);

  TH1 *histMET = new TH1F("MET", "MET", 100, 0.0, 1000);
  TH1 *histMET_X = new TH1F("MET_X", "MET_X", 100, -100.0, 100.0);
  TH1 *histMET_Y = new TH1F("MET_Y", "MET_Y", 100, -100.0, 100.0);

  TH1 *histMHT = new TH1F("MHT", "MHT", 100, 0.0, 1000);
  TH1 *histMHT_X = new TH1F("MHT_X", "MHT_X", 100, -100.0, 100.0);
  TH1 *histMHT_Y = new TH1F("MHT_Y", "MHT_Y", 100, -100.0, 100.0);

  // Book histograms for MET
  TH1 *histGenJetPT = new TH1F("genjet_pt", "jet P_{T}", 100, 0.0, 100.0);
  TH1 *histMatchedGenJetPT = new TH1F("matchjet_pt", "jet P_{T}", 100, 0.0, 100.0);
  // Book histograms for MHT
  
  TProfile *histJetResPT = new TProfile("JetResPT", "GenJet PT", 100, 0,500);
  TProfile *histJetResEta = new TProfile("JetResEta", "GenJet Eta", 100, -5, 5);
  std::map<std::string, TH1*> JetPTScale;
  double eta[5] ={ -5, -2.5, 0, 2.5, 5 };
  double Pt[11] ={30, 50, 70, 100, 150, 200, 300, 500, 700, 900, 1200};
  for (int i = 1; i < 5; ++i)
  {
    for (int j = 1; j < 11; ++j)
    {
      std::stringstream ss;
      ss <<"JetPTScale" << i <<"-"<<j;
      std::string name = ss.str();
      ss << ";Reco Jet / Gen Jet " << "( " << eta[i-1] << 
        " < #eta < " << eta[i] << ", " << Pt[j-1] <<" < P_{T} < " << Pt[j] <<" )" << ";Events" ;
      JetPTScale[name] = new TH1F(name.c_str(), ss.str().c_str(), 200, -1, 3);
    }
  }
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
      histJetPT->Fill(jet->PT);
      
      // Print jet transverse momentum
      //std::cout << jet->PT << std::endl;
    }


//----------------------------------------------------------------------------
//  Calculate MET and MHT
//----------------------------------------------------------------------------
    TVector2 MHT = CalcMHT(branchJet, branchElectron, branchMuon, branchPhoton);
    histMHT->Fill(MHT.Mod());
    histMHT_X->Fill(MHT.Px());
    histMHT_Y->Fill(MHT.Py());
    MissingET *MET = (MissingET*)branchMet->At(0);
    histMET->Fill(MET->MET);
    histMET_X->Fill(MET->P4().Px());
    histMET_Y->Fill(MET->P4().Py());


    for (int i = 0; i < branchJet->GetEntries(); ++i)
    {
      Jet *jet = (Jet*)branchJet->At(i);
      if (jet->PT > 30 && fabs(jet->Eta) < 5)
      {
        histJetPT->Fill(jet->PT);
        histJetEta->Fill(jet->Eta);
      }
    }

//----------------------------------------------------------------------------
//  Find the matching index
//----------------------------------------------------------------------------
    std::map<int, int> MatchIdx = MatchingJet(branchGenJet, branchJet);
    for (int i = 0; i < branchGenJet->GetEntries(); ++i)
    {
      if (MatchIdx.find(i) == MatchIdx.end() ) continue;
      if (MatchIdx[i] == -1) continue;

      Jet* gjet = (Jet*)branchGenJet->At(i);
      Jet *jet = (Jet*) branchJet->At(MatchIdx[i]);

      histJetResPT->Fill(gjet->PT, jet->PT/gjet->PT);
      histJetResEta->Fill(gjet->Eta, jet->PT/gjet->PT);

      for (int j = 1; j < 5; ++j)
      {
        if (gjet->Eta < eta[j-1] || gjet->Eta > eta[j]) continue;
        for (int k = 1; k < 11; ++k)
        {
          if (gjet->PT > Pt[k-1] && gjet->PT < Pt[k])
          {
            std::stringstream ss;
            ss <<"JetPTScale" << j <<"-"<<k;
            JetPTScale[ss.str()]->Fill(jet->PT/gjet->PT);
          }
        }
      }
    }

  } // End of looping event


  // Show resulting histograms
  //histJetPT->Draw();

  histJetPT->Write();
  histJetResPT->Write();
  histJetResEta->Write();
  histJetEta->Write();
  histMET->Write();
  histMET_X->Write();
  histMET_Y->Write();
  histMHT->Write();
  histMHT_X->Write();
  histMHT_Y->Write();

  for(std::map<std::string, TH1*>::iterator it=JetPTScale.begin();
    it!=JetPTScale.end(); it++)
  {
    it->second->Write();
    //delete it->second;
  }

  outputfile.Close();
  //app->Run();

  return EXIT_SUCCESS;
}				// ----------  end of function main  ----------



// ===  FUNCTION  ============================================================
//         Name:  IsHadronic
//  Description:  
// ===========================================================================
bool IsHadronic(TClonesArray *branchParticle)
{
  int GenSize = branchParticle->GetEntries(); 
  int lepcount = 0;
  for (int i = 0; i < GenSize; ++i)
  {
    GenParticle *p = (GenParticle*) branchParticle->At(i);
    if (p->Status != 3 || p->M1 > GenSize || p->M2 > GenSize )  continue;
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

