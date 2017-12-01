#!/bin/bash
echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
echo "System software: `cat /etc/redhat-release`" #Operating System on that node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc6_amd64_gcc530
eval `scramv1 project CMSSW CMSSW_9_0_1`
cd CMSSW_9_0_1/src/
echo "PWD = "$PWD
eval `scramv1 runtime -sh` # cmsenv is an alias not on the workers
echo "CMSSW: "$CMSSW_BASE
xrdcp  root://cmseos.fnal.gov//store/user/rasharma/TMVA/Trial1/TMVAClassification.C .
xrdcp  root://cmseos.fnal.gov//store/user/rasharma/TMVA/Trial1/TMVAClassificationApplication.C .
xrdcp  root://cmseos.fnal.gov//store/user/rasharma/TMVA/Trial1/RunAllFiles.py .
rm TraningOutput.dat ApplicationOutput.dat RunAll.dat
echo "Start running TMVA Traning...."
root -l -b -q TMVAClassification.C  | tee TraningOutput.dat
echo "Start running TMVA Application Traning...."
root -l -b -q TMVAClassificationApplication.C | tee ApplicationOutput.dat
echo "Start running TMVA Application For all root files"
python RunAllFiles.py >& RunAll.dat
echo "List all files"
ls 
echo "*******************************************"
OUTDIR=root://cmseos.fnal.gov//store/user/rasharma/TMVA/Trial1/
echo "xrdcp output for condor"
for FILE in *
do
  echo "xrdcp -r -f ${FILE} ${OUTDIR}/"
  xrdcp -r -f ${FILE} ${OUTDIR}/ 2>&1
  #XRDEXIT=$?
  #if [[ $XRDEXIT -ne 0 ]]; then
  #  rm *.root
  #  echo "exit code $XRDEXIT, failure in xrdcp"
  #  exit $XRDEXIT
  #fi
  rm ${FILE}
done
