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

// Classes from ROOT
#include "TH1.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TApplication.h"

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
  const std::string inputFile  = argv[1];
  const std::string outputFile_name  = argv[2];

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile.c_str());

  // Create the output file
  TFile outputfile(outputFile_name.c_str(), "RECREATE");

  // Keep the root window open
  TApplication *app = new TApplication("DelHATS", &argc, argv);
  
  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *branchJet = treeReader->UseBranch("Jet");
  TClonesArray *branchElectron = treeReader->UseBranch("Electron");
  TClonesArray *branchGenParticles = treeReader->UseBranch("Particle");
  
  // Book histograms
  TH1 *histJetPT = new TH1F("jet_pt", "jet P_{T}", 100, 0.0, 100.0);
  TH1 *histMass = new TH1F("mass", "M_{inv}(e_{1}, e_{2})", 100, 40.0, 140.0);

  // Book histograms for Lepton Efficiency
  TH1 *histGenLepEta = new TH1F("GenParticle_Eta", "GenP Eta", 120, -6, 6);
  TH1 *histMatchGenLepEta = new TH1F("MatchGenParticle_Eta", "MatchGenP Eta", 120, -6, 6);
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
      std::cout << jet->PT << std::endl;
    }

    Electron *elec1, *elec2;

    // If event contains at least 2 electrons
    if(branchElectron->GetEntries() > 1)
    {
      // Take first two electrons
      elec1 = (Electron *) branchElectron->At(0);
      elec2 = (Electron *) branchElectron->At(1);

      // Plot their invariant mass
      histMass->Fill(((elec1->P4()) + (elec2->P4())).M());
    }

//----------------------------------------------------------------------------
//  
//----------------------------------------------------------------------------
    std::map<int, int> MatchIdx = MatchingLepton<Electron>(branchGenParticles, branchElectron, 11);
    
    std::cout << MatchIdx.size() << std::endl;
    for(std::map<int, int>::iterator it=MatchIdx.begin();
      it!=MatchIdx.end(); it++)
    {
      GenParticle *gen = (GenParticle*) branchGenParticles->At(it->first);
      histGenLepEta->Fill(gen->Eta);
      if (it->second != -1)
      {
        histMatchGenLepEta->Fill(gen->Eta);
      }
    }

  } // End of looping events

  // Show resulting histograms
  histJetPT->Draw();
  histMass->Draw();
  histGenLepEta->Write();
  histMatchGenLepEta->Write();

  outputfile.Close();
  app->Run();

  return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

// ===  FUNCTION  ============================================================
//         Name:  MatchingLepton
//  Description:  
// ===========================================================================
  template <class T>
std::map<int, int> MatchingLepton(TClonesArray *branchParticle, TClonesArray *branchLep, int PID)
{
  //Mapping the GenParticle index with Lepton index
  std::map<int, int> MatchIdx;

//----------------------------------------------------------------------------
//  Getting the Gen Lepton
//----------------------------------------------------------------------------
  int GenSize = branchGenP->GetEntries(); 
  for (int i = 0; i < GenSize; ++i)
  {
    GenParticle *p = (GenParticle*) branchGenP->At(i);
    if (p->Status != 3 || p->M1 > GenSize || p->M2 > GenSize )  continue;
    if (std::fabs(p->PID) == PID) //Electron 
    {
      MatchIdx[i] = -1;
    }
  }

//----------------------------------------------------------------------------
//  Getting the matched lepton
//----------------------------------------------------------------------------

  for (int i = 0; i < branchLep->GetEntries(); ++i)
  {
    T *lep = (T*)branchLep->At(i);
    for(std::map<int, int>::iterator it=MatchIdx.begin();
      it!=MatchIdx.end(); it++)
    {
      GenParticle *p = (GenParticle*) branchGenP->At(it->first);
      if (p->P4().DeltaR(lep->P4())<0.3)
      {
        it->second = i;
        break;
      }
    }
  }

  return MatchIdx;
}       // -----  end of function MatchingLepton  -----
