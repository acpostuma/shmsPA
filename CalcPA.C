void CalcPA(TString filename = "PAOutData.root"){
	TFile *f1 = new TFile(filename);
	TTree *t1 = (TTree*)f1->Get("PA");
	//define variables and assign to tree
	Double_t s1x_time, s1x_energy;
	Double_t s1y_time, s1y_energy;
	Double_t s2x_time, s2x_energy;
	Double_t s2y_time, s2y_energy;
	Int_t NPE, copyNo;
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
	TH1I *h1 = new TH1I("h1","copyNo",60,0,60);
	//other things needed:
	Double_t dt1, dt2, dt3; //time differences
	Double_t energy_threshold = 0.5; //MeV
	Double_t time_window = 20; //ns
	Int_t npe_threshold = 100; //unitless
	Int_t nEvents = t1->GetEntries();
	Int_t nMissed = 0; //missed triggers
	Int_t nTrig; //fired triggers per event
	Int_t nnS1X =0;
        Int_t nnS1Y=0;
        Int_t nnS2X =0;
	//loop through tree to simulate 3/4 trigger
	for (int i=0; i<nEvents; i++){
		t1->GetEntry(i);
		nTrig=0;
		if (s1x_energy > energy_threshold) nTrig++;
		dt1 = s1y_time - s1x_time;
                dt2 = s2x_time - s1x_time;
                dt3 = s2y_time - s1x_time;
		if (s1y_energy > energy_threshold && abs(dt1)<time_window)nTrig++;
		if (s2x_energy > energy_threshold && abs(dt2)<time_window)nTrig++;
		if (NPE > npe_threshold && abs(dt3)<time_window)nTrig++;
		if (nTrig<3)nMissed++;
	}
	//diagnostic plots
	TCanvas *c4 = new TCanvas("PA diagnostics");
	c4->Divide(3,2);
	c4->cd(1);
	t1->Draw("S1XEnergy>>h2(100,0,2)");
	c4->cd(2);
	t1->Draw("S1YEnergy>>h3(100,0,2)");
	c4->cd(3);
	t1->Draw("S2XEnergy>>h4(100,0,2)");
	c4->cd(4);
	t1->Draw("S2YNPE>>h5(100,0,1000)");
	c4->cd(5);
	t1->Draw("S2YEnergy>>h6(100,0,4)");
	c4->cd(6);
	t1->Draw("S2YEnergy:S2YNPE","","COLZ");
	c4->Print("PAplots.pdf"); //print
	
	//print results
	cout<<"Total events: "<<nEvents<<endl;
	cout<<"Missed 3/4 triggers: "<<nMissed<<" or ";
	//turn counters into doubles
	Double_t x = 0;
	x+=nMissed;
	Double_t y = 0;
	y+=nEvents;
	Double_t abs = x*100/y; //calculate as percentage
	cout<<abs<<"%"<<endl;
}	
