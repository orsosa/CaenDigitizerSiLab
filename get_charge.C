#include <iostream>
#include "TROOT.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TH1F.h"
#include "TFile.h"

int get_charge(char* name="data_from_digitizer.root"){
  cout<<"updating: "<<name<<endl;

  TFile *f = new TFile(name,"update");
  TNtuple * t =(TNtuple *)f->Get("data");
  //char varList[100];
  TString varList;
  const int NCh=8;
  const int NSamples=100;
  varList.Append("q0");
  for (int k=1; k<NCh; k++)
  {
    varList.Append(Form(":q%d",k));
  }
  for (int k=0; k<NCh; k++)
  {
    varList.Append(Form(":min%d",k));
  }
  varList.Append(":ev");
  TNtuple *tq = new TNtuple("tq","charge (a.u.), min (a.u.)",varList.Data()); 
  TString branches;
  branches.Append(Form("Ch%d:min%d:tmin%d",0,0,0));
  for (int k =1;k<NCh;k++)
  {
    branches.Append(":");    
    branches.Append(Form("Ch%d:min%d:tmin%d",k,k,k));
  }
  branches.Append(":time:event:adc2v:dc_offset");
  std::cout<<"branches: "<<branches.Data()<<std::endl;
  TNtuple * tdm =new TNtuple("tdm","raw data with min and adc2V gain",branches.Data());
  
  Float_t time,c[NCh],c1[NCh],evt,evt_prev,min[NCh],tmin[NCh],q[NCh],dt,hl,ll,amp[NCh][NSamples];
  Float_t dataArr[2*NCh+1],dataTime[3*NCh+3];
  Float_t adc2v = 2.0/( (1<<14) - 1.0 );
  Float_t dc_offset= 2.0*0x1000/0xffff - 1.0  - 1.0;//second -1 is due to  adc2v

  // gate definition.
  ll = 15;// low time edge
  hl = 65;// high time edge
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
    tmin[k]=-1;
    q[k]=0;
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
	dataArr[k]=q[k];
	dataArr[k+NCh]=min[k];
      }
      dataArr[2*NCh]=evt_prev;
      tq->Fill(dataArr);

      for (int k=0;k<NSamples;k++)
      {
	for (int ch=0;ch<NCh;ch++)
	{
	  dataTime[3*ch]=amp[ch][k];
	  dataTime[3*ch+1]=min[ch];
	  dataTime[3*ch+2]=tmin[ch];	  
	  //std::cout<<"min: "<<min[ch]<<std::endl;
	}
	dataTime[3*NCh]=k;
	dataTime[3*NCh+1]=evt_prev;
	dataTime[3*NCh+2]=adc2v;
	dataTime[3*NCh+3]=dc_offset;
	
	tdm->Fill(dataTime);
      }
      //reset values.
      for (int k=0;k<NCh;k++)
      {
	q[k]=0;
	min[k]=1e7;
	tmin[k] = -1;
      }
      evt_prev = evt;
      ///////////
    }

    for (int k=0;k<NCh;k++)
    {
      amp[k][(int)time]=c[k];
      if (ll<time&&time<hl)
      {
	//	q[k]+=((float)((1<<14)-1) -c[k])*2.0/((float)(1<<14))/50*200;
	q[k]+=c[k];
      }
      if(min[k]>c[k]) 
      {
	min[k] = c[k];
	tmin[k] = time;
      }
    }

    std::cout<<100.*(i+1.)/float(Ne)<<"\r";
    std::cout.flush();
  }
  tq->Write("",TObject::kOverwrite);
  tdm->Write("",TObject::kOverwrite);
  f->Close();
  return 0;
}
