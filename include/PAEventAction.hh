#ifndef PAEventAction_h
#define PAEventAction_h 1


#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>
#include <array>

// named constants
const G4int kDim = 4; //total number SDs

/// Event action

class PAEventAction : public G4UserEventAction
{
public:
    PAEventAction();
    virtual ~PAEventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

private:
    // hit collections Ids
    std::array<G4int, kDim> fHodHCID;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
