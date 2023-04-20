#!/usr/bin/env python

# Main function for the python wrapper
# The main tasks are just to get the input parameters from the parser, prepare the job and launch (locally or on the batch)
# Here a list of the possible flags of the parser:
# - analysis: specifies the analysis of the online ntuples, eg produced by 4tops or ttHbb
# - samples: samples to process (comma-separated list)
# - channel: channel to be analysed: 1LOS, SSML
# - type: process just mc16a, mc16cd (or both) or data
# - batch: flags to enable running on the batch
#   - batch-queue: specify the queue where to run
#   - batch-ncores: specify the number of cores to use
# - variables-to-store: comma-separated list of variables to store, in particular the object you are interested
#   - jet: kinematics, b-tagging, jvt
#   - rcjet: kinematics
#   - lepton: muon and electron kinematics, isTight flag
#   - event: event-based variables such as event number, run, mu, ht, nb-tags, njets, nelectrons, nmuons
# - regions: defining the regions to split your outut file
#   - Nj : require nJets == N
#   - geNj: require nJets >= N
# - config: absolute path to the configuration.txt file to use, other files are taken from the paths inside the file
#           (also in case they are relative to run_offline folder)
# - systematics: declare the systematics you want to add. This is shaped on ttHbb systematics ntuples, the choice: nominal, sys, jer
# - tools: comma-separated list of the tools to be ran. The tools can be custimized via the options in the format <option>:<value>
#   - SM4topAnalysis-opt: default MeVToGeV=true
#   - TtTRF-opt: default nBTags=ge4b, Path=TTHbbAnalysis/TtTRF/data, Selection= 5j6j, Systematic:nominal, Splitting:split3b4b, Parametrisation:pt/dRj, Debug = False
#   - OfflineFakes: default DataPath2015/16 = /eos/user/p/psabatin/Rel21_efficiencies_20180615, UsePrescaledTrigger = False
# - tmp: runs the code downloading the samples in /tmp area, not in /eos. If --batch is also required it is disabled and ran locally

# Declaration of modules to import
import os, shutil
import sys

# get the path to the executable
exe_dir = sys.executable
exe_path =  os.path.realpath(__file__)[:-19]
sys.path.insert(0, exe_path+'/lib')
from Class import check_path
from Class import Job
from Class import Tool
import argparse

