#include "TH1F.h"
#include <cmath>
#include <string>

void plot_eff_eta_scan()
{
    TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210522_1201/AnalysedTree_MC_Zmumu_efficiency_merged.root";
//"/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1733/AnalysedTree_MC_Zmumu_efficiency_merged.root"; ///lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1143/AnalysedTree_data_341343_efficiency_merged_4.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1132/AnalysedTree_data_341343_efficiency_merged_3.root";
    //TString inputfile = "/lustre/cms/store/user/fsimone/GEM_PFA/Analysis/20210518_1117/AnalysedTree_data_341343_efficiency_merged_2.root";
    TString run = "Zmumu";

    TChain *t = new TChain("outputTree");
    t->Add(inputfile);
    t->Print();

    TString cut[] = {
        "prop_errR < 1",
        "prop_errR < 0.9",
        "prop_errR < 0.8",
        "prop_errR < 0.7",
        "prop_errR < 0.6",
        "prop_errR < 0.5",
        "prop_errPhi < 0.05E-3",
        "prop_errPhi < 0.06E-3",
        "prop_errPhi < 0.07E-3",
        "prop_errPhi < 0.08E-3",
        "prop_errPhi < 0.09E-3",
        "prop_errPhi < 0.10E-3",
        "prop_errPhi < 0.11E-3",
        "prop_errPhi < 0.12E-3",
        "prop_errPhi < 0.13E-3",
        "prop_errPhi < 0.14E-3",
        "prop_errPhi < 0.15E-3",
        "prop_errR < 0.6 && prop_errPhi < 0.14E-3"
    };
    int ncut = sizeof(cut) / sizeof(cut[0]);

    for(int i =0; i<ncut; i++){
    TH2D *hnum = new TH2D("hnum", "hnum", 8, 1.55, 2.18, 15, 20, 80);
    TH2D *hden = new TH2D("hden", "hden", 8, 1.55, 2.18, 15, 20, 80);

    TString binning = "(8, 1.55, 2.18, 15, 20, 80)";
    TString den_sel = "prop_pt>20 &&"+cut[i] ;
    TString num_sel = den_sel + "&& matched";
    TString den_name = "hden";
    TString num_name = "hnum";
   
    TString varname = "prop_pt:abs(prop_eta)"; 
    t->Draw(varname+">>"+num_name, num_sel, "colz");
    hnum = (TH2D *)gDirectory->Get(num_name);
    t->Draw(varname+">>"+den_name, den_sel, "colz");
    hden = (TH2D *)gDirectory->Get(den_name);
    
    TCanvas *c2 = new TCanvas("c2","c2",150,10,800,800);
    //// Upper plot will be in pad1
    //TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    //pad1->SetBottomMargin(0); // Upper and lower plot are joined
    //pad1->SetGridx();      // Vertical grid
    //pad1->Draw();          // Draw the upper pad: pad1
    //pad1->cd();            // pad1 becomes the current pad
    //
    //TString chname = "GE11-";
    //hden->SetTitle("run"+run+", "+chname);
    //hden->GetXaxis()->SetTitle("|#eta|");
    //hden->GetYaxis()->SetTitle("recHits");
    //hden->Draw("hist");
    //hnum->SetLineColor(kRed);
    //hnum->Draw("hist same");
    //
    //// lower plot will be in pad2
    //c1->cd();          // Go back to the main canvas before defining pad2
    //TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    //pad2->SetTopMargin(0);
    //pad2->SetBottomMargin(0.2);
    //pad2->SetGridx(); // vertical grid
    //pad2->Draw();
    //pad2->cd();       // pad2 becomes the current pad    
    // Define the ratio plot
    TH2D *h_x_ratio = (TH2D*)hnum->Clone("h_x_ratio");
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
    h_x_ratio->GetYaxis()->SetTitle("p_{T}");
    h_x_ratio->GetXaxis()->SetTitle("|#eta|");
    h_x_ratio->GetXaxis()->SetTitleSize(22);
    h_x_ratio->GetXaxis()->SetTitleFont(43);
    h_x_ratio->GetXaxis()->SetTitleOffset(2.0);
    h_x_ratio->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    h_x_ratio->GetXaxis()->SetLabelSize(15);
    h_x_ratio->SetMaximum(1.0);
    h_x_ratio->SetMinimum(0.96);
    h_x_ratio->Draw("colz text0");
    
    c2->Update();
   
    TString cut_label = cut[i];
    cut_label = cut_label.ReplaceAll(" ", "");
    cut_label = cut_label.ReplaceAll("<", "less");
    cut_label = cut_label.ReplaceAll(".", "p");

    c2->SaveAs("plots/Efficiency/Inclusive/Eff_eta_run"+run+"_cut"+std::to_string(i)+"_"+cut_label+".png");
}
}
