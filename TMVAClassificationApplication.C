/// \file
/// \ingroup tutorial_tmva
/// \notebook -nodraw
/// This macro provides a simple example on how to use the trained classifiers
/// within an analysis module
/// - Project   : TMVA - a Root-integrated toolkit for multivariate data analysis
/// - Package   : TMVA
/// - Exectuable: TMVAClassificationApplication
///
/// \macro_output
/// \macro_code
/// \author Andreas Hoecker

// USER CHANGES

// 1. Input root file store path
// 2. Output root file name  ( if this file will exist in $PWD then it will update it else create a new one having only mva response
// 3. Name of input variables

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

using namespace TMVA;

void TMVAClassificationApplication( TString fname = "/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-26_18h59/HaddedFiles/ZTo2LZTo2JJJ_EWK_LO_SM.root", TString myMethodList = "" )
{

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // Boosted Decision Trees
   Use["BDT"]             = 0; // uses Adaptive Boost
   Use["BDTG"]            = 1; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
   //

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassificationApplication" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod
                      << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
               std::cout << it->first << " ";
            }
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // Create the Reader object

   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
   Float_t var[37];
   reader->AddVariable( "l_pt1",	&var[0] );
   reader->AddVariable( "l_eta1",	&var[1] );
   reader->AddVariable( "pfMET_Corr",	&var[2] );
   reader->AddVariable( "vbf_maxpt_jj_m", 	&var[3] );
   reader->AddVariable( "v_pt_type0", 	&var[4] );
   reader->AddVariable( "v_eta_type0", 	&var[5] );
   reader->AddVariable( "ungroomed_PuppiAK8_jet_pt", 	&var[6] );
   reader->AddVariable( "ungroomed_PuppiAK8_jet_eta", 	&var[7] );
   reader->AddVariable( "mass_lvj_type0_PuppiAK8", 	&var[8] );
   reader->AddVariable( "pt_lvj_type0_PuppiAK8", 	&var[9] );
   reader->AddVariable( "eta_lvj_type0_PuppiAK8", 	&var[10] );
   reader->AddVariable( "deltaphi_METak8jet", 	&var[11] );
   //reader->AddVariable( "PuppiAK8_jet_tau2tau1", 	&var[12] );
   reader->AddVariable( "njets", 	&var[13] );
   reader->AddVariable( "vbf_maxpt_j1_pt", 	&var[14] );
   reader->AddVariable( "vbf_maxpt_j2_pt", 	&var[15] );
   reader->AddVariable( "vbf_maxpt_j1_eta", 	&var[16] );
   reader->AddVariable( "vbf_maxpt_j2_eta", 	&var[17] );
   reader->AddVariable( "PtBalance_type0", 	&var[18] );
   reader->AddVariable( "BosonCentrality_type0", 	&var[19] );
   reader->AddVariable( "vbf_maxpt_jj_Deta", 	&var[20] );
   reader->AddVariable( "PuppiAK8_jet_mass_so_corr", 	&var[21] );
   reader->AddVariable( "ZeppenfeldWH/DEtajj := ZeppenfeldWH/vbf_maxpt_jj_Deta", &var[22]);
   reader->AddVariable( "ZeppenfeldWL/DEtajj := ZeppenfeldWL_type0/vbf_maxpt_jj_Deta", &var[23] ); 
   reader->AddVariable( "costheta1_type0", 	&var[24] );
   reader->AddVariable( "costheta2_type0", 	&var[25] );
   reader->AddVariable( "phi_type0", 	&var[26] );
   reader->AddVariable( "phi1_type0", 	&var[27] );
   reader->AddVariable( "costhetastar_type0", 	&var[28] );
   //reader->AddVariable( "WWRapidity", 	&var[29] );
   //reader->AddVariable( "RpT_type0", 	&var[30] );
   reader->AddVariable( "v_mt_type0", 	&var[31] );
   //reader->AddVariable( "LeptonProjection_type0", 	&var[32] );
   //reader->AddVariable( "VBSCentrality_type0", 	&var[33] );
   //reader->AddVariable( "ZeppenfeldWL_type0", 	&var[34] );
   //reader->AddVariable( "ZeppenfeldWH",	&var[35] );
   //reader->AddVariable( "ht 	&var[36] );
   ////reader->AddVariable( "myvar1 := var1+var2", &var1 );
   //reader->AddVariable( "myvar2 := var1-var2", &var2 );
   //reader->AddVariable( "var3",                &var3 );
   //reader->AddVariable( "var4",                &var4 );

   // Spectator variables declared in the training have to be added to the reader, too
   Float_t spec1,spec2;
   reader->AddSpectator( "spec1 := l_pt1",   &spec1 );
   reader->AddSpectator( "spec2 := pfMET_Corr",   &spec2 );

   // Book the MVA methods

   TString dir    = "dataset/weights/";
   TString prefix = "TMVAClassification";

   // Book method(s)
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = TString(it->first) + TString(" method");
         TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         reader->BookMVA( methodName, weightfile );
      }
   }

   // Book output histograms
   UInt_t nbin = 100;
   TH1F *histBdt(0);
   TH1F *histBdtG(0);
   TH1F *histBdtB(0);
   TH1F *histBdtD(0);
   TH1F *histBdtF(0);

   if (Use["BDT"])           histBdt     = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.8, 0.8 );
   if (Use["BDTG"])          histBdtG    = new TH1F( "MVA_BDTG",          "MVA_BDTG",          nbin, -1.0, 1.0 );
   if (Use["BDTB"])          histBdtB    = new TH1F( "MVA_BDTB",          "MVA_BDTB",          nbin, -1.0, 1.0 );
   if (Use["BDTD"])          histBdtD    = new TH1F( "MVA_BDTD",          "MVA_BDTD",          nbin, -0.8, 0.8 );
   if (Use["BDTF"])          histBdtF    = new TH1F( "MVA_BDTF",          "MVA_BDTF",          nbin, -1.0, 1.0 );


   // Prepare input tree (this must be replaced by your data source)
   // in this example, there is a toy tree with signal and one with background events
   // we'll later on use only the "signal" events for the test in this example.
   //
   TFile *input(0);
   //TString fname = "./tmva_class_example.root";
   if (!gSystem->AccessPathName( fname )) {
      input = TFile::Open( fname ); // check if file in local directory exists
      //input = TFile::Open( "root://cmsxrootd.fnal.gov/"+ fname ); // check if file in local directory exists
   }
   if (!input) {
      std::cout << "ERROR: could not open data file : "<< fname << std::endl;
      exit(1);
   }
   std::cout << "--- TMVAClassificationApp    : Using input file: " << input->GetName() << std::endl;



   //----------------------------------------------
   //  Below patch: Grab input root file name only without path; For the output file name in $PWD
   //----------------------------------------------
   TString tok;
   TString OutFileName = "";
   Ssiz_t from = 0;
   while (fname.Tokenize(tok, from, "/")) {
    //cout<<"** "<<tok<<endl;
    OutFileName = tok;
   }
   cout<<"--- Output file name\t:\t"<<OutFileName<<endl;
   //---------------------------------------------- END...
   // Defile output root file 
   TFile *target  = new TFile( OutFileName, "RECREATE" );

   // Event loop

   // Prepare the event tree
   // - Here the variable names have to corresponds to your tree
   // - You can use the same variables as above which is slightly faster,
   //   but of course you can use different ones and copy the values inside the event loop
   //
   std::cout << "--- Select signal sample" << std::endl;
   // Get the tree name from input root file
   TTree* theTree = (TTree*)input->Get("otree");
   
   // Clone the input tree in "Outtree"; to clone input tree in output root file
   TTree *Outtree = theTree->CloneTree(0);;

   // Define all input variables to access from input tree
   Float_t userVar1[37];
   Int_t userVarI[3];
   theTree->SetBranchAddress( "l_pt1",	&userVar1[0] );
   theTree->SetBranchAddress( "l_eta1",	&userVar1[1] );
   theTree->SetBranchAddress( "pfMET_Corr",	&userVar1[2] );
   theTree->SetBranchAddress( "vbf_maxpt_jj_m", 	&userVar1[3] );
   theTree->SetBranchAddress( "v_pt_type0", 	&userVar1[4] );
   theTree->SetBranchAddress( "v_eta_type0", 	&userVar1[5] );
   theTree->SetBranchAddress( "ungroomed_PuppiAK8_jet_pt", 	&userVar1[6] );
   theTree->SetBranchAddress( "ungroomed_PuppiAK8_jet_eta", 	&userVar1[7] );
   theTree->SetBranchAddress( "mass_lvj_type0_PuppiAK8", 	&userVar1[8] );
   theTree->SetBranchAddress( "pt_lvj_type0_PuppiAK8", 	&userVar1[9] );
   theTree->SetBranchAddress( "eta_lvj_type0_PuppiAK8", 	&userVar1[10] );
   theTree->SetBranchAddress( "deltaphi_METak8jet", 	&userVar1[11] );
   //theTree->SetBranchAddress( "PuppiAK8_jet_tau2tau1", 	&userVar1[12] );
   //theTree->SetBranchAddress( "njets", 	&userVar1[13] );
   theTree->SetBranchAddress( "njets", 	&userVarI[0] );
   theTree->SetBranchAddress( "vbf_maxpt_j1_pt", 	&userVar1[14] );
   theTree->SetBranchAddress( "vbf_maxpt_j2_pt", 	&userVar1[15] );
   theTree->SetBranchAddress( "vbf_maxpt_j1_eta", 	&userVar1[16] );
   theTree->SetBranchAddress( "vbf_maxpt_j2_eta", 	&userVar1[17] );
   theTree->SetBranchAddress( "PtBalance_type0", 	&userVar1[18] );
   theTree->SetBranchAddress( "BosonCentrality_type0", 	&userVar1[19] );
   theTree->SetBranchAddress( "vbf_maxpt_jj_Deta", 	&userVar1[20] );
   theTree->SetBranchAddress( "PuppiAK8_jet_mass_so_corr", 	&userVar1[21] );
   theTree->SetBranchAddress( "ZeppenfeldWH", &userVar1[22]);
   theTree->SetBranchAddress( "ZeppenfeldWL_type0", &userVar1[23] ); 
   //theTree->SetBranchAddress( "ZeppenfeldWH/DEtajj := ZeppenfeldWH/vbf_maxpt_jj_Deta", &userVar1[22]);
   //theTree->SetBranchAddress( "ZeppenfeldWL/DEtajj := ZeppenfeldWL_type0/vbf_maxpt_jj_Deta", &userVar1[23] ); 
   theTree->SetBranchAddress( "costheta1_type0", 	&userVar1[24] );
   theTree->SetBranchAddress( "costheta2_type0", 	&userVar1[25] );
   theTree->SetBranchAddress( "phi_type0", 	&userVar1[26] );
   theTree->SetBranchAddress( "phi1_type0", 	&userVar1[27] );
   theTree->SetBranchAddress( "costhetastar_type0", 	&userVar1[28] );
   //theTree->SetBranchAddress( "WWRapidity", 	&userVar1[29] );
   //theTree->SetBranchAddress( "RpT_type0", 	&userVar1[30] );
   theTree->SetBranchAddress( "v_mt_type0", 	&userVar1[31] );
   //theTree->SetBranchAddress( "LeptonProjection_type0", 	&userVar1[32] );
   //theTree->SetBranchAddress( "VBSCentrality_type0", 	&userVar1[33] );
   //theTree->SetBranchAddress( "ZeppenfeldWL_type0", 	&userVar1[34] );
   //theTree->SetBranchAddress( "ZeppenfeldWH",	&userVar1[35] );
   //theTree->SetBranchAddress( "ht 	&userVar1[36] );
 
   // Define the branch to save in output root file
   Float_t BDT_response;
   TBranch *bR = Outtree->Branch("BDT_response",&BDT_response);

   // Efficiency calculator for cut method
   Int_t    nSelCutsGA = 0;
   Double_t effS       = 0.7;

   std::vector<Float_t> vecVar(4); // vector for EvaluateMVA tests

   std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
   TStopwatch sw;
   sw.Start();
   for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {

      if (ievt%50000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      theTree->GetEntry(ievt);

      //var1 = userVar1 + userVar2;
      //var2 = userVar1 - userVar2;
      var[0]  = userVar1[0];
      var[1]  = userVar1[1];
      var[2]  = userVar1[2];
      var[3]  = userVar1[3];
      var[4]  = userVar1[4];
      var[5]  = userVar1[5];
      var[6]  = userVar1[6];
      var[7]  = userVar1[7];
      var[8]  = userVar1[8];
      var[9]  = userVar1[9];
      var[10] = userVar1[10];
      var[11] = userVar1[11];
      //var[12] = userVar1[12];
      //var[13] = userVar1[13];
      var[13] = userVarI[0];
      var[14] = userVar1[14];
      var[15] = userVar1[15];
      var[16] = userVar1[16];
      var[17] = userVar1[17];
      var[18] = userVar1[18];
      var[19] = userVar1[19];
      var[20] = userVar1[20];
      var[21] = userVar1[21];
      var[22] = userVar1[22]/userVar1[20];
      var[23] = userVar1[23]/userVar1[20];
      //var[22] = userVar1[22];
      //var[23] = userVar1[23];
      var[24] = userVar1[24];
      var[25] = userVar1[25];
      var[26] = userVar1[26];
      var[27] = userVar1[27];
      var[28] = userVar1[28];
      //var[29] = userVar1[29];
      //var[30] = userVar1[30];
      var[31] = userVar1[31];
      //var[32] = userVar1[32];
      //var[33] = userVar1[33];

      // Return the MVA outputs and fill into histograms

      if (Use["BDT"          ])   histBdt    ->Fill( reader->EvaluateMVA( "BDT method"           ) );
      if (Use["BDTG"         ])   
      {
      		histBdtG   ->Fill( reader->EvaluateMVA( "BDTG method"          ) );
      		BDT_response = reader->EvaluateMVA( "BDTG method"          );
      }
      if (Use["BDTB"         ])   histBdtB   ->Fill( reader->EvaluateMVA( "BDTB method"          ) );
      if (Use["BDTD"         ])   histBdtD   ->Fill( reader->EvaluateMVA( "BDTD method"          ) );
      if (Use["BDTF"         ])   histBdtF   ->Fill( reader->EvaluateMVA( "BDTF method"          ) );

   Outtree->Fill();
   }
   Outtree->Write();

   // Get elapsed time
   sw.Stop();
   std::cout << "--- End of event loop: "; sw.Print();



   // Write histograms
   if (Use["BDT"          ])   histBdt    ->Write();
   if (Use["BDTG"         ])   histBdtG   ->Write();
   if (Use["BDTB"         ])   histBdtB   ->Write();
   if (Use["BDTD"         ])   histBdtD   ->Write();
   if (Use["BDTF"         ])   histBdtF   ->Write();

   target->Close();

   std::cout << "--- Created root file: \""<<OutFileName<<"\" containing the MVA output histograms" << std::endl;

   delete reader;

   std::cout << "==> TMVAClassificationApplication is done!" << std::endl << std::endl;
}

int main( int argc, char** argv )
{
   TString methodList;
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(",");
      methodList += regMethod;
   }
   TMVAClassificationApplication(methodList);
   return 0;
}
