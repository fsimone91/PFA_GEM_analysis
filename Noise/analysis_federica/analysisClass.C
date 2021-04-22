#define analysisClass_cxx
#include "analysisClass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

using namespace std;

//initialise output minitree
void outTreeInit(TTree *tree_, Int_t &run, Int_t &lumi, Int_t &evt, Int_t cout[2][2][36]){
    tree_->Branch("run", &run);
    tree_->Branch("evt", &evt);
    tree_->Branch("lumi", &lumi);
    tree_->Branch("count", cout, "count[2][2][36]/I");
}
//initialise output minitree
void outTreeLumiInit(TTree *tree_, Int_t &nEvents, Int_t &lumi, Int_t nHits[2][2][36]){
    tree_->Branch("nEvents", &nEvents);
    tree_->Branch("lumi", &lumi);
    tree_->Branch("nHits", nHits, "nHits[2][2][36]/I");
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

   // Creation of output file & final tree
   TString root_fileName = fileName;
   TFile *fout = new TFile(root_fileName, "RECREATE");
   fout->cd();
   TTree *outTree = new TTree("outputTree","outputTree");
   TTree *outLumiTree = new TTree("outputLumiTree","outputLumiTree");
   //initialise variables
   Int_t run = 0, lumi = 0, evt = 0;
   int counts[2][2][36] = {0};
   int region_index[2] = {-1, +1};
   int layer_index[2] = {1, 2};

   vector<Int_t> lumi_list;
   Int_t nevents = 0;
   Int_t nhits[2][2][36] = {0};

   //initialise output minitree
   outTreeInit(outTree, run, lumi, evt, counts);
   outTreeLumiInit(outLumiTree, nevents, lumi, nhits);

   if (fChain == 0) return;

   bool isVerbose = false;
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if(isVerbose) cout<<"\n==================\nrun "<<event_runNumber<<" lumi "<<event_lumiBlock<<" evt "<<event_eventNumber<<endl;

    //  if(isVerbose) cout<<"n. GEM RecHit = "<<gemRecHit_region->size()<<endl;
    //  //loop on GEM rechits
    //  for(int j=0; j<gemRecHit_region->size(); j++){
    //      if(isVerbose) cout<<j<<" RecHit region|chamber|layer= "<<gemRecHit_region->at(j)<<" | "<<gemRecHit_chamber->at(j)<<" | "<<gemRecHit_layer->at(j)<<endl;
    //      //loop on endcaps
    //      for(int e=0; e<2; e++){
    //          //loop on layers
    //          for(int l=0; l<2 && gemRecHit_region->at(j) == region_index[e]; l++){
    //              //loop on chambers
    //              for(int c=0; c<36 && gemRecHit_layer->at(j) == layer_index[l]; c++){
    //                  if(gemRecHit_chamber->at(j) == c) counts[e][l][c]++;
    //              }
    //          }
    //      }
    //  }
    //  //for each event, fill output Tree
    //  run = event_runNumber;
    //  evt = event_eventNumber;
    //  lumi = event_lumiBlock;
    //  outTree->Fill();
    //  //reset counts to zero
    //  memset(counts, 0, sizeof(counts));

      //per lumi analysis
      lumi_list.push_back(event_lumiBlock);
   } //loop on events

   //new loop on events for per lumi analysis
   nentries = fChain->GetEntriesFast();
   nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if(isVerbose) cout<<"\n==================\nrun "<<event_runNumber<<" lumi "<<event_lumiBlock<<" evt "<<event_eventNumber<<endl;
      if(isVerbose) cout<<"jentry "<<jentry<<"event_lumiBlock "<<event_lumiBlock<<" lumi_list.at(jentry) "<<lumi_list.at(jentry)<<endl;
      if((jentry < nentries-1) && event_lumiBlock == lumi_list.at(jentry+1)){
          nevents++;
          if(isVerbose) cout<<"n. GEM RecHit = "<<gemRecHit_region->size()<<endl;
          //loop on GEM rechits
          for(int j=0; j<gemRecHit_region->size(); j++){
              if(isVerbose) cout<<j<<" RecHit region|chamber|layer= "<<gemRecHit_region->at(j)<<" | "<<gemRecHit_chamber->at(j)<<" | "<<gemRecHit_layer->at(j)<<endl;
              //loop on endcaps
              for(int e=0; e<2; e++){
                  //loop on layers
                  for(int l=0; l<2 && gemRecHit_region->at(j) == region_index[e]; l++){
                      //loop on chambers
                      for(int c=0; c<36 && gemRecHit_layer->at(j) == layer_index[l]; c++){
                          if(gemRecHit_chamber->at(j) == c) nhits[e][l][c]++;
                      }
                  }
              }
          }
      }else{ //if(jentry>0 && event_lumiBlock != lumi_list.at(jentry-1)){
          if(isVerbose){
              cout<<"event_lumiBlock "<<event_lumiBlock<<" nEvents="<<nevents;
              cout<<"hits in chamber [1][1][20] "<<nhits[1][1][20]<<endl; }
          lumi = event_lumiBlock;
          outLumiTree->Fill();
          //reset
          memset(nhits, 0, sizeof(nhits));
          nevents=0;
      }
   } //loop on events

   fout->cd();
   //Write and close the file
   fout->Write();
   fout->Close();
}
