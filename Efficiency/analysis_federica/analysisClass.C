#define analysisClass_cxx
#include "analysisClass.h"
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

using namespace std;

Float_t dR(Float_t eta1, Float_t eta2, Float_t phi1, Float_t phi2){
    auto dp = std::abs(phi1 - phi2);
    auto deta = std::abs(eta1 - eta2);
    if (dp > Float_t(M_PI))
        dp -= Float_t(2 * M_PI);
    Float_t n = TMath::Sqrt(dp*dp + deta*deta);
    return n;
}

//initialise output minitree
void outTreeInit(TTree *tree_, int &prop_region, int &prop_layer, int &prop_chamber, int &prop_etaP, float &prop_pt, float &prop_eta, float &prop_errR, float &prop_errPhi, int &matched, int &doublematched, int &bad_chamber){
    tree_->Branch("prop_region",  &prop_region);
    tree_->Branch("prop_layer",   &prop_layer);
    tree_->Branch("prop_chamber", &prop_chamber);
    tree_->Branch("prop_etaP",    &prop_etaP);
    tree_->Branch("prop_pt",      &prop_pt);
    tree_->Branch("prop_eta",     &prop_eta);
    tree_->Branch("prop_errR",     &prop_errR);
    tree_->Branch("prop_errPhi",     &prop_errPhi);
    tree_->Branch("matched", &matched);
    tree_->Branch("doublematched", &doublematched);
    tree_->Branch("bad_chamber", &bad_chamber);
}

