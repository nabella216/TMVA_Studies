#!/usr/bin/env python

# 
# This example is basically the same as $ROOTSYS/tmva/test/TMVAClassification.C
# 

import ROOT

# in order to start TMVA
from ROOT import TMVA
TMVA.Tools.Instance()

# note that it seems to be mandatory to have an
# output file, just passing None to TMVA::Factory(..)
# does not work. Make sure you don't overwrite an
# existing file.

# open input file, get trees, create output file
inputSignal1 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWminusTo2JJJ_EWK_LO_aQGC.root");
inputSignal2 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JWminusToLNuJJ_EWK_LO_aQGC.root");
#inputSignal3 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWplusTo2JJJ_EWK_LO_aQGC.root");
inputSignal4 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuWminusTo2JJJ_EWK_LO_aQGC.root");
inputSignal5 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusTo2JZTo2LJJ_EWK_LO_aQGC.root");
inputSignal6 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuZTo2JJJ_EWK_LO_aQGC.root");
inputSignal7 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JZTo2LJJ_EWK_LO_aQGC.root");
inputSignal8 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuZTo2JJJ_EWK_LO_aQGC.root");
inputSignal9 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZTo2LZTo2JJJ_EWK_LO_aQGC.root");

inputBkg_Wjet1 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_100To200.root");
inputBkg_Wjet2 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_200To400.root");
inputBkg_Wjet3 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_400To600.root");
inputBkg_Wjet4 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_600To800.root");
inputBkg_Wjet5 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_800To1200.root");
inputBkg_Wjet6 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_1200To2500.root");
inputBkg_Wjet7 = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WJetsToLNu_HT_2500ToInf.root");

inputBkg_Top1  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ST_tW_antitop_5f_NoFullyHadronicDecays.root");
inputBkg_Top2  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ST_tW_top_5f_NoFullyHadronicDecays.root");
inputBkg_Top3  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTToSemilepton.root");
inputBkg_Top4  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTWJetsToLNu.root");
inputBkg_Top5  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTWJetsToQQ.root");
inputBkg_Top6  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTZToLLNuNu_M-10.root");
inputBkg_Top7  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/TTZToQQ.root");

inputBkg_Diboson1  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWminusTo2JJJ_QCD_LO_SM.root");
inputBkg_Diboson2  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JWminusToLNuJJ_QCD_LO_SM.root");
inputBkg_Diboson3  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuWplusTo2JJJ_QCD_LO_SM.root");
inputBkg_Diboson4  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuWminusTo2JJJ_QCD_LO_SM.root");
inputBkg_Diboson5  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusToLNuZTo2JJJ_QCD_LO_SM.root");
inputBkg_Diboson6  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WplusTo2JZTo2LJJ_QCD_LO_SM.root");
inputBkg_Diboson7  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusToLNuZTo2JJJ_QCD_LO_SM.root");
inputBkg_Diboson8  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/WminusTo2JZTo2LJJ_QCD_LO_SM.root");
inputBkg_Diboson9  = ROOT.TFile("/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-13_01h34/HaddedFiles/ZTo2LZTo2JJJ_QCD_LO_SM.root");

signalTree1     = inputSignal1.Get("otree");
signalTree2     = inputSignal2.Get("otree");
#signalTree3     = inputSignal3.Get("otree");
signalTree4     = inputSignal4.Get("otree");
signalTree5     = inputSignal5.Get("otree");
signalTree6     = inputSignal6.Get("otree");
signalTree7     = inputSignal7.Get("otree");
signalTree8     = inputSignal8.Get("otree");
signalTree9     = inputSignal9.Get("otree");

background_Wjet1     = inputBkg_Wjet1.Get("otree");
background_Wjet2     = inputBkg_Wjet2.Get("otree");
background_Wjet3     = inputBkg_Wjet3.Get("otree");
background_Wjet4     = inputBkg_Wjet4.Get("otree");
background_Wjet5     = inputBkg_Wjet5.Get("otree");
background_Wjet6     = inputBkg_Wjet6.Get("otree");
background_Wjet7     = inputBkg_Wjet7.Get("otree");

background_Top1     = inputBkg_Top1.Get("otree");
background_Top2     = inputBkg_Top2.Get("otree");
background_Top3     = inputBkg_Top3.Get("otree");
background_Top4     = inputBkg_Top4.Get("otree");
background_Top5     = inputBkg_Top5.Get("otree");
background_Top6     = inputBkg_Top6.Get("otree");
background_Top7     = inputBkg_Top7.Get("otree");

