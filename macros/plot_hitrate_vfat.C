#include "TH1F.h"
#include <cmath>
#include <string>

void plot_hitrate_vfat()
{
    TString inputfile = "AnalysedTree_data_341343_noise0_vfat.root"; //AnalysedTree_data_341343_noise0.root"; //"AnalysedTree_data_340220_noise0.root";
    TString run = "341343";
    TString endcap[] = {"N", "P"};
    TString layer[] = {"L1", "L2"};

    TChain *t = new TChain("outputLumiTree");
    t->Add(inputfile);
    t->Print();

    TH2D *hrate[2][2][36][24];
    TH1D *hrate1d[2][2][36][24];

    //loop on endcaps
    for(int e=0; e<2; e++){
        //loop on layers
        for(int l=0; l<2; l++){
            //loop on chambers
            for(int c=0; c<36; c++){
                if(!(e==0 && c==30)) continue;
                //loop on vfats
                for(int v=0; v<24; v++){
                    TString plot1D = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]["+std::to_string(v)+"]/nEvents)";
                    TString plot1D_name = "hrate1d["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]["+std::to_string(v)+"]";
                    t->Draw(plot1D+">>"+plot1D_name);
                    hrate1d[e][l][c][v] = (TH1D *)gDirectory->Get(plot1D_name);

                    Double_t ymax = hrate1d[e][l][c][v]->GetXaxis()->GetXmax();

                    TCanvas *c1 = new TCanvas("c1","c1",150,10,800,800);
                    TString plot = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]["+std::to_string(v)+"]/nEvents):lumi";
                    TString plot_name = "hrate["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]["+std::to_string(v)+"]";
                    cout<<plot<<endl;
                    TString select = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]["+std::to_string(v)+"]/nEvents)<"+std::to_string(ymax);
                    cout<<"selection "<<select<<endl;
                    t->Draw(plot+">>"+plot_name, select);
                    hrate[e][l][c][v] = (TH2D *)gDirectory->Get(plot_name);

                    TString chname = "GE11-"+endcap[e]+"-"+std::to_string(c+1)+layer[l];
                    hrate[e][l][c][v]->SetTitle("run"+run+", "+chname+" VFAT"+std::to_string(v));
                    hrate[e][l][c][v]->GetXaxis()->SetTitle("lumi section");
                    hrate[e][l][c][v]->GetYaxis()->SetTitle("n. recHits/event");
                    hrate[e][l][c][v]->Draw("candle");
                    cout<<"run"+run<<" "<<endcap[e]+" endcap, chamber "+chname<<" vfat"<<std::to_string(v)<<endl;
                    c1->SaveAs("plots_vfat/run"+run+"_"+chname+"_vfat"+std::to_string(v)+".png");
                }
            }
        }
    }
}
