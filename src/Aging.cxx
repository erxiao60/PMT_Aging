#include <stdio.h>
#include <iostream>
#include <fstream>

//#ifndef __CINT__
//#define __CINT__
#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TTree.h"
#include "TH2.h"
#include "TProfile.h"
#include "TPostScript.h"
#include "TCanvas.h"
#include "TFile.h"
#include "string.h"
#include "TSpectrum.h"
//using std::cout;
//using std::endl;
using namespace std;
//#endif
 Double_t SinglePE(Double_t *x,Double_t *par){
//par[0]:Pedestal Peak,by PedFit parameters
//par[1]:threshold cut ~50fC = ~60ADC counts
//par[2]:heihgt of noise
//par[3]:delay constant of noise 
//par[4]:height of Single EP guas 
//par[5]:Mean of Single EP guas
//par[6]:Sigma of Single EP 
Double_t y;//for returning
if(x[0]<(par[0]+par[1]))
y=(par[4]*TMath::Exp(-(par[0]+par[1]-par[5])*(par[0]+par[1]-par[5])/2/par[6]/par[6])+par[2]/par[3]*TMath::Exp(-par[1]/par[3]))*TMath::Exp((x[0]-par[0]-par[1]));
//y=(par[4]/par[6]/TMath::Sqrt(2*3.1415926)*TMath::Exp(-(par[0]+par[1]-par[5])*(par[0]+par[1]-par[5])/2/par[6]/par[6])+par[2]/par[3]*TMath::Exp(-par[1]/par[3]))*TMath::Exp((x[0]-par[0]-par[1]));
else{ 
y=par[4]*TMath::Exp(-(x[0]-par[5])*(x[0]-par[5])/2/par[6]/par[6])+par[2]/par[3]*TMath::Exp(-(x[0]-par[0])/par[3]);
//y=par[4]/par[6]/TMath::Sqrt(2*3.1415926)*TMath::Exp(-(x[0]-par[5])*(x[0]-par[5])/2/par[6]/par[6])+par[2]/par[3]*TMath::Exp(-(x[0]-par[0])/par[3]);
}  
return y;
}
//Pedestal Fit
string filename;
void helpinfo(){
	cout<<"Usage is ./main path/filename  for single HV point!"<<endl;
	cout<<"Usage is ./main LA  for Looping all HV points!"<<endl;
	return;
}
void phrase_command(int argc,char *argv[]){
	if (argc<2){ helpinfo();
	     exit(0);
	}else {filename=(string) argv[1]; cout<<"Input File is "<<filename<<endl;}
}

