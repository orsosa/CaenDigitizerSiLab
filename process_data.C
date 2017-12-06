#include <iostream>
#include "TROOT.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TH1F.h"
#include "TFile.h"

int process_data(char* name="data_from_digitizer.root"){
  cout<<"updating: "<<name<<endl;

  TFile *f = new TFile(name,"read");
  //  TFile *f = new TFile(name,"update");
  TNtuple * t =(TNtuple *)f->Get("data");
  //char varList[100];
  TString varList="q0:q1:q2:q3:q4:q5:q6:q7:min0:min1:min2:min3:min4:min5:min6:min7:max0:max1:max2:max3:max4:max5:max6:max7:tmin0:tmin1:tmin2:tmin3:tmin4:tmin5:tmin6:tmin7:ped0:ped1:ped2:ped3:ped4:ped5:ped6:ped7:ev:adc2v:dc_offset:dt";
  const int NCh=8;
  //  const int NSamples=100;

  TFile *fout= new TFile(Form("%s.out.root",name),"recreate");
  TTree * tout = new TTree("data","data processed. time (N sample (dt=2ns)), Ch0... (V), q0... (nC)");

  t->SetMaxEntryLoop(1e4);
  Int_t Nmeas= t->Draw("Ch1","event==0");
  t->SetMaxEntryLoop();


  Float_t **Ch = new Float_t*[NCh];
  for (int k=0;k<NCh;k++)
  {
    Ch[k] = new Float_t[Nmeas];
    tout->Branch(Form("Ch%d",k),Ch[k],Form("Ch%d[%d]/F",k,Nmeas));   
  }

  Float_t *TIME = new Float_t[Nmeas];
  Float_t  *data = new Float_t[varList.CountChar(':')+1];
  
  tout->Branch("time",TIME,Form("time[%d]/F",Nmeas));
  tout->Branch("measured",data,varList);

  Float_t time,c[NCh],evt,evt_prev,min[NCh],max[NCh],tmin[NCh],q[NCh],ped[NCh],dt,hl,ll,hlp,llp;

  Float_t adc2v = 2.0/( (1<<14) - 1.0 );
  Float_t dc_offset= 2.0*0x1000/0xffff - 1.0  - 1.0;//second -1 is due to  adc2v
  Float_t DT = 2;// sampling period (ns).


  ///// GATE AND PEDESTAL LIMITS DEFINITION.////
  ll = 10;// low time charge edge
  hl = 75;// high time charge edge
  llp = 0;// low time ped edge
  hlp = 8;// high time ped edge

  //////////////////////////////////////////////

  t->SetBranchAddress("time",&time);
  for (int k=0;k<NCh;k++)
    t->SetBranchAddress(Form("Ch%d",k),&c[k]);

  t->SetBranchAddress("event",&evt);
  Long_t Ne=t->GetEntries();
  //Long_t Ne =1000;
  t->GetEntry(0);
  evt_prev=evt;

  for (int k=0;k<NCh;k++)
  {
    min[k]=1e7;
    max[k]=-1e7;
    tmin[k]=-100;
    q[k]=0;
    ped[k]=0;
  }
  //  dt = time;
  //  t->GetEntry(1);
  //  dt =time-dt;
  for (int i=0;i<Ne;i++)
  {
    t->GetEntry(i);
    if (evt != evt_prev)
    {
      for (int k=0;k<NCh;k++)
      {
	data[k]=-q[k]/50*DT; //nC
	data[k+NCh]=min[k];
	data[k+2*NCh]=max[k];
	data[k+3*NCh]=tmin[k]; 
	data[k+4*NCh]=-ped[k]/50*DT*(hl-ll)/(hlp-llp); //nC

	data[k+4*NCh+1]=evt_prev; 
	data[k+4*NCh+2]=adc2v;
	data[k+4*NCh+3]=dc_offset;
	data[k+4*NCh+4]=DT; 
      }
      
      tout->Fill();
      //reset values.
      for (int k=0;k<NCh;k++)
      {
	q[k]=0;
	ped[k]=0;
	min[k]=1e7;
	tmin[k] = -100;
	max[k]=-1e7;
      }
      evt_prev = evt;
      ///////////
    }
    TIME[i%Nmeas]=time;
    for (int k=0;k<NCh;k++)
    {
      Ch[k][i%Nmeas]=c[k];

      if (ll<time&&time<hl)
      {
	q[k]+=c[k]*adc2v + dc_offset;
      }
      if (llp<time&&time<hlp)
      {
	ped[k]+=c[k]*adc2v + dc_offset;
      }
      if(min[k]>c[k]) 
      {
	min[k] = c[k];
	tmin[k] = time;
      }
      if(max[k]<c[k]) 
      {
	max[k] = c[k];
      }
    }

    std::cout<<100.*(i+1.)/float(Ne)<<"\r";
    std::cout.flush();
  }
  fout->cd();
  tout->Write("",TObject::kOverwrite);
  fout->Close();
  f->Close();
  return 0;
}
