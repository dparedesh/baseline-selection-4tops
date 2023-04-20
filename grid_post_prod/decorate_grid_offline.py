#! /usr/bin/env python

import sys, os, argparse
import ROOT
ROOT.gROOT.LoadMacro("ProductionUtils.C")

folder = "/eos/home-b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/"

parser = argparse.ArgumentParser(description='Merge the grid output offline ntuples.')
parser.add_argument("-i", "--input", dest='input_path', default='',
                    help='Input directory where the offline ntuples are stored.')
parser.add_argument("-f", "--filename", dest='file_name', default='',
                    help='to be used if you want to run on a specific ntuple.')

args = parser.parse_args()
if args.input_path != '': folder = args.input_path

for dirpath, regions, ntuples in os.walk(folder):
    for n in ntuples:
        if "data" in n: continue
        if args.file_name != '' and not args.file_name in n: continue
        isSyst = "syst" in n
        ROOT.add_normalisation(os.path.join(dirpath,n), isSyst)