background_Diboson1 =inputBkg_Diboson1.Get("otree");
background_Diboson2 =inputBkg_Diboson2.Get("otree");
background_Diboson3 =inputBkg_Diboson3.Get("otree");
background_Diboson4 =inputBkg_Diboson4.Get("otree");
background_Diboson5 =inputBkg_Diboson5.Get("otree");
background_Diboson6 =inputBkg_Diboson6.Get("otree");
background_Diboson7 =inputBkg_Diboson7.Get("otree");
background_Diboson8 =inputBkg_Diboson8.Get("otree");
background_Diboson9 =inputBkg_Diboson9.Get("otree");

fout = ROOT.TFile("TMVA.root","RECREATE")

# define factory with options
factory = TMVA.Factory("TMVAClassification", fout,
                            ":".join([    "!V",
                                          "!Silent",
                                          "Color",
                                          "DrawProgressBar",
                                          "Transformations=I",
                                          "AnalysisType=Classification"]
                                     ))

# Set Verbosity
factory.SetVerbose(True)

dataloader = TMVA.DataLoader("dataset")

# add discriminating variables for training
#dataloader.AddVariable("var0","F")
#dataloader.AddVariable("var1","F")
dataloader.AddVariable( "l_pt1",  "", "GeV", 'F' );
dataloader.AddVariable( "l_eta1",     'F' );
dataloader.AddVariable( "pfMET_Corr", "pfMET", "GeV", 'F' );
# dataloader.AddVariable( "pfMET_Corr_phi", "MET #phi", "", 'F' );
dataloader.AddVariable( "vbf_maxpt_jj_m", "mjj", "GeV", 'F' );
dataloader.AddVariable( "v_pt_type0", "Leptonic W p_{T}", "GeV", 'F' );
dataloader.AddVariable( "v_eta_type0", "Leptonic W #eta", "", 'F' );
dataloader.AddVariable( "ungroomed_PuppiAK8_jet_pt", "AK8 p_{T}", "GeV", 'F' );
dataloader.AddVariable( "ungroomed_PuppiAK8_jet_eta", "AK8 #eta", "", 'F' );
dataloader.AddVariable( "mass_lvj_type0_PuppiAK8", "mWW", "GeV", 'F' );
dataloader.AddVariable( "pt_lvj_type0_PuppiAK8", "WW p_{T}", "GeV", 'F' );
dataloader.AddVariable( "eta_lvj_type0_PuppiAK8", "WW #eta", "", 'F' );
dataloader.AddVariable( "deltaphi_METak8jet", "#delta #phi (MET, AK8)", "", 'F' );
#dataloader.AddVariable( "PuppiAK8_jet_tau2tau1", "#tau2/#tau1", "", 'F' );
dataloader.AddVariable( "njets", "njets", "", 'F' );
dataloader.AddVariable( "vbf_maxpt_j1_pt", "VBF J1 p_{T}", "GeV", 'F' );
dataloader.AddVariable( "vbf_maxpt_j2_pt", "VBF J2 p_{T}", "GeV", 'F' );
dataloader.AddVariable( "vbf_maxpt_j1_eta", "VBF J1 #eta", "", 'F' );
dataloader.AddVariable( "vbf_maxpt_j2_eta", "VBF J2 #eta", "", 'F' );
dataloader.AddVariable( "PtBalance_type0", "pT Balance", "", 'F' );
dataloader.AddVariable( "BosonCentrality_type0", "Boson Centrality", "", 'F' );
dataloader.AddVariable( "vbf_maxpt_jj_Deta", "#delta #eta_{jj}", "", 'F' );
dataloader.AddVariable( "PuppiAK8_jet_mass_so_corr", "AK8 mass", "GeV", 'F' );
dataloader.AddVariable( "ZeppenfeldWH/DEtajj := ZeppenfeldWH/vbf_maxpt_jj_Deta", "ZeppenfeldWH/#Delta#eta_{jj}", "", 'F' );
dataloader.AddVariable( "ZeppenfeldWL/DEtajj := ZeppenfeldWL_type0/vbf_maxpt_jj_Deta", "ZeppenfeldWL_type0/#Delta#eta_{jj}", "", 'F' );
dataloader.AddVariable( "costheta1_type0", "cos(#theta 1)", "", 'F' );
dataloader.AddVariable( "costheta2_type0", "cos(#theta 2)", "", 'F' );
dataloader.AddVariable( "phi_type0", "phi_type0", "", 'F' );
dataloader.AddVariable( "phi1_type0", "phi1_type0", "", 'F' );
dataloader.AddVariable( "costhetastar_type0", "costhetastar_type0", "", 'F' );
#dataloader.AddVariable( "WWRapidity", "yWW", "", 'F' );
# dataloader.AddVariable( "RpT_type0", "Rp_{T}", "", 'F' );
# dataloader.AddVariable( "njets", "njets", "", 'F' );
dataloader.AddVariable( "v_mt_type0", "mT (Leptonic W)", "", 'F' );
#dataloader.AddVariable( "LeptonProjection_type0", "Lepton Proj.", "", 'F' );
#dataloader.AddVariable( "VBSCentrality_type0", "VBS Centrality", "", 'F' );
#dataloader.AddVariable( "ZeppenfeldWL_type0", "Zeppenfeld (Wlep)", "", 'F' );
#dataloader.AddVariable( "ZeppenfeldWH", "Zeppenfeld (Whad)", "", 'F' );
# dataloader.AddVariable( "ht := ungroomed_PuppiAK8_jet_pt+vbf_maxpt_j1_pt+vbf_maxpt_j2_pt", "HT", "GeV", 'F');

