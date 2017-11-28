#!/usr/bin/python

import sys
import os
import ROOT as ROOT
import subprocess

ROOT.gROOT.SetBatch(True)

source = "/eos/uscms/store/user/rasharma/SecondStep/WWTree_2017-11-26_18h59/HaddedFiles/"
ArrayDirPath = []
for root, dirs, filenames in os.walk(source):
	#print filenames
	for files in filenames:
		#print root+os.sep+files
		print "**"*20
		print ''
		print 'Running Command:\n'
		print('root -l -b -q TMVAClassificationApplication.C'+'\(\\"'+root+os.sep+files+'\\"\)')
		print ''
		print ''
		os.system('root -l -b -q TMVAClassificationApplication.C'+'\(\\"'+root+os.sep+files+'\\"\)')
