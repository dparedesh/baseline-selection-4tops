#!/usr/bin/env python

import os, shutil
import sys
import argparse
import subprocess

if __name__ == "__main__":

    input_path = "/eos/home-s/sberlend/BSM4t/"
    output_path = "/eos/home-b/bsm4tops/bsm4teos/"
    output_name = "BSM4t-2121200-1LOS-v1"
    zip_directory = "BSM4t-2121200-1LOS-v1"
    tmp_folder = os.environ['TMPDIR']

    parser = argparse.ArgumentParser(description='Merge the grid output offline ntuples.')
    parser.add_argument("-i", "--input", dest='input_path', default='',
                        help='Input path with the grid output datasets are stored.')
    parser.add_argument("-o", "--output", dest='output_path', default='',
                        help='Output path where to store the ntuples.')
    parser.add_argument("-n", "--name", dest='output_name', default='',
                        help='Name of the output directory (e.g BSM4t-2121200-1LOS-v1).')
    parser.add_argument("-zd", "--zip-dir", dest='zip_directory', default='',
                        help='Name of the zip directory (e.g BSM4t-2121200-1LOS-v1).')
    parser.add_argument("-t", "--tmp", dest='tmp_folder', default='',
                        help='Temporary path (e.g /tmp/username/username/).')
    parser.add_argument("-f", "--file", dest='file_torun', default='',
                        help='to be used if you want to run on a specific ntuple.')
    
    args = parser.parse_args()
    if args.input_path != '': input_path = args.input_path
    if args.output_path != '': output_path = args.output_path
    if args.output_name != '': output_name = args.output_name
    if args.zip_directory != '': zip_directory = args.zip_directory
    if args.tmp_folder != '': tmp_folder = args.tmp_folder
        
    output_folder = os.path.join(output_path,output_name)
    if not os.path.isdir(output_folder): os.mkdir(output_folder)
    print(output_folder)

    datasets = os.listdir(input_path)
    
    for ds in datasets:        

        if args.file_torun != '' and not args.file_torun in ds: continue
        print('DataSet: '+ds)

        ntuple_name = ds.split(".")[2]+".root"
        print('Ntuple: '+ntuple_name)

        filenames = os.listdir(os.path.join(input_path,ds))
        ntuples_to_merge = {} # sorted by regions

        for fname in filenames:
            print('File name: '+fname)

            print("Unzip loca: {0}{1}/{2} -C {3}".format(input_path, ds, fname, tmp_folder))
            os.system("tar xvf {0}{1}/{2} -C {3}".format(input_path, ds, fname, tmp_folder))
            tmp_output = os.path.join(tmp_folder,fname.replace(".tar.gz",""))
            print('TMP output: '+tmp_output)
            if os.path.isdir(tmp_output): os.system("rm -r "+tmp_output)
            print('MV: '+"{0}{1}/ {2}".format(tmp_folder,zip_directory,tmp_output))
            os.system("mv {0}{1}/ {2}".format(tmp_folder,zip_directory,tmp_output))

            for dirpath, regions, ntuples in os.walk(tmp_output):

                for r in regions:
                    if r not in ntuples_to_merge: 
                        ntuples_to_merge[r] = []

                for n in ntuples:
                    region = dirpath.split("/")[-1]
                    ntuples_to_merge[region].append(os.path.join(dirpath,n))

        for region, ntuples in ntuples_to_merge.items():

            list_input_ntuple = ""
            for n in ntuples: list_input_ntuple += " "+n
            
            output_region = os.path.join(output_folder,region)
            if not os.path.isdir(output_region): os.mkdir(output_region)

            output_ntuple = os.path.join(output_region,ntuple_name)
            if os.path.isdir(output_ntuple): os.system("rm -rf "+output_ntuple)

            print("hadd -v -f {0} {1}".format(output_ntuple,list_input_ntuple))
            os.system("hadd -v -f -j 8 {0} {1}".format(output_ntuple,list_input_ntuple))
