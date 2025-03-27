// copied from
/// \file optical/OpNovice2/include/SteppingAction.hh
/// \brief Definition of the SteppingAction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PASteppingAction_h
#define PASteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "PAEventAction.hh"
#include "globals.hh"


class PASteppingAction : public G4UserSteppingAction
{
  public:
    PASteppingAction(PAEventAction*);
    virtual ~PASteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);
    //void SetVerbose(G4int verbose){fVerbose = verbose;};
    /**
     * virtual void SetNPE(G4int npe){NPE=npe;}
    virtual void SetAGCNPE(G4int npe){AGC_NPE=npe;}
    virtual void SetHGCNPE(G4int npe){HGC_NPE=npe;}
    virtual void SetNGCNPE(G4int npe){NGC_NPE=npe;}
    G4int GetNPE(){return NPE;} 
    G4int GetAGCNPE(){return AGC_NPE;} 
    G4int GetHGCNPE(){return NGC_NPE;} 
    G4int GetNGCNPE(){return HGC_NPE;} 
    **/

  private:
    G4int fVerbose;
    PAEventAction *fEventAction;
    G4int S2YNPE;
    G4int AGCNPE;
    G4int HGCNPE;
    G4int NGCNPE;
    G4double edep;
    //G4int NGC_NPE;
    //G4int HGC_NPE;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
