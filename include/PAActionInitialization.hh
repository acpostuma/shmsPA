#ifndef PAActionInitialization_h
#define PAActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class PAActionInitialization : public G4VUserActionInitialization
{
  public:
    PAActionInitialization();
    virtual ~PAActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