if __name__ == "__main__":

    username = os.environ["USER"]

    parser = argparse.ArgumentParser("Setting up the HSM4t-offline ntuple production", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--analysis",action="store", dest="analysis", help="Choose the analysis online ntuples to use in input: HBSM4t",
                        choices=["HBSM4t-212120"],default="HBSM4t-212120")
    parser.add_argument("--channel",action="store", dest="channel", help="Choose the channel to use in input: 1LOS, SSML",choices=['1LOS', "SSML","BSM","LooseSSML"],default="")
    parser.add_argument("--samples","-s", action="store", dest="samples", help="Comma separated list of samples to be processed", default="all")
    parser.add_argument("--type","-t", action="store", dest="types", help="Comma separated list of types: mc16a, mc16cd, mc16e, mc16acd, mc16acde, data",default="mc16acde")
    parser.add_argument("--split-and-combine", action="store_true", dest="split_and_combine", help="Split input lists into smaller and recombine output files",default=False)
    parser.add_argument("--batch","-b", action="store", dest="batch", help="Use if you want to run on batch",choices = ["condor","lsf","none"], default="none")
    parser.add_argument("--batch-queue","-q", action="store", dest="queue", help="Queue to use in the batch",default="2nw")
    parser.add_argument("--batch-ncores","-n", action="store", dest="ncores", help="Number of cores to use on the batch",default=4)
    parser.add_argument("--batch-mail", action="store", dest="mail", help="Address where to receive HTCondor notification (if none, disabled)",default="none")
    parser.add_argument("--input-path","-i", action="store", dest="input_path", help="Path from the input mother directory",
                        default="/eos/user/"+username[0]+"/"+username+"/SM4t-212380_ntuples/online/")
    parser.add_argument("--eos-area", action="store_true", dest="eos_area", help="Flag to specify the input folder in SM4tops /eos/ area",default=False)
    parser.add_argument("--output-path","-o", action="store", dest="output_path", help="Path for the output mother directory",
                        default="/eos/user/"+username[0]+"/"+username+"/SM4t-212380_ntuples/offline/")
    parser.add_argument("--xrootd",action="store_true", dest="xrootd", help="Flag to enable the use of XRootD to retrieve online samples",default=False)
    parser.add_argument("--find-file",action="store_true", dest="find_file",
                        help="Flag to enable the use of 'Find-file' option: in case the file is not found in the given path (physical file or symlink) the XRootD is used",default=False)
    parser.add_argument("--variables-to-store","-v", action="store", dest="vars", help="Variables to be saved in the output file",default="event,common,jet,lepton,rcjet,met")
    parser.add_argument("--regions","-r", action="store", dest="regions", help="Parametrisation of the regions",default="4j,5j,6j,7j,8j,9j,ge10j")
    parser.add_argument("--config","-c", action="store", dest="configs", help="Configuration files to be used",default="NONE")
    parser.add_argument("--systematics", action="store", dest="syst", help="Declare the systematics to process, type: nominal,sys,jer,jes,eg,mu,met",default="nominal")
    parser.add_argument("--tools", action="store", dest="tools", help="Tools to be to be used and add to the output",default="NONE")
    parser.add_argument("--build-area", action="store", dest="build_area", help="Directory to build the offline-framework. ",default="")
    parser.add_argument("--CommonTools-opt", action="store", dest="CommonTools-opt", help="Options for CommonTools tools",default="")
    parser.add_argument("--Add1LOSVar-opt", action="store", dest="Add1LOSVar-opt", help="Options for Add1LOSVar tools",default="MeVToGeV:true,AddHigherLevelVariables:true")
    parser.add_argument("--AddSSMLVar-opt", action="store", dest="AddSSMLVar-opt", help="Options for AddSSMLVar tools",default="TurnOn:true")
    parser.add_argument("--AddBDT1LOS-opt", action="store", dest="AddBDT1LOS-opt", help="Options for AddBDT1LOS",
                        default="XMLpath_1l:/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/BDTXMLs/BDT_240519_1l_set1/,XMLpath_2l:/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/BDTXMLs/BDT_240519_2l_set1/")
    parser.add_argument("--DoTTbarBfilter-opt", action="store", dest="DoTTbarBfilter-opt", help="Options for DoTTbarBfilter tools",default="")
    parser.add_argument("--DoTTbarDileptonFilter-opt", action="store", dest="DoTTbarDileptonFilter-opt", help="Options for DoTTbarDileptonFilter tools",default="")
    parser.add_argument("--OfflineHistogrammer-opt", action="store", dest="OfflineHistogrammer-opt", help="Options for OfflineHistogrammer tools",default="Variables:jet_pt")
    parser.add_argument("--MVAVariables-opt", action="store", dest="MVAVariables-opt", help="Options for MVAVariables tools",default="bTagWPs:DL1r_70")
    parser.add_argument("--TtTRF-opt", action="store", dest="TtTRF-opt", help="Options for TtTRF tools",
                        default="nBTags:3b,Path:/eos/atlas/atlascerngroupdisk/phys-top/4tops/ljetsOS/DDbkgEstimate/ttTRF/data/2.0.5/,Selection:5j6j,Systematic:nominal,Splitting:split4b,Parametrisation:pt/dRj/Nj,Sample:data_mc16a")
    parser.add_argument("--OfflineFakes-opt", action="store", dest="OfflineFakes-opt", help="Options for MM fakes estimation tools",
                        default="DataPath2015:/eos/user/p/psabatin/Rel21_efficiencies_20180615,DataPath2016:/eos/user/p/psabatin/Rel21_efficiencies_20180615,Use2016PrescaledTriggers:false")
    parser.add_argument("--tmp",action="store_true", dest="tmp", help="Store the online ntuple in /tmp and not in /eos space",default=False)
    parser.add_argument("--no-run",action="store_true", dest="norun", help="Flag in case we don't want to run but just setup",default=False)

    args = parser.parse_args()

    # figure out the build area of the offline framework
    build_area = args.build_area
    if build_area != "" :
        build_area = os.path.abspath(build_area)
        if not os.path.exists(build_area) :
            print "[ERROR] Could not resolve the offline framework build area",build_area
            exit(1)

    # Split and recombine option only if batch is enabled
    if args.split_and_combine and args.batch != "condor":
        print "[WARNING] Split-and-combine enabled, but batch condor is not. Split-and-combine is ignored."
        args.split_and_combine = False

    # In case input-path is put to 'eosArea' some changings in the input-path and folder structure is needed
    if args.eos_area:
        if '212380' in args.analysis:
            args.input_path = '/eos/atlas/atlascerngroupdisk/phys-top/4tops/ntuples/common-fw_tag212380/preProd/'
        args.input_path += '<type>/'

    # In case --tmp the input_path is modified to be in /tmp and --batch option disabled
    if args.tmp:
        args.input_path = "/tmp/"+username+"/";
        if args.batch == True:
            args.batch = False
            print "[ALERT] Cannot use --tmp option if running on the batch: batch disabled"

    # Reshaping the input and output path with --analysis and --type args
    if args.channel is not "":
        ch = args.channel
        if ch == "BSM": ch == "1LOS"
        args.input_path += ch +"/"
        args.output_path += ch +"/"
    if args.analysis is not "HSM4t-212120":
        args.input_path = args.input_path.replace("HSM4t-212120",args.analysis)
        args.output_path = args.output_path.replace("HSM4t-212120",args.analysis)

    # Type forced to be data in case data are in input and erase truth from variables
    if "data" in args.samples:
        args.types = "data"
        args.vars = args.vars.replace(",truth","")
    if "nominal" not in args.syst:
        args.vars = args.vars.replace(",truth","")


    # In case 'all' is selected, all the available samples are added to the sample list
    if args.samples == "all":
        if "mc16" in args.types :
            args.samples = ['4tops','ttbar','ttX','single-top','vjets','vv']
        if "data" in args.types:
            args.samples = ['data15','data16','data17','data18']

    # In case 'all' is selected, all the available samples are added to the sample list
    if args.samples == "all-syst":
        if "mc16" in args.types :
            args.samples = ['ttbar_PhHerwig_AFII','ttbar_PhPy8OL_ttbb4F_AFII','ttbar_PhPy8_AFII_HF_filt','ttbar_aMcAtNloPy8_AFII_HF_filt','ttbar_PhPy8_Rad_AFII','ttbar_aMcAtNloPy8_AFII','ttbar_Sherpa211_AFII','ttbar_PhPy8_FS_HT_sliced','ttbar_PhHerwig_AFII_HF_filt','ttbar_PhPy8_AFII']

    # The dictionary for tools are initiated: the Tool module has as attributes the options passed in the string <tool>-opt with the
    # respective passed value
    tool_dict = {}
    if not args.tools == "NONE":
        for item in args.tools.split(","):
            tool = Tool(item,str(args.__dict__[item+"-opt"]))
            tool_dict [item] = tool

    # Here we set the way to access to the ntupeles:
    # standard: looking locally for ntuples, if not found, download
    # xrootd: looking for ntuples on the grid via XRootD
    # find_file = mix of the two, looking locally for ntuples (physical files or symlinks), if not found XRootD (no downloading)
    access_method = 'xrootd' if args.xrootd else 'find_file' if args.find_file else 'standard'

    # Here the job are initialized and ran.
    # For each systematic (if present, otherwise 'nominal'), for each type (mc16a/cd in case we want split), and for each process,
    # a separated job is initialized and launched
    for syst in args.syst.split(","):
        for typ in args.types.split(","):
            for sam in args.samples.split(","):
                # Declaration and creation of the job folder
                job_dir = "job_"+sam+"_"+typ+"_"+args.analysis+"_"+args.channel+"_"+syst
                if os.path.exists(job_dir):
                    tag_count = 1
                    while os.path.exists(job_dir+'_v%i'%tag_count) :
                        tag_count += 1
                    # os.system("cp -r %s %s_v%i"%(job_dir, job_dir, tag_count) )
                    # shutil.rmtree(job_dir)
                    job_dir = job_dir+"_v%i"%tag_count
                os.system("mkdir -p "+job_dir)
                os.chdir(job_dir)
                # Job declaration
                job = Job(exe_path,sam, typ, args.analysis, args.channel,args.input_path, args.output_path, tool_dict,syst, access_method, build_area)
                # Job launched depending on batch option
                if args.batch == "none":
                    job.executeLocal(args.configs,args.vars.split(","),args.regions.split(","),syst, args.norun)
                else:
                    if args.split_and_combine:
                        job.splitAndCombineBatch(args.configs,args.vars.split(","),args.regions.split(","),syst,args.queue, args.ncores, args.norun, args.batch, args.mail)
                    else:
                        job.executeBatch(args.configs,args.vars.split(","),args.regions.split(","),syst,args.queue, args.ncores, args.norun, args.batch, args.mail)
                os.chdir("..")
