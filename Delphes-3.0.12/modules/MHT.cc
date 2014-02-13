
/** \class MHT
 *
 *  Selects candidates from the InputArray according to the efficiency formula.
 *
 *  $Date: 2013-02-12 14:57:44 +0100 (Tue, 12 Feb 2013) $
 *  $Revision: 905 $
 *
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
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
  fJetEffFormula(0),fElectronEffFormula(0),fMuonEffFormula(0),fPhotonEffFormula(0)
{
  fJetEffFormula = new DelphesFormula;
  fElectronEffFormula = new DelphesFormula;
  fMuonEffFormula = new DelphesFormula;
  fPhotonEffFormula = new DelphesFormula;
}

//------------------------------------------------------------------------------

MHT::~MHT()
{
}

//------------------------------------------------------------------------------

void MHT::Init()
{
  fJetInputArray = ImportArray(GetString("JetInputArray", "UniqueObjectFinder/jets"));
  fItJetInputArray = fJetInputArray->MakeIterator();

  fElectronInputArray = ImportArray(GetString("ElectronInputArray", "UniqueObjectFinder/electrons"));
  fItElectronInputArray = fElectronInputArray->MakeIterator();

  fMuonInputArray = ImportArray(GetString("MuonInputArray", "UniqueObjectFinder/muons"));
  fItMuonInputArray = fMuonInputArray->MakeIterator();

  fPhotonInputArray = ImportArray(GetString("PhotonInputArray", "UniqueObjectFinder/photons"));
  fItPhotonInputArray = fPhotonInputArray->MakeIterator();

  // create output array
  fMomentumOutputArray = ExportArray(GetString("MomentumOutputArray", "momentum"));

  fJetEffFormula->Compile(GetString("JetEffFormula", "pt>30"));
  fElectronEffFormula->Compile(GetString("ElectronEffFormula", "pt>30"));
  fMuonEffFormula->Compile(GetString("MuonEffFormula", "pt>30"));
  fPhotonEffFormula->Compile(GetString("PhotonEffFormula", "pt>30"));
}

//------------------------------------------------------------------------------

void MHT::Finish()
{
}

//------------------------------------------------------------------------------

void MHT::Process()
{
  Candidate *candidate;
  TLorentzVector momentum;
  TLorentzVector candidatePosition, candidateMomentum;

  vector< TIterator * >::iterator itInputList;
  TIterator *iterator;

  DelphesFactory *factory = GetFactory();

  momentum.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);

  // loop over all input jets
  fItJetInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItJetInputArray->Next())))
    {
      candidatePosition = candidate->Position;
      candidateMomentum = candidate->Momentum;

      // apply an efficency formula
      if(gRandom->Uniform() <= fJetEffFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
      	{
	  momentum+=candidateMomentum;
	}
    }

  // loop over all input electrons
  fItElectronInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItElectronInputArray->Next())))
    {
      candidatePosition = candidate->Position;
      candidateMomentum = candidate->Momentum;

      // apply an efficency formula
      if(gRandom->Uniform() <= fElectronEffFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
        {
          momentum+=candidateMomentum;
	}
    }

  // loop over all input muons
  fItMuonInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItMuonInputArray->Next())))
    {
      candidatePosition = candidate->Position;
      candidateMomentum = candidate->Momentum;

      // apply an efficency formula      
      if(gRandom->Uniform() <= fMuonEffFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
        {
          momentum+=candidateMomentum;
	}
    }

  // loop over all input photons
  fItPhotonInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItPhotonInputArray->Next())))
    {
      candidatePosition = candidate->Position;
      candidateMomentum = candidate->Momentum;

      // apply an efficency formula
      if(gRandom->Uniform() <= fPhotonEffFormula->Eval(candidateMomentum.Pt(), candidateMomentum.Eta()))
        {
          momentum+=candidateMomentum;
	}
    }

  candidate = factory->NewCandidate();

  candidate->Position.SetXYZT(0.0, 0.0, 0.0, 0.0);
  candidate->Momentum = momentum;

  fMomentumOutputArray->Add(candidate);
}

//------------------------------------------------------------------------------
