/// \file
/// \ingroup tutorial_tmva
/// \notebook -nodraw
/// This macro provides examples for the training and testing of the
/// TMVA classifiers.
///
/// As input data is used a toy-MC sample consisting of four Gaussian-distributed
/// and linearly correlated input variables.
/// The methods to be used can be switched on and off by means of booleans, or
/// via the prompt command, for example:
///
///     root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)
///
/// (note that the backslashes are mandatory)
/// If no method given, a default set of classifiers is used.
/// The output file "TMVA.root" can be analysed with the use of dedicated
/// macros (simply say: root -l <macro.C>), which can be conveniently
/// invoked through a GUI that will appear at the end of the run of this macro.
/// Launch the GUI via the command:
///
///     root -l ./TMVAGui.C
///
/// You can also compile and run the example with the following commands
///
///     make
///     ./TMVAClassification <Methods>
///
/// where: `<Methods> = "method1 method2"` are the TMVA classifier names
/// example:
///
///     ./TMVAClassification Fisher LikelihoodPCA BDT
///
/// If no method given, a default set is of classifiers is used
///
/// - Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis
/// - Package   : TMVA
/// - Root Macro: TMVAClassification
///
/// \macro_output
/// \macro_code
/// \author Andreas Hoecker


#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

int TMVAClassification( TString myMethodList = "" )
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // Methods to be processed can be given as an argument; use format:
   //
   //     mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // Cut optimisation
   Use["Cuts"]            = 0;
   Use["CutsD"]           = 0;
   Use["CutsPCA"]         = 0;
   Use["CutsGA"]          = 0;
   Use["CutsSA"]          = 0;
   //
   // 1-dimensional likelihood ("naive Bayes estimator")
   Use["Likelihood"]      = 0;
   Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
   Use["LikelihoodPCA"]   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
   Use["LikelihoodKDE"]   = 0;
   Use["LikelihoodMIX"]   = 0;
   //
   // Mutidimensional likelihood and Nearest-Neighbour methods
   Use["PDERS"]           = 0;
   Use["PDERSD"]          = 0;
   Use["PDERSPCA"]        = 0;
   Use["PDEFoam"]         = 0;
   Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
   Use["KNN"]             = 0; // k-nearest neighbour method
   //
   // Linear Discriminant Analysis
   Use["LD"]              = 0; // Linear Discriminant identical to Fisher
   Use["Fisher"]          = 0;
   Use["FisherG"]         = 0;
   Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
   Use["HMatrix"]         = 0;
   //
   // Function Discriminant analysis
   Use["FDA_MC"]          = 0;
   Use["FDA_MT"]          = 0;
   Use["FDA_GAMT"]        = 0;
   Use["FDA_MCMT"]        = 0;
   //
   // Neural Networks (all are feed-forward Multilayer Perceptrons)
   Use["MLP"]             = 0; // Recommended ANN
   Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
   Use["MLPBNN"]          = 0; // Recommended ANN with BFGS training method and bayesian regulator
   Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
   Use["TMlpANN"]         = 0; // ROOT's own ANN
   Use["DNN_GPU"]         = 0; // CUDA-accelerated DNN training.
   Use["DNN_CPU"]         = 0; // Multi-core accelerated DNN.
   //
   // Support Vector Machine
   Use["SVM"]             = 0;
   //
   // Boosted Decision Trees
   Use["BDT"]             = 0; // uses Adaptive Boost
   Use["BDTG"]            = 1; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
   //
   // Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
   Use["RuleFit"]         = 0;
   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return 1;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // Here the preparation phase begins

   // Read training and test data
   // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
