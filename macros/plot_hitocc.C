#include "TH1F.h"
#include <cmath>
#include <string>

void plot_hitocc()
{
    TString inputfile = "AnalysedTree_data_341343_noise0.root"; //"AnalysedTree_data_340220_noise0.root";
    TString run = "341343";
    TString endcap[] = {"negative", "positive"};
    TString layer[] = {"ly1", "ly2"};

    TChain *t = new TChain("outputLumiTree");
    t->Add(inputfile);
    t->Print();

    TH2D *hrate[2][2][36];
    TH1D *hrate1d[2][2][36];
    std::vector<double> rate_limit;
    //loop on endcaps
    for(int e=0; e<2; e++){
        //loop on layers
        for(int l=0; l<2; l++){
            //loop on chambers
            for(int c=0; c<36; c++){
//                if(!(c==7)) continue;
                TString plot1D = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]/nEvents)";
                TString plot1D_name = "hrate1d["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]";
                t->Draw(plot1D+">>"+plot1D_name);
                hrate1d[e][l][c] = (TH1D *)gDirectory->Get(plot1D_name);

                Double_t ymax = hrate1d[e][l][c]->GetXaxis()->GetXmax();
                rate_limit.push_back(ymax);

//                TCanvas *c1 = new TCanvas("c1","c1",150,10,800,800);
//                TString plot = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]/nEvents):lumi";
//                TString plot_name = "hrate["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]";
//                cout<<plot<<endl;
//                TString select = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]/nEvents)<"+std::to_string(ymax);
//                cout<<"selection "<<select<<endl;
//                t->Draw(plot+">>"+plot_name, select);
//                hrate[e][l][c] = (TH2D *)gDirectory->Get(plot_name);
//                hrate[e][l][c]->SetTitle("run340220, "+endcap[e]+" endcap, chamber "+std::to_string(c)+" "+layer[l]);
//                hrate[e][l][c]->GetXaxis()->SetTitle("lumi section");
//                hrate[e][l][c]->GetYaxis()->SetTitle("n. recHits/event");
//                hrate[e][l][c]->Draw("candle");
//                cout<<"run"+run<<" "<<endcap[e]+" endcap, chamber "+std::to_string(c)+" "+layer[l]<<endl;
//                c1->SaveAs("run"+run+"_"+endcap[e]+"_ch"+std::to_string(c)+"_"+layer[l]+".png");
            }
        }
    }
    auto n = rate_limit.size(); 
    double average = 0.0f;
    if ( n != 0) {
        average = accumulate( rate_limit.begin(), rate_limit.end(), 0.0) / n; 
    }
    cout<<"average = "<<average<<endl;

    //index of noisy chambers
    std::vector<int> ind;
    for( int i = 0; i < 2*2*36; i++){
        double diff = abs(rate_limit.at(i) - average)/average;
        if(diff>1) ind.push_back(i);
    }
    int i = 0;
    for(int e=0; e<2; e++){
        //loop on layers
        for(int l=0; l<2; l++){
            //loop on chambers
            for(int c=0; c<36; c++){
                double diff = abs(rate_limit.at(i) - average)/average;
                i++;
                TString plot1D = "(nHits["+std::to_string(e)+"]["+std::to_string(l)+"]["+std::to_string(c)+"]/nEvents)";
                //cout<<"rate "<<plot1D<<" "<<rate_limit.at(i)<<" diff="<<diff<<endl;
                if(diff<1) { //normal chamber
                    
                }
            }
        }
    }
}
