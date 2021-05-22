#include "TH1F.h"
#include <cmath>
#include <string>

void plot_eff_eta()
{
    TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210522_1201/AnalysedTree_MC_Zmumu_efficiency_merged.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210522_1155/AnalysedTree_MC_Zmumu_efficiency_merged.root";
//"/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1733/AnalysedTree_MC_Zmumu_efficiency_merged.root"; ///lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1143/AnalysedTree_data_341343_efficiency_merged_4.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1132/AnalysedTree_data_341343_efficiency_merged_3.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1117/AnalysedTree_data_341343_efficiency_merged_2.root";
    TString run = "Zmumu";
    TString endcap[] = {"N", "P"};
    TString layer[] = {"L1", "L2"};

    TChain *t = new TChain("outputTree");
    t->Add(inputfile);
    t->Print();

    TH1D *hnum;
    TH1D *hden;
    TString endcap_sel[] = {"-1", "+1"};

    TString binning = "(8, 1.55, 2.18)";
    TString den_sel = "prop_pt>20 && prop_errR < 0.6"; // && prop_errPhi < 0.14E-3" ;
    TString num_sel = den_sel + "&& matched";
    TString den_name = "hden";
    TString num_name = "hnum";
    
    t->Draw("abs(prop_eta)>>"+num_name+binning, num_sel);
    hnum = (TH1D *)gDirectory->Get(num_name);
    t->Draw("abs(prop_eta)>>"+den_name+binning, den_sel);
    hden = (TH1D *)gDirectory->Get(den_name);
    
    TCanvas *c1 = new TCanvas("c1","c1",150,10,800,800);
    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();      // Vertical grid
    pad1->Draw();          // Draw the upper pad: pad1
    pad1->cd();            // pad1 becomes the current pad
    
    TString chname = "GE11-";
    hden->SetTitle("run"+run+", "+chname);
    hden->GetXaxis()->SetTitle("|#eta|");
    hden->GetYaxis()->SetTitle("recHits");
    hden->Draw("hist");
    hnum->SetLineColor(kRed);
    hnum->Draw("hist same");
    
    // lower plot will be in pad2
    c1->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();       // pad2 becomes the current pad    
    // Define the ratio plot
    TH1D *h_x_ratio = (TH1D*)hnum->Clone("h_x_ratio");
    h_x_ratio->Sumw2(); 
    h_x_ratio->Divide(hden);
    h_x_ratio->SetStats(0);
    // Ratio plot settings
    gStyle->SetLineWidth(2);
    h_x_ratio->SetTitle(""); // Remove the ratio title
    h_x_ratio->GetYaxis()->SetTitle("Efficiency");
    h_x_ratio->GetYaxis()->SetRangeUser(0.96,1.0);
    h_x_ratio->SetLineColor(kBlack);
    h_x_ratio->SetMarkerStyle(7);
    h_x_ratio->SetMarkerColor(1);
    h_x_ratio->GetYaxis()->SetTitleSize(22);
    h_x_ratio->GetYaxis()->SetTitleFont(43);
    h_x_ratio->GetYaxis()->SetTitleOffset(1.25);
    h_x_ratio->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    h_x_ratio->GetYaxis()->SetLabelSize(15);
    
    // X axis ratio plot settings
    h_x_ratio->GetXaxis()->SetTitle("|#eta|");
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
    line.DrawLine(1.55,mean,2.18,mean);            
    h_x_ratio->Draw("same");
    c1->cd();
    c1->Update();
    
    
    cout<<"run"+run<<endl;
    c1->SaveAs("plots/Efficiency/Inclusive/Eff_eta_run"+run+".png");

}
