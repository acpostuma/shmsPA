#include "PAHodoscopeHit.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<PAHodoscopeHit>* PAHodoscopeHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAHodoscopeHit::PAHodoscopeHit(G4int id,G4double time, G4double edep)
: G4VHit(), 
  fId(id), fTime(time), fEdep(edep), fPos(0.), fPLogV(nullptr)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAHodoscopeHit::~PAHodoscopeHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PAHodoscopeHit::PAHodoscopeHit(const PAHodoscopeHit &right)
: G4VHit(),
  fId(right.fId),
  fTime(right.fTime),
  fPos(right.fPos),
  fRot(right.fRot),
  fPLogV(right.fPLogV),
  fEdep(right.fEdep)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const PAHodoscopeHit& PAHodoscopeHit::operator=(const PAHodoscopeHit &right)
{
  fId = right.fId;
  fTime = right.fTime;
  fEdep = right.fEdep;
  fPos = right.fPos;
  fRot = right.fRot;
  fPLogV = right.fPLogV;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PAHodoscopeHit::operator==(const PAHodoscopeHit &/*right*/) const
{
  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAHodoscopeHit::Draw()
{
  auto visManager = G4VVisManager::GetConcreteInstance();
  if (! visManager) return;

  G4Transform3D trans(fRot.inverse(),fPos);
  G4VisAttributes attribs;
  auto pVA = fPLogV->GetVisAttributes();
  if (pVA) attribs = *pVA;
  G4Colour colour(0.,1.,1.);
  attribs.SetColour(colour);
  attribs.SetForceSolid(true);
  visManager->Draw(*fPLogV,attribs,trans);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::map<G4String,G4AttDef>* PAHodoscopeHit::GetAttDefs() const
{
  G4bool isNew;
  auto store = G4AttDefStore::GetInstance("PAHodoscopeHit",isNew);

  if (isNew) {
    (*store)["HitType"] 
      = G4AttDef("HitType","Hit Type","Physics","","G4String");
    
    (*store)["ID"] 
      = G4AttDef("ID","ID","Physics","","G4int");
    
    (*store)["Time"] 
      = G4AttDef("Time","Time","Physics","G4BestUnit","G4double");
    
    (*store)["Edep"] 
      = G4AttDef("Edep","Edep","Physics","G4BestUnit","G4double");
    
    (*store)["Pos"] 
      = G4AttDef("Pos","Position","Physics","G4BestUnit","G4ThreeVector");
    
    (*store)["LVol"] 
      = G4AttDef("LVol","Logical Volume","Physics","","G4String");
  }
  return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* PAHodoscopeHit::CreateAttValues() const
{
  auto values = new std::vector<G4AttValue>;
  
  values
    ->push_back(G4AttValue("HitType","HodoscopeHit",""));
  values
    ->push_back(G4AttValue("ID",G4UIcommand::ConvertToString(fId),""));
  values
    ->push_back(G4AttValue("Time",G4BestUnit(fTime,"Time"),""));
  values
    ->push_back(G4AttValue("Edep",G4BestUnit(fEdep,"Edep"),""));
  values
    ->push_back(G4AttValue("Pos",G4BestUnit(fPos,"Length"),""));
  
  if (fPLogV)
    values->push_back(G4AttValue("LVol",fPLogV->GetName(),""));
  else
    values->push_back(G4AttValue("LVol"," ",""));
  
  return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PAHodoscopeHit::Print()
{
  G4cout << "  Hodoscope[" << fId << "] " << fEdep/MeV << " (MeV)" << fTime/ns << " (nsec)" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