//    TFile * inputSignal1 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWminusTo2JJJ_EWK_LO_SM.root");
//    TFile * inputSignal2 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JWminusToLNuJJ_EWK_LO_SM.root");
//    TFile * inputSignal3 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWplusTo2JJJ_EWK_LO_SM.root");
//    TFile * inputSignal4 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuWminusTo2JJJ_EWK_LO_SM.root");
//    TFile * inputSignal5 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusTo2JZTo2LJJ_EWK_LO_SM.root");
//    TFile * inputSignal6 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuZTo2JJJ_EWK_LO_SM.root");
//    TFile * inputSignal7 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JZTo2LJJ_EWK_LO_SM.root");
//    TFile * inputSignal8 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuZTo2JJJ_EWK_LO_SM.root");
//    TFile * inputSignal9 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZTo2LZTo2JJJ_EWK_LO_SM.root");
    
    TFile * inputSignal1 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWminusTo2JJJ_EWK_LO_aQGC.root");
    TFile * inputSignal2 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JWminusToLNuJJ_EWK_LO_aQGC.root");
    TFile * inputSignal3 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWplusTo2JJJ_EWK_LO_aQGC.root");
    TFile * inputSignal4 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuWminusTo2JJJ_EWK_LO_aQGC.root");
    TFile * inputSignal5 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusTo2JZTo2LJJ_EWK_LO_aQGC.root");
    TFile * inputSignal6 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuZTo2JJJ_EWK_LO_aQGC.root");
    TFile * inputSignal7 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JZTo2LJJ_EWK_LO_aQGC.root");
    TFile * inputSignal8 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuZTo2JJJ_EWK_LO_aQGC.root");
    TFile * inputSignal9 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZTo2LZTo2JJJ_EWK_LO_aQGC.root");
    
    TFile * inputBkg_Wjet1 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_100To200.root");
    TFile * inputBkg_Wjet2 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_200To400.root");
    TFile * inputBkg_Wjet3 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_400To600.root");
    TFile * inputBkg_Wjet4 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_600To800.root");
    TFile * inputBkg_Wjet5 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_800To1200.root");
    TFile * inputBkg_Wjet6 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_1200To2500.root");
    TFile * inputBkg_Wjet7 = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_2500ToInf.root");

    //TFile * inputBkg_DY0  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DYToLL_0J_13TeV.root");
    //TFile * inputBkg_DY1  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DYToLL_1J_13TeV.root");
    //TFile * inputBkg_DY2  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DYToLL_2J_13TeV.root");
      
    TFile * inputBkg_Top1  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ST_tW_antitop_5f_NoFullyHadronicDecays.root");
    TFile * inputBkg_Top2  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ST_tW_top_5f_NoFullyHadronicDecays.root");
    TFile * inputBkg_Top3  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTToSemilepton.root");
    TFile * inputBkg_Top4  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTWJetsToLNu.root");
    TFile * inputBkg_Top5  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTWJetsToQQ.root");
    TFile * inputBkg_Top6  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTZToLLNuNu_M-10.root");
    TFile * inputBkg_Top7  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTZToQQ.root");
    
    TFile * inputBkg_Diboson1  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWminusTo2JJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson2  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JWminusToLNuJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson3  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWplusTo2JJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson4  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuWminusTo2JJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson5  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuZTo2JJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson6  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JZTo2LJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson7  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuZTo2JJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson8  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusTo2JZTo2LJJ_QCD_LO_SM.root");
    TFile * inputBkg_Diboson9  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZTo2LZTo2JJJ_QCD_LO_SM.root");

    //TFile * inputBkg_VV1  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WWTo1L1Nu2Q_13TeV.root");
    //TFile * inputBkg_VV2  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WWZ_13TeV_amcatnlo.root");
    //TFile * inputBkg_VV3  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WZTo1L1Nu2Q_13TeV.root");
    //TFile * inputBkg_VV4  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZZTo2L2Q_13TeV.root");
    //TFile * inputBkg_VV5  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZZZ_13TeV_amcatnlo.root");
    //TFile * inputBkg_VV6  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZZ_13TeV_pythia8.root");


        
    //TFile * inputBkg_QCD1  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT100to200.root");
    //TFile * inputBkg_QCD2  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT200to300.root");
    //TFile * inputBkg_QCD3  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT300to500.root");
    //TFile * inputBkg_QCD4  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT500to700.root");
    //TFile * inputBkg_QCD5  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT700to1000.root");
    //TFile * inputBkg_QCD6  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT1000to1500.root");
    //TFile * inputBkg_QCD7  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT1500to2000.root");
    //TFile * inputBkg_QCD8  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/QCD_HT2000toInf.root");
    //TFile * inputBkg_DY1  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DY1JetsToLL.root");
    //TFile * inputBkg_DY2  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DY2JetsToLL.root");
    //TFile * inputBkg_DY3  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DY3JetsToLL.root");
    //TFile * inputBkg_DY4  = TFile::Open("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/DY4JetsToLL.root");

   // Register the training and test trees

    TTree *signalTree1     = (TTree*)inputSignal1->Get("otree");
    TTree *signalTree2     = (TTree*)inputSignal2->Get("otree");
    TTree *signalTree3     = (TTree*)inputSignal3->Get("otree");
    TTree *signalTree4     = (TTree*)inputSignal4->Get("otree");
    TTree *signalTree5     = (TTree*)inputSignal5->Get("otree");
    TTree *signalTree6     = (TTree*)inputSignal6->Get("otree");
    TTree *signalTree7     = (TTree*)inputSignal7->Get("otree");
    TTree *signalTree8     = (TTree*)inputSignal8->Get("otree");
    TTree *signalTree9     = (TTree*)inputSignal9->Get("otree");
    
    TTree *background_Wjet1     = (TTree*)inputBkg_Wjet1->Get("otree");
    TTree *background_Wjet2     = (TTree*)inputBkg_Wjet2->Get("otree");
    TTree *background_Wjet3     = (TTree*)inputBkg_Wjet3->Get("otree");
    TTree *background_Wjet4     = (TTree*)inputBkg_Wjet4->Get("otree");
    TTree *background_Wjet5     = (TTree*)inputBkg_Wjet5->Get("otree");
    TTree *background_Wjet6     = (TTree*)inputBkg_Wjet6->Get("otree");
    TTree *background_Wjet7     = (TTree*)inputBkg_Wjet7->Get("otree");
   
    //TTree *background_Zjet0     = (TTree*)inputBkg_DY0->Get("otree");
    //TTree *background_Zjet1     = (TTree*)inputBkg_DY1->Get("otree");
    //TTree *background_Zjet2     = (TTree*)inputBkg_DY2->Get("otree");

    TTree *background_Top1     = (TTree*)inputBkg_Top1->Get("otree");
    TTree *background_Top2     = (TTree*)inputBkg_Top2->Get("otree");
    TTree *background_Top3     = (TTree*)inputBkg_Top3->Get("otree");
    TTree *background_Top4     = (TTree*)inputBkg_Top4->Get("otree");
    TTree *background_Top5     = (TTree*)inputBkg_Top5->Get("otree");
    TTree *background_Top6     = (TTree*)inputBkg_Top6->Get("otree");
    TTree *background_Top7     = (TTree*)inputBkg_Top7->Get("otree");
    
    TTree *background_Diboson1 =(TTree*)inputBkg_Diboson1->Get("otree");
    TTree *background_Diboson2 =(TTree*)inputBkg_Diboson2->Get("otree");
    TTree *background_Diboson3 =(TTree*)inputBkg_Diboson3->Get("otree");
    TTree *background_Diboson4 =(TTree*)inputBkg_Diboson4->Get("otree");
    TTree *background_Diboson5 =(TTree*)inputBkg_Diboson5->Get("otree");
    TTree *background_Diboson6 =(TTree*)inputBkg_Diboson6->Get("otree");
    TTree *background_Diboson7 =(TTree*)inputBkg_Diboson7->Get("otree");
    TTree *background_Diboson8 =(TTree*)inputBkg_Diboson8->Get("otree");
    TTree *background_Diboson9 =(TTree*)inputBkg_Diboson9->Get("otree");

    //TTree *background_VV1 =(TTree*)inputBkg_VV1->Get("otree");
    //TTree *background_VV2 =(TTree*)inputBkg_VV2->Get("otree");
    //TTree *background_VV3 =(TTree*)inputBkg_VV3->Get("otree");
    //TTree *background_VV4 =(TTree*)inputBkg_VV4->Get("otree");
    //TTree *background_VV5 =(TTree*)inputBkg_VV5->Get("otree");
    //TTree *background_VV6 =(TTree*)inputBkg_VV6->Get("otree");


    //TTree *background_QCD1 =(TTree*)inputBkg_QCD1->Get("otree");
    //TTree *background_QCD2 =(TTree*)inputBkg_QCD2->Get("otree");
    //TTree *background_QCD3 =(TTree*)inputBkg_QCD3->Get("otree");
    //TTree *background_QCD4 =(TTree*)inputBkg_QCD4->Get("otree");
    //TTree *background_QCD5 =(TTree*)inputBkg_QCD5->Get("otree");
    //TTree *background_QCD6 =(TTree*)inputBkg_QCD6->Get("otree");
    //TTree *background_QCD7 =(TTree*)inputBkg_QCD7->Get("otree");
    //TTree *background_QCD8 =(TTree*)inputBkg_QCD8->Get("otree");
    //TTree *background_DY1 =(TTree*)inputBkg_DY1->Get("otree");
    //TTree *background_DY2 =(TTree*)inputBkg_DY2->Get("otree");
    //TTree *background_DY3 =(TTree*)inputBkg_DY3->Get("otree");
    //TTree *background_DY4 =(TTree*)inputBkg_DY4->Get("otree");
   

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory is
   // the only TMVA object you have to interact with
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );
                                               //"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");
   // If you wish to modify default settings
   // (please check "src/Config.h" to see all available global options)
   //
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
   dataloader->AddVariable( "l_pt1",  "", "GeV", 'F' );
   dataloader->AddVariable( "l_eta1",     'F' );
   dataloader->AddVariable( "pfMET_Corr", "pfMET", "GeV", 'F' );
   // dataloader->AddVariable( "pfMET_Corr_phi", "MET #phi", "", 'F' );
   dataloader->AddVariable( "vbf_maxpt_jj_m", "mjj", "GeV", 'F' );
   dataloader->AddVariable( "v_pt_type0", "Leptonic W p_{T}", "GeV", 'F' );
   dataloader->AddVariable( "v_eta_type0", "Leptonic W #eta", "", 'F' );
   dataloader->AddVariable( "ungroomed_PuppiAK8_jet_pt", "AK8 p_{T}", "GeV", 'F' );
   dataloader->AddVariable( "ungroomed_PuppiAK8_jet_eta", "AK8 #eta", "", 'F' );
   dataloader->AddVariable( "mass_lvj_type0_PuppiAK8", "mWW", "GeV", 'F' );
   dataloader->AddVariable( "pt_lvj_type0_PuppiAK8", "WW p_{T}", "GeV", 'F' );
   dataloader->AddVariable( "eta_lvj_type0_PuppiAK8", "WW #eta", "", 'F' );
   dataloader->AddVariable( "deltaphi_METak8jet", "#delta #phi (MET, AK8)", "", 'F' );
   //dataloader->AddVariable( "PuppiAK8_jet_tau2tau1", "#tau2/#tau1", "", 'F' );
   dataloader->AddVariable( "njets", "njets", "", 'F' );
   dataloader->AddVariable( "vbf_maxpt_j1_pt", "VBF J1 p_{T}", "GeV", 'F' );
   dataloader->AddVariable( "vbf_maxpt_j2_pt", "VBF J2 p_{T}", "GeV", 'F' );
   dataloader->AddVariable( "vbf_maxpt_j1_eta", "VBF J1 #eta", "", 'F' );
   dataloader->AddVariable( "vbf_maxpt_j2_eta", "VBF J2 #eta", "", 'F' );
   dataloader->AddVariable( "PtBalance_type0", "pT Balance", "", 'F' );
   dataloader->AddVariable( "BosonCentrality_type0", "Boson Centrality", "", 'F' );
   dataloader->AddVariable( "vbf_maxpt_jj_Deta", "#delta #eta_{jj}", "", 'F' );
   dataloader->AddVariable( "PuppiAK8_jet_mass_so_corr", "AK8 mass", "GeV", 'F' );
   dataloader->AddVariable( "ZeppenfeldWH/DEtajj := ZeppenfeldWH/vbf_maxpt_jj_Deta", "ZeppenfeldWH/#Delta#eta_{jj}", "", 'F' );
   dataloader->AddVariable( "ZeppenfeldWL/DEtajj := ZeppenfeldWL_type0/vbf_maxpt_jj_Deta", "ZeppenfeldWL_type0/#Delta#eta_{jj}", "", 'F' );
   dataloader->AddVariable( "costheta1_type0", "cos(#theta 1)", "", 'F' );
   dataloader->AddVariable( "costheta2_type0", "cos(#theta 2)", "", 'F' );
   dataloader->AddVariable( "phi_type0", "phi_type0", "", 'F' );
   dataloader->AddVariable( "phi1_type0", "phi1_type0", "", 'F' );
   dataloader->AddVariable( "costhetastar_type0", "costhetastar_type0", "", 'F' );
   //dataloader->AddVariable( "WWRapidity", "yWW", "", 'F' );
   // dataloader->AddVariable( "RpT_type0", "Rp_{T}", "", 'F' );
   // dataloader->AddVariable( "njets", "njets", "", 'F' );
   dataloader->AddVariable( "v_mt_type0", "mT (Leptonic W)", "", 'F' );
   //dataloader->AddVariable( "LeptonProjection_type0", "Lepton Proj.", "", 'F' );
   //dataloader->AddVariable( "VBSCentrality_type0", "VBS Centrality", "", 'F' );
   //dataloader->AddVariable( "ZeppenfeldWL_type0", "Zeppenfeld (Wlep)", "", 'F' );
   //dataloader->AddVariable( "ZeppenfeldWH", "Zeppenfeld (Whad)", "", 'F' );

   // dataloader->AddVariable( "ht := ungroomed_PuppiAK8_jet_pt+vbf_maxpt_j1_pt+vbf_maxpt_j2_pt", "HT", "GeV", 'F');

   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables

   dataloader->AddSpectator( "spec1 := l_pt1*2",  "Spectator 1", "units", 'F' );
   dataloader->AddSpectator( "spec2 := pfMET_Corr*3",  "Spectator 2", "units", 'F' );


   // global event weights per tree (see below for setting event-wise weights)
   Double_t signalWeight     = 35867.06;
   Double_t backgroundWeight = 35867.06;

   // You can add an arbitrary number of signal or background trees
   dataloader->AddSignalTree    ( signalTree1,     signalWeight*0.9114/1981107.0 );
    dataloader->AddSignalTree    ( signalTree2,     signalWeight*0.9107/1923847.0 );
    dataloader->AddSignalTree    ( signalTree3,     signalWeight*0.08793/198848.0 );
    dataloader->AddSignalTree    ( signalTree4,     signalWeight*0.03259/199525.0 );
    dataloader->AddSignalTree    ( signalTree5,     signalWeight*0.02982/198896.0 );
    dataloader->AddSignalTree    ( signalTree6,     signalWeight*0.1/169938.0 );
    dataloader->AddSignalTree    ( signalTree7,     signalWeight*0.05401/188998.0 );
    dataloader->AddSignalTree    ( signalTree8,     signalWeight*0.1825/393171.0 );
    dataloader->AddSignalTree    ( signalTree9,     signalWeight*0.01589/99997.0 );
    
    dataloader->AddBackgroundTree( background_Wjet1, backgroundWeight*1627.45/79165703.0 );
    dataloader->AddBackgroundTree( background_Wjet2, backgroundWeight*435.24/38925816.0 );
    dataloader->AddBackgroundTree( background_Wjet3, backgroundWeight*59.18/7754252.0);
    dataloader->AddBackgroundTree( background_Wjet4, backgroundWeight*14.58/18578604.0);
    dataloader->AddBackgroundTree( background_Wjet5, backgroundWeight*6.655/7688957.0 );
    dataloader->AddBackgroundTree( background_Wjet6, backgroundWeight*1.60809/6708656.0 );
    dataloader->AddBackgroundTree( background_Wjet7, backgroundWeight*0.0389136/2520618.0 );

    //dataloader->AddBackgroundTree( background_Zjet0, backgroundWeight*4274.1645/(1749590.0-2*161090.0) );
   // dataloader->AddBackgroundTree( background_Zjet1, backgroundWeight*1115.0463/(35950579.0-9808428.0) );
    //dataloader->AddBackgroundTree( background_Zjet2, backgroundWeight*222.57608/(21571879.0-2*7649488.0) );

    dataloader->AddBackgroundTree( background_Top1, backgroundWeight*19.5741/(5424956.0-0.0) );
   // dataloader->AddBackgroundTree( background_Top2, backgroundWeight*19.5741/(5372830.0-0.0) );
    dataloader->AddBackgroundTree( background_Top3, backgroundWeight*364.3/(91832423.0-0.0) );
    dataloader->AddBackgroundTree( background_Top4, backgroundWeight*0.2043/(5280251.0-2*1282079.0) );
    dataloader->AddBackgroundTree( background_Top5, backgroundWeight*0.4062/(833257.0-2*201483.0) );
    dataloader->AddBackgroundTree( background_Top6, backgroundWeight*0.2529/(7969186.0-2*2126557.0) );
    dataloader->AddBackgroundTree( background_Top7, backgroundWeight*0.5297/(749367.0-2*199113.0) );

    dataloader->AddBackgroundTree( background_Diboson1, backgroundWeight*5.633/(3949170.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson2, backgroundWeight*5.633/(3994663.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson3, backgroundWeight*0.07584/(99992.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson4, backgroundWeight*0.03203/(99657.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson5, backgroundWeight*1.938/(1991348.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson6, backgroundWeight*0.575/(499432.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson7, backgroundWeight*1.166/(981540.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson8, backgroundWeight*0.3488/(489280.0-0.0) );
    dataloader->AddBackgroundTree( background_Diboson9, backgroundWeight*0.3449/(49999.0-0.0) );
    
    //dataloader->AddBackgroundTree( background_VV1, backgroundWeight*49.997/(5057358.0-2*953706.0) );
    //dataloader->AddBackgroundTree( background_VV2, backgroundWeight*0.1651/(269990.0-2*15372.0) );
    //dataloader->AddBackgroundTree( background_VV3, backgroundWeight*10.71/(23766546.0-2*4986275.0) );
    //dataloader->AddBackgroundTree( background_VV4, backgroundWeight*3.22/(15345161.0-2*2828391.0) );
    //dataloader->AddBackgroundTree( background_VV5, backgroundWeight*0.01398/(249232.0-2*18020.0) );
    //dataloader->AddBackgroundTree( background_VV6, backgroundWeight*10.31/(990051.0-2*0.0) );

    //dataloader->AddBackgroundTree( background_QCD1, backgroundWeight*27990000.0/(1) );
    //dataloader->AddBackgroundTree( background_QCD2, backgroundWeight*1712000.0/(1) );
    //dataloader->AddBackgroundTree( background_QCD3, backgroundWeight*347700.0/(26924854.0) );
    //dataloader->AddBackgroundTree( background_QCD4, backgroundWeight*32100.0/(53744436.0) );
    //dataloader->AddBackgroundTree( background_QCD5, backgroundWeight*6831.0/(15578398.0) );
    //dataloader->AddBackgroundTree( background_QCD6, backgroundWeight*1207.0/(13080692.0) );
    //dataloader->AddBackgroundTree( background_QCD7, backgroundWeight*119.9/(11624720.0) );
    //dataloader->AddBackgroundTree( background_QCD8, backgroundWeight*25.24/(5875869.0) );
    //dataloader->AddBackgroundTree( background_DY1, backgroundWeight*/(-2*) );
    //dataloader->AddBackgroundTree( background_DY2, backgroundWeight*/(-2*) );
    //dataloader->AddBackgroundTree( background_DY3, backgroundWeight*/(-2*) );
    //dataloader->AddBackgroundTree( background_DY4, backgroundWeight*/(-2*) );

    
   // To give different trees for training and testing, do as follows:
   //
   //     dataloader->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
   //     dataloader->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );

   // Use the following code instead of the above two or four lines to add signal and background
   // training and test events "by hand"
   // NOTE that in this case one should not give expressions (such as "var1+var2") in the input
   //      variable definition, but simply compute the expression before adding the event
   // ```cpp
   // // --- begin ----------------------------------------------------------
   // std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
   // Float_t  treevars[4], weight;
   //
   // // Signal
   // for (UInt_t ivar=0; ivar<4; ivar++) signalTree->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   // for (UInt_t i=0; i<signalTree->GetEntries(); i++) {
   //    signalTree->GetEntry(i);
   //    for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //    // add training and test events; here: first half is training, second is testing
   //    // note that the weight can also be event-wise
   //    if (i < signalTree->GetEntries()/2.0) dataloader->AddSignalTrainingEvent( vars, signalWeight );
   //    else                              dataloader->AddSignalTestEvent    ( vars, signalWeight );
   // }
   //
   // // Background (has event weights)
   // background->SetBranchAddress( "weight", &weight );
   // for (UInt_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   // for (UInt_t i=0; i<background->GetEntries(); i++) {
   //    background->GetEntry(i);
   //    for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //    // add training and test events; here: first half is training, second is testing
   //    // note that the weight can also be event-wise
   //    if (i < background->GetEntries()/2) dataloader->AddBackgroundTrainingEvent( vars, backgroundWeight*weight );
   //    else                                dataloader->AddBackgroundTestEvent    ( vars, backgroundWeight*weight );
   // }
   // // --- end ------------------------------------------------------------
   // ```
   // End of tree registration

   // Set individual event weights (the variables must exist in the original TTree)
   // -  for signal    : `dataloader->SetSignalWeightExpression    ("weight1*weight2");`
   // -  for background: `dataloader->SetBackgroundWeightExpression("weight1*weight2");`
   dataloader->SetSignalWeightExpression    ("genWeight*LHEWeight[992]/LHEWeight[0]");
   dataloader->SetBackgroundWeightExpression( "genWeight" );

   // Apply additional cuts on the signal and background samples (can be different)
    TCut mycuts = "(type==1 || type==0) && (l_pt2<0) && ((ZeppenfeldWL_type0/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWL_type0/vbf_maxpt_jj_Deta<1.0)) && ((ZeppenfeldWH/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWH/vbf_maxpt_jj_Deta<1.0)) && (l_pt1>30) && (vbf_maxpt_j1_pt>30) && (vbf_maxpt_j2_pt>30)  &&  (nBTagJet_loose==0) && (vbf_maxpt_jj_m>500) && (pfMET_Corr>50) && ((ungroomed_PuppiAK8_jet_pt>200)&&(abs(ungroomed_PuppiAK8_jet_eta)<2.4)) && ((PuppiAK8_jet_mass_so_corr>40) && (PuppiAK8_jet_mass_so_corr<150)) && (PuppiAK8_jet_tau2tau1<0.55)"; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    TCut mycutb = "(type==1 || type==0) && (l_pt2<0) && ((ZeppenfeldWL_type0/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWL_type0/vbf_maxpt_jj_Deta<1.0)) && ((ZeppenfeldWH/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWH/vbf_maxpt_jj_Deta<1.0)) && (l_pt1>30) && (vbf_maxpt_j1_pt>30) && (vbf_maxpt_j2_pt>30)  &&  (nBTagJet_loose==0) && (vbf_maxpt_jj_m>500) && (pfMET_Corr>50) && ((ungroomed_PuppiAK8_jet_pt>200)&&(abs(ungroomed_PuppiAK8_jet_eta)<2.4)) && ((PuppiAK8_jet_mass_so_corr>40) && (PuppiAK8_jet_mass_so_corr<150)) && (PuppiAK8_jet_tau2tau1<0.55)"; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    //TCut mycuts = "(type==1) && (l_pt2<0) && ( ( PuppiAK8_jet_mass_so_corr>65.0 ) && ( PuppiAK8_jet_mass_so_corr<105.0 )) && (PuppiAK8_jet_tau2tau1<0.55) && (nBTagJet_loose==0) && (vbf_maxpt_jj_m>900) && (BosonCentrality_type0>0)"; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    //TCut mycutb = "(type==1) && (l_pt2<0) && ( ( PuppiAK8_jet_mass_so_corr>65.0 ) && ( PuppiAK8_jet_mass_so_corr<105.0 )) && (PuppiAK8_jet_tau2tau1<0.55) && (nBTagJet_loose==0) && (vbf_maxpt_jj_m>900)"; // for example: TCut mycutb = "abs(var1)<0.5";

   // Tell the dataloader how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used
   // for training, and the other half for testing:
   //
   //    dataloader->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
   //
   // To also specify the number of testing events, use:
   //
   //    dataloader->PrepareTrainingAndTestTree( mycut,
   //         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
   dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,
                                        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );

   // ### Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   // Cut optimisation
   if (Use["Cuts"])
      factory->BookMethod( dataloader, TMVA::Types::kCuts, "Cuts",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   if (Use["CutsD"])
      factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsD",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

   if (Use["CutsPCA"])
      factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsPCA",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

   if (Use["CutsGA"])
      factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsGA",
                           "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

   if (Use["CutsSA"])
      factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsSA",
                           "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

   // Likelihood ("naive Bayes estimator")
   if (Use["Likelihood"])
      factory->BookMethod( dataloader, TMVA::Types::kLikelihood, "Likelihood",
                           "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );

   // Decorrelated likelihood
   if (Use["LikelihoodD"])
      factory->BookMethod( dataloader, TMVA::Types::kLikelihood, "LikelihoodD",
                           "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );

   // PCA-transformed likelihood
   if (Use["LikelihoodPCA"])
      factory->BookMethod( dataloader, TMVA::Types::kLikelihood, "LikelihoodPCA",
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" );

   // Use a kernel density estimator to approximate the PDFs
   if (Use["LikelihoodKDE"])
      factory->BookMethod( dataloader, TMVA::Types::kLikelihood, "LikelihoodKDE",
                           "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" );

   // Use a variable-dependent mix of splines and kernel density estimator
   if (Use["LikelihoodMIX"])
      factory->BookMethod( dataloader, TMVA::Types::kLikelihood, "LikelihoodMIX",
                           "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" );

   // Test the multi-dimensional probability density estimator
   // here are the options strings for the MinMax and RMS methods, respectively:
   //
   //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
   //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
   if (Use["PDERS"])
      factory->BookMethod( dataloader, TMVA::Types::kPDERS, "PDERS",
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

   if (Use["PDERSD"])
      factory->BookMethod( dataloader, TMVA::Types::kPDERS, "PDERSD",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

   if (Use["PDERSPCA"])
      factory->BookMethod( dataloader, TMVA::Types::kPDERS, "PDERSPCA",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

   // Multi-dimensional likelihood estimator using self-adapting phase-space binning
   if (Use["PDEFoam"])
      factory->BookMethod( dataloader, TMVA::Types::kPDEFoam, "PDEFoam",
                           "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

   if (Use["PDEFoamBoost"])
      factory->BookMethod( dataloader, TMVA::Types::kPDEFoam, "PDEFoamBoost",
                           "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

   // K-Nearest Neighbour classifier (KNN)
   if (Use["KNN"])
      factory->BookMethod( dataloader, TMVA::Types::kKNN, "KNN",
                           "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

   // H-Matrix (chi2-squared) method
   if (Use["HMatrix"])
      factory->BookMethod( dataloader, TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

   // Linear discriminant (same as Fisher discriminant)
   if (Use["LD"])
      factory->BookMethod( dataloader, TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

   // Fisher discriminant (same as LD)
   if (Use["Fisher"])
      factory->BookMethod( dataloader, TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

   // Fisher with Gauss-transformed input variables
   if (Use["FisherG"])
      factory->BookMethod( dataloader, TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

   // Composite classifier: ensemble (tree) of boosted Fisher classifiers
   if (Use["BoostedFisher"])
      factory->BookMethod( dataloader, TMVA::Types::kFisher, "BoostedFisher",
                           "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

   // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
   if (Use["FDA_MC"])
      factory->BookMethod( dataloader, TMVA::Types::kFDA, "FDA_MC",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

   if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( dataloader, TMVA::Types::kFDA, "FDA_GA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=100:Cycles=2:Steps=5:Trim=True:SaveBestGen=1" );

   if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod( dataloader, TMVA::Types::kFDA, "FDA_SA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

   if (Use["FDA_MT"])
      factory->BookMethod( dataloader, TMVA::Types::kFDA, "FDA_MT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

   if (Use["FDA_GAMT"])
      factory->BookMethod( dataloader, TMVA::Types::kFDA, "FDA_GAMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

   if (Use["FDA_MCMT"])
      factory->BookMethod( dataloader, TMVA::Types::kFDA, "FDA_MCMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

   // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
   if (Use["MLP"])
      factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );

   if (Use["MLPBFGS"])
      factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

   if (Use["MLPBNN"])
      factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=60:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators


   // Multi-architecture DNN implementation.
   if (Use["DNN_CPU"] or Use["DNN_GPU"]) {
      // General layout.
      TString layoutString ("Layout=TANH|128,TANH|128,TANH|128,LINEAR");

      // Training strategies.
      TString training0("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.5+0.5+0.5, Multithreading=True");
      TString training1("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
      TString training2("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
      TString trainingStrategyString ("TrainingStrategy=");
      trainingStrategyString += training0 + "|" + training1 + "|" + training2;

      // General Options.
      TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
                          "WeightInitialization=XAVIERUNIFORM");
      dnnOptions.Append (":"); dnnOptions.Append (layoutString);
      dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);

      // Cuda implementation.
      if (Use["DNN_GPU"]) {
         TString gpuOptions = dnnOptions + ":Architecture=GPU";
         factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_GPU", gpuOptions);
      }
      // Multi-core CPU implementation.
      if (Use["DNN_CPU"]) {
         TString cpuOptions = dnnOptions + ":Architecture=CPU";
         factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_CPU", cpuOptions);
      }
   }

   // CF(Clermont-Ferrand)ANN
   if (Use["CFMlpANN"])
      factory->BookMethod( dataloader, TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...

   // Tmlp(Root)ANN
   if (Use["TMlpANN"])
      factory->BookMethod( dataloader, TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

   // Support Vector Machine
   if (Use["SVM"])
      factory->BookMethod( dataloader, TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );

   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTG",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2:NegWeightTreatment=Pray" );

   if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",
                           "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );

   if (Use["BDTB"]) // Bagging
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTB",
                           "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );

   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTD",
                           "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );

   if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTF",
                           "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );

   // RuleFit -- TMVA implementation of Friedman's method
   if (Use["RuleFit"])
      factory->BookMethod( dataloader, TMVA::Types::kRuleFit, "RuleFit",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

   // For an example of the category classifier usage, see: TMVAClassificationCategory
   //
   // --------------------------------------------------------------------------------------------------
   //  Now you can optimize the setting (configuration) of the MVAs using the set of training events
   // STILL EXPERIMENTAL and only implemented for BDT's !
   //
   //     factory->OptimizeAllMethods("SigEffAt001","Scan");
   //     factory->OptimizeAllMethods("ROCIntegral","FitGA");
   //
   // --------------------------------------------------------------------------------------------------

   // Now you can tell the factory to train, test, and evaluate the MVAs
   //
   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;
   delete dataloader;
   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );

   return 0;
}

int main( int argc, char** argv )
{
   // Select methods (don't look at this code - not of interest)
   TString methodList;
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(",");
      methodList += regMethod;
   }
   return TMVAClassification(methodList);
}
