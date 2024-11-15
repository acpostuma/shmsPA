#include "PADetectorConstruction.hh"

#include "PAHodoscopeSD.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4SDManager.hh"
//#include "G4UImanager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PADetectorConstruction::PADetectorConstruction()
: G4VUserDetectorConstruction(), 
  fVisAttributes(),
  fS1XLogical(nullptr),
  fS2XLogical(nullptr),
  fS1YLogical(nullptr),
  fS2YLogical(nullptr),
  fSHMSPhys(nullptr)

{
	fUseNGC=0;
	ReadParameters("detectors.dat");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PADetectorConstruction::~PADetectorConstruction(){
  for (auto visAttributes: fVisAttributes) {
    delete visAttributes;
  }  
  //delete fDetMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* PADetectorConstruction::Construct(){
  
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;


  //material shortcut codes
  auto air = G4Material::GetMaterial("G4_AIR");
  auto argonGas = G4Material::GetMaterial("G4_Ar");
  auto scintillator = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  auto csI = G4Material::GetMaterial("G4_CESIUM_IODIDE");
  auto lead = G4Material::GetMaterial("G4_Pb");


  // geometries --------------------------------------------------------------
  // experimental hall (world volume)
  auto worldSolid 
    = new G4Box("worldBox",10.*m,10.*m,10.*m); //large box, half-length in xyz
  auto worldLogical
    = new G4LogicalVolume(worldSolid,air,"worldLogical");
  auto worldPhysical
    = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,
                        false,0,checkOverlaps);
  
  
  // SHMS spectrometer as a whole
  auto SHMSSolid 
    = new G4Box("SHMSBox",2.*m,2.*m,5*m); //z is beam axis
  auto SHMSLogical
    = new G4LogicalVolume(SHMSSolid,air,"SHMSLogical");
  fSHMSPhys
    = new G4PVPlacement(0,G4ThreeVector(0,0,5.*m),SHMSLogical,
                        "fSHMSPhys",worldLogical,
                        false,0,checkOverlaps); 
  			//place so that front face is on (0,0,0)
  
  //NGC: optional, some modifications if turned off
  if (fUseNGC ==1){
        auto NGCSolid
          = new G4Box("NGCBox",1.0*m,30.*cm,50.*cm);
        auto fNGCLogical
          = new G4LogicalVolume(NGCSolid,argonGas,"NGCLogical");
        new G4PVPlacement(0,G4ThreeVector(0.,0.,0.5*m),fNGCLogical,
                          "NGCPhysical",SHMSLogical,
                          false,0,checkOverlaps);
  	fNGCLogical->SetVisAttributes(G4Colour(0.4,0.6,0.7));
  } else {
          //do nothing
  }


  // drift chambers: 1<z<2 m
  auto DCSolid 
    = new G4Box("DCBox",1.0*m,30.*cm,1.*cm);
  auto fDCLogical
    = new G4LogicalVolume(DCSolid,argonGas,"chamber2Logical");
  
  for (auto i=0;i<12;i++) {
    G4double z2 = (i+1)*5.*cm; //separation between chambers
    new G4PVPlacement(0,G4ThreeVector(0.,0.,1.0*m+z2),fDCLogical,
                      "DCPhysical",SHMSLogical,
                      false,i,checkOverlaps);
  }
  // S1 hodoscopes: 2<z<2.5 m
  auto S1Solid 
    = new G4Box("S1Box",60.*cm,60.*cm,0.5*cm);
  fS1XLogical
    = new G4LogicalVolume(S1Solid,scintillator,"S1XLogical");
  fS1YLogical
    = new G4LogicalVolume(S1Solid,scintillator,"S1YLogical");

  //place both S1X and S1Y
  new G4PVPlacement(0,G4ThreeVector(0.,0.,2.1*m),fS1XLogical,
                     "S1Physical",SHMSLogical,
                      false,0,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,2.2*m),fS1YLogical,
                     "S1Physical",SHMSLogical,
                      false,0,checkOverlaps);
  
  // Heavy Gas Cerenkov: 2.5<z<3.5 m
  auto HGCSolid
    = new G4Box("HGCBox",1.0*m,30.*cm,50.*cm);
  auto fHGCLogical
    = new G4LogicalVolume(HGCSolid,argonGas,"HGCLogical");
  new G4PVPlacement(0,G4ThreeVector(0.,0.,3.*m),fHGCLogical,
                    "HGCPhysical",SHMSLogical,
                    false,0,checkOverlaps);
  
  // Aerogel Cerenkov
  auto AGCSolid
    = new G4Box("AGCBox",1.0*m,30.*cm,50.*cm);
  auto fAGCLogical
    = new G4LogicalVolume(AGCSolid,argonGas,"AGCLogical");
  new G4PVPlacement(0,G4ThreeVector(0.,0.,4.*m),fAGCLogical,
                    "AGCPhysical",SHMSLogical,
                    false,0,checkOverlaps);
  
  // S2 hodoscopes: 4-4.5 m or so
  auto S2Solid 
    = new G4Box("S1Box",60.*cm,60.*cm,0.5*cm);
  fS2XLogical
    = new G4LogicalVolume(S2Solid,scintillator,"S2XLogical");
  fS2YLogical
    = new G4LogicalVolume(S2Solid,scintillator,"S2YLogical");
  new G4PVPlacement(0,G4ThreeVector(0.,0.,4.6*m),fS2XLogical,
                     "S2XPhysical",SHMSLogical,
                     false,0,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,4.7*m),fS2YLogical,
                     "S2YPhysical",SHMSLogical,
                     false,0,checkOverlaps);
  
  
  // lead calorimeter: 5<z<6 m
  auto CalSolid
    = new G4Box("CalBox",1.0*m,30.*cm,50.*cm);
  auto fCalLogical
    = new G4LogicalVolume(CalSolid,lead,"CalLogical");
  new G4PVPlacement(0,G4ThreeVector(0.,0.,5.5*m),fCalLogical,
                    "CalPhysical",SHMSLogical,
                    false,0,checkOverlaps);
  
  
  
  
  
  // visualization attributes ------------------------------------------------
  
  //world volume, SHMS general volume invisible
  auto visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  visAttributes->SetVisibility(false);
  worldLogical->SetVisAttributes(visAttributes);
  SHMSLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);
  
  //all Cerenkovs in Light Gray 
  visAttributes = new G4VisAttributes(G4Colour(0.9,0.9,0.9));   // LightGray 
  fHGCLogical->SetVisAttributes(visAttributes);
  fAGCLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);
  
  //not sure what colour this is: use for hodoscopes 
  visAttributes = new G4VisAttributes(G4Colour(0.8888,0.0,0.0));
  fS1XLogical->SetVisAttributes(visAttributes);
  fS2XLogical->SetVisAttributes(visAttributes);
  fS1YLogical->SetVisAttributes(visAttributes);
  fS2YLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);
  
  //new colour for drift chambers
  visAttributes = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  fDCLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);
  
  //new colour for calorimeter 
  visAttributes = new G4VisAttributes(G4Colour(0.8888,0.8888,0.0));
  fCalLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);
  
  // make hodoscopes a sensitive detector -----------------------------------
  ConstructSD();
  
  // return the world physical volume ----------------------------------------
  
  return worldPhysical;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PADetectorConstruction::ConstructSD()
{
  G4cout<<"ConstructSD()"<<G4endl;
  // sensitive detectors -----------------------------------------------------
  auto sdManager = G4SDManager::GetSDMpointer();
  G4String SDname;
  

  auto hodoscope1x = new PAHodoscopeSD(SDname="/S1X");
  sdManager->AddNewDetector(hodoscope1x);
  fS1XLogical->SetSensitiveDetector(hodoscope1x);
  
  auto hodoscope1y = new PAHodoscopeSD(SDname="/S1Y");
  sdManager->AddNewDetector(hodoscope1y);
  fS1YLogical->SetSensitiveDetector(hodoscope1y);
  
  auto hodoscope2x = new PAHodoscopeSD(SDname="/S2X");
  sdManager->AddNewDetector(hodoscope2x);
  fS2XLogical->SetSensitiveDetector(hodoscope2x);
  
  auto hodoscope2y = new PAHodoscopeSD(SDname="/S2Y");
  sdManager->AddNewDetector(hodoscope2y);
  fS2YLogical->SetSensitiveDetector(hodoscope2y);
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//read the NGC option
void PADetectorConstruction::ReadParameters(const char* file){
        //define file contents
        char* keylist[] = { (char*) "Use-NGC:", NULL}; //only NGC option
        enum {ENGC, ENULL};
        G4int ikey, iread;
        G4int ierr = 0;
        char line[256];
        char delim[64];
        //open the file
        FILE* pdata;
        if( (pdata = fopen(file,"r")) == NULL ){
                printf("Error opening detector parameter file: %s\n",file);
                return;
        }
        //start reading the file
        while( fgets(line,256,pdata) ){
        if( line[0] == '#' ) continue; //skip commented lines
        printf("%s\n",line);
        sscanf(line,"%s",delim);
        for(ikey=0; ikey<ENULL; ikey++) //look for the defined keys
            if(!strcmp(keylist[ikey],delim)) break;
        switch( ikey ){
        default:
            //stop running if the file contains something strange
            printf("Unrecognised delimiter: %s\n",delim);
            ierr++;
            break;
        case ENGC: //dimensions of main target vessel
            iread = sscanf(line,"%*s%d", //line contains text, one integer
                           &fUseNGC); //assign to this variable
            if( iread != 1) ierr++;
            break;
            }
        if( ierr ){ //if something strange is in the file, error and exit
                printf("Fatal Error: invalid read of parameter line %s\n %s\n",
                   keylist[ikey],line);
            exit(-1);
                }
        }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PADetectorConstruction::ConstructMaterials()
{
	G4cout<<"ConstructMaterials()"<<G4endl;
  auto nistManager = G4NistManager::Instance();

  // Air 
  nistManager->FindOrBuildMaterial("G4_AIR");
  
  // Argon gas
  nistManager->FindOrBuildMaterial("G4_Ar");
  // With a density different from the one defined in NIST
  // G4double density = 1.782e-03*g/cm3; 
  // nistManager->BuildMaterialWithNewDensity("PA_Ar","G4_Ar",density);
  // !! cases segmentation fault

  // Scintillator
  // (PolyVinylToluene, C_9H_10)
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  
  // CsI
  nistManager->FindOrBuildMaterial("G4_CESIUM_IODIDE");
  
  // Lead
  nistManager->FindOrBuildMaterial("G4_Pb");
  
  // Vacuum "Galactic"
  // nistManager->FindOrBuildMaterial("G4_Galactic");

  // Vacuum "Air with low density"
  // auto air = G4Material::GetMaterial("G4_AIR");
  // G4double density = 1.0e-5*air->GetDensity();
  // nistManager
  //   ->BuildMaterialWithNewDensity("Air_lowDensity", "G4_AIR", density);

  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