int main(int argc,char *argv[]){
phrase_command(argc,argv);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//read map
ifstream InputM;
InputM.open("./map/map_aging_01");
if(!InputM.good())
  {
  cout<<"can not open map file!!!"<<endl;
  exit(1);
  }
cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
cout<<"~~~~~~~~~~~~map inf:~~~~~~~~~~~~~~"<<endl;
char noteheader[30];
InputM.getline(noteheader,30);
cout<<noteheader<<endl;
int nP;
InputM>>nP;
cout<<nP<<endl;
InputM.getline(noteheader,30);
InputM.getline(noteheader,30);
cout<<noteheader<<endl;
const int nPMT=nP;
int  Chn[nPMT];
int  Position[nPMT];
char PMTsn[nPMT][7];//NA or NB like :NA1433
char blank[20];
for(int ip=0;ip<nPMT;ip++)
  {  
  InputM.get(PMTsn[ip],7);
  InputM>>Position[ip];
  InputM>>Chn[ip];
  InputM.getline(blank,20); 
 }
InputM.close();
for(int ip=0;ip<nPMT;ip++)
//cout.width(10);
cout<<PMTsn[ip]<<"  "<<Position[ip]<<"  "<<Chn[ip]<<"  "<<endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //define output
  //String namesuffix=".root";
  string outputfile=filename;
  outputfile.replace(strlen(filename.c_str())-4,4,"");
  string O_root="./Raw2ROOT/"+outputfile+".root";
  string O_text="./Results/"+outputfile+".txt";
  string O_ped="./Results/"+outputfile+"_ped.txt";
  string O_eps="./Results/"+outputfile+".eps";
  //outputfile.replace(strlen(filename.c_str())-4,4,namesuffix);
  cout<<"Output file name are: "<<"\n"<<O_root<<"\n"<<O_text<<"\n"<<O_eps<<endl;
  TFile *outFile=new TFile(O_root.c_str(),"RECREATE");
  if (outFile==(TFile*) NULL)
  { 
    std::cerr<<"Error:Could not Open Conversion ROOT File!"<<endl;
    exit(1);
  }
  TTree *AgingTree= new TTree("AgingPmts","AgingPmts");  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//define TH1D TH2D
TH1D *signal[nPMT]; 
//TH1D *SigPedCut[nPMT]; 
for (int ib=0;ib<nPMT;ib++)
  {char sT[50];
//   char sC[50];
  sprintf(sT,"%s(No.%d)",PMTsn[ib],Position[ib]); 
  signal[ib]=new TH1D(sT,sT,1500,300,1800);
//  sprintf(sC,"%s(No.%d)_PedCut",PMTsn[ib],Position[ib]); 
//  SigPedCut[ib]=new TH1D(sC,sC,1500,0,1500);
  } 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//read raw data
//const Int_t channel_FEE = 144; //have defined
Long64_t time;
Long64_t time_start=0;
Long64_t time_stop=0;
Int_t header[4]={0};
Int_t tail[4]={0};
Int_t time_b[4]={0};
Int_t header_subDAQ[2]={0,0};
Int_t tail_subDAQ=0;
Int_t trigID=0;
unsigned char data[144][2];
Int_t data_2[144]={'0'};//for 2 FEE cards
Int_t FEEcard;
  bool timeflag=false;
  ifstream stream(filename.c_str(), ios_base::binary);
  for(Int_t j=0; !stream.eof(); ++j)
    {
    if(timeflag==true)
	    break;
    if((j%100000)==0)
    cout<<"Event ID is "<<j<<endl;
    //stream.read((char *)(&header_subDAQ),2);
    stream.read((char *)(&header_subDAQ[0]),1);
    if(header_subDAQ[0]==0xeb){
    stream.read((char *)(&header_subDAQ[1]),1);
    if(header_subDAQ[1]==0x90){
    stream.read((char *)(&header[0]), 1);
    stream.read((char *)(&header[1]), 1);
    stream.read((char *)(&header[2]), 1);
    stream.read((char *)(&header[3]), 1);
//time package check    
    if (header[0]==0x30&&header[1]==0x00){
    for(int t=0;t<4;t++)  
    stream.read((char *)(&time_b[t]), 1);
    time=time_b[0]*256*256*256+time_b[1]*256*256+time_b[2]*256+time_b[3];
    time=time/1000;//units :second
    if(time%3600==0)
	    cout<<"Time: "<<time/3600<<" h"<<endl;
    //if(time==7200)
    //        timeflag=true;
    if(time_start==0){
	    time_start=time;cout<<"Start time:"<<time_start/60/60<<"h"<<endl;}
    continue;
    } 
    else{
    for(Int_t i=0; i!=144;++i)
      { 
      stream.read((char *)(&data[i][0]), 1);
      stream.read((char *)(&data[i][1]), 1);
       }
    stream.read((char *)(&tail[0]), 1);
    stream.read((char *)(&tail[1]), 1);
    stream.read((char *)(&tail[2]), 1);
    stream.read((char *)(&tail[3]), 1);
    stream.read((char *)(&tail_subDAQ),2);
    trigID=header[0];
     trigID=trigID&0x7fff;
    if(trigID>20479)
      { 
      trigID=trigID-32768;
      }
   
   
   if(tail_subDAQ==42330)
      {  
      for(Int_t i=0; i!=144; ++i)
        { 
        data_2[i]=data[i][0]*256+data[i][1]*1;
        if(data_2[i]>32767)
	data_2[i]-=65536;	
        } 
    
//int DyChan[nPMT][nDy];map information
     FEEcard=header[1]%64;
      int cID=2;
      if(FEEcard==26)
      cID=0;
      else if(FEEcard==24)
      cID=1;
    if(cID==0||cID==1){
    if(time>0*3600&&time<1.5*3600)
    for (int ib=cID*nPMT;ib<cID*nPMT+nPMT;ib++)//cID*22 
      {   
       Int_t charge=data_2[Chn[ib]];
       signal[ib]->Fill(charge);
      }
    else if(time>1.5*3600&&time<2*3600)
            timeflag=true;
    }
    }
    else continue;
  }}  
  else continue;
   }
  else continue;
} 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//draw and fit  
//define noise counts >pedestalM+5*PedestalSigma
  Long64_t NoiseCounts[nPMT];
  memset(NoiseCounts,0,sizeof(NoiseCounts));
  Double_t NoiseRate[nPMT];
  memset(NoiseRate,0,sizeof(NoiseRate));
  Double_t SpePar[nPMT][7];
//Run time 
  Double_t runtime=0;//minites
  cout<<"The data file is: "<<filename<<endl;
  cout<<"Please enter run time(minites): "<<endl;
  cin>>runtime;


  gStyle->SetOptFit(1111);
TF1 *PedFit=new TF1("PedFit","gaus",100,800);
TF1 *mygaus=new TF1("mygaus","gaus",100,1800);
TF1 *SpeFit=new TF1("SpeFit",SinglePE,0,1800,7); 
Double_t pedpar[100][3];
Double_t gauspar[3];
TSpectrum *s=new TSpectrum(1,30);
Int_t nfound =0;
//printf("Found %d candidate peaks \n",nfound);
Float_t *xpeaks;

  string figure=O_eps;
  string figure_open=O_eps+"[";
  string figure_close=O_eps+"]";
  
   TCanvas *figures=new TCanvas("figures","figures",0,0,600,400); 
   figures->Print(figure_open.c_str(),"Portrait");//open figure file
   TCanvas *Pmt[8];

    for(int np=0;np<nPMT;np++)
    {
    int bc=(int)(np/11);
    int nmb=np%11;
    if(nmb==0){
    char cc[50];
    sprintf(cc,"Batch%d_11PMTs",bc+1);
    Pmt[bc]=new TCanvas(cc,cc,0,0,600,800);
    Pmt[bc]->Divide(3,4);
    }
    Pmt[bc]->cd(nmb+1);
    gPad->SetLogy();
    signal[np]->Draw();
    nfound=s->Search(signal[np],22,"",0.10);
    xpeaks=s->GetPositionX();
    PedFit->SetRange(xpeaks[0]-30,xpeaks[0]+20);
    signal[np]->Fit(PedFit,"RQ0");
    PedFit->GetParameters(pedpar[np]);
    PedFit->SetRange(pedpar[np][1]-5*pedpar[np][2],pedpar[np][1]+5*pedpar[np][2]);
    PedFit->SetParameters(pedpar[np]);
    signal[np]->Fit(PedFit,"R0");
    PedFit->GetParameters(pedpar[np]);
//reset Axis for abstracting the effective entries
    signal[np]->SetAxisRange(pedpar[np][1]+5*pedpar[np][2],2000,"X");
    NoiseCounts[np]=signal[np]->GetEffectiveEntries();
    NoiseRate[np]=NoiseCounts[np]/runtime/60;//Hz
    cout<<"Noise Counts(PMT):"<<NoiseCounts[np]<<"("<<np<<")"<<endl;
    cout<<"Noise Rate(PMT):"<<NoiseRate[np]<<"("<<np<<")"<<endl;
//P.E prefit
    mygaus->SetRange(pedpar[np][1]+70,1200);
    signal[np]->Fit(mygaus,"R0");
    mygaus->GetParameters(gauspar);

   //again show the parameters 
//par[0]:Pedestal Peak,by PedFit parameters
//par[1]:threshold cut ~50fC = ~60ADC counts
//par[2]:heihgt of noise 
//par[3]:delay constant of noise 
//par[4]:height of Single EP guas
//par[5]:Mean of Single EP guas
//par[6]:Sigma of Single EP 
    cout<<"Gaus Par:"<<pedpar[np][1]<<endl; 
    //SpeFit->SetParLimits(0,pedpar[1],pedpar[1]);
    SpeFit->FixParameter(0,pedpar[np][1]);
    SpeFit->SetParLimits(1,55,75);
   // SpeFit->SetParLimits(2,200,2000);
    SpeFit->SetParLimits(3,100,2500);
    SpeFit->SetParLimits(4,0.7*gauspar[0],1.5*gauspar[0]);
    SpeFit->SetParLimits(5,0.7*gauspar[1],1.5*gauspar[1]);
    SpeFit->SetParLimits(6,0.7*gauspar[2],1.5*gauspar[2]);
    SpeFit->SetParameter(1,60);
    SpeFit->SetParameter(2,20*pedpar[np][0]);
    SpeFit->SetParameter(3,1500);
    SpeFit->SetParameter(4,gauspar[0]);
    SpeFit->SetParameter(5,gauspar[1]);
    SpeFit->SetParameter(6,gauspar[2]);

    SpeFit->SetRange(pedpar[np][1]+5*pedpar[np][2],1800);
    //SpeFit->SetRange(pedpar[1]+70,1800);
    //signal[np]->Rebin(2);
    //signal[np]->SetAxisRange(pedpar[1]+5*pedpar[2],2000,"X");
    signal[np]->Fit(SpeFit,"R");
    SpeFit->GetParameters(SpePar[np]);
    SpeFit->SetLineColor(2);
    //SpeFit->Draw("same");
    if(bc>=0&&nmb==10)
    Pmt[bc]->Print(figure.c_str());
     } 
  figures->Print(figure_close.c_str(),"Portrait");//open figure file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ofstream OutputPar;
OutputPar.open(O_text.c_str());
for(int i=0;i<nPMT;i++){
  OutputPar<<NoiseRate[i]<<" ";
  for(int k=0;k<7;k++){
  OutputPar<<SpePar[i][k]<<" ";
  }
  OutputPar<<"\n";
}
OutputPar.close();

Double_t PedChn[144][3];
for(int n=0;n<144;n++){
  for(int j=0;j<3;j++){
  PedChn[n][j]=1000;
  }
}
for(int i=0;i<nPMT;i++){
  for(int j=0;j<3;j++){
  PedChn[Chn[i]][j]=pedpar[i][j];
  }
}

 
	
	
	ofstream Outputped;
 Outputped.open(O_ped.c_str());
 for(int i=0;i<144;i++){
   for(int k=1;k<3;k++){
     Outputped<<PedChn[i][k]<<"  ";
   }
   Outputped<<"\n";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
time_stop=time;

cout<<"Start time:"<<time_start/60/60<<" hours"<<endl;
cout<<"Stop time:"<<time_stop/60/60<<" hours"<<endl;
AgingTree->Fill();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
  outFile->cd();
  outFile->Write();
  outFile->Close();
 return 0;
}

