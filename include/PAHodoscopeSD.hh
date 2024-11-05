#ifndef PAHodoscopeSD_h
#define PAHodoscopeSD_h 1

#include "G4VSensitiveDetector.hh"
#include "PAHodoscopeHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/// Hodoscope sensitive detector

class PAHodoscopeSD : public G4VSensitiveDetector
{
  public:
    PAHodoscopeSD(G4String name);
    virtual ~PAHodoscopeSD();
    
    virtual void Initialize(G4HCofThisEvent*HCE);
    virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
    
  private:
    PAHodoscopeHitsCollection* fHitsCollection;
    G4int fHCID;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
