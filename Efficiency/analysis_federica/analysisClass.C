#define analysisClass_cxx
#include "analysisClass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

Float_t dR(Float_t eta1, Float_t eta2, Float_t phi1, Float_t phi2){
    auto dp = std::abs(phi1 - phi2);
    auto deta = std::abs(eta1 - eta2);
    if (dp > Float_t(M_PI))
        dp -= Float_t(2 * M_PI);
    Float_t n = TMath::Sqrt(dp*dp + deta*deta);
    return n;
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

   TH1F *hdeltaX_GenMatch = new TH1F("hdeltaX_GenMatch", "hdeltaX_GenMatch", 80, -2, 2);
   TH1F *hdeltaPhi_GenMatch = new TH1F("hdeltaPhi_GenMatch", "hdeltaPhi_GenMatch", 80, -0.001, 0.001);

   bool isVerbose = true;
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if(isVerbose) cout<<"\n==================\nrun "<<event_runNumber<<" lumi "<<event_lumiBlock<<" evt "<<event_eventNumber<<endl;

      std:vector<int> g_match;
      for(int j=0; j<mu_propagated_pt->size(); j++){
          int g_index = -999;
          float min_deltap = 999, min_deltaR = 999;
          for(int g=0; g<genParticle_Pt->size(); g++){
              if( !(abs(genParticle_PdgId->at(g)) == 13) ) continue; //gen particle is not a muon 
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


      //Loop on propagated
      for(int j=0; j<mu_propagated_pt->size(); j++){
          bool isZ = false;
          int genP = g_match.at(j);
          if(genP<0) continue;
          if(abs(genParticle_MotherPdgId->at(genP)) == 23) isZ = true;
          if(abs(genParticle_MotherPdgId->at(genP)) == 13 && abs(genParticle_GrandMotherPdgId->at(genP)) == 23) isZ = true;

          if(!isZ) continue;
          if(isVerbose) cout<<"isZ = "<<isZ<<endl;

          float min_deltax = 999, min_deltaphi = 999;
          for(int i=0; i<gemRecHit_nRecHits; i++){
              bool sameRegion = gemRecHit_region->at(i) == mu_propagated_region->at(j);
              bool sameLayer  = gemRecHit_layer->at(i) == mu_propagated_layer->at(j);
              bool sameChamber  = gemRecHit_chamber->at(i) == mu_propagated_chamber->at(j);
              if(!sameRegion) continue;
              if(!sameLayer) continue;
              if(!sameChamber) continue;

              float deltax = mu_propagatedLoc_x->at(j) - gemRecHit_loc_x->at(i);
              float deltaphi = mu_propagatedLoc_phi->at(j) - gemRecHit_loc_phi->at(i);
              if(deltax<min_deltax) min_deltax=deltax;
              if(deltaphi<min_deltaphi) min_deltaphi=deltaphi;
          }
          if(abs(min_deltax)<2) hdeltaX_GenMatch->Fill(min_deltax);
          if(abs(min_deltaphi)<1E-3) hdeltaPhi_GenMatch->Fill(min_deltaphi);
      }
   }
}