# define signal and background trees
signalWeight = 35867.06
backgroundWeight = 35867.06

dataloader.AddSignalTree    ( signalTree1,     signalWeight*0.9114/1981107.0 );
dataloader.AddSignalTree    ( signalTree2,     signalWeight*0.9107/1923847.0 );
#dataloader.AddSignalTree    ( signalTree3,     signalWeight*0.08793/198848.0 );
dataloader.AddSignalTree    ( signalTree4,     signalWeight*0.03259/199525.0 );
dataloader.AddSignalTree    ( signalTree5,     signalWeight*0.02982/198896.0 );
dataloader.AddSignalTree    ( signalTree6,     signalWeight*0.1/169938.0 );
dataloader.AddSignalTree    ( signalTree7,     signalWeight*0.05401/188998.0 );
dataloader.AddSignalTree    ( signalTree8,     signalWeight*0.1825/393171.0 );
dataloader.AddSignalTree    ( signalTree9,     signalWeight*0.01589/99997.0 );

dataloader.AddBackgroundTree( background_Wjet1, backgroundWeight*1627.45/79165703.0 );
dataloader.AddBackgroundTree( background_Wjet2, backgroundWeight*435.24/38925816.0 );
dataloader.AddBackgroundTree( background_Wjet3, backgroundWeight*59.18/7754252.0);
dataloader.AddBackgroundTree( background_Wjet4, backgroundWeight*14.58/18578604.0);
dataloader.AddBackgroundTree( background_Wjet5, backgroundWeight*6.655/7688957.0 );
dataloader.AddBackgroundTree( background_Wjet6, backgroundWeight*1.60809/6708656.0 );
dataloader.AddBackgroundTree( background_Wjet7, backgroundWeight*0.0389136/2520618.0 );
#dataloader.AddBackgroundTree( background_Zjet0, backgroundWeight*4274.1645/(1749590.0-2*161090.0) );
# dataloader.AddBackgroundTree( background_Zjet1, backgroundWeight*1115.0463/(35950579.0-9808428.0) );
#dataloader.AddBackgroundTree( background_Zjet2, backgroundWeight*222.57608/(21571879.0-2*7649488.0) );

dataloader.AddBackgroundTree( background_Top1, backgroundWeight*19.5741/(5424956.0-0.0) );
# dataloader.AddBackgroundTree( background_Top2, backgroundWeight*19.5741/(5372830.0-0.0) );
#dataloader.AddBackgroundTree( background_Top3, backgroundWeight*364.3/(91832423.0-0.0) );
dataloader.AddBackgroundTree( background_Top4, backgroundWeight*0.2043/(5280251.0-2*1282079.0) );
dataloader.AddBackgroundTree( background_Top5, backgroundWeight*0.4062/(833257.0-2*201483.0) );
dataloader.AddBackgroundTree( background_Top6, backgroundWeight*0.2529/(7969186.0-2*2126557.0) );
dataloader.AddBackgroundTree( background_Top7, backgroundWeight*0.5297/(749367.0-2*199113.0) );

