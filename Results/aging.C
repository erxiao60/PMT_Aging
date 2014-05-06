#include <TH2D.h>
int aging(){
Double_t rate_pmt1[8]={2.27451,2.3481,2.41833,6.96722,8.40926,7.11407,6.08889,5.49463};
Double_t rate_pmt2[8]={3.46471,3.3673,3.24487,9.06444,11.187,9.4663,7.94111,7.11722};
Double_t rate_pmt16[8]={0.019804,0.0246835,0.0242308,0.24,0.255185,0.195741,0.18537,0.177778};

Double_t time[8]={42.5,767,2197,7164,7884,8604,9324,10044};//read the note file
Double_t PE_peak1[8]={793.662-560.137,790.746-560.584,779.712-560.599,764.72-551.186,763.342-551.594,762.713-551.456,763.11-552.201,766.823-552.248};
Double_t PE_peak2[8]={837.596-603.928,839.558-603.675,824.061-604.083,809.336-594.774,806.336-594.463,803.501-594.083,806.302-594.874,807.642-594.725};
Double_t PE_sigma1[8]={82.1108,84.2133,95.97,109.629,108.22,106.28,107.055,104.862};
Double_t PE_sigma2[8]={87.6287,85.9618,102.044,113.33,111.594,111.657,110.317,111.582};


TH2D *rate1=new TH2D("noiserate1","PMT1&PMT2;time(h);rate(Hz)",200,0,200,300,0,30);
TH2D *rate2=new TH2D("noiserate2",";time(h);rate(Hz)",200,0,200,300,0,30);
TH2D *gain1=new TH2D("gain1","PMT1&PMT2;time(h);PE_peak(ADC counts)",200,0,200,200,150,350);
TH2D *gain2=new TH2D("gain2",";time(h);PE_peak(ADC counts)",200,0,200,200,150,350);
TH2D *res1=new TH2D("res1","res;time(h);resolution(%)",200,0,200,100,0,100);
TH2D *res2=new TH2D("res2",";time(h);resolution(%)",200,0,200,100,0,100);

  for(int i=0;i<8;i++){
  rate1->Fill(time[i]/60,rate_pmt1[i]);
  gain1->Fill(time[i]/60,PE_peak1[i]);
  res1->Fill(time[i]/60,PE_sigma1[i]/PE_peak1[i]*100);
  }
  for(int i=0;i<8;i++){
  rate2->Fill(time[i]/60,rate_pmt2[i]);
  gain2->Fill(time[i]/60,PE_peak2[i]);
  res2->Fill(time[i]/60,PE_sigma2[i]/PE_peak2[i]*100);
  }
 TCanvas *Rate=new TCanvas("Rate","Rate");
 rate1->SetMarkerSize(0.5);
 rate1->SetMarkerStyle(24);
 rate1->Draw();
 rate2->SetMarkerSize(0.5);
 rate2->SetMarkerStyle(25);
 rate2->SetMarkerColor(kRed);
 rate2->Draw("same");
 TCanvas *Gain=new TCanvas("Gain","Gain");
 gain1->SetMarkerSize(0.5);
 gain1->SetMarkerStyle(24);
 gain1->Draw();
 gain2->SetMarkerSize(0.5);
 gain2->SetMarkerStyle(25);
 gain2->SetMarkerColor(kRed);
 gain2->Draw("same");
 TCanvas *Res=new TCanvas("Res","Res");
 res1->SetMarkerSize(0.5);
 res1->SetMarkerStyle(24);
 res1->Draw();
 res2->SetMarkerSize(0.5);
 res2->SetMarkerStyle(25);
 res2->SetMarkerColor(kRed);
 res2->Draw("same");
return 222;
}
