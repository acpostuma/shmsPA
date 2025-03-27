#include "PAActionInitialization.hh"
#include "PAPrimaryGeneratorAction.hh"
#include "PARunAction.hh"
#include "PAEventAction.hh"
#include "PASteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAActionInitialization::PAActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAActionInitialization::~PAActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAActionInitialization::BuildForMaster() const
{
  PAEventAction* eventAction = new PAEventAction;
  SetUserAction(new PARunAction(eventAction));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAActionInitialization::Build() const
{
  SetUserAction(new PAPrimaryGeneratorAction);

  auto eventAction = new PAEventAction;
  SetUserAction(eventAction);
  
  auto steppingAction = new PASteppingAction(eventAction);
  SetUserAction(steppingAction);

  SetUserAction(new PARunAction(eventAction));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
