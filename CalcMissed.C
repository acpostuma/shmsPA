//Calculation of missed triggers correction
//ACP 2025
//Takes name of file to analyze and central momentum as arguments
//Should also change the analysis cuts: jump to section marked by -----------ooooooOOOOOooooo---------

void CalcMissed(TString filename = "PAOutData.root", Double_t P = 5){
	//import file, tree
	TFile *f1 = new TFile(filename);
	TTree *t1 = (TTree*)f1->Get("PA");
	//define variables and assign to tree
	Double_t s1x_time, s1x_energy;
	Double_t s1y_time, s1y_energy;
	Double_t s2x_time, s2x_energy;
	Double_t s2y_time, s2y_energy;
	Int_t NPE, copyNo;
	Int_t AGC_NPE, NGC_NPE, HGC_NPE;
	Double_t cal_energy;
	t1->SetBranchAddress("S1XEnergy",&s1x_energy);
	t1->SetBranchAddress("S1YEnergy",&s1y_energy);
	t1->SetBranchAddress("S2XEnergy",&s2x_energy);
	t1->SetBranchAddress("S2YEnergy",&s2y_energy);
	t1->SetBranchAddress("S1XTime",&s1x_time);
	t1->SetBranchAddress("S1YTime",&s1y_time);
	t1->SetBranchAddress("S2XTime",&s2x_time);
	t1->SetBranchAddress("S2YTime",&s2y_time);
	t1->SetBranchAddress("S2YNPE",&NPE);
	t1->SetBranchAddress("CopyNo",&copyNo);
	t1->SetBranchAddress("AGCNPE",&AGC_NPE);
	t1->SetBranchAddress("HGCNPE",&HGC_NPE);
	t1->SetBranchAddress("NGCNPE",&NGC_NPE);
	t1->SetBranchAddress("CalEnergy",&cal_energy);
	TH1I *h1 = new TH1I("h1","copyNo",60,0,60);
	//other things needed:
	Double_t dt1, dt2, dt3; //time differences
	Double_t energy_threshold = 0.5; //MeV
	Double_t time_window = 20; //ns
	Int_t npe_threshold = 100; //unitless
	Int_t nEvents = t1->GetEntries();
	//initialize counters to zero
	Int_t nMissed = 0; //missed triggers
	Int_t nTrig; //fired triggers per event
	Int_t nProton=0;
        Int_t nPion=0;
        Int_t nPositron=0;
        Int_t nKaon =0;
        Int_t nContam =0;
	Int_t nStopped = 0;
	P=P*1000; //convert to MeV
	//loop through tree to simulate 3/4 trigger
	for (int i=0; i<nEvents; i++){
		t1->GetEntry(i);
		//count raw missed triggers
		nTrig=0;
		if (s1x_energy > energy_threshold) nTrig++;
		dt1 = s1y_time - s1x_time;
                dt2 = s2x_time - s1x_time;
                dt3 = s2y_time - s1x_time;
		if (s1y_energy > energy_threshold && abs(dt1)<time_window)nTrig++;
		if (s2x_energy > energy_threshold && abs(dt2)<time_window)nTrig++;
		if (NPE > npe_threshold && abs(dt3)<time_window)nTrig++;
		if (nTrig<3)nMissed++;
		//count stopped particles
		if (copyNo!=0 && copyNo!=4)nStopped++;
		//determine signals of different particles
		if (nTrig>2){
		//USER EDITS: change cuts in if statements
		//--------ooooooOOOOOOooooo---------ooooOOOOOooooo---------ooooOOOOOooooo--------
		//proton: no signal in HGC, AGC, hadron in Cal
			if (AGC_NPE<20 && HGC_NPE <50)nProton++;
		//kaon: signal in AGC, hadron in Cal
			else if (AGC_NPE>20 && HGC_NPE<50)nKaon++;
		//electron: signals in all Cherenkovs, lepton in Cal
			else if (AGC_NPE>20 && HGC_NPE>50 && NGC_NPE>5 && cal_energy/P > 0.7)nPositron++;
		//pion: signals in HGC, AGC, hadron in Cal
			else if (AGC_NPE>20 && HGC_NPE>50)nPion++;
		//conflicting info
			else {
				//can't be classified: assume secondary midway through detectors
				nContam++; //contaminated track
			}
		//--------ooooooOOOOOOooooo---------ooooOOOOOooooo---------ooooOOOOOooooo--------

		}
	}
	
	//print results
	cout<<endl;
	cout<<"Total events: "<<nEvents<<endl;
	cout<<"Missed 3/4 triggers: "<<nMissed<<" or ";
	cout<<(double)nMissed*100/nEvents<<"%"<<endl;
	cout<<"Total stopped tracks: "<<nStopped<<" or "<<(double)nStopped*100/nEvents<<"%"<<endl;
	cout<<Form("3/4 was triggered by: %d p+, %d pi+, %d K+, %d positrons, and %d contaminated tracks",nProton,nPion,nKaon,nPositron,nContam)<<endl;
	cout<<endl;
	cout<<"Absorption correction (by incident particle type):"<<endl;
	cout<<"p+:  "<<(nEvents-nProton)*100/(double)nEvents<<"%"<<endl;
	cout<<"K+:  "<<(nEvents-nKaon)*100/(double)nEvents<<"%"<<endl;
	cout<<"pi+: "<<(nEvents-nPion)*100/(double)nEvents<<"%"<<endl;
	cout<<"e+:  "<<(nEvents-nPositron)*100/(double)nEvents<<"%"<<endl;
}


