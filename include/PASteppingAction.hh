// copied from
/// \file optical/OpNovice2/include/SteppingAction.hh
/// \brief Definition of the SteppingAction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PASteppingAction_h
#define PASteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "globals.hh"


class PASteppingAction : public G4UserSteppingAction
{
  public:
    PASteppingAction();
    virtual ~PASteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);
    //void SetVerbose(G4int verbose){fVerbose = verbose;};

  private:
    G4int fVerbose;
    G4int NPE;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
