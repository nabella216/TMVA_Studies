# TMVA_Studies

## Steps To Do
1. Cuts





## List of errors and fixes

### Error: signal and background histograms have different or invalid dimensions
The source of this error is the decorrelation. In the default example many decorrelation functions are switched on but for BDT we don't need them at all. So, we need to skip the decorrelation part.
The default line in TMVA where this correlation was called is:

    TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

In the above line we need to change `Transformations=I;D;P;G,D` to `Transformations=I`  (where I is for identity transformation).

### How to change the range of input variables

> To do this we need to make cut on variable. Presently, no other better option we get.
