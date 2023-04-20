# Producing offline ntuples (post-production) using the grid

## Introduction

This folder contains the necessary scripts to produce offline ntuples using the grid system. 

While the TTHbb framework was designed to produce the offline ntuples locally or using batch system, 
it quickly turns out to be complicated to make the full production in an efficient and fast way because 
of the large size the input online ntuples and the number of systematic trees to consider.
The eos area was also found to be unreliable because of its unstability and its limited size capacity.

This new production workflow is designed to use the grid system to produce the offline ntuples. 
The advantage of this workflow is that the online ntuples do not need to be downloaded 
as the grid job will directly use the datasets where they are stored. 
Moreover, the production is faster (~ few days), more efficient (lower fail rate) and easier to monitor.

However, this procedure brings more complexity mainly due to the fact that the TTHbb offline production
was not originally designed to be performed in the grid. 

First, a grid job does not produce a single but several outputs. 
When the input dataset contains several files, the grid job is usually split in several subjobs that are run subsets of these input files.
Also, in the TTHbb framework, each run will often produce an output file for each region defined in the run configuration.
Therefore, the output of the offline production is not a single root file, but a folder that contains a sub-folder for
each region, each of them containing an output root file.
In this workflow, this complexity is solved by concatenating the output folder into a tarball and by
adding an extra step which merge the output of all the subjobs.

Second, the TTHbb framework is designed to compute the normalisation weight (i.e. the cross section over the sum of the event weight before any selection).
This needs to be perfomed with all input folder to extract the total sum of event weights.
In the grid production, this can't be perfomed as the grid job are split in several grid subjobs.
This was solved my adding an extra option to save the `sumWeights` tree that contains the necessary information to recompute the sum of event weights in post-production.
An extra step is then added to compute the final normalisation weight and decorate it in the nominal and systematic trees.

Contact:
 - Simon Berlendis (simon.berlendis@cern.ch)

## Description 

The folder contains the following files:
 * `dict_datasets.py`: python script that contains dictionnaries of the input datasets (online ntuples) and the list of systematic trees.
 * `produce_grid_offline.py`: python script to launch grid jobs for offline production
 * `merge_grid_offline.py`: python script to merge the output files from the grid datasets
 * `decorate_grid_offline.py`: python script to decorate the output files with the normalisation weight 
 * `ProductionUtils.C`: Root script that contains the tool to compute the normalisation weight and to decoreate the input file with it.

## Prerequisite

The framework need to be downloaded and compiled in a Tier3 area (e.g. lxplus, bnl). See the Readme in the parent directory.

Setup panda, rucio and your voms:
```bash
lsetup panda rucio pyami
voms-proxy-init -voms atlas
```

If you want to run the production with your exotic production role (if you have it):
```bash
voms-proxy-init --voms atlas:/atlas/phys-exotics/Role=production
```

## Step 1: launch the grid jobs

The grid jobs are launched using the `produce_grid_offline.py` script. 

First, open the script and change the jobs list of the samples you want to run. 

Then, launch the script as follow:
```bash
./produce_grid_offline.py -u <username> -p <post_name> -n <output_name>
```
Where `<username>` is the grid username,  `<post_name>` is the suffix name added in the datasets (used for versioning the production), and  `output_name` is the name of the output folder.

The script creates a folder for each grid job, copies the necessary config and txt files to run the grid jobs, modifies them and launch the grid job using the prun command.
The grid job launches the excutable SM4t-offline for each bunch of input files. 
The output files of each subjob are contained in an output folder with the name set by the `-n` option. 
The output files are then concatenated in a tarball.

## Step 2: wait...

Wait that the grid jobs are finished. You can monitor the grid jobs using the bigpanda interface: https://bigpanda.cern.ch/user/.

If some jobs are failed, you can retry them using pbook:
```bash
pbook -c "retry(<job id>)"
```
where `<job id>` is the id of the failed/finished job.

## Step 3: download the finished datasets 

Once the grid jobs are finished, download the output datasets using rucio:
```bash
rucio download --ndownloader 5 <dataset_name>
```

Be sure to download the download the datasets in an area where you have enough disk space (e.g. eos, scratchdisks).

## Step 4: merge the output files

Merging the output files from the datasets are performed using a python script called `merge_grid_offline.py`. 

The script can be used as follow:
```bash
./merge_grid_offline.py -i <input_dir> -o <output_dir> -n <output_name> 
```
where `<input_dir>` is the input directory where the output datasets are downloaded, 
`<output_dir>` is the output directory where you want to have your offline ntuples stored 
and `<output_name>` is the name of the output as defined by the `-n` option in the production script.

The script will unconcatenate each tarball of the datasets in the temporary directory (which is by default set by the environment variable $TMPDIR).
It will then merge the files using the `hadd` command.

By default, the script will merge all the datasets in the `<input_dir>` directory. 
If you want to only merge some specific datasets, you can select them using the `-f` option. 
The script will then only merge the datasets where the pattern defined by the `-f` is contained in the output datatset name. 
Wildcard characters (e.g. `*`) are however not supported yet.

## Step 5: Decorate the output files with the normalisation weights

The decoration is performed using the python script called `decorate_grid_offline.py`.

The script cen be used as follow:
```bash
./decorate_grid_offline.py -i <intput_path>
```
where `<intput_path>` is the input path of the directory where the offline ntuples are stored.

The script will loop over the files in the input directory and launch the function `add_normalisation` defined in the file `ProductionUtils.C`.
The function will loop over the elements of the `sumWeights` tree is extract the input sum of event weight and the cross section. 
It will then compute the normalisation weight for each DSID. 
Finally, the function loop of the input trees and decoreate each event with the correspoding normalisation weight. 
If the input file is not a systematic ntuple, the function will also decorate the normalisation weight for each internal mc weight.

If you want to run on some specific files, the `-f` option can be used to select the specific input files to compute the normalisation weight.


