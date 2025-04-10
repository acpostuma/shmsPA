#include "PADetectorConstruction.hh"

#include "PAHodoscopeSD.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4MaterialPropertiesTable.hh"
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
	fAeroTray=10;
	ReadParameters("detectors.dat");
	ConstructMaterials();
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
  G4bool checkOverlaps = false;
  // Max step size for the hodoscopes
  G4double maxStep = 1.0*mm;
  auto hodoStepLimit = new G4UserLimits(maxStep);


  //material shortcut codes
  auto air = G4Material::GetMaterial("G4_AIR");
  auto scintillator = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  auto leadglass = G4Material::GetMaterial("G4_GLASS_LEAD");
  auto vacuum = G4Material::GetMaterial("G4_Galactic");
  auto nobleGas = G4Material::GetMaterial("NobleGas");
  auto heavyGas = G4Material::GetMaterial("C4F10");
  auto driftGas = G4Material::GetMaterial("DriftGas");
  auto aerogel = G4Material::GetMaterial("SiAerogel");
  auto quartz = G4Material::GetMaterial("Quartz");
  auto aluminum = G4Material::GetMaterial("G4_Al");
  auto copper = G4Material::GetMaterial("G4_Cu");
  auto tungsten = G4Material::GetMaterial("G4_W");
  auto mylar = G4Material::GetMaterial("Mylar");
  auto tedlar = G4Material::GetMaterial("G4_POLYVINYLIDENE_FLUORIDE");
  auto kapton = G4Material::GetMaterial("G4_KAPTON");
  auto rohacell = G4Material::GetMaterial("RohaCell");
  auto sio2 = G4Material::GetMaterial("SiO2");
  auto LH2 = G4Material::GetMaterial("LH2");

  //define colours
  auto greyblue = new G4VisAttributes(G4Colour(0.4,0.6,0.7));
  auto red = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  auto blue = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  auto green = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  auto yellow = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  auto purple = new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  auto orange = new G4VisAttributes(G4Colour(1.0,0.5,0.0));

  //distances from focal plane to center of detector
  //estinated: some conflicting information
  G4double fTGT_cm = -313;
  G4double fHB_cm = -290;
  G4double fDip_noNGC_cm = -50;
  G4double fDip_NGC_cm = -280;
  G4double fNGC_cm = -177;
  G4double fDC1_cm = -40;
  G4double fDC2_cm = 40;
  G4double fS1X_cm = 56;
  G4double fS1Y_cm = 60;
  G4double fHGC_cm = 137;
  G4double fAGC_cm = 220;
  G4double fS2X_cm = 276;
  G4double fS2Y_cm = 280;
  G4double fCal_cm = 292+100; //to front plus half length


  // geometries --------------------------------------------------------------
  // experimental hall (world volume): air
  auto worldSolid 
    = new G4Box("worldBox",10.*m,10.*m,10.*m); //large box, half-length in xyz
  auto worldLogical
    = new G4LogicalVolume(worldSolid,air,"worldLogical");
  worldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());
  auto worldPhysical
    = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,
                        false,0,checkOverlaps);
  
  
  // SHMS spectrometer as a whole: also air
  // will include HB entrance and dipole exit
  auto SHMSSolid 
    = new G4Box("SHMSBox",2.*m,2.*m,10.*m); //z is beam axis
  auto SHMSLogical
    = new G4LogicalVolume(SHMSSolid,air,"SHMSLogical");
  SHMSLogical->SetVisAttributes(G4VisAttributes::GetInvisible());
  fSHMSPhys
    = new G4PVPlacement(0,G4ThreeVector(0,0,0.*m),SHMSLogical,
                        "fSHMSPhys",worldLogical,
                        false,0,checkOverlaps); 
  
  //Target and entrance window -------------------------------------------
  //LH2 in loop
  //size is 4cm diameter, inner diameter measured on DocDB
  //make larger in order to work with the simulated xtar, ytar
  auto targetSolid
	  = new G4Tubs("targetCyl",0.*cm, 20.*cm, 5.*cm, 0.*deg, 360.*deg);
  auto targetLogical 
	  = new G4LogicalVolume(targetSolid,LH2,"targetLogical");
  new G4PVPlacement(0, G4ThreeVector(0,0,fTGT_cm*cm),targetLogical,
		  "fTargetPhys",SHMSLogical,
		  false,10,checkOverlaps);
  targetLogical->SetVisAttributes(blue);
  //target exit window
  auto targetExitSolid 
	  = new G4Tubs("targetExitCyl",0.*cm,2.*cm,0.005*cm,0.*deg,360.*deg);
  auto targetExitLogical
	  = new G4LogicalVolume(targetExitSolid,aluminum,"targetExitLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,4.995*cm),targetExitLogical,
		  "fTargetExitPhys",targetLogical,
		  false,10,checkOverlaps);
  //HB entrance
  //Make smaller so it's clear what this is 
  auto HBExitSolid
	  = new G4Box("HBExitBox",0.5*m,0.5*m,0.015*cm);
  auto HBExitLogical
	  = new G4LogicalVolume(HBExitSolid,aluminum,"HBExitLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,fHB_cm*cm),HBExitLogical,
		  "fHBExitPhys",SHMSLogical,
		  false,11,checkOverlaps);
  HBExitLogical->SetVisAttributes(orange);
  //Dipole exit
  auto DipExitSolid
	  = new G4Box("DipExitBox",0.5*m,0.5*m,0.025*cm);
  auto DipExitLogical
	  = new G4LogicalVolume(DipExitSolid,aluminum,"DipExitLogical");
  DipExitLogical->SetVisAttributes(orange);
  //placement depends on Noble Gas enabled or not

  //Noble Gas Cherenkov -----------------------------------------------
  if (fUseNGC ==1){ //if NGC included in experimental configuration
	//main Cherenkov volume
        auto NGCSolid
          = new G4Box("NGCBox",1.0*m,1.0*m,1.0*m); //200 cm long
        fNGCLogical
          = new G4LogicalVolume(NGCSolid,nobleGas,"NGCLogical");
        new G4PVPlacement(0,G4ThreeVector(0.,0.,fNGC_cm*cm),fNGCLogical,
                          "NGCPhysical",SHMSLogical,
                          false,20,checkOverlaps);
  	fNGCLogical->SetVisAttributes(greyblue);
	
	//entrance and exit windows
	auto NGCWindowSolid = new G4Box("NGCWindowBox",1.0*m,1.0*m,0.005*cm); 
	auto fNGCWindowLogical = new G4LogicalVolume(NGCWindowSolid,tedlar,
			"NGCWindowLogical");
	//entrance
	new G4PVPlacement(0,G4ThreeVector(0,0,-0.995*m),fNGCWindowLogical,
			"fNGCEntrancePhys",fNGCLogical,false,21,checkOverlaps);
	//exit
	new G4PVPlacement(0,G4ThreeVector(0,0,0.995*m),fNGCWindowLogical,
			"fNGCExitPhys",fNGCLogical,false,24,checkOverlaps);
	
	//mirror
	auto NGCMirrorSolid = new G4Box("NGCMirrorBox",1.0*m,1.0*m,0.15*cm); 
	auto fNGCMirrorLogical = new G4LogicalVolume(NGCMirrorSolid,sio2,
					"NGCMirrorLogical");
	//guessing at positioning
	new G4PVPlacement(0,G4ThreeVector(0,0,0.8*m),fNGCMirrorLogical,
		"fNGCMirrorPhys",fNGCLogical,false,22,checkOverlaps);
	//mirror support
	auto NGCSupportSolid = new G4Box("NGCSupportBox",1.0*m,1.0*m,0.9*cm); 
	auto fNGCSupportLogical = new G4LogicalVolume(NGCSupportSolid,rohacell,
					"NGCSupportLogical");
	//guessing at positioning
	new G4PVPlacement(0,G4ThreeVector(0,0,0.815*m),fNGCSupportLogical,
		"fNGCSupportPhys",fNGCLogical,false,23,checkOverlaps);
  
	//now place the dipole exit
	new G4PVPlacement(0,G4ThreeVector(0,0,fDip_NGC_cm*cm),DipExitLogical,
		  "fDipExitPhys",SHMSLogical,
		  false,12,checkOverlaps);
  } else {
          
	new G4PVPlacement(0,G4ThreeVector(0,0,fDip_noNGC_cm*cm),DipExitLogical,
		  "fDipExitPhys",SHMSLogical,
		  false,12,checkOverlaps);
  }


  // Drift Chambers ----------------------------------------------------------
  //  1<z<2 m
  auto DCSolid 
    = new G4Box("DCBox",1.0*m,1.0*m,1.905*cm); //unclear now large these acutally are
  auto fDCLogical
    = new G4LogicalVolume(DCSolid,driftGas,"DCLogical");
  fDCLogical->SetVisAttributes(green);
  //windows on each side
  auto DCWindowSolid = new G4Box("DCWindowBox",1.0*m,1.0*m,0.00125*cm);
  auto fDCWindowLogical = new G4LogicalVolume(DCWindowSolid,mylar,"DCWindowLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,-1.90625*cm),fDCWindowLogical,
		  "DCEntrancePhys",fDCLogical,false,31,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0,0,1.90625*cm),fDCWindowLogical,
		  "DCExitPhys",fDCLogical,false,35,checkOverlaps);
  
  //field wires: slab of equivalent width 
  auto DCWireSolid = new G4Box("DCWireBox",1.*m,1.*m,0.02415*mm);
  //wrong material for now
  auto fDCWireLogical = new G4LogicalVolume(DCWireSolid,tungsten,"DCWireLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,0),fDCWireLogical,"DCWirePhys",fDCLogical,false,32,checkOverlaps);
  //sensing wires
  auto DCSenseSolid = new G4Box("DCWireBox",1.*m,1.*m,0.0015*mm);
  //wrong material for now
  //copper-plated beryllium but density is higher than both??
  auto fDCSenseLogical = new G4LogicalVolume(DCSenseSolid,copper,"DCSenseLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,2*mm),
		  fDCSenseLogical,"DCSensePhys",fDCLogical,false,33,
		  checkOverlaps);
  //cathode 
  auto DCCathodeSolid = new G4Box("DCCathodeBox",1.*m,1.*m,0.045*cm);
  auto fDCCathodeLogical = new G4LogicalVolume(DCCathodeSolid,kapton,"DCCathodeLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,0),fDCCathodeLogical,"DCCathodePhys",fDCLogical,false,34,checkOverlaps);

  //place the finished drift chambers: two with 1.1m separation 
   new G4PVPlacement(0,G4ThreeVector(0.,0.,fDC1_cm*cm),fDCLogical,
                      "DC1Physical",SHMSLogical,
                      false,30,checkOverlaps);
   new G4PVPlacement(0,G4ThreeVector(0.,0.,fDC2_cm*cm),fDCLogical,
                      "DC2Physical",SHMSLogical,
                      false,30,checkOverlaps);

  // S1 Hodoscopes --------------------------------------------------------
  auto S1Solid 
    = new G4Box("S1Box",50.*cm,50.*cm,0.25*cm); //paddles are .5cm thick
  fS1XLogical
    = new G4LogicalVolume(S1Solid,scintillator,"S1XLogical");
  fS1YLogical
    = new G4LogicalVolume(S1Solid,scintillator,"S1YLogical");
  fS1XLogical->SetVisAttributes(red);
  fS1YLogical->SetVisAttributes(red);
  fS1XLogical->SetUserLimits(hodoStepLimit);
  fS1YLogical->SetUserLimits(hodoStepLimit);

  //place both S1X and S1Y
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fS1X_cm*cm),fS1XLogical,
                     "S1Physical",SHMSLogical,
                      false,1,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fS1Y_cm*cm),fS1YLogical,
                     "S1Physical",SHMSLogical,
                      false,2,checkOverlaps);
  
  // Heavy Gas Cerenkov -------------------------------------------------
  auto HGCSolid
    = new G4Box("HGCBox",1.0*m,1.0*m,52.22*cm);
  auto fHGCLogical
    = new G4LogicalVolume(HGCSolid,heavyGas,"HGCLogical");
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fHGC_cm*cm),fHGCLogical,
                    "HGCPhysical",SHMSLogical,
                    false,40,checkOverlaps);
  //entrance and exit windows
  auto HGCWindowSolid = new G4Box("HGCWindowBox",1.0*m,1.0*m,0.05*cm);
  auto fHGCWindowLogical = new G4LogicalVolume(HGCWindowSolid,aluminum,"HGCWindowLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,-52.17*cm),fHGCWindowLogical,"HGCEntrancePhys",fHGCLogical,false,41,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0,0,52.17*cm),fHGCWindowLogical,"HGCExitPhys",fHGCLogical,false,43,checkOverlaps);
  //mirror
  auto HGCMirrorSolid = new G4Box("HGCMirrorBox",1.0*m,1.0*m,0.15*cm);
  auto fHGCMirrorLogical = new G4LogicalVolume(HGCMirrorSolid,sio2,"HGCMirrorLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,45*cm),fHGCMirrorLogical,"HGCMirrorPhys",fHGCLogical,false,42,checkOverlaps);
  fHGCLogical->SetVisAttributes(purple);

  // Aerogel Cerenkov -----------------------------------------------------
  auto AGCSolid
    = new G4Box("AGCBox",1.0*m,1.0*m,13.195*cm); //detector including air gap
  auto fAGCLogical
    = new G4LogicalVolume(AGCSolid,air,"AGCLogical");
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fAGC_cm*cm),fAGCLogical,
                    "AGCPhys",SHMSLogical,
                    false,50,checkOverlaps);
  auto AGCTraySolid = new G4Box("AGCTrayBox",1.0*m,1.0*m,4.5*cm);
  auto fAGCTrayLogical = new G4LogicalVolume(AGCTraySolid,aerogel,"AGCTrayLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,-8.63*cm),fAGCTrayLogical,
		  "AGCTrayPhys",fAGCLogical, 
		  false,52,checkOverlaps);
  auto AGCEntranceSolid = new G4Box("AGCEntranceBox",1.0*m,1.0*m,0.065*cm);
  auto fAGCEntranceLogical = new G4LogicalVolume(AGCEntranceSolid,aluminum,"AGCEntranceLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,-13.13*cm),fAGCEntranceLogical,
		  "AGCEntrancePhys",fAGCLogical,
		  false,51,checkOverlaps);
  auto AGCExitSolid = new G4Box("AGCExitBox",1.0*m,1.0*m,0.08*cm);
  auto fAGCExitLogical = new G4LogicalVolume(AGCExitSolid,aluminum,"AGCExitLogical");
  new G4PVPlacement(0,G4ThreeVector(0,0,13.115*cm),fAGCExitLogical,
		  "AGCExitPhys",fAGCLogical,
		  false,53,checkOverlaps);
 fAGCTrayLogical->SetVisAttributes(blue);



  // S2 hodoscopes --------------------------------------------------------
  auto S2XSolid 
    = new G4Box("S2XBox",55.*cm,70.*cm,0.25*cm); //seems to be 110 x 140
  auto S2YSolid 
    = new G4Box("S2YBox",55.*cm,70.*cm,1.25*cm); //seems to be 110 x 140
  fS2XLogical
    = new G4LogicalVolume(S2XSolid,scintillator,"S2XLogical");
  fS2YLogical
    = new G4LogicalVolume(S2YSolid,quartz,"S2YLogical");
  fS2XLogical->SetVisAttributes(red);
  fS2YLogical->SetVisAttributes(red);
  fS2XLogical->SetUserLimits(hodoStepLimit);
  fS2YLogical->SetUserLimits(hodoStepLimit);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fS2X_cm*cm),fS2XLogical,
                     "S2XPhysical",SHMSLogical,
                     false,3,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fS2Y_cm*cm),fS2YLogical,
                     "S2YPhysical",SHMSLogical,
                     false,4,checkOverlaps);
  
  
  // Calorimeter ---------------------------------------------------------
  auto CalSolid
    = new G4Box("CalBox",1.0*m,1.0*m,1.0*m);
  fCalLogical
    = new G4LogicalVolume(CalSolid,leadglass,"CalLogical");
  fCalLogical->SetVisAttributes(yellow);
  new G4PVPlacement(0,G4ThreeVector(0.,0.,fCal_cm*cm),fCalLogical,
                    "CalPhysical",SHMSLogical,
                    false,0,checkOverlaps);
  
  
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
        char* keylist[] = { (char*) "Use-NGC:", (char*) "Aero-Tray:", NULL};
        enum {ENGC, EAERO, ENULL};
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
        case EAERO: //dimensions of main target vessel
            iread = sscanf(line,"%*s%d", //line contains text, one integer
                           &fAeroTray); //assign to this variable
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
  auto nistManager = G4NistManager::Instance();

  //pre-defined materials:
  // Air 
  nistManager->FindOrBuildMaterial("G4_AIR");
  // Vacuum "Galactic"
  nistManager->FindOrBuildMaterial("G4_Galactic");
  // Aluminum
  nistManager->FindOrBuildMaterial("G4_Al");
  // Tungsten
  nistManager->FindOrBuildMaterial("G4_W");
  // Copper
  nistManager->FindOrBuildMaterial("G4_Cu");
  // Kapton
  nistManager->FindOrBuildMaterial("G4_KAPTON");
  // LH2
  // PVT scintillator
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); //PVT
  // Tedlar
  nistManager->FindOrBuildMaterial("G4_POLYVINYLIDENE_FLUORIDE"); //tedlar
  // SiO2
  nistManager->BuildMaterialWithNewDensity("SiO2","G4_SILICON_DIOXIDE",2.20*g/cm3);
  // Quartz for S2Y
  G4Material* Quartz = nistManager->BuildMaterialWithNewDensity("Quartz","G4_SILICON_DIOXIDE",2.65*g/cm3);
  // Lead glass material for calorimeter
  nistManager->FindOrBuildMaterial("G4_GLASS_LEAD");



  

  //custom build materials: 
  //first define elements
  G4Element* elH = nistManager->FindOrBuildElement("H");
  G4Element* elO = nistManager->FindOrBuildElement("O");
  G4Element* elN = nistManager->FindOrBuildElement("N");
  G4Element* elC = nistManager->FindOrBuildElement("C");
  G4Element* elF = nistManager->FindOrBuildElement("F");
  G4Element* elAr = nistManager->FindOrBuildElement("Ar");
  G4Element* elNe = nistManager->FindOrBuildElement("Ne");
  G4Material* ethane = nistManager->FindOrBuildMaterial("G4_ETHANE");
  // Mylar
  G4Material* Mylar = new G4Material("Mylar",1.39*g/cm3,3);
  Mylar->AddElement(elO,2);
  Mylar->AddElement(elC,5);
  Mylar->AddElement(elH,4);
  //NEED NEW DENSITY: HGC and HNG gases
  G4Material* HeavyGas = new G4Material("C4F10",0.0112*g/cm3,2);
  HeavyGas->AddElement(elC,4);
  HeavyGas->AddElement(elF,10);
  G4Material* NobleGas = new G4Material("NobleGas",0.0015*g/cm3,2);
  NobleGas->AddElement(elAr,70.*perCent);
  NobleGas->AddElement(elNe,30.*perCent);
  //Rohacell
  G4Material* RohaCell = new G4Material("RohaCell",0.110*g/cm3,4);
  RohaCell->AddElement(elC,4);
  RohaCell->AddElement(elH,7);
  RohaCell->AddElement(elN,1);
  RohaCell->AddElement(elO,1);
  //Drift chamber gas
  G4Material* DriftGas = new G4Material("DriftGas",0.00154*g/cm3,2);
  DriftGas->AddElement(elAr,50.*perCent);
  DriftGas->AddMaterial(ethane,50.*perCent);
  //Liquid hydrogen
  G4Material* lH2 = new G4Material("LH2",0.0723*g/cm3,1,kStateLiquid,19*kelvin); //density of liquid h2 at cryo temp
  lH2->AddElement(elH,2);

  //manual definition of refractive index as function of light energy  
  G4double energy[4] = {0.77*eV,1.03*eV,1.55*eV,2.07*eV}; 
  G4double energy2[4] = {0.77*eV,1.55*eV,3.1*eV,6.9*eV}; //1600,800,400,180; 
    //corresponds to wavelengths of 1600, 1200, 800, 600
    //refractive indices:
    G4double nHGC[4] = {1.0014,1.0014,1.0014,1.0015};
    G4double nNGC[4] = {1.0001,1.0001,1.0001,1.0001};
    G4double nQuartz[4] = {1.457,1.453,1.447,1.443};//quartz defined from
    //https://www.researchgate.net/figure/Refractive-indices-of-quartz-glass-calculated-by-the-Sellmeier-approximation-with-an_fig12_345357798

    //change AGC based on which tray is used
    G4double nAGC[4] = {1.030, 1.030, 1.030, 1.030};
    G4double aeroDensity = 0.143;
    if (fAeroTray == 11){
	for (int i=0; i<4; i++){ nAGC[i]=1.011; }
	aeroDensity = 0.052;
    } else if (fAeroTray == 15){
	for (int i=0; i<4; i++){ nAGC[i]=1.015; }
	aeroDensity = 0.071;
    } else if (fAeroTray == 20){
	for (int i=0; i<4; i++){ nAGC[i]=1.020; }
	aeroDensity = 0.095;
    } else if (fAeroTray == 30){
	//no change, continue
    } else {
	G4cout<<"Invalid Aero-Tray option. Defaulting to n=1.30"<<G4endl;
    }
   // Silicon Aerogel
   G4Material* aero = nistManager->BuildMaterialWithNewDensity("SiAerogel","G4_SILICON_DIOXIDE",aeroDensity*g/cm3);
    
    //assign to relevant materials via material properties table class
    G4MaterialPropertiesTable *mptAGC = new G4MaterialPropertiesTable();
    mptAGC->AddProperty("RINDEX", energy, nAGC, 4);
    aero->SetMaterialPropertiesTable(mptAGC);
    
    G4MaterialPropertiesTable *mptHGC = new G4MaterialPropertiesTable();
    mptHGC->AddProperty("RINDEX", energy2, nHGC, 4);
    HeavyGas->SetMaterialPropertiesTable(mptHGC);
    
    G4MaterialPropertiesTable *mptNGC = new G4MaterialPropertiesTable();
    mptNGC->AddProperty("RINDEX", energy, nNGC, 4);
    NobleGas->SetMaterialPropertiesTable(mptNGC);

    G4MaterialPropertiesTable *mptQuartz = new G4MaterialPropertiesTable();
    mptQuartz->AddProperty("RINDEX", energy, nQuartz, 4);
    Quartz->SetMaterialPropertiesTable(mptQuartz);

  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

