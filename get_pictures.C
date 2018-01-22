int get_pictures(){
  TFile *file = new TFile("data_from_digitizer.root");
  TNtuple *tq = (TNtuple*)file->Get("tq");
  TCanvas *c = new TCanvas("c","c",800,600);
  for(int k=0;k<8;k++) {
    tq->Draw(Form("q%d>>h(1000,1440e3,1580e3)",k));
    c->SaveAs(Form("pictures/histCh%d.gif",k));
    c->SaveAs(Form("pictures/histCh%d.C",k));
  }
  return 0;
}