void analysisClass::Loop()
{
//   In a ROOT session, you can do:
//      root> .L analysisClass.C
//      root> analysisClass t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;
   bool isMC = false;

   // Creation of output file & final tree
   TString root_fileName = fileName;
   TFile *fout = new TFile(root_fileName, "RECREATE");
   fout->cd();
   TTree *outTree = new TTree("outputTree","outputTree");
   int prop_region = 0, prop_layer = 0, prop_chamber = 0, prop_etaP = 0;
   float prop_eta = 0, prop_pt = 0;
   float prop_errR = 0, prop_errPhi = 0;
   int matched = 0, doublematched = 0, bad_chamber = 0;
   outTreeInit(outTree, prop_region, prop_layer, prop_chamber, prop_etaP, prop_pt, prop_eta, prop_errR, prop_errPhi, matched, doublematched, bad_chamber);

   TH1F *hdeltaX_GenMatch = new TH1F("hdeltaX_GenMatch", "hdeltaX_GenMatch", 80, -2, 2);
   TH1F *hdeltaPhi_GenMatch = new TH1F("hdeltaPhi_GenMatch", "hdeltaPhi_GenMatch", 80, -0.001, 0.001);
   TH1F *hEff_GenMatch_den = new TH1F("hEff_GenMatch_den", "hEff_GenMatch_den", 36, 0, 36);
   TH1F *hEff_GenMatch_num = new TH1F("hEff_GenMatch_num", "hEff_GenMatch_num", 36, 0, 36);

   TH1F *hSimHit_Digi_den = new TH1F("hSimHit_Digi_den", "hSimHit_Digi_den", 50, -250, 250);
   TH1F *hSimHit_Digi_num = new TH1F("hSimHit_Digi_num", "hSimHit_Digi_num", 50, -250, 250);
   TH1F *hSimHit_RecHit_den = new TH1F("hSimHit_RecHit_den", "hSimHit_RecHit_den", 50, -250, 250);
   TH1F *hSimHit_RecHit_num = new TH1F("hSimHit_RecHit_num", "hSimHit_RecHit_num", 50, -250, 250);


   bool isVerbose = false;
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if(isVerbose) cout<<"\n==================\nrun "<<event_runNumber<<" lumi "<<event_lumiBlock<<" evt "<<event_eventNumber<<endl;
      
      //gen Matching
      std:vector<int> g_match;
      if(isMC){
          if(isVerbose) cout<<"Loop on "<<mu_propagated_pt->size()<<" propagated muons"<<endl;
          for(int j=0; j<mu_propagated_pt->size(); j++){
              if(isVerbose) cout<<j<<" prop"<<endl;
              int g_index = -999;
              float min_deltap = 999, min_deltaR = 999;
              if(isVerbose) cout<<"Loop on "<<genParticle_Pt->size()<<" genParticles"<<endl;
              for(int g=0; g<genParticle_Pt->size(); g++){
//                  if( !(abs(genParticle_PdgId->at(g)) == 13) ) continue; //gen particle is not a muon 
                  float deltap = abs(mu_propagated_pt->at(j) - genParticle_Pt->at(g))/genParticle_Pt->at(g);
                  float deltaR = dR(mu_propagated_eta->at(j), genParticle_Eta->at(g), mu_propagated_phi->at(j), genParticle_Phi->at(g));
                  if(deltap < min_deltap && deltaR < min_deltaR) {
                      min_deltap = deltap;
                      min_deltaR = deltaR;
                      g_index = g;
                  }
              }
              if(min_deltap<0.1 && min_deltaR<0.05){
                  g_match.push_back(g_index);
                  if(isVerbose) {
                      cout<<"mu_propagated pt | eta | phi"<<endl;
                      cout<<mu_propagated_pt->at(j)<<" | "<<mu_propagated_eta->at(j)<<" | "<<mu_propagated_phi->at(j)<<endl;
                      cout<<"genParticle   pt | eta | phi"<<endl;
                      cout<<genParticle_Pt->at(g_index)<<" | "<<genParticle_Eta->at(g_index)<<" | "<<genParticle_Phi->at(g_index)<<endl;
                      cout<<"genParticle   GrandMotherPdgId->motherPdgId->PdgId"<<endl;
                      cout<<genParticle_GrandMotherPdgId->at(g_index)<< " -> "<<genParticle_MotherPdgId->at(g_index)<< " -> "<<genParticle_PdgId->at(g_index)<<endl;
                  }
              } else {
                  g_match.push_back(-999);
              }
          }//Loop on propagated
      }//isMC

      //Matching with recomuon
      std::vector<int> r_match;
      if(isVerbose) cout<<"Loop on "<<mu_propagated_pt->size()<<" propagated muons"<<endl;
      for(int j=0; j<mu_propagated_pt->size(); j++){
          if(isVerbose){
              cout<<j<<" #prop"<<endl;
              cout<<"mu_propagated pt | eta | phi"<<endl;
              cout<<mu_propagated_pt->at(j)<<" | "<<mu_propagated_eta->at(j)<<" | "<<mu_propagated_phi->at(j)<<endl;
              cout<<"mu_propagated x | y | z"<<endl;
              cout<<mu_propagatedGlb_x->at(j)<<" | "<<mu_propagatedGlb_y->at(j)<<" | "<<mu_propagatedGlb_z->at(j)<<endl;
          }
          int r_index = -999; int r_matched=0;
          float min_deltap = 999, min_deltaR = 999;
          if(isVerbose) cout<<"    Loop on "<<mu_pt->size()<<" recoMuons"<<endl;
          for(int r=0; r<mu_pt->size(); r++){
              if(isVerbose) {
                  cout<<"    recoMuon   pt | eta | phi"<<endl;
                  cout<<"    "<<mu_pt->at(r)<<" | "<<mu_eta->at(r)<<" | "<<mu_phi->at(r)<<endl;
              }
              if( (mu_propagated_pt->at(j) == mu_pt->at(r)) && 
                  (mu_propagated_eta->at(j) == mu_eta->at(r)) && 
                  (mu_propagated_phi->at(j) == mu_phi->at(r)) ) {
                  r_index = r; r_matched++;
                  if(isVerbose) cout<<"matched"<<endl;
              }
          }
          if(r_matched>0){ 
              r_match.push_back(r_index);
              if(isVerbose) {
                  cout<<"--> mu_propagated pt | eta | phi"<<endl;
                  cout<<"--> "<<mu_propagated_pt->at(j)<<" | "<<mu_propagated_eta->at(j)<<" | "<<mu_propagated_phi->at(j)<<endl;
                  cout<<"--> recoMuon   pt | eta | phi"<<endl;
                  cout<<"--> "<<mu_pt->at(r_index)<<" | "<<mu_eta->at(r_index)<<" | "<<mu_phi->at(r_index)<<endl;
              }
          } else {
              r_match.push_back(-999);
          }
      }//Loop on propagated

      //Loop on propagated
      Int_t gen_den = 0;
      Int_t gen_num = 0;
      for(int j=0; j<mu_propagated_pt->size(); j++){
         if(isVerbose) cout<<"Loop on "<<mu_propagated_pt->size()<<" propagated muonsi for efficiency"<<endl;
         bool isZ = false;

          if(isMC){
              int genP = g_match.at(j);
              if(genP<0) continue;
              if(abs(genParticle_PdgId->at(genP)) == 13 && abs(genParticle_MotherPdgId->at(genP)) == 23) isZ = true;
              //if(abs(genParticle_MotherPdgId->at(genP)) == 13 && abs(genParticle_GrandMotherPdgId->at(genP)) == 23) isZ = true;
          } 
          if(!isZ && isMC) continue;

          //bug fix propagation from opposite endcap!
          if(mu_propagated_Outermost_z->at(j)*mu_propagated_region->at(j)<0) continue;

          //check muon ID
          //if(!mu_isTight->at(r_match.at(j))) continue;

          //requirements on reco
          //if( isMC && mu_propagated_numberOfValidPixelHits->at(j) < 2) continue;
          //if( isMC && mu_propagated_numberOfValidTrackerHits->at(j) < 2) continue;
          //if( mu_propagated_pt->at(j) < 3.5 ) continue; //cut at 20 GeV
          //if( abs(mu_propagated_eta->at(j))<1.55 || abs(mu_propagated_eta->at(j))>2.18 ) continue;

          //requirements on propagation error
          //if(mu_propagatedGlb_errR->at(j) > 1.0) continue;
          //if(mu_propagatedGlb_errPhi->at(j) > 2E-3) continue;

          //requirements on fiducial region
          if( abs(mu_propagatedGlb_r->at(j) - mu_propagated_EtaPartition_rMin->at(j)) < 1) continue;

          if(mu_propagated_chamber->at(j) != 19){
              if( abs(mu_propagatedGlb_phi->at(j) - mu_propagated_EtaPartition_phiMin->at(j)) < 0.005) continue;
              if( abs(-mu_propagatedGlb_phi->at(j) + mu_propagated_EtaPartition_phiMax->at(j)) < 0.005) continue;
          }else{
              if( abs(mu_propagatedGlb_phi->at(j)) - mu_propagated_EtaPartition_phiMin->at(j) < 0.005) continue;
          }

          //Fill denominator
          gen_den++;
          hEff_GenMatch_den->Fill(mu_propagated_chamber->at(j)); 

          prop_region  = mu_propagated_region->at(j);
          prop_layer   = mu_propagated_layer->at(j);
          prop_chamber = mu_propagated_chamber->at(j);
          prop_etaP    = mu_propagated_etaP->at(j);
          prop_pt      = mu_propagated_pt->at(j);
          prop_eta     = mu_propagated_eta->at(j);
          prop_errR    = mu_propagatedGlb_errR->at(j);
          prop_errPhi  = mu_propagatedGlb_errPhi->at(j);
          matched = 0;
          doublematched = 0;
          bad_chamber = 0;

          //CHAMBERS OFF:
          if(prop_region<0 && prop_layer==1 && prop_chamber == 01) bad_chamber = 1; //-GE11-M-01L1
          if(prop_region<0 && prop_layer==1 && prop_chamber == 11) bad_chamber = 1; //-GE11-M-11L1
          if(prop_region<0 && prop_layer==1 && prop_chamber == 20) bad_chamber = 1; //-GE11-M-20L1
          if(prop_region<0 && prop_layer==1 && prop_chamber == 34) bad_chamber = 1; //-GE11-M-34L1
          if(prop_region<0 && prop_layer==2 && prop_chamber == 04) bad_chamber = 1; //-GE11-M-04L2
          if(prop_region<0 && prop_layer==2 && prop_chamber == 11) bad_chamber = 1; //-GE11-M-11L2
          if(prop_region>0 && prop_layer==1 && prop_chamber == 14) bad_chamber = 1; //-GE11-P-14L1
          //BAD LATENCY:
          if(prop_region<0 && prop_layer==2 && prop_chamber == 36) bad_chamber = 1; //-GE11-M-36L2
          if(prop_region>0 && prop_layer==1 && prop_chamber == 03) bad_chamber = 1; //-GE11-P-03L1
          if(prop_region>0 && prop_layer==2 && prop_chamber == 36) bad_chamber = 1; //-GE11-P-36L2
           //DAQ ERROR:
          if(prop_region<0 && prop_layer==2 && prop_chamber == 29) bad_chamber = 1; //-GE11-M-29L2
          if(prop_region<0 && prop_layer==2 && prop_chamber == 14) bad_chamber = 1; //-GE11-M-14L2
          if(prop_region>0 && prop_layer==2 && prop_chamber == 18) bad_chamber = 1; //-GE11-P-18L2

          float min_deltax = 999, min_deltaphi = 999;
          //Loop on gemRecHits
          for(int i=0; i<gemRecHit_nRecHits; i++){
              bool sameRegion = gemRecHit_region->at(i) == mu_propagated_region->at(j);
              bool sameLayer  = gemRecHit_layer->at(i) == mu_propagated_layer->at(j);
              bool sameChamber  = gemRecHit_chamber->at(i) == mu_propagated_chamber->at(j);
              bool sameEtaP     = abs(gemRecHit_etaPartition->at(i) - mu_propagated_etaP->at(j)) < 2;
              if(!sameRegion) continue;
              if(!sameLayer) continue;
              if(!sameChamber) continue;
              if(!sameEtaP) continue;

              float deltax = abs(mu_propagatedGlb_x->at(j) - gemRecHit_g_x->at(i));
              float deltaphi = abs(mu_propagatedGlb_phi->at(j) - gemRecHit_g_phi->at(i));
              if(deltax<min_deltax) min_deltax=deltax;
              if(deltaphi<min_deltaphi) min_deltaphi=deltaphi;
          }
          if(abs(min_deltax)<2) hdeltaX_GenMatch->Fill(min_deltax);
          if(abs(min_deltaphi)<1E-3) hdeltaPhi_GenMatch->Fill(min_deltaphi);

          if(mu_propagatedGlb_r->at(j)*abs(min_deltaphi) < 1){ //r*dphi 1 cm
              //matched!
              hEff_GenMatch_num->Fill(mu_propagated_chamber->at(j));
              matched = 1;
          }
          float min_deltaphi2 = 999;
          //if matched==1, look for match in other layer
          for(int i=0; i<gemRecHit_nRecHits && matched==1; i++){
              bool sameRegion = gemRecHit_region->at(i) == mu_propagated_region->at(j);
              bool differentLayer  = gemRecHit_layer->at(i) != mu_propagated_layer->at(j);
              bool sameChamber  = gemRecHit_chamber->at(i) == mu_propagated_chamber->at(j);
              bool sameEtaP     = abs(gemRecHit_etaPartition->at(i) - mu_propagated_etaP->at(j)) < 2;
              if(!sameRegion) continue;
              if(!differentLayer) continue;
              if(!sameChamber) continue;
              if(!sameEtaP) continue;

              float deltaphi = abs(mu_propagatedGlb_phi->at(j) - gemRecHit_g_phi->at(i));
              if(deltaphi<min_deltaphi2) min_deltaphi2=deltaphi;
          }
          if(mu_propagatedGlb_r->at(j)*abs(min_deltaphi2) < 1){ //r*dphi 1 cm
              //double matched!
              doublematched = 1;
          }

          outTree->Fill(); 
      }

      //Loop on SimHits
      if(isMC){
          for(int j=0; j<gemSimHit_particleType->size(); j++){
              if(abs(gemSimHit_particleType->at(j))!=13) continue;
              hSimHit_Digi_den->Fill(gemSimHit_g_x->at(j)); 
              Double_t min_rdphi = 999;
              Int_t digi_index = 0;
              //Loop on Digis
              for(int k=0; k<gemDigi_g_z->size(); k++){
                if(gemDigi_g_z->at(k)*gemSimHit_g_z->at(j) < 0) continue; //different endcap
                if(abs(gemDigi_g_z->at(k) - gemSimHit_g_z->at(j)) > 1 ) continue; //different layer

                Double_t rdphi = gemDigi_g_r->at(k) * abs(gemDigi_g_phi->at(k) - gemSimHit_g_phi->at(j));
                Double_t radius = sqrt( pow(gemDigi_g_x->at(k) - gemSimHit_g_x->at(j), 2.0) + pow(gemDigi_g_y->at(k) - gemSimHit_g_y->at(j), 2.0) );
                //if(isVerbose) cout<<" Digi "<<k<<" radius="<<radius<<endl;
                if(rdphi < min_rdphi) {min_rdphi = rdphi; digi_index = k;}
              }
              cout<<" min_rdphi="<<min_rdphi<<endl;
              cout<<"SimHit local x, y, z = "<<gemSimHit_loc_x->at(j)<<", "<<gemSimHit_loc_y->at(j)<<", "<<gemSimHit_loc_z->at(j)<<endl;
              //cout<<"  Digi local x, y, z = "<<gemDigi_loc_x->at(digi_index)<<", "<<gemDigi_loc_y->at(digi_index)<<", "<<gemDigi_loc_z->at(digi_index)<<endl;
              cout<<"SimHit global x, y, z = "<<gemSimHit_g_x->at(j)<<", "<<gemSimHit_g_y->at(j)<<", "<<gemSimHit_g_z->at(j)<<endl;
              if(min_rdphi<999) cout<<"  Digi global x, y, z = "<<gemDigi_g_x->at(digi_index)<<", "<<gemDigi_g_y->at(digi_index)<<", "<<gemDigi_g_z->at(digi_index)<<endl;

              if(min_rdphi<0.05) hSimHit_Digi_num->Fill(gemSimHit_g_x->at(j)); 
          }
          
          //Loop on SimHits
          for(int j=0; j<gemSimHit_particleType->size(); j++){
              if(abs(gemSimHit_particleType->at(j))!=13) continue;
              hSimHit_RecHit_den->Fill(gemSimHit_g_x->at(j)); 
              Double_t min_rdphi = 999;
              Int_t digi_index = 0;
              //Loop on RecHits
              for(int k=0; k<gemRecHit_g_z->size(); k++){
                if(gemRecHit_g_z->at(k)*gemSimHit_g_z->at(j) < 0) continue; //different endcap
                if(abs(gemRecHit_g_z->at(k) - gemSimHit_g_z->at(j)) > 1 ) continue; //different layer

                Double_t rdphi = gemRecHit_g_r->at(k) * abs(gemRecHit_g_phi->at(k) - gemSimHit_g_phi->at(j));
                Double_t radius = sqrt( pow(gemRecHit_g_x->at(k) - gemSimHit_g_x->at(j), 2.0) + pow(gemRecHit_g_y->at(k) - gemSimHit_g_y->at(j), 2.0) );
                //if(isVerbose) cout<<" RecHit "<<k<<" radius="<<radius<<endl;
                if(rdphi < min_rdphi) {min_rdphi = rdphi; digi_index = k;}
              }
              cout<<" min_rdphi="<<min_rdphi<<endl;
              cout<<"SimHit local x, y, z = "<<gemSimHit_loc_x->at(j)<<", "<<gemSimHit_loc_y->at(j)<<", "<<gemSimHit_loc_z->at(j)<<endl;
              if(min_rdphi<999) cout<<"RecHit local x, y, z = "<<gemRecHit_loc_x->at(digi_index)<<", "<<gemRecHit_loc_y->at(digi_index)<<", "<<gemRecHit_loc_z->at(digi_index)<<endl;
              cout<<"SimHit global x, y, z = "<<gemSimHit_g_x->at(j)<<", "<<gemSimHit_g_y->at(j)<<", "<<gemSimHit_g_z->at(j)<<endl;
              if(min_rdphi<999) cout<<"  RecHit global x, y, z = "<<gemRecHit_g_x->at(digi_index)<<", "<<gemRecHit_g_y->at(digi_index)<<", "<<gemRecHit_g_z->at(digi_index)<<endl;

              if(min_rdphi<0.5) hSimHit_RecHit_num->Fill(gemSimHit_g_x->at(j)); 
           }
        }
    }
   fout->cd();
   //Write and close the file
   fout->Write();
   fout->Close();
}
