#ifndef PAPrimaryGeneratorAction_h
#define PAPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

/// Primary generator
///
/// A single particle is generated.
/// User can select 
/// - the initial momentum and angle
/// - the momentum and angle spreads
/// - random selection of a particle type from proton, kaon+, pi+, muon+, e+ 


class PAPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PAPrimaryGeneratorAction();
    virtual ~PAPrimaryGeneratorAction();
    
    virtual void GeneratePrimaries(G4Event*);

    void ReadGeneratedEvents();
    
    void SetMomentum(G4double val) { fMomentum = val; }
    G4double GetMomentum() const { return fMomentum; }

    
  private:
    void DefineCommands();

    G4ParticleGun* fParticleGun;
    G4GenericMessenger* fMessenger;
    G4ParticleDefinition* fProton;
    G4double fMomentum;

    std::vector<double> delta, xfp, yfp, xpfp, ypfp; //event kinematics
    G4int NEVENTS;
    G4int nev;
    G4bool fUseGenerated;
    G4double ztar = -3.5;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
