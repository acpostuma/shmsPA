#include "PAEventAction.hh"
#include "PASteppingAction.hh"
#include "PAHodoscopeHit.hh"
#include "PAConstants.hh"
#include "PAAnalysis.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

using std::array;
using std::vector;


namespace {

// Utility function which finds a hit collection with the given Id
// and print warnings if not found 
G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
  auto hce = event->GetHCofThisEvent();
  if (!hce) {
      G4ExceptionDescription msg;
      msg << "No hits collection of this event found." << G4endl; 
      G4Exception("PAEventAction::EndOfEventAction()",
                  "PACode001", JustWarning, msg);
      return nullptr;
  }

  auto hc = hce->GetHC(collId);
  if ( ! hc) {
    G4ExceptionDescription msg;
    msg << "Hits collection " << collId << " of this event not found." << G4endl; 
    G4Exception("PAEventAction::EndOfEventAction()",
                "PACode001", JustWarning, msg);
  }
  return hc;  
}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAEventAction::PAEventAction()
: G4UserEventAction(), 
  fHodHCID  {{ -1, -1 }}
      // std::array<T, N> is an aggregate that contains a C array. 
      // To initialize it, we need outer braces for the class itself 
      // and inner braces for the C array
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAEventAction::~PAEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAEventAction::BeginOfEventAction(const G4Event*)
{
  // Find hit collections and histogram Ids by names (just once)
  // and save them in the data members of this class

  if (fHodHCID[0] == -1) {
    auto sdManager = G4SDManager::GetSDMpointer();
    auto analysisManager = G4AnalysisManager::Instance();

    // hits collections names
    array<G4String, kDim> hHCName 
      = {{ "S1X/HodoHitsColl", "S1Y/HodoHitsColl", "S2X/HodoHitsColl", "S2Y/HodoHitsColl" }};

    for (G4int iDet = 0; iDet < kDim; ++iDet) {
      // hit collections IDs
      fHodHCID[iDet]   = sdManager->GetCollectionID(hHCName[iDet]);
    }
  }
  //initialize to zero
  S2YNPE=0;
  AGCNPE=0;
  HGCNPE=0;
  NGCNPE=0;
  CalEnergy=0;
}     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAEventAction::EndOfEventAction(const G4Event* event)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
 
   
  // Fill Ntuple: Hodoscopes hits
  for (G4int iDet = 0; iDet < 4; ++iDet) {
    auto hc = GetHC(event, fHodHCID[iDet]);
    if ( ! hc ) return;

    for (unsigned int i = 0; i<hc->GetSize(); ++i) {
      auto hit = static_cast<PAHodoscopeHit*>(hc->GetHit(i));
      analysisManager->FillNtupleDColumn(iDet, hit->GetTime());
      analysisManager->FillNtupleDColumn(iDet+4, hit->GetEdep()); 
    }
  }
  //Fill  Ntuple: NPE numbers 
  analysisManager->FillNtupleIColumn(8,S2YNPE);
  analysisManager->FillNtupleIColumn(10,AGCNPE);
  analysisManager->FillNtupleIColumn(11,HGCNPE);
  analysisManager->FillNtupleIColumn(12,NGCNPE);
  analysisManager->FillNtupleDColumn(13,CalEnergy);
  
  //add row to Ntuple
  analysisManager->AddNtupleRow();

  //
  // Print diagnostics: for debugging purposes
  /**
  
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( printModulo == 0 || event->GetEventID() % printModulo != 0) return;
  
  auto primary = event->GetPrimaryVertex(0)->GetPrimary(0);
  G4cout 
    << G4endl
    << ">>> Event " << event->GetEventID() << " >>> Simulation truth : "
    << primary->GetG4code()->GetParticleName()
    << " " << primary->GetMomentum() << G4endl;
  
  // Hodoscopes
  for (G4int iDet = 0; iDet < kDim; ++iDet) {
    auto hc = GetHC(event, fHodHCID[iDet]);
    if ( ! hc ) return;
    G4cout << "Hodoscope " << iDet + 1 << " has " << hc->GetSize()  << " hits." << G4endl;
    for (unsigned int i = 0; i<hc->GetSize(); ++i) {
      hc->GetHit(i)->Print();
    }
  }
  ***/

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
