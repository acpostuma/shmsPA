#include "PARunAction.hh"
#include "PAEventAction.hh"
#include "G4AnalysisManager.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PARunAction::PARunAction(PAEventAction* eventAction)
 : G4UserRunAction(),
   fEventAction(eventAction)
{ 
  // Create analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Default settings
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFileName("PAOutData.root"); //default, can overwrite in macro

  // Creating ntuple
  if ( fEventAction ) {
    analysisManager->CreateNtuple("PA", "Hits");
    analysisManager->CreateNtupleDColumn("S1XTime");    // column Id = 0
    analysisManager->CreateNtupleDColumn("S1YTime");    // column Id = 1
    analysisManager->CreateNtupleDColumn("S2XTime");    // column Id = 2
    analysisManager->CreateNtupleDColumn("S2YTime");    // column Id = 3
    analysisManager->CreateNtupleDColumn("S1XEnergy");    // column Id = 4
    analysisManager->CreateNtupleDColumn("S1YEnergy");    // column Id = 5
    analysisManager->CreateNtupleDColumn("S2XEnergy");    // column Id = 6
    analysisManager->CreateNtupleDColumn("S2YEnergy");    // column Id = 7
    analysisManager->CreateNtupleIColumn("S2YNPE");    // column Id = 8
    analysisManager->CreateNtupleIColumn("CopyNo");    // column Id = 9
    analysisManager->CreateNtupleIColumn("AGCNPE");    // column Id = 10
    analysisManager->CreateNtupleIColumn("HGCNPE");    // column Id = 11
    analysisManager->CreateNtupleIColumn("NGCNPE");    // column Id = 12
    analysisManager->CreateNtupleDColumn("CalEnergy");    // column Id = 13
    analysisManager->FinishNtuple();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PARunAction::~PARunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PARunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file 
  analysisManager->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PARunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // save histograms & ntuple
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
