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
    virtual void AddS2YNPE(G4int npe){S2YNPE+=npe;}
    virtual void AddAGCNPE(G4int npe){AGCNPE+=npe;}
    virtual void AddHGCNPE(G4int npe){HGCNPE+=npe;}
    virtual void AddNGCNPE(G4int npe){NGCNPE+=npe;}
    virtual void AddCalEnergy(G4double edep){CalEnergy+=edep;}

private:
    // hit collections Ids
    std::array<G4int, kDim> fHodHCID;
    G4int S2YNPE;
    G4int AGCNPE;
    G4int HGCNPE;
    G4int NGCNPE;
    G4double CalEnergy;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
