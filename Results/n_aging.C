#include <TH2D.h>
int n_aging(){
const int nPMT=44;
Double_t time[8]={42.5,767,2197,7164,7884,8604,9324,10044};//read the note file
Double_t par[44][8];
TH2D *rate[nPMT];
TH2D *gain[nPMT];
TH2D *res[nPMT];
for(int np=0;np<nPMT;np++){
  char rc[50];
  char gc[50];
  char sc[50];
  sprintf(rc,"PMT%d_rate",np+1);
  rate[np]=new TH2D(rc,rc,200,0,200,300,0,100);
  sprintf(gc,"PMT%d_gain",np+1);
  gain[np]=new TH2D(gc,gc,200,0,200,300,50,450);
  sprintf(sc,"PMT%d_res",np+1);
  res[np]=new TH2D(sc,sc,200,0,200,200,0,100);
  }
ifstream inname;
ifstream indata;
inname.open("filelist");
for(int n=0;n<8;n++){
  char filename[80];
  memset(filename,0,sizeof(filename));
  inname.getline(filename,80);
  indata.open(filename);
  for(int p=0;p<44;p++){
    for(int j=0;j<8;j++){
      indata>>par[p][j];
      }
    }
  indata.close();
  for(int np=0;np<nPMT;np++){
    rate[np]->Fill(time[n]/60,par[np][0]);
    gain[np]->Fill(time[n]/60,par[np][6]-par[np][1]);
    res[np]->Fill(time[n]/60,par[np][7]/(par[np][6]-par[np][1])*100);
    }
  } 
gStyle->SetOptStat(000);
TCanvas *Rate=new TCanvas("Rate","Rate");
TLegend *rl=new TLegend(0.3,0.2,0.6,0.8);
rl->SetNColumns(2);
rl->SetTextFont(72);
rl->SetHeader("rate");
rl->SetTextSize(0.03);
rl->SetFillColor(0);
  for(int np=0;np<nPMT;np++){
  rate[np]->SetMarkerSize(0.8);
  rate[np]->SetMarkerStyle(1+np);
  rate[np]->SetMarkerColor(np+1);
  rate[np]->Draw("same");
  char pm[20];
  memset(pm,0,sizeof(pm));
  sprintf(pm,"%d",np+1);
  rl->AddEntry(rate[np],pm,"p");
  } 
rl->Draw();

TCanvas *Gain=new TCanvas("Gain","Gain");
TLegend *gl=new TLegend(0.3,0.2,0.6,0.8);
gl->SetNColumns(2);
gl->SetTextFont(72);
gl->SetHeader("gain");
gl->SetTextSize(0.03);
gl->SetFillColor(0);
  for(int np=0;np<nPMT;np++){
  gain[np]->SetMarkerSize(0.8);
  gain[np]->SetMarkerStyle(1+np);
  gain[np]->SetMarkerColor(np+1);
  gain[np]->Draw("same");
  char pm[20];
  memset(pm,0,sizeof(pm));
  sprintf(pm,"%d",np+1);
  gl->AddEntry(rate[np],pm,"p");
  } 
gl->Draw();

TCanvas *Res=new TCanvas("Res","Res");
TLegend *sl=new TLegend(0.3,0.2,0.6,0.8);
sl->SetNColumns(2);
sl->SetTextFont(72);
sl->SetHeader("resolution");
sl->SetTextSize(0.03);
sl->SetFillColor(0);
  for(int np=0;np<nPMT;np++){ 
  res[np]->SetMarkerSize(0.8);
  res[np]->SetMarkerStyle(1+np);
  res[np]->SetMarkerColor(np+1);
  res[np]->Draw("same");
  char pm[20];
  memset(pm,0,sizeof(pm));
  sprintf(pm,"%d",np+1);
  sl->AddEntry(rate[np],pm,"p");
  }
sl->Draw();
return 222;
}
