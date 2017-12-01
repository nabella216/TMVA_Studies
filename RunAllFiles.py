#!/usr/bin/python

import sys
import os
import ROOT as ROOT
import subprocess

ROOT.gROOT.SetBatch(True)

source = "/store/user/rasharma/SecondStep/WWTree_2017-11-26_18h59/HaddedFiles/"

with os.popen('xrdfs root://cmseos.fnal.gov ls '+source) as pipe:
	for line in pipe:
		#print line.strip()
		print ('root -l -b -q TMVAClassificationApplication.C'+'\(\\"'+line.strip()+'\\"\)')
		os.system('root -l -b -q TMVAClassificationApplication.C'+'\(\\"'+'root://cmsxrootd.fnal.gov/'+line.strip()+'\\"\)')
