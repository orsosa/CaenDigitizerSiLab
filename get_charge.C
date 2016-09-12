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
  branches.Append(Form("Ch%d:min%d",0));
  for (int32_t k =1;k<NCh;k++)
  {
    branches.Append(":");    
    branches.Append(Form("Ch%d:min%d",k));
  }
  branches.Append(":time:event");
  TNtuple * tdm =new TNtuple("tdm","raw data with min",branches.Data());
  
  Float_t time,c[NCh],c1[NCh],evt,evt_prev,min[NCh],q[NCh],dt,hl,ll,amp[NCh][NSamples];
  Float_t dataArr[2*NCh+1],dataTime[2*NCh+2];
  // gate definition.
  ll = 0;// low time edge
  hl = 100;// high time edge
  t->SetBranchAddress("time",&time);
  for (int k=0;k<NCh;k++)
    t->SetBranchAddress(Form("Ch%d",k),&c[k]);

  t->SetBranchAddress("event",&evt);
  Long_t Ne=t->GetEntries();
  t->GetEntry(0);
  evt_prev=evt;

  for (int k=0;k<NCh;k++)
  {
    min[k]=1e7;
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
      evt_prev = evt;
      for (int k=0;k<NCh;k++)
      {
	q[k]=0;
	min[k]=1e7;
      }
      for (int k=0;k<NSamples;k++)
      {
	for (int ch=0;ch<NCh;ch++)
	{
	  dataTime[2*ch]=amp[ch][k];
	  dataTime[2*ch+1]=min[ch];
	}
	dataTime[2*NCh]=k;
	dataTime[2*NCh+1]=event;
	tdm->Fill(dataTime);
      }
    }

    for (int k=0;k<NCh;k++)
    {
      amp[k][(int)time]=c[k];
      if (ll<time&&time<hl)
      {
	//	q[k]+=((float)((1<<14)-1) -c[k])*2.0/((float)(1<<14))/50*200;
	q[k]+=c[k];
      }
      if(min[k]>c[k]) min[k] = c[k];
    }

    
  }
  tq->Write("",TObject::kOverwrite);
  tdm->Write("",TObject::kOverwrite);
  f->Close();
  return 0;
}
