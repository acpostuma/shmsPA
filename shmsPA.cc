/******
 * Main program shmsPA for determination of proton absorption coefficient
 * in the SHMS spectrometer. Based on Geant4 example basic/PA.
 *
 ******/

#include "PADetectorConstruction.hh"
#include "PAActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "FTFP_BERT_ATL.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BERT.hh"
#include "G4PhysListFactory.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//copy some physics lists from A2Geant4
#include "G4OpticalPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4FastSimulationPhysics.hh" //low E electron physics


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  //fix random seed bs
  G4Random::setTheEngine(new CLHEP::RanecuEngine());
  G4long seed = time(NULL);
  CLHEP::HepRandom::setTheSeed(seed); 
  G4Random::setTheSeed(seed);

  // Construct the default run manager
  auto runManager = new G4RunManager;

  // Mandatory user initialization classes
  runManager->SetUserInitialization(new PADetectorConstruction);

  auto physicsList = new FTFP_BERT; 
  physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  //add optical and low energy electromagnetic models
  physicsList->RegisterPhysics(new G4OpticalPhysics()); //optical photons: want Cherenkov
  physicsList->ReplacePhysics(new G4EmLivermorePhysics()); //replace standard EM

  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new PAActionInitialization());

  // Visualization manager construction
  auto visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  if ( !ui ) {
    // execute an argument macro file if exist
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    UImanager->ApplyCommand("/control/execute macros/init_vis.mac");
    if (ui->IsGUI()) {
         UImanager->ApplyCommand("/control/execute macros/gui.mac");
    }     
    // start interactive session
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
