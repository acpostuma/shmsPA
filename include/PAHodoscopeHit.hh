#ifndef PAHodoscopeHit_h
#define PAHodoscopeHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/// Hodoscope hit
///
/// It records:
/// - the strip ID
/// - the particle time
/// - the strip logical volume, its position and rotation

class PAHodoscopeHit : public G4VHit
{
  public:
    PAHodoscopeHit(G4int i,G4double t, G4double e);
    PAHodoscopeHit(const PAHodoscopeHit &right);
    virtual ~PAHodoscopeHit();

    const PAHodoscopeHit& operator=(const PAHodoscopeHit &right);
    G4bool operator==(const PAHodoscopeHit &right) const;
    
    inline void *operator new(size_t);
    inline void operator delete(void*aHit);
    
    void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    void Print();
    
    G4int GetID() const { return fId; }

    void SetTime(G4double val) { fTime = val; }
    G4double GetTime() const { return fTime; }
    
    void SetEdep(G4double val) { fEdep = val; }
    void AddEdep(G4double val) { fEdep += val; }
    G4double GetEdep() const { return fEdep; }

    void SetPos(G4ThreeVector xyz) { fPos = xyz; }
    G4ThreeVector GetPos() const { return fPos; }

    void SetRot(G4RotationMatrix rmat) { fRot = rmat; }
    G4RotationMatrix GetRot() const { return fRot; }

    void SetLogV(G4LogicalVolume* val) { fPLogV = val; }
    const G4LogicalVolume* GetLogV() const { return fPLogV; }
    
  private:
    G4int fId;
    G4double fTime;
    G4double fEdep;
    G4ThreeVector fPos;
    G4RotationMatrix fRot;
    const G4LogicalVolume* fPLogV;
};

using PAHodoscopeHitsCollection = G4THitsCollection<PAHodoscopeHit>;

extern G4ThreadLocal G4Allocator<PAHodoscopeHit>* PAHodoscopeHitAllocator;

inline void* PAHodoscopeHit::operator new(size_t)
{
  if (!PAHodoscopeHitAllocator) {
       PAHodoscopeHitAllocator = new G4Allocator<PAHodoscopeHit>;
  }
  return (void*)PAHodoscopeHitAllocator->MallocSingle();
}

inline void PAHodoscopeHit::operator delete(void*aHit)
{
  PAHodoscopeHitAllocator->FreeSingle((PAHodoscopeHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
