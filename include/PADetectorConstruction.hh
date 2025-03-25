#ifndef PADetectorConstruction_h
#define PADetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"
#include "G4NistManager.hh"

#include <vector>


class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;

/// Detector construction

class PADetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    PADetectorConstruction();
    virtual ~PADetectorConstruction();
    
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSD();
    
    void ConstructMaterials();
    void ReadParameters(const char*); //ngc or no ngc

  private:
    /**
    G4LogicalVolume* fCalLogical;
    G4LogicalVolume* fDCLogical;
    G4LogicalVolume* fAGCLogical;
    G4LogicalVolume* fHGCLogical;
**/
    
    G4LogicalVolume* fNGCLogical;
    std::vector<G4VisAttributes*> fVisAttributes;
    
    G4LogicalVolume* fS2XLogical;
    G4LogicalVolume* fS2YLogical;
    G4LogicalVolume* fS1XLogical;
    G4LogicalVolume* fS1YLogical;
    G4LogicalVolume* fCalLogical;
    G4VPhysicalVolume* fSHMSPhys;

    G4int fUseNGC;
    G4int fAeroTray;

};

#endif
