#!/usr/bin/env python

# Declaration of modules to import
import os, shutil
import sys
import argparse
import subprocess
from collections import OrderedDict

# global list storing missing samples
missing_samples = {}
skipped_samples = []

def writeIntro(f, camp, tag, ch,stream):
    f.write('# File containing the naming of all the samples for:\n')
    f.write('# - Analysis: %s\n' % tag)
    f.write('# - Channel: %s\n' % ch)
    f.write('# - Campaign: %s\n' % camp)
    f.write('# - Stream: %s\n' % stream)
    f.write('# Structure of the file:\n')
    f.write('# ## <process>\n')
    f.write('# <list of the samples>\n\n\n')

def writeOutro(f, camp, tag, ch):
    f.write('## END\n')

# import MC lists
def preProdSample(sample_category, mc,tag,camp,ch,sys):
    scope = 'group.bsm4tops'
    if ch == 'SSML':
        # scope = 'user.dparedes'
        scope = 'group.phys-exotics'


    preprod = mc.replace(mc.split(".")[0], scope).replace("deriv.","")+"."+tag+"_"+camp+"_"+ch
    if sys != 'none':
        preprod += "_"+sys
    preprod += "_v*_output*root"
    print preprod
    if "data" in camp :
        preprod = preprod.replace("."+preprod.split('.')[4]+".","*")
    else :
        preprod = preprod.replace("."+preprod.split('.')[3]+".","*")
    preprod = preprod.replace("TOPQ1.","TOPQ1*")

    print " - looking for: %s" % preprod

    # metsai - adding this to accomdate local path of samples
    if len(args.mainfolder)>0:
        mainfolder = args.mainfolder
        if sys == 'loose':
            mainfolder = mainfolder+camp+'/LooseSSML/'
        else:
            mainfolder = mainfolder+camp+'/'+ch+'/'
        cmd = 'ls -d ' + mainfolder+preprod

    else: # original method
        cmd = 'rucio ls '+scope+':'+preprod+' --filter type=CONTAINER'

    rucio_output = str()
    try:
        rucio_output = subprocess.check_output(cmd, shell=True)
    except subprocess.CalledProcessError as exc:
        print("subprocess failed with return code 1".format(exc.returncode))
    if scope not in rucio_output:
        print "\t\t\t => Not found"
        if not sample_category in missing_samples :
            missing_samples[sample_category] = []
        missing_samples[sample_category].append(preprod)
        return ""

    if len(args.mainfolder)>0: # metsai - find the samples from the local path
        preprod_final = rucio_output.split("\n")[-2].split("/")[-1]
    else:
        rucio_output = [ v.split("|")[1].strip() for v in rucio_output.split("\n")[3:-2] ]
        preprod_final = ""
        for v in rucio_output :
            if v >= preprod_final : preprod_final = v

    print "\t\t\t => Found",preprod_final
    print ""

    if len(args.mainfolder)>0: # metsai - samples from the local path
        return preprod_final
    else:
        return preprod_final.split(":")[1]



