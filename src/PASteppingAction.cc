//copied from:
/// \file optical/OpNovice2/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PASteppingAction.hh"
#include "PAEventAction.hh"
#include "G4Run.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4ProcessManager.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PASteppingAction::PASteppingAction(PAEventAction* evt)
: G4UserSteppingAction(), fEventAction(evt),
  fVerbose(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PASteppingAction::~PASteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PASteppingAction::UserSteppingAction(const G4Step* step)
{
  static G4ParticleDefinition* opticalphoton = 
              G4OpticalPhoton::OpticalPhotonDefinition();

  //initialize NPE to zero at beginning of step
  S2YNPE=0;
  AGCNPE=0;
  HGCNPE=0;
  NGCNPE=0;
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  G4Track* track = step->GetTrack();
  G4StepPoint* endPoint   = step->GetPostStepPoint();
  G4StepPoint* startPoint = step->GetPreStepPoint();

  G4String particleName = track->GetDynamicParticle()->
                                 GetParticleDefinition()->GetParticleName();



  auto logV = startPoint->GetTouchable()->GetVolume()->GetLogicalVolume();
  auto ekin = track->GetKineticEnergy();

  //find where primary track ends, if it is stopped before the calorimeter
  if (track->GetTrackStatus()==fStopAndKill && track->GetParentID()==0){
	  if (logV->GetName()!="CalLogical"){
		//output volume in which track stops
		//for spreadsheet validation
		G4int copyNo = startPoint->GetTouchable()->GetVolume()->GetCopyNo();
		analysisManager->FillNtupleIColumn(9, copyNo);
	  }
  }

  //Cherenkov photon counting
  //S2Y
  if (logV->GetName()=="S2YLogical" && particleName!="opticalphoton"){
    // loop over secondaries, create statistics
    const std::vector<const G4Track*>* secondaries =
                                step->GetSecondaryInCurrentStep();
    //count NPE this step
    for (auto sec : *secondaries) {
       if (sec->GetDynamicParticle()->GetParticleDefinition() == opticalphoton){
          if (sec->GetCreatorProcess()->GetProcessName().compare("Cerenkov")==0){
		S2YNPE+=1;
          }
        }
      }
  }

  //AGC
  if (logV->GetName()=="AGCTrayLogical" && particleName!="opticalphoton"){
    // loop over secondaries, create statistics
    const std::vector<const G4Track*>* secondaries =
                                step->GetSecondaryInCurrentStep();
    for (auto sec : *secondaries) {
      if (sec->GetDynamicParticle()->GetParticleDefinition() == opticalphoton){
        if (sec->GetCreatorProcess()->GetProcessName().compare("Cerenkov")==0){
		AGCNPE+=1;
        }
       }
    }
  }
  
  if (logV->GetName()=="NGCLogical" && particleName!="opticalphoton"){
    // loop over secondaries, create statistics
    const std::vector<const G4Track*>* secondaries =
                                step->GetSecondaryInCurrentStep();
    for (auto sec : *secondaries) {
      if (sec->GetDynamicParticle()->GetParticleDefinition() == opticalphoton){
        if (sec->GetCreatorProcess()->GetProcessName().compare("Cerenkov")==0){
		NGCNPE+=1;
        }
       }
    }
  }
  
  if (logV->GetName()=="HGCLogical" && particleName!="opticalphoton"){
    // loop over secondaries, create statistics
    const std::vector<const G4Track*>* secondaries =
                                step->GetSecondaryInCurrentStep();
    for (auto sec : *secondaries) {
      if (sec->GetDynamicParticle()->GetParticleDefinition() == opticalphoton){
        if (sec->GetCreatorProcess()->GetProcessName().compare("Cerenkov")==0){
		HGCNPE+=1;
        }
       }
    }
  }

  if (logV->GetName()=="CalLogical"){
	  edep = step->GetTotalEnergyDeposit();
	  fEventAction->AddCalEnergy(edep);
  } 

  fEventAction->AddS2YNPE(S2YNPE);
  fEventAction->AddAGCNPE(AGCNPE);
  fEventAction->AddHGCNPE(HGCNPE);
  fEventAction->AddNGCNPE(NGCNPE);

  return;
}  


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
