#!/usr/bin/env python

# Declaration of modules to import 
import os, shutil
import sys
import argparse

from dict_datasets import *

extFile = [
    # "/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/BDTXMLs/BDT_050919_1l_set13/*.xml",
    # "/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/BDTXMLs/BDT_050919_2l_set13/*.xml",
]

jobs = [
    "data15", "data16", "data17", "data18",
    "ttbar_aMcAtNloPy8_AFII_AllFilt_mc16a","ttbar_aMcAtNloPy8_AFII_AllFilt_mc16d","ttbar_aMcAtNloPy8_AFII_AllFilt_mc16e",
    "ttbar_PhHerwig_AFII_AllFilt_mc16a","ttbar_PhHerwig_AFII_AllFilt_mc16d","ttbar_PhHerwig_AFII_AllFilt_mc16e",
    "tttt_aMcAtNloPy8_mc16a", "tttt_aMcAtNloPy8_mc16d", "tttt_aMcAtNloPy8_mc16e",
    "tttt_aMcAtNloHerwig7_mc16a","tttt_aMcAtNloHerwig7_mc16d","tttt_aMcAtNloHerwig7_mc16e",
    "OtherTop_MgPy8_mc16a","OtherTop_MgPy8_mc16d","OtherTop_MgPy8_mc16e",
    "OtherTop_aMcAtNloPy8_mc16a","OtherTop_aMcAtNloPy8_mc16d","OtherTop_aMcAtNloPy8_mc16e",
    "SingleTop_PhPy8_mc16a","SingleTop_PhPy8_mc16d","SingleTop_PhPy8_mc16e",
    "ttgamma_MgPy8_mc16a","ttgamma_MgPy8_mc16d","ttgamma_MgPy8_mc16e",
    "ttH_PhPy8_mc16a","ttH_PhPy8_mc16d","ttH_PhPy8_mc16e",
    "ttW_aMcAtNloPy8_mc16a","ttW_aMcAtNloPy8_mc16d","ttW_aMcAtNloPy8_mc16e",
    "ttX_aMcAtNloPy8_mc16a","ttX_aMcAtNloPy8_mc16d","ttX_aMcAtNloPy8_mc16e",
    "Vgamma_mc16a","Vgamma_mc16d","Vgamma_mc16e",
    "VH_Py8_mc16a","VH_Py8_mc16d","VH_Py8_mc16e",
    "Vjets_Sherpa221_mc16a","Vjets_Sherpa221_mc16d","Vjets_Sherpa221_mc16e",
    "VV_mc16a","VV_mc16d","VV_mc16e",
    "VVV_mc16a","VVV_mc16d","VVV_mc16e",
    "ttbar_PhPy8_AFII_AllFilt_mc16a", "ttbar_PhPy8_AFII_AllFilt_mc16d", "ttbar_PhPy8_AFII_AllFilt_mc16e",
    "ttH_tttt_m400_MgPy8_mc16a", "ttH_tttt_m400_MgPy8_mc16d", "ttH_tttt_m400_MgPy8_mc16e",
    "ttH_tttt_m500_MgPy8_mc16a", "ttH_tttt_m500_MgPy8_mc16d", "ttH_tttt_m500_MgPy8_mc16e",
    "ttH_tttt_m600_MgPy8_mc16a", "ttH_tttt_m600_MgPy8_mc16d", "ttH_tttt_m600_MgPy8_mc16e",
    "ttH_tttt_m700_MgPy8_mc16a", "ttH_tttt_m700_MgPy8_mc16d", "ttH_tttt_m700_MgPy8_mc16e",
    "ttH_tttt_m800_MgPy8_mc16a", "ttH_tttt_m800_MgPy8_mc16d", "ttH_tttt_m800_MgPy8_mc16e",
    "ttH_tttt_m900_MgPy8_mc16a", "ttH_tttt_m900_MgPy8_mc16d", "ttH_tttt_m900_MgPy8_mc16e",
    "ttH_tttt_m1000_MgPy8_mc16a", "ttH_tttt_m1000_MgPy8_mc16d", "ttH_tttt_m1000_MgPy8_mc16e",
    "SingleTop_PhPy8_FS_mc16a","SingleTop_PhPy8_FS_mc16d","SingleTop_PhPy8_FS_mc16e",
    "SingleTop_PhPy8_FS_DS_mc16a","SingleTop_PhPy8_FS_DS_mc16d","SingleTop_PhPy8_FS_DS_mc16e",
    "SingleTop_PhH7_AFII_mc16a","SingleTop_PhH7_AFII_mc16d","SingleTop_PhH7_AFII_mc16e",
    "SingleTop_aMcAtNloPy8_AFII_mc16a","SingleTop_aMcAtNloPy8_AFII_mc16d","SingleTop_aMcAtNloPy8_AFII_mc16e"
    "tttt_aMcAtNloPy8_syst_mc16a","tttt_aMcAtNloPy8_syst_mc16d", "tttt_aMcAtNloPy8_syst_mc16e",
    "OtherTop_MgPy8_syst_mc16a", "OtherTop_MgPy8_syst_mc16d", "OtherTop_MgPy8_syst_mc16e",
    "OtherTop_aMcAtNloPy8_syst_mc16a", "OtherTop_aMcAtNloPy8_syst_mc16d", "OtherTop_aMcAtNloPy8_syst_mc16e",
    "SingleTop_PhPy8_syst_mc16a", "SingleTop_PhPy8_syst_mc16d", "SingleTop_PhPy8_syst_mc16e",
    "ttH_PhPy8_syst_mc16a", "ttH_PhPy8_syst_mc16d", "ttH_PhPy8_syst_mc16e",
    "ttW_aMcAtNloPy8_syst_mc16a", "ttW_aMcAtNloPy8_syst_mc16d", "ttW_aMcAtNloPy8_syst_mc16e",
    "Vjets_Sherpa221_syst_mc16a","Vjets_Sherpa221_syst_mc16d","Vjets_Sherpa221_syst_mc16e",
    "VV_syst_mc16a","VV_syst_mc16d","VV_syst_mc16e"
    

]