if __name__ == "__main__":

    d_sample_names = {
        'data2015' : 'data15',
        'data2016' : 'data16',
        'data2017' : 'data17',
        'data2018' : 'data18',
        'ttH_tttt' : 'BSM4tops',
        # 'tttt_aMcAtNloPy8_AFII' : '4tops',
        'tttt_aMcAtNloPy8_AFII': '4tops', # nominal 4top NLO sample in fit
        'tttt_MadGraphPy8LO_NNPDF31_AFII': '4topsLO', # nominal 4top LO sample in training
        'tttt_Sherpa2210_FS': '4tops_sh2210', # sh2210 4top samples (use in post-EPS)
        'tttt_aMcAtNloHerwig7_AFII': '4tops_Alt', # MG+Herwig7 (for gen / PS showering comparison)
        'ttWNLO': 'ttWSherpa', # ttW sherpa 2.2.1 (previously nominal sample)
        'ttWSh228': 'ttWSh228', # ttW sherpa 2.2.8 (new nominal sample for EPS)
        'ttWSh2210': 'ttWSh2210', # ttW sherpa 2.2.10 (new nominal sample for post-EPS)
        'ttW' : 'ttW', # ttW MG (for generator comparison)
        'ttH' : 'ttH', # ttH nominal sample
        'ttHMGPhy': 'ttHMGPhy', # ttH MG (for generator comparison)
        'ttZ' : 'ttZ', # nominal ttZ sample (ttZ+ttll)
        'ttll' : 'ttll', # nominal ttZ sample (ttZ+ttll)
        'ttZSherpa': 'ttZSherpa', # ttZ Sherpa (for generator comparison)
        'ttbar' : 'ttbar', # nominal ttbar sample
        'SingleTop' : 'single-top', # nominal SingleTop sample
        'ttt' : 'ttt', # nominal 3top sample
        'ttWW' : 'ttWW', # nominal ttWW sample
        'ttVV': 'rarettVV', # nominal rarettVV sample
        'VH' : 'vh', # nominal vh sample
        'Vjets_Sherpa221' : 'vjets',
        'VV' : 'vv',
        'VVV' : 'vvv',
        }




    list_dir = os.getcwd()

    parser = argparse.ArgumentParser("Create input file list")
    parser.add_argument("--tag","-t",action="store", dest="tag", help="Tag of the FW",choices=["HBSM4t-212120"],default="HBSM4t-212120")
    parser.add_argument("--debug",action="store_true", dest="debug", help="Flag for debugging",default=False)
    parser.add_argument("--campaign","-c", action="store", dest="campaign", help="Comma separated list of samples to be processed", default="mc16a,mc16d,mc16e")
    parser.add_argument("--stream","-s", action="store", dest="stream", help="Comma separated list of streams: syst,nominal",default="nominal")
    parser.add_argument("--channel", action="store", dest="channel", help="Comma separated list of channels: 1LOS,SSML",default="1LOS,SSML")
    parser.add_argument("--mainfolder", action="store", dest="mainfolder", help="this is used when the online production is not stored on grid but on your local folder",default="")
    parser.add_argument("--preprod_file", action="store", dest="preprod_file", help="define you input preproduction file",default="")

    args = parser.parse_args()

    # if debug is on, add prefix TEST_ to the output sample list
    pre = ""
    if args.debug: pre = "TEST_"

    # metsai - add this to use customed preproduction file
    if args.preprod_file!="":
        preprod_file = args.preprod_file
    else: # original way
        mc_path =  os.path.realpath(__file__)[:-13]+"/../../../../preProd/"

    for camp in args.campaign.split(","):

        # metsai - add this to use customed preproduction file
        if args.preprod_file=="":
            preprod_file = mc_path+camp+"_SM4tops.py"
        # read the TOPQ1 list
        # to be translated to offline production input list

        with open(preprod_file,"r") as f_mc:
            data = f_mc.read().replace("\n","").replace("\t","")

            data = data.split("grid.Add")
            f_mc.close()

        # compose the dictionary for each sample category and the samples (dsid) in the category
        # e.g. { "ttbar":["...410470...", "...410471...", "...410472...", ], ... }
        sample_dictionary = OrderedDict() # change to ordered dict, help bookkeeping update in sample list
        for slot in data:
            if "import" in slot: continue
            header = slot.split(".datasets")[0].replace('")',"").replace('("',"").replace(camp+"_","")
            samples = slot.split(".datasets")[1].replace('=',"").replace('[',"").replace(']',"").replace(' ',"").replace('"',"").split(",")
            sample_dictionary [header] = samples

        # loop over differen channels
        for ch in args.channel.split(","):
            for stream in args.stream.split(","):
                systFlag = ""
                if sys == "syst": systFlag = "syst"
                elif sys == "loose":  systFlag = "Loose"

                # compose the name of the list file
                listname = pre+args.tag+"_"+ch+"_"+camp
                # add _syst suffix if syst stream
                if stream != "nominal" : listname += "_syst"
                listname += ".py"

                f_list = open(listname,"w+")
                print "[INFO] Creating: %s " % listname
                writeIntro(f_list, camp,args.tag,ch,stream)

                for sample, datasets in sample_dictionary.iteritems():
                    if sample not in d_sample_names :
                        skipped_samples.append(sample)
                        continue # skip those not in the list - in case of assembly of samples used for production, e.g. SingleTop and SingleTop_Alt
                    # print sample, datasets
                    if stream == "nominal":
                        f_list.write ('## '+d_sample_names[sample]+"\n")
                        for dsid in datasets:
                            if dsid == '': continue
                            f_list.write (str(preProdSample(d_sample_names[sample], dsid, args.tag, camp, ch, 'none'))+'\n')

                    else:

                        syst_list = list()
                        if stream == "syst": syst_list = ['syst']
                        elif stream == "loose": syst_list = "loose"
                        # skip anything that's not nominal sample
                        #if not d_sample_names[sample] in ["4tops", "ttbar", "ttbar_PhPy8_AFII", "ttbar_PhPy8_AFII_HF_filt", "ttbar_PhPy8_FS_HT_sliced", "single-top", "ttX", "vv", "vvv", "vh", "vjets"] : continue
                        # if not d_sample_names[sample] in ["tttt_aMcAtNloPy8_AFII", "ttbar_PhPy8_AFII", "ttbar_PhPy8_AFII_HF_filt", "ttbar_PhPy8_AFII_HT_sliced", "SingleTop_tchan_PhPy8_FS", "SingleTop_Wt_PhPy8_FS", "SingleTop_schan_PhPy8_FS", "ttZ", "ttW", "ttH", "vv", "vvv", "vh", "vjets"] : continue
                        for sys in syst_list:
                            print(sample)# metsai
                            if not sample in d_sample_names: continue
                            f_list.write ('## '+d_sample_names[sample]+" "+sys+"\n")
                            for dsid in datasets:
                                if dsid == '': continue
                                f_list.write (str(preProdSample(d_sample_names[sample], dsid, args.tag, camp, ch, sys))+'\n')
                    f_list.write("\n")

                writeOutro(f_list, camp,args.tag,ch)
                f_list.close()

    # write a list of missing online prod samples
    with open(('missing_samples_%s_%s_%s_%s.txt'%(args.tag, args.campaign, args.channel, args.stream)).replace(",", "_"), 'w+') as f:
        for key,item in missing_samples.items() :
            for s in item :
                print >> f, "%s: %s: %s"%(args.campaign ,key, s)

    # in case of skipped nominal sample sets, print them out at the end of the run
    print "WARNING::The following samples are skipped, either because they are assembly of other subsets, or you need to update the sample list in this script. "
    for s in skipped_samples :
        print s
