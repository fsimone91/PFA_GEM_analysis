#include "TH1F.h"
#include <cmath>
#include <string>

void plot_hitrate()
{
    TString inputfile = "AnalysedTree_data_340220_noise0.root";

    TString endcap[] = {"negative", "positive"};
    TString layer[] = {"ly1", "ly2"};

    TChain *t = new TChain("outputLumiTree");
    t->Add(inputfile);
    t->Print();

    TH2D *hrate[2][2][36];
    TH2D *htemp;
    TCanvas *c1 = new TCanvas("c1","c1",150,10,800,800);
    //loop on endcaps
    for(int e=0; e<2; e++){
        //loop on layers
        for(int l=0; l<2; l++){
            //loop on chambers
            for(int c=0; c<36; c++){
                //if(!(e==1 && l==1 && c==20)) continue;
                TString plot = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]/nEvents):lumi";
                cout<<plot<<endl;
                t->Draw(plot+">>htemp");
                hrate[e][l][c] = (TH2D *)gDirectory->Get("htemp");
                hrate[e][l][c]->SetTitle("run340220, "+endcap[e]+" endcap, chamber "+std::to_string(c)+" "+layer[l]);
                hrate[e][l][c]->Draw("candle");
                cout<<"run340220, "+endcap[e]+" endcap, chamber "+std::to_string(c)+" "+layer[l]<<endl;
                c1->SaveAs("run340220_"+endcap[e]+"_ch"+std::to_string(c)+"_"+layer[l]+".png");
            }
        }
    }
}
