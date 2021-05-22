#include "TH1F.h"
#include <cmath>
#include <string>

void plot_eff_inclusive()
{
    TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1143/AnalysedTree_data_341343_efficiency_merged_4.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1132/AnalysedTree_data_341343_efficiency_merged_3.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1117/AnalysedTree_data_341343_efficiency_merged_2.root";
    TString run = "341343";
    TString endcap[] = {"N", "P"};
    TString layer[] = {"L1", "L2"};

    TChain *t = new TChain("outputTree");
    t->Add(inputfile);
    t->Print();

    TH1D *hnum[2][2];
    TH1D *hden[2][2];
    TString endcap_sel[] = {"-1", "+1"};

    TString binning = "(8, 1, 9)";
    //loop on endcaps
    for(int e=0; e<2; e++){
        //loop on layers
        for(int l=0; l<2; l++){
		TString den_sel = "!bad_chamber && prop_region == "+endcap_sel[e]+" && prop_layer == "+std::to_string(l+1);
                TString num_sel = den_sel + "&& matched";
                TString den_name = "hden["+std::to_string(e)+"]["+std::to_string(l)+"]";
                TString num_name = "hnum["+std::to_string(e)+"]["+std::to_string(l)+"]";

                t->Draw("prop_etaP>>"+num_name+binning, num_sel);
                hnum[e][l] = (TH1D *)gDirectory->Get(num_name);
                t->Draw("prop_etaP>>"+den_name+binning, den_sel);
                hden[e][l] = (TH1D *)gDirectory->Get(den_name);

                TCanvas *c1 = new TCanvas("c1","c1",150,10,800,800);
                // Upper plot will be in pad1
                TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
                pad1->SetBottomMargin(0); // Upper and lower plot are joined
                pad1->SetGridx();      // Vertical grid
                pad1->Draw();          // Draw the upper pad: pad1
                pad1->cd();            // pad1 becomes the current pad

                TString chname = "GE11-"+endcap[e]+"-XX"+layer[l];
                hden[e][l]->SetTitle("run"+run+", "+chname);
                hden[e][l]->GetXaxis()->SetTitle("GE1/1 eta partition");
                hden[e][l]->GetYaxis()->SetTitle("recHits");
                hden[e][l]->Draw("hist");
                hnum[e][l]->SetLineColor(kRed);
                hnum[e][l]->Draw("hist same");

                // lower plot will be in pad2
                c1->cd();          // Go back to the main canvas before defining pad2
                TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
                pad2->SetTopMargin(0);
                pad2->SetBottomMargin(0.2);
                pad2->SetGridx(); // vertical grid
                pad2->Draw();
                pad2->cd();       // pad2 becomes the current pad    
                // Define the ratio plot
                TH1D *h_x_ratio = (TH1D*)hnum[e][l]->Clone("h_x_ratio");
                h_x_ratio->Sumw2(); 
                h_x_ratio->Divide(hden[e][l]);
                h_x_ratio->SetStats(0);
                // Ratio plot settings
                gStyle->SetLineWidth(2);
                h_x_ratio->SetTitle(""); // Remove the ratio title
                h_x_ratio->GetYaxis()->SetTitle("Efficiency");
                h_x_ratio->GetYaxis()->SetRangeUser(0.5,1.1);
                h_x_ratio->SetLineColor(kBlack);
                h_x_ratio->SetMarkerStyle(7);
                h_x_ratio->SetMarkerColor(1);
                h_x_ratio->GetYaxis()->SetTitleSize(22);
                h_x_ratio->GetYaxis()->SetTitleFont(43);
                h_x_ratio->GetYaxis()->SetTitleOffset(1.25);
                h_x_ratio->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
                h_x_ratio->GetYaxis()->SetLabelSize(15);
            
                // X axis ratio plot settings
                h_x_ratio->GetXaxis()->SetTitle("GE1/1 eta partition");
                h_x_ratio->GetXaxis()->SetTitleSize(22);
                h_x_ratio->GetXaxis()->SetTitleFont(43);
                h_x_ratio->GetXaxis()->SetTitleOffset(2.0);
                h_x_ratio->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
                h_x_ratio->GetXaxis()->SetLabelSize(15);
            
                //Compute weighted average ratio
                Double_t mean = 0;
                Double_t std_dev = 0;
                for (int c=1; c<=(h_x_ratio->GetNbinsX()); c++)
                {
                    if(h_x_ratio->GetBinContent(c) == 0 || h_x_ratio->GetBinError(c) == 0) continue;
                    //cout<<c<<" "<<h_x_ratio->GetBinContent(c)<<" +- "<<h_x_ratio->GetBinError(c)<<endl;
                    mean = mean + h_x_ratio->GetBinContent(c) / ( h_x_ratio->GetBinError(c) * h_x_ratio->GetBinError(c) );
                    std_dev = std_dev + 1/( h_x_ratio->GetBinError(c) * h_x_ratio->GetBinError(c));
                }
                mean = mean/std_dev;
                std_dev = 1/std_dev;

                //Draw line corresponding to mean value on ratio plot
                TLine line;
                h_x_ratio->Draw("ep");
                line.DrawLine(1.0,mean,9.0,mean);            
                h_x_ratio->Draw("same");
                c1->cd();
                c1->Update();


                cout<<"run"+run<<" "<<endcap[e]+" endcap, chamber "+chname<<endl;
                c1->SaveAs("plots/Efficiency/Inclusive/Eff_etaP_run"+run+"_"+chname+".png");
        }
    }
}
