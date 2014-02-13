#ifndef MHT_h
#define MHT_h

/** \class MHT
 *
 *  Selects candidates from the InputArray according to the efficiency formula.
 *
 *  $Date: 2012-11-18 15:57:08 +0100 (Sun, 18 Nov 2012) $
 *  $Revision: 814 $
 *
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "classes/DelphesModule.h"

#include <deque>

class TObjArray;
class DelphesFormula;

class MHT: public DelphesModule
{
public:

  MHT();
  ~MHT();

  void Init();
  void Process();
  void Finish();

private:
  
  const TObjArray *fJetInputArray,*fElectronInputArray,*fMuonInputArray,*fPhotonInputArray; //!  
  TIterator *fItJetInputArray,*fItElectronInputArray,*fItMuonInputArray,*fItPhotonInputArray; //!
  TObjArray *fMomentumOutputArray; //!
  DelphesFormula *fJetEffFormula,*fElectronEffFormula,*fMuonEffFormula,*fPhotonEffFormula; //!

  ClassDef(MHT, 1)
};

#endif
