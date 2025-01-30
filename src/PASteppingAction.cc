//copied from:
/// \file optical/OpNovice2/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PASteppingAction.hh"
//#include "EventAction.hh"
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
#include "PAAnalysis.hh"
#include "G4ProcessManager.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PASteppingAction::PASteppingAction()
: G4UserSteppingAction(),
  fVerbose(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PASteppingAction::~PASteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PASteppingAction::UserSteppingAction(const G4Step* step)
{
  static G4ParticleDefinition* opticalphoton = 
              G4OpticalPhoton::OpticalPhotonDefinition();
  
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
		G4cout<<"Track stopped in "<<logV->GetName()<<G4endl;
	  }
  }

  //ONLY if in the particle is in S2Y
  //count one level of secondaries, not arbitrary amounts
  if (logV->GetName()=="S2YLogical" && track->GetParentID()==0){

    // loop over secondaries, create statistics
     const std::vector<const G4Track*>* secondaries =
                                step->GetSecondaryInCurrentStep();

    //if new track, reset NPE
    if (step->IsFirstStepInVolume()){
    	    NPE = 0;
    }
    //count NPE this step
    G4int npe_temp = 0;
    for (auto sec : *secondaries) {
      if (sec->GetDynamicParticle()->GetParticleDefinition() == opticalphoton){
        if (sec->GetCreatorProcess()->GetProcessName().compare("Cerenkov")==0){
		npe_temp+=1;
        }
       }
    }
    //add to total NPE
    NPE+=npe_temp;
    //fill ntuple with total number of photoelectrons 
    if (step->IsLastStepInVolume()){
    	analysisManager->FillNtupleIColumn(8,NPE);
    }
  }


  return;
}  


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
