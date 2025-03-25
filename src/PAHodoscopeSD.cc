#include "PAHodoscopeSD.hh"
#include "PAHodoscopeHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Electron.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAHodoscopeSD::PAHodoscopeSD(G4String name)
: G4VSensitiveDetector(name), 
  fHitsCollection(nullptr), fHCID(-1)
{
  collectionName.insert("HodoHitsColl");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAHodoscopeSD::~PAHodoscopeSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAHodoscopeSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection = new PAHodoscopeHitsCollection
  (SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) { 
    fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection); 
  }
  hce->AddHitsCollection(fHCID,fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PAHodoscopeSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto edep = step->GetTotalEnergyDeposit();
  if (edep==0.) return true;
  
  auto preStepPoint = step->GetPreStepPoint();
  auto touchable = preStepPoint->GetTouchable();
  auto copyNo = touchable->GetVolume()->GetCopyNo(); //which hodoscope is hit
  auto hitTime = preStepPoint->GetGlobalTime();


  //for debugging purposes
  //G4cout<<edep/keV<<"  "<<copyNo<<"  "<<hitTime<<G4endl;
 
  // check if this hodoscope already has a hit
  auto ix = -1;
  for (auto i=0;i<fHitsCollection->entries();i++) {
    if ((*fHitsCollection)[i]->GetID()==copyNo) {
      ix = i;
      break;
    }
  }
  // if it has, then take the earlier time
  if (ix>=0) {
    if ((*fHitsCollection)[ix]->GetTime()>hitTime) { 
      (*fHitsCollection)[ix]->SetTime(hitTime);
    }
     (*fHitsCollection)[ix]->AddEdep(edep); 
  }
  // if not, create a new hit and set it to the collection
  else {
    auto hit = new PAHodoscopeHit(copyNo,hitTime,edep);
    auto physical = touchable->GetVolume();
    hit->SetLogV(physical->GetLogicalVolume());
    auto transform = touchable->GetHistory()->GetTopTransform();
    transform.Invert();
    hit->SetRot(transform.NetRotation());
    hit->SetPos(transform.NetTranslation());
    fHitsCollection->insert(hit);
  }    
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
