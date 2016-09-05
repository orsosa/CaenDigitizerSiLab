#include "TROOT.h"
#include "TNtuple.h"
#include "TH1F.h"
#include "TFile.h"

int get_charge(char* name="data_from_digitizer.root"){
  cout<<"updating: "<<name<<endl;

  TFile *f = new TFile(name,"update");
  TNtuple * t =(TNtuple *)f->Get("data");
  char varList[100]="q0:q1:q2:q3:q4:q5:q6:q7:ev";
  TNtuple *tq = new TNtuple("tq","charge (a.u.), min (a.u.)",varList);
  const int NCh=8;
  Float_t time,c[8],evt,evt_prev,min,q[NCh],dt,hl,ll;
  Float_t dataArr[9];
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
  min=100;
  for (int k=0;k<NCh;k++)
    q[k]=0;
  dt = time;
  t->GetEntry(1);
  dt =time-dt;
  for (int i=0;i<Ne;i++)
  {
    t->GetEntry(i);
    for (int k=0;k<NCh;k++)
      if (ll<time&&time<hl) q[k]+=((float)((1<<14)-1) -c[k])*2.0/((float)(1<<14))/50*200;
    //    if(min[i]>c1) min[i] = c1;
    if (evt != evt_prev)

    {
      tq->Fill(q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],evt_prev);
      evt_prev = evt;
      for (int k=0;k<NCh;k++)
	 q[k]=0;
      min=100;
    }
    
  }
  tq->Write("",TObject::kOverwrite);
  f->Close();
  return 0;
}
