#ifndef PARunAction_h
#define PARunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class PAEventAction;

class G4Run;

/// Run action class

class PARunAction : public G4UserRunAction
{
  public:
    PARunAction(PAEventAction* eventAction);
    virtual ~PARunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

  private:
    PAEventAction* fEventAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