############################################

parser = argparse.ArgumentParser(description='Launch offline ntuple production in the grid.')
parser.add_argument("-u", "--username", dest='username', default='prod',
                    help='Grid username (e.g sberlend). set "prod" if using exotic production role.')
parser.add_argument("-n", "--outputname", dest='outputname', default='BSM4t-2121200-1LOS-v1',
                    help='Output directory name (e.g BSM4t-2121200-1LOS-v1).')
parser.add_argument("-p", "--postds", dest='post_outds', default='v1',
                    help='post name to appear in the output dataset (e.g v1).')
parser.add_argument("-l", "--local", action='store_true', dest='doLocal',
                    help='Generates offline ntuples locally (for testing purpose).')
parser.add_argument("-v", "--verbose", action='store_true', dest='verbose',
                    help='verbose run (for testing purpose).')

args = parser.parse_args()

#############################################

for name in jobs:
    if args.verbose: print("Creating directory: "+name)

    # Create launch folder
    if not os.path.isdir("job_"+name): os.mkdir("job_"+name)
    os.chdir("job_"+name)
    os.system("cp $SM4topsFW_DIR/data/OffSM4topsTool/GridConfig/* ./")

    # Copy additionnal files (e.g. BDT)
    for e in extFile: os.system("cp {0} ./".format(e))

    # List of input datasets
    datasets = ",".join(list_samples[name])
    if args.verbose: print("Input dataset(s): "+datasets)

    # Output dataset
    if args.username == "prod":
        outds = "group.phys-exotics.{}.{}.{}.off".format(name, args.outputname, args.post_outds)
    else:
        outds = "user.{}.{}.{}.{}.off".format(args.username, name, args.outputname, args.post_outds)
    print("Output dataset: "+outds)
    
    # Configuring the offline run
    if args.verbose: print("Configuring the offline run...")

    os.system("sed -i 's/SampleName.*/SampleName {}/' configuration_grid.1LOS.txt".format(name))
    os.system("sed -i 's/OutputDirectory.*/OutputDirectory {}/' configuration_grid.1LOS.txt".format(args.outputname))

    os.system("sed -i 's/Normalisation.*/Normalisation None/' configuration_grid.1LOS.txt")
    os.system("sed -i 's/WeightsFile.*/WeightsFile weights_grid.1LOS.txt/' configuration_grid.1LOS.txt")

    # data
    if "data" in name :
        os.system("sed -i 's/SampleType.*/SampleType data/' configuration_grid.1LOS.txt")
        os.system("sed -i 's/WeightsFile.*//' configuration_grid.1LOS.txt")
        os.system("sed -i 's/SaveSumOfWeightsTree.*//' configuration_grid.1LOS.txt")

    # systematics
    if "syst" in name :
        os.system("sed -i 's/Systematics .*/Systematics {0}/' configuration_grid.1LOS.txt".format(",".join(list_syst)))
        os.system("sed -i 's/SaveDefaultVariables.*/SaveDefaultVariables runNumber,eventNumber,mcChannelNumber,met_met,met_phi,jet_pt,jet_eta,jet_phi,jet_e,el_pt,el_eta,el_phi,el_e,mu_pt,mu_eta,mu_phi,mu_e/' configuration_grid.1LOS.txt")
        os.system("sed -i 's/VariablesToRead_grid.1LOS.txt.*/VariablesToRead_grid_sys.1LOS.txt/' configuration_grid.1LOS.txt")
        os.system("sed -i 's/VariablesToSave_grid.1LOS.txt.*/VariablesToSave_grid_sys.1LOS.txt/' configuration_grid.1LOS.txt")
        os.system("sed -i 's/DecoToSave_grid.1LOS.txt.*/DecoToSave_grid_sys.1LOS.txt/' configuration_grid.1LOS.txt")

    # ttbar filters
    if "ttbar_PhPy8_AFII_incl" in name:
        os.system("echo \"DoTTbarDileptonFilter true\" >> configuration_grid.1LOS.txt")
    if "combHF" in name:
        os.system("echo \"DoTTbarBfilter true\" >> configuration_grid.1LOS.txt")
        os.system("echo \"DoTTbarDileptonFilter true\" >> configuration_grid.1LOS.txt")
    if "combHT" in name:
        os.system("echo \"DoTTbarHTfilter true\" >> configuration_grid.1LOS.txt")
        os.system("echo \"DoTTbarDileptonFilter true\" >> configuration_grid.1LOS.txt")
    if "AllFilt" in name:
        os.system("echo \"DoTTbarHTfilter true\" >> configuration_grid.1LOS.txt")
        os.system("echo \"DoTTbarBfilter true\" >> configuration_grid.1LOS.txt")
        os.system("echo \"DoTTbarDileptonFilter true\" >> configuration_grid.1LOS.txt")
        os.system("echo \"DoTTbarCombination true\" >> configuration_grid.1LOS.txt")

    # # bdt stuff
    # if not args.doLocal:
    #     os.system("sed -i 's$AddBDT1LOS.XMLpath_1l .*$AddBDT1LOS.XMLpath_1l ./$' configuration_grid.1LOS.txt")
    #     os.system("sed -i 's$AddBDT1LOS.XMLpath_2l .*$AddBDT1LOS.XMLpath_2l ./$' configuration_grid.1LOS.txt")

    # Grid job launch 
    if args.verbose: print("Preparing the launch command...")

    if args.username == "prod":
        command = """
        prun --useAthenaPackages --cmtConfig={0} --official --voms atlas:/atlas/phys-exotics/Role=production \\
        --inDS={1} \\
        --outDS={2} \\
        --writeInputToTxt=IN:in.txt --exec=\"sh breakline.sh in.txt; SM4t-offline configuration_grid.1LOS.txt in.txt; tar -cvf output.tar {3}/\" --outputs=output.tar \\
        --extFile=*.xml
        """.format(os.environ['SM4topsFW_PLATFORM'], datasets, outds, args.outputname)
    else:
        command = """
        prun --useAthenaPackages --cmtConfig={0} \\
        --inDS={1} \\
        --outDS={2} \\
        --writeInputToTxt=IN:in.txt --exec=\"sh breakline.sh in.txt; SM4t-offline configuration_grid.1LOS.txt in.txt; tar -cvf output.tar {3}/\" --outputs=output.tar \\
        --extFile=*.xml
        """.format(os.environ['SM4topsFW_PLATFORM'], datasets, outds, args.outputname)

    if args.verbose: print(command)

    os.system("echo '{}' > launch_grid.sh".format(command))
    
    if not args.doLocal:
        os.system(command)        

    if args.verbose: print
    os.chdir("../")

