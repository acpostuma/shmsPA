void CalcPA(TString filename = "PAOutData.root"){
	TFile *f1 = new TFile(filename);
	TTree *t1 = (TTree*)f1->Get("PA");
	//define variables and assign to tree
	Double_t s1x_time, s1x_energy;
	Double_t s1y_time, s1y_energy;
	Double_t s2x_time, s2x_energy;
	Double_t s2y_time, s2y_energy;
	t1->SetBranchAddress("S1XEnergy",&s1x_energy);
	t1->SetBranchAddress("S1YEnergy",&s1y_energy);
	t1->SetBranchAddress("S2XEnergy",&s2x_energy);
	t1->SetBranchAddress("S2YEnergy",&s2y_energy);
	t1->SetBranchAddress("S1XTime",&s1x_time);
	t1->SetBranchAddress("S1YTime",&s1y_time);
	t1->SetBranchAddress("S2XTime",&s2x_time);
	t1->SetBranchAddress("S2YTime",&s2y_time);
	//other things needed:
	Double_t dt1, dt2, dt3; //time differences
	Double_t energy_threshold = 0.2; //not sure about units
	Double_t time_window = 20; //ns I presume, not sure
	Int_t nEvents = t1->GetEntries();
	Int_t nMissed = 0; //missed triggers
	Int_t nTrig; //fired triggers per event
	//loop through tree to simulate 3/4 trigger
	for (int i=0; i<nEvents; i++){
		t1->GetEntry(i);
		nTrig=0;
		if (s1x_energy > energy_threshold){ //if s1x fires
			nTrig++;
			//calculate timing differences wrt s1x
			dt1 = s1y_time - s1x_time;
			dt2 = s2x_time - s1x_time;
			dt3 = s2y_time - s1x_time;
			//see if other three fire
			if (s1y_energy > energy_threshold && abs(dt1)<time_window)nTrig++;
			if (s2x_energy > energy_threshold && abs(dt2)<time_window)nTrig++;
			if (s2y_energy > energy_threshold && abs(dt3)<time_window)nTrig++;
			if (nTrig<3) nMissed++; //missed 3/4 trigger
		} else if (s1y_energy > energy_threshold){ //s1x does not fire, but s1y does
			nTrig++;
			//calc timing differences wrt s1y
			dt1 = s2x_time - s1y_time;
			dt2 = s2y_time - s1y_time;
			//see if other two fire
			if (s2x_energy > energy_threshold && abs(dt1)<time_window)nTrig++;
			if (s2y_energy > energy_threshold && abs(dt2)<time_window)nTrig++;
			if (nTrig<3) nMissed++; //missed 3/4 tigger
		} else { //neither s1x nor s1y fire
			nMissed++; //missed 3/4 trigger
		}
	}
	//diagnostic plots
	TCanvas *c4 = new TCanvas("PA diagnostics");
	c4->Divide(2,2);
	c4->cd(1);
	t1->Draw("S1XEnergy");
	c4->cd(2);
	t1->Draw("S1YEnergy");
	c4->cd(3);
	t1->Draw("S2XEnergy");
	c4->cd(4);
	t1->Draw("S2YEnergy");
	c4->Print("PAplots.pdf");
	
	//print results
	cout<<"Total events: "<<nEvents<<endl;
	cout<<"Missed triggers: "<<nMissed<<endl;
	Double_t abs = nMissed*100/nEvents;
	cout<<Form("Absorption: %4.2f percent",abs)<<endl;
}	
