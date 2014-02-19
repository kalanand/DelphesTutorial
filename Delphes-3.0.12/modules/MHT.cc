
/** \class MHT
 *
 *
 *  \author John Stupak
 *
 */

#include "modules/MHT.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesFormula.h"

#include "ExRootAnalysis/ExRootResult.h"
#include "ExRootAnalysis/ExRootFilter.h"
#include "ExRootAnalysis/ExRootClassifier.h"

#include "TMath.h"
#include "TString.h"
#include "TFormula.h"
#include "TRandom3.h"
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"

#include <algorithm> 
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;

//------------------------------------------------------------------------------

MHT::MHT() :
  fJetInputArray(0),fItJetInputArray(0),fElectronInputArray(0),fItElectronInputArray(0),
  fMuonInputArray(0),fItMuonInputArray(0),fPhotonInputArray(0),fItPhotonInputArray(0),
  fMomentumOutputArray(0),
  fJetSelectionFormula(0),fElectronSelectionFormula(0),fMuonSelectionFormula(0),fPhotonSelectionFormula(0)
{
  fJetSelectionFormula = new DelphesFormula;
  fElectronSelectionFormula = new DelphesFormula;
  fMuonSelectionFormula = new DelphesFormula;
  fPhotonSelectionFormula = new DelphesFormula;
}

//------------------------------------------------------------------------------

MHT::~MHT()
{
}

//------------------------------------------------------------------------------

void MHT::Init()
{
  //Get input array names from the config file
  //The arguments to GetString are the name of the parameter whose value we want, and a default value in case the parameter is not present in the config file
  fElectronInputArray = ImportArray(GetString("ElectronInputArray", "UniqueObjectFinder/electrons"));
  fItElectronInputArray = fElectronInputArray->MakeIterator();

  fMuonInputArray = ImportArray(GetString("MuonInputArray", "UniqueObjectFinder/muons"));
  fItMuonInputArray = fMuonInputArray->MakeIterator();

  fPhotonInputArray = ImportArray(GetString("PhotonInputArray", "UniqueObjectFinder/photons"));
  fItPhotonInputArray = fPhotonInputArray->MakeIterator();

  //JETS

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //Get efficiency formulas from the config file
  fElectronSelectionFormula->Compile(GetString("ElectronSelectionFormula", "pt>30"));
  fMuonSelectionFormula->Compile(GetString("MuonSelectionFormula", "pt>30"));
  fPhotonSelectionFormula->Compile(GetString("PhotonSelectionFormula", "pt>30"));
  //JETS

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //Create output array
  //fMomentumOutputArray = ExportArray(GetString("MomentumOutputArray", "momentum"));

}

//------------------------------------------------------------------------------

void MHT::Finish()
{
}

//------------------------------------------------------------------------------

void MHT::Process()
{
  Candidate *candidate;   //Used in loops over electrons, muons, photons, and jets
  TLorentzVector candidateMomentum;   //Used in loops over electron, muons, photons, and jets to store 4-momenta of candidate
  TLorentzVector momentum;   //This will be used as running sum of all candidate's 4-momenta, from which the output will be derived

  vector< TIterator * >::iterator itInputList;
  TIterator *iterator;

  DelphesFactory *factory = GetFactory();

  //Initialize the total 4-momentum to 0
  momentum.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);

  // loop over all input electrons
  fItElectronInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItElectronInputArray->Next())))
    {
      candidateMomentum = candidate->Momentum;   //Get electron 4-momentum

      // apply an efficency formula
      if(gRandom->Uniform() <= fElectronSelectionFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
        {
          momentum+=candidateMomentum;   //Add electron 4-momentum to running total
	}
    }

  // loop over all input muons
  fItMuonInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItMuonInputArray->Next())))
    {
      candidateMomentum = candidate->Momentum;   //Get muon 4-momentum

      // apply an efficency formula      
      if(gRandom->Uniform() <= fMuonSelectionFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
        {
          momentum+=candidateMomentum;//Add muon 4-momentum to running total
	}
    }

  // loop over all input photons
  fItPhotonInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItPhotonInputArray->Next())))
    {
      candidateMomentum = candidate->Momentum;   //Get photon 4-momentum

      // apply an efficency formula
      if(gRandom->Uniform() <= fPhotonSelectionFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
        {
          momentum+=candidateMomentum;   //Add photon 4-momentum to running total
	}
    }

  //JETS

  //Candidate used to output MHT
  candidate = factory->NewCandidate();

  //Set 4-momentum of output candidate
  candidate->Position.SetXYZT(0.0, 0.0, 0.0, 0.0);
  momentum.SetPhi(3.14159+momentum.Phi());
  candidate->Momentum = momentum;

  //Output must be in the form of an array - Here the array will always contain exactly 1 element
  //fMomentumOutputArray->Add(candidate);
}

//------------------------------------------------------------------------------
