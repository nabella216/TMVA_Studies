**TMVA Studies**
<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Steps To Do](#steps-to-do)
	- [Cut Scan](#cut-scan)
	- [Variable to use](#variable-to-use)
- [List of errors and fixes](#list-of-errors-and-fixes)
	- [Error: signal and background histograms have different or invalid dimensions](#error-signal-and-background-histograms-have-different-or-invalid-dimensions)
	- [How to change the range of input variables](#how-to-change-the-range-of-input-variables)

<!-- /TOC -->
# Steps To Do

## Cut Scan

## Variable to use

# List of errors and fixes

## Error: signal and background histograms have different or invalid dimensions
The source of this error is the decorrelation. In the default example many decorrelation functions are switched on but for BDT we don't need them at all. So, we need to skip the decorrelation part.
The default line in TMVA where this correlation was called is:

    TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

In the above line we need to change `Transformations=I;D;P;G,D` to `Transformations=I`  (where I is for identity transformation).

## How to change the range of input variables

> To do this we need to make cut on variable. Presently, no other better option we get.
