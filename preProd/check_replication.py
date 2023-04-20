#!/usr/bin/env python

# Replication checker for the SM4tops ntuple production
#
# Produce a txt file containing the commands to save the samples in a local group disk 
#
# simon.berlendisATcern.ch

import sys, os, argparse
import rucio.client

# Define the couple (scope, stream name)
prods = [
    ("group.phys-top", "data_1LOS"),
    ("group.phys-top", "mc16a_1LOS"),
    ("group.phys-top", "mc16d_1LOS"),
    ("group.phys-top", "mc16e_1LOS"),
    ("user.zzheng", "data_SSML"),
    ("group.phys-top", "data_SSML"),
    ("user.zzheng", "mc16a_SSML"),
    ("group.phys-top", "mc16a_SSML"),
    ("group.phys-top", "mc16d_SSML"),
    ("user.yaqin", "mc16e_SSML"),
    ("group.phys-top", "mc16e_SSML"),
    ]

# samples to be ignored
ignore = [
    "group.phys-top.periodI.physics_Main.DAOD_TOPQ1.grp18_v02_p3544_p3553_p3583.SM4t-212560_data_1LOS_preodTest_v2_output.root",
    "group.phys-top.410080.MadGraphPythia8EvtGen.DAOD_TOPQ1.e4111_s3126_r9364_p3629.SM4t-212560_mc16a_1LOS_preodTest_v1_output.root",
    "group.phys-top.410080.MadGraphPythia8EvtGen.DAOD_TOPQ1.e4111_s3126_r9364_p3629.SM4t-212560_mc16a_1LOS_PRODTEST_v1_output.root",
    "group.phys-top.364242.Sherpa.DAOD_TOPQ1.e5887_s3126_r10724_p3629.SM4t-212560_mc16e_1LOS_prodTest_v3_output.root",
    "group.phys-top.412045.aMcAtNloPythia8EvtGen.DAOD_TOPQ1.e6951_e5984_a875_r10724_p3629.SM4t-212560_mc16e_1LOS_prodTest_v3_output.root",
    "group.phys-top.410080.MadGraphPythia8EvtGen.DAOD_TOPQ1.e4111_s3126_r9364_p3629.SM4t-212560_mc16a_SSML_prodTest_v1_output.root",
    "group.phys-top.410080.MadGraphPythia8EvtGen.DAOD_TOPQ1.e4111_s3126_r10201_p3629.SM4t-212560_mc16d_SSML_prodTest_v1_output.root",
    "group.phys-top.periodI.physics_Main.DAOD_TOPQ1.grp18_v02_p3544_p3553_p3583.SM4t-212560_data_SSML_prodTest_v2_output.root",    
    ]


########################################################
# Anything after this point should  fine if left as is #
########################################################

parser = argparse.ArgumentParser(description='Replication checker for the SM4tops ntuple production')
parser.add_argument("tag", help="The ntuple production tag (e.g. 212560).")
parser.add_argument("-r", "--rse", dest='rse', default="BNL-OSG2_LOCALGROUPDISK", 
                    help="The rse name where the files will be replicated (default: BNL-OSG2_LOCALGROUPDISK).")

args = parser.parse_args()
tag = args.tag
rse = args.rse

fileName = "replicate."+tag+".txt"
outputfile = open(fileName, "w") 
print("Writing into file " + fileName )

# setup the rucio client
rucioClient = rucio.client.Client()
# to print functions contained in the rucio client: 
# print(dir(rucioClient))

# loop over stream production
for scope, stream in prods:

    name = scope+".*SM4t-"+tag+"_"+stream+"*output.root"
    print("Checking "+name)

    # loop over containers
    for sample in sorted(rucioClient.list_dids(scope=scope, filters={"name":name}, type="container")):
        # print(sample)
        if sample in ignore: continue

        # check whether the sample has replicas
        files = list( rucioClient.list_replicas([{"scope":scope, "name":sample}]))

        # Ignore empty containers 
        if len(files) == 0: continue

        # loop over files in the container, 
        # if one of the file is not in a local group disk, the container will be replicated
        toBeReplicated = False
        for f in files:
            isFileSaved = False

            for file_rses in f["rses"]:
                if file_rses.find("LOCALGROUPDISK") >= 0 : 
                    isFileSaved = True

            if not isFileSaved: 
                toBeReplicated = True
                break

        # print into the file
        if toBeReplicated: 
            print >> outputfile, ("rucio add-rule --lifetime 31536000 {0} 1 {1}".format(sample, rse))

print("Done")

