#include <iostream>
#include "TROOT.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TH1F.h"
#include "TFile.h"

int get_charge_minfix(char* name="data_from_digitizer.root"){
  cout<<"updating: "<<name<<endl;

  TFile *f = new TFile(name,"update");
  TNtuple * t =(TNtuple *)f->Get("tdm");
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
  TNtuple *tq = new TNtuple("tqmf","charge (pC), min (a.u.) using min fixed in time",varList.Data()); 
  
  Float_t time,c[NCh],c1[NCh],evt,evt_prev,min[NCh],tmin[NCh],q[NCh],dt,hl,ll,llp,hlp,ped[NCh],count[NCh],countp[NCh],amp[NCh][NSamples];
  Float_t dataArr[2*NCh+1],dataTime[3*NCh+2];
  // gate definition.
  ll = 10;// low time edge
  hl = 100;// high time edge
  hlp= 5;
  llp= 0;
  t->SetBranchAddress("time",&time);
  for (int k=0;k<NCh;k++)
  {
    t->SetBranchAddress(Form("Ch%d",k),&c[k]);
    t->SetBranchAddress(Form("tmin%d",k),&tmin[k]);
    t->SetBranchAddress(Form("min%d",k),&min[k]);
  }

  t->SetBranchAddress("event",&evt);
  Long_t Ne=t->GetEntries();
  //Long_t Ne =1000;
  t->GetEntry(0);
  evt_prev=evt;

  for (int k=0;k<NCh;k++)
  {
    q[k]=0;
    ped[k]=0;
    count[k]=0;
    countp[k]=0;
  }  //  dt = time;
  //  t->GetEntry(1);
  //  dt =time-dt;
  for (int i=0;i<Ne;i++)
  {
    t->GetEntry(i);
    if (evt != evt_prev)
    {
      for (int k=0;k<NCh;k++)
      {
	dataArr[k]=-((q[k]-ped[k]/countp[k]*count[k])* 2*(hl-ll)/50.)/1000.;
	dataArr[k+NCh]=min[k];
      }
      dataArr[2*NCh]=evt_prev;
      tq->Fill(dataArr);

      //reset values.
      for (int k=0;k<NCh;k++)
      {
	q[k]=0;
	count[k]=0;
	countp[k]=0;
	ped[k]=0;
      }
      evt_prev = evt;
      ///////////
    }

    for (int k=0;k<NCh;k++)
    {
      if (ll<(time)&&(time)<hl)
      {
	//	q[k]+=((float)((1<<14)-1) -c[k])*2.0/((float)(1<<14))/50*200;
	count[k]++;
	q[k]+=c[k];
      }
      //      if (llp<(time-tmin[k])&&(time-tmin[k])<hlp
      if (llp<time&&time<hlp)
      {
	//	q[k]+=((float)((1<<14)-1) -c[k])*2.0/((float)(1<<14))/50*200;
	countp[k]++;
	ped[k]+=c[k];
      }
    }

    std::cout<<100.*(i+1.)/float(Ne)<<"\r";
    std::cout.flush();
  }
  tq->Write("",TObject::kOverwrite);
  f->Close();
  return 0;
}