dataloader.AddBackgroundTree( background_Diboson1, backgroundWeight*5.633/(3949170.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson2, backgroundWeight*5.633/(3994663.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson3, backgroundWeight*0.07584/(99992.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson4, backgroundWeight*0.03203/(99657.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson5, backgroundWeight*1.938/(1991348.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson6, backgroundWeight*0.575/(499432.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson7, backgroundWeight*1.166/(981540.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson8, backgroundWeight*0.3488/(489280.0-0.0) );
dataloader.AddBackgroundTree( background_Diboson9, backgroundWeight*0.3449/(49999.0-0.0) );

#dataloader.AddBackgroundTree( background_VV1, backgroundWeight*49.997/(5057358.0-2*953706.0) );
#dataloader.AddBackgroundTree( background_VV2, backgroundWeight*0.1651/(269990.0-2*15372.0) );
#dataloader.AddBackgroundTree( background_VV3, backgroundWeight*10.71/(23766546.0-2*4986275.0) );
#dataloader.AddBackgroundTree( background_VV4, backgroundWeight*3.22/(15345161.0-2*2828391.0) );
#dataloader.AddBackgroundTree( background_VV5, backgroundWeight*0.01398/(249232.0-2*18020.0) );
#dataloader.AddBackgroundTree( background_VV6, backgroundWeight*10.31/(990051.0-2*0.0) );

#dataloader.AddBackgroundTree( background_QCD1, backgroundWeight*27990000.0/(1) );
#dataloader.AddBackgroundTree( background_QCD2, backgroundWeight*1712000.0/(1) );
#dataloader.AddBackgroundTree( background_QCD3, backgroundWeight*347700.0/(26924854.0) );
#dataloader.AddBackgroundTree( background_QCD4, backgroundWeight*32100.0/(53744436.0) );
#dataloader.AddBackgroundTree( background_QCD5, backgroundWeight*6831.0/(15578398.0) );
#dataloader.AddBackgroundTree( background_QCD6, backgroundWeight*1207.0/(13080692.0) );
#dataloader.AddBackgroundTree( background_QCD7, backgroundWeight*119.9/(11624720.0) );
#dataloader.AddBackgroundTree( background_QCD8, backgroundWeight*25.24/(5875869.0) );
#dataloader.AddBackgroundTree( background_DY1, backgroundWeight*/(-2*) );
#dataloader.AddBackgroundTree( background_DY2, backgroundWeight*/(-2*) );
#dataloader.AddBackgroundTree( background_DY3, backgroundWeight*/(-2*) );
#dataloader.AddBackgroundTree( background_DY4, backgroundWeight*/(-2*) );


dataloader.SetSignalWeightExpression("genWeight*LHEWeight[992]/LHEWeight[0]")
dataloader.SetBackgroundWeightExpression( "genWeight" )

# define additional cuts 
sigCut = ROOT.TCut("(type==1 || type==0) && (l_pt2<0) && ((ZeppenfeldWL_type0/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWL_type0/vbf_maxpt_jj_Deta<1.0)) && ((ZeppenfeldWH/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWH/vbf_maxpt_jj_Deta<1.0)) && (l_pt1>30) && (vbf_maxpt_j1_pt>30) && (vbf_maxpt_j2_pt>30)  &&  (nBTagJet_loose==0) && (vbf_maxpt_jj_m>500) && (pfMET_Corr>50) && ((ungroomed_PuppiAK8_jet_pt>200)&&(abs(ungroomed_PuppiAK8_jet_eta)<2.4)) && ((PuppiAK8_jet_mass_so_corr>40) && (PuppiAK8_jet_mass_so_corr<150)) && (PuppiAK8_jet_tau2tau1<0.55)")
bgCut = ROOT.TCut("(type==1 || type==0) && (l_pt2<0) && ((ZeppenfeldWL_type0/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWL_type0/vbf_maxpt_jj_Deta<1.0)) && ((ZeppenfeldWH/vbf_maxpt_jj_Deta>-1.0)&&(ZeppenfeldWH/vbf_maxpt_jj_Deta<1.0)) && (l_pt1>30) && (vbf_maxpt_j1_pt>30) && (vbf_maxpt_j2_pt>30)  &&  (nBTagJet_loose==0) && (vbf_maxpt_jj_m>500) && (pfMET_Corr>50) && ((ungroomed_PuppiAK8_jet_pt>200)&&(abs(ungroomed_PuppiAK8_jet_eta)<2.4)) && ((PuppiAK8_jet_mass_so_corr>40) && (PuppiAK8_jet_mass_so_corr<150)) && (PuppiAK8_jet_tau2tau1<0.55)")

# set options for trainings
dataloader.PrepareTrainingAndTestTree(sigCut, 
                                   bgCut, 
                                   ":".join(["nTrain_Signal=0",
                                             "nTrain_Background=0",
                                             "SplitMode=Random",
                                             "NormMode=NumEvents",
                                             "!V"
                                             ]))

## book and define methods that should be trained
method = factory.BookMethod(dataloader, ROOT.TMVA.Types.kBDT, "BDTG",
                            ":".join([ "!H",
                                       "!V",
                                       "NTrees=1000",
				       "MinNodeSize=2.5%",
                                       "BoostType=Grad",
				       "Shrinkage=0.10",
				       "UseBaggedBoost",
				       "BaggedSampleFraction=0.5",
                                       "nCuts=20",
				       "MaxDepth=2",
				       "NegWeightTreatment=Pray",
                                       ]))

# self-explaining
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()
