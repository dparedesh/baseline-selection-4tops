#!/usr/bin/env python
#
#author: Daniela PAREDES
#email: dparedes@cern.ch
#
# Create symbolic links from our EOS area to the Exotics disk where the samples have been replicated
#
#Inspired from: https://docs.google.com/presentation/d/1h-gfnPhgBoorEyhyee40yJ5On8ykSGvzbHkeCHqeFh0/edit#slide=id.g60c2eaeeb8_0_0
#
#main command: rucio list-file-replicas --pfns --rse CERN-PROD_PHYS-EXOTICS --link /eos/:/eos/ group.phys-exotics:group.phys-exotics.410470.PhPy8EG.DAOD_TOPQ1.e6337_s3126_r10724_p4174.HBSM4t-212120_mc16e_SSML_v2_output_root
#
# More info: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InfoForExoticsAnalyzers#Storage
#



import sys, os, argparse
import subprocess


parser = argparse.ArgumentParser(description='Create symbolic links for ntuples from the Exotics disk to our EOS area')
parser.add_argument("-tag", default="212120",help="ntuple production tag (e.g. 212120).")
parser.add_argument("-r", "--rse", dest='rse', default="CERN-PROD_PHYS-EXOTICS", 
                    help="Name of the disk where ntuples are replicated")
parser.add_argument("--ch",dest="ch",help="Channel to be processed, i.e. SSML, 1LOS, or SSML_loose")
parser.add_argument("--stream",dest="stream",help="comma separated list of the streams to be processed for example, mc16a,mc16d,mc16e,data")

args = parser.parse_args()
tag = args.tag
rse = args.rse
ch = args.ch
mc16=args.stream.split(',')

#output file from check_production.py
outDS = "outDS."+tag+".txt"

EOS="/eos/user/b/bsm4tops/bsm4teos/ntuples_HBSM4t-"+tag+"/online/"+ch+"/"


with open(outDS) as f:
    lines = f.readlines()

    for mc in mc16:
        fullDir = EOS+mc       


        subprocess.call("mkdir -p "+fullDir,shell=True)
        os.chdir(fullDir)
       
        #dont't mix loose with tight... better separate them!
        sublist = list(filter(lambda k: (mc in k and ch in k and 'loose' not in k and 'loose' not in ch) or (mc in k and ch in k and 'loose' in ch and 'loose' in k), lines))

        for sample in sublist:

            sam = sample.replace(".root","_root")

            subprocess.call("mkdir -p "+sam,shell=True)

            os.chdir(sam.replace(' \n',''))
           
            scope=sam.split(".")
            user=scope[0]+"."+scope[1]

            cmd = 'rucio list-file-replicas --pfns --rse {0} --link /eos/:/eos/ {1}:{2}'.format(rse,user,sam)

            print cmd
            subprocess.call(cmd,shell=True)

            os.chdir(fullDir)

