# common-framework

## Structure
##### Introduction
##### Useful links
##### Download the framework
##### Ntuple Pre-production
##### Ntuple Post-production
##### Running the post-production
##### Using ttTRF
##### Framework code modifications
##### Updating the TTHbb submodule

---

## Introduction

This project contains the framework of the BSM 4tops analysis for the ntuples production for both 1L/2LOS and 2LSS/3L channels.
This project was forked from the SM 4tops analysis framework ([link](https://gitlab.cern.ch/atlasphys-top/xs/4tops/frameworks/common-framework)).

It is currently structured as follows:
* *Framework*: Directory containing the TTHbb framework and the SM4tops-related packages.
* *preProd*: Python scripts and config files to be used for the ntuple pre-production.
* *postProd*: Python scripts and config files to be used for the ntuple post-production.

## Useful links

* SM4tops twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SMFourTopsXS13TeV
* TTHbb git page: https://gitlab.cern.ch/atlasHTop/TTHbbAnalysis/
* Release 21 software tutorial: https://atlassoftwaredocs.web.cern.ch/ABtutorial/
* AnalysisTop tutorial: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AnalysisTopTutorials
* Git tutorial: https://www.atlassian.com/git/

## Download the framework

The framework can be downloaded as follows:
```
lsetup git || echo ignore alrb
git clone --recursive ssh://git@gitlab.cern.ch:7999/atlas-phys/exot/hqt/bsm_h_4t/common-framework.git
cd common-framework/
```
Assuming that your private ssh key was properly uploaded in gitlab (go to [settings](https://gitlab.cern.ch/profile/keys/) if this is not the case).
The option `--recursive` will automatically download the submodules of the repository (e.g. TTHbbAnalysis).

If TTHbbAnalysis module wasn't properly downloaded, you can retry it manually with the following commands:
```
git submodule init
git submodule update --remote
```

In order for the CI in this repository to run successfully, the following command must be run in order to remove the extra top-level `CMakeLists.txt` from within the `TTHbbAnalysis` framework. You can compile and run locally without running this yourself, as the error it produces is benign.

```
rm Framework/TTHbbAnalysis/CMakeLists.txt
```

## Ntuple Pre-production

The ntuple pre-production refers to the production of the MC and data ntuples from the DAOD derivations.
It includes the extraction of the objects needed for the analysis (e.g. electrons, muons, jets, etc) using
the object definitions specified in the config files. It also extracts various useful variables needed for
the different studies (e.g. Monte-Carlo truth informations).
The events are also pre-skimmed according to the pre-selections specified in the config files.
All the systematic trees/weights are also automatically computed when specified in the config files.

The TTHbb framework is used here as a submodule. To compile it for the first time, you can do the following
commands **inside the common-framework/ directory**:
```
mkdir build
cd build
setupATLAS
(or source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh)
asetup AnalysisBase,21.2.120
cmake ../Framework
cmake --build ./
source */setup.sh
```
The AnalysisBase version needs to be compatible with the version used in the TTHbbAnalysis framework. 
If the compilation fails, retry `cmake --build ./`. 
If you want to pass commands to the native compiler (e.g. the numbers of cores used for the compilation) you can do `cmake --build ./ -- -j N` where N is the number of cores. 
Alternatively you could invoke the compiler more directly by doing `make -j N` instead.

One can then simply set the environment back up by doing the following:
```
cd build/
asetup --restore
source */setup.sh
```

The python script and the config files for the ntuple pre-production are kept inside the `preProd`
subdirectory. It includes:
* `cuts_*.txt`: the config files used for the ntuple production,
* `confGenerator.py`: pythin script used to create automatically the config files,
* `[data/mc16*]_SM4tops.py`: python script containing the lists of the input data/MC derivation samples,
* `updateTOPQ1helper.py`: python script used to create the lists of the input data/MC derivation samples,
* `01SubmitToGrid_simple.py` and `submitToGrid_*.py`: scripts used to launch ntuples pre-production in the grid,
* `validation.py`: python script used to compare the output ntuple between different productions,
* `check_production.py`: python script used to monitor a specific ntuple production in the grid,
* `check_production.py`: python script used to create a list a command to replicate the grid datasets into a specific grid site.

If you want to produce ntuples locally, you can do the following command:
```
cd preProd/
top-xaod <config> <list-input-files>
```
`<config>` should be replaced by the name of your config file (e.g. `cuts_mc16a_SM4tops.txt`), and
`<list-input-files>` should be replaced by either the name on an input DAOD file or the name of
a txt file that contains a list of input DAOD file names.

If you want to launch the ntuple production in the grid, you can use the script `01SubmitToGrid_simple.py`.
You would have to modify it with your own grid username (*gridUsername*), the destination of the grid site
to store the output ntuples (*destSE*, leave that empty to use the scratch area), your config files
(*settingsFile*), the suffix of the output datatsets (*suffix*) and the MC/data samples to process.


## Ntuple Post-production

:warning: **FRAMEWORK IS STILL NOT UPDATED FOR OFFLINE SAMPLES   -->  TO BE UPDATED**

The ntuple post-production refers to the generation of mini-ntuples for dedicated analysis using the pre-produced
ntuple in input. This includes the addition of new variables from dedicated tools (BDTs, Fake, ttTRF, etc) and
some extra-skimming steps.

For the moment, no official code is supported, but your can use the offline code of the TTHbbAnalysis framework.
First, you need to compile the code as follows:
```
mkdir build
cd build
asetup AnalysisBase,21.2.120
cmake -DATLAS_PACKAGE_FILTER_FILE=../Framework/OffSM4topsTool/PackageFilters/offline_packages.txt ../Framework/
cmake --build ./
source */setup.sh
```

To produce the mini-ntuples using the following command:


## Running the post-production
:warning: **TO BE UPDATED**

Paolo created an offline-wrapper in his branch which you can checkout by

```
git checkout user/psabatin/offline-wrapper
```
this branch should be merged in the future and is kept up-to date with the master branch.

After checking out the branch it has to be build as explained in *Ntuple Post-production*.

Then setup, from `common-framework` directory, the path to find the python executable in `Framework/OffSM4topsTool/python` (where the code is located)
```
export PATH=$PATH:$PWD/Framework/OffSM4topsTool/python
```
and run (in any working directory)

```
offline-wrapper.py --samples <sample> --typology  <type> --analysis <analysis> --input-path <input-path>  --output-path <input-path>
```
To know the default settings and the meaning of the settigs, read the comments in `offline-wrapper.py` file,or just type

```
offline-wrapper.py -h
```

A dedicated [README for the offline wrapper](https://gitlab.cern.ch/atlasphys-top/xs/4tops/frameworks/common-framework/blob/user/psabatin/offline-wrapper/Framework/OffSM4topsTool/python/README.md) is also available 

The advantages of the wrapper are that
* It checks in the input_path (that can be set very easily to the group eos-area by using the flag --eos-area) if all necessary samples are available and, if rucio is setup, downloads missing samples
* It provides pre-defined sets of variables that can be used so you do not have to worry about which variable you want
* It supports all tools such as e.g. ttTRF since it only redirects the input variables in a propper way
* It still remains fully customable since. For example it only groups variables in a dictionary which is fully customisable
* It essentially is only made out of the *offlline-wrapper.py* file and the *./lib/Class.py file* and is therefore easy to understand and to adapt


If you do not want to use the wrapper you can produce mini-ntuples by

```
cd run/
ttH-offline configuration.txt <list-input-files>
```

Examples of config scripts are avaialable in `postProd/` subdirectory or in Framework/TTHbbAnalysis/OfflineTTHbb/share/.
The required config files are 'weights.txt', 'VariablesToRead.txt', 'VariablesToSave.txt' and 'configuration.txt'



## Using ttTRF
:warning: **TO BE UPDATED**

The code to use Tag Rate Functions (TRF) is also not yet in the master (20.11.18). If you want to use it you can check the following branch

```
https://gitlab.cern.ch/atlasphys-top/xs/4tops/frameworks/common-framework/blob/user/gpanizzo/ttTRFdev/Framework/OffSM4topsTool/README.md
```

Compiling the tool the tool is similar to prevous steps but with some preparational steps. A [README](https://gitlab.cern.ch/atlasphys-top/xs/4tops/frameworks/common-framework/blob/user/gpanizzo/ttTRFdev/Framework/OffSM4topsTool/README.md) is also available.

First check not to have any remainder piece of old TtTRF code anywhere else, i.e. you should get

>>>
[USER common-framework]$ cd Framework/TTHbbAnalysis/  
[USER TTHbbAnalysis]$ grep -R "TtTRF"  
Dockerfile:COPY TtTRF          	     source/TtTRF
>>>

(you can remove also this last occurrence, and maybe anybody is volunteering 
to ask for a merge request of our fork of TTHbbAnalysis with everything stripped like here shown?)

Then just follow instructions for the offline code, BUT using the PackageFilter inside this folder:
```
cd common-framework #!!
mkdir build  #!!
cd build  #!!
asetup AnalysisTop,21.2.83 || echo ignore alrb  #!! (or --restore from the 2nd  time)
cmake -DATLAS_PACKAGE_FILTER_FILE=../Framework/OffSM4topsTool/PackageFilters/offline_packages.txt ../Framework #!!
cmake --build ./ #!!
source */setup.sh #!!
```
---

## BDT implementation in 1LOS 
:warning: **TO BE UPDATED**

The framework supports the implementation of BDT score for the 4tops 1LOS channel. The BDTs are assumed to be trained semi-inclusively, i.e. inclusive in b-jets multiplicity (3bi) but exclusive in jet multiplicity (8je, 9je and 10ji for 1l, and 6je, 7je and 8ji for 2l), using a 3-fold cross-validation procedure based in  events with different modulo 3 event numbers. The implemented BDT score variables are named `BDT_1l_score` and `BDT_2l_score`, and they are initialized to be equal to -999 before being calculated.

The following options are implemented in the SM4t-offline script, here the most up-to-date recommendation (set 1.3): 
```
             AddBDT1LOS.TurnOn:	true
                              	(Tool to include the BDT score)
         AddBDT1LOS.XMLpath_1l:	/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/BDTXMLs/BDT_050919_1l_set13
                              	(Path to the 1l BDT XML files)
         AddBDT1LOS.XMLpath_2l:	/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/BDTXMLs/BDT_050919_2l_set13
                              	(Path to the 2l BDT XML files)
            AddBDT1LOS.Vars_1l:	HT_all,jet_pt_0_:jet_pt[0],nJets/I,Centrality_all,dRjj_Avg,dRbb_MindR_MV2c10_70,dRbl_MindR_MV2c10_70,Mjjj_MindR:Mjjj_MindR_new,Mbbb_Avg_MV2c10_70,Mbb_MinM_MV2c10_70,nRCJetsM100/I,Sum__rcjet_d12_:Sum$(rcjet_d12),Sum__rcjet_d23_:Sum$(rcjet_d23),mtw,met_met,Sum__jet_pcb_MV2c10_btag_ordered_T__Iteration__6__:Sum$(jet_pcb_MV2c10_btag_ordered*(Iteration$<6))
                              	(List of variables used by the 1l BDT separated by commas)
            AddBDT1LOS.Vars_2l: HT_all,jet_pt_0_:jet_pt[0],nJets/I,Centrality_all,dRjj_Avg,dRbb_MindR_MV2c10_70,dRbl_MindR_MV2c10_70,Mjjj_MindR:Mjjj_MindR_new,Mbbb_Avg_MV2c10_70,Mbb_MinM_MV2c10_70,nRCJetsM100/I,Sum__rcjet_d12_:Sum$(rcjet_d12),Sum__rcjet_d23_:Sum$(rcjet_d23),met_met,Sum__jet_pcb_MV2c10_btag_ordered_T__Iteration__6__:Sum$(jet_pcb_MV2c10_btag_ordered*(Iteration$<6))	
                              	(List of variables used by the 2l BDT separated by commas)
```

The `AddBDT1LOS.XMLpath_1l/2l` option is used to define the path of the xml files for the 1l or 2l BDT. The xml files should then respect the following name convention `TMVA_4tops_<channel>_<region>_cross_val_sample_<0/1/2>_BDTG.weights.xml` where `<channel>` refers to either 1l or 2l, `<region>` to the semi-inclusive region (e.g. `9je3bi`) and `<0/1/2>` corresponds to the modulo%3==0 events-trained (0), modulo%3==1 events-trained (1) and modulo%3==2 events-trained (2) xml files.

The `AddBDT1LOS.Vars_1l/2l` option is used to define the list of variables separated by commas for the 1l or 2l BDT reading. They should be defined using the following format `<name>:<bdt name>/<type>` where `<name>` is the name of the event-level variable defined in the framework, `<bdt name>` is the name of the corresponding variable as defined in the xml file, amd `<type>` being the type of the variable : '/F' for float and '/I' for integer. If no '<bdt name>' is defined, it is assumed to be equaled to `<name>`. If no '<type>' is defined, the variable is considered as float by default. 

Example of list of variables definition : 
```
AddBDT1LOS.Vars_1l HT_jets,mJSum,nBTags_MV2c10_77/I,nJets/I,nRCJets/I,jetpt0:jet_pt[0],jetpt1:jet_pt[1],jetpt2:jet_pt[2],jetpt3:jet_pt[3]
```

*Important note:* 
 * The implementation only supports float or integer **event-level** variable for '<name>'.
 * Since the list of variables is defined in a comma-separated string, the use of commas should be avoided in the names of the variable. 
 * Since ':' is used to create the correspondence between the variable in the ntuples and in the xml file, the use of ':' should be avoided in the names of the variable.
 
## Framework code modifications

To make any modification, you need to follow the system of branches explained in the
[TTHbb repository](https://gitlab.cern.ch/atlasHTop/TTHbbAnalysis/):

* At first pull the latest version of `master`, so you're sure you have a good basis: `git pull origin master`
* Create a branch and switch to it:  `git checkout -b user/<username>/description`
* Stage your modifications: `git add <file>` or `git add .` or `git add -a` and so on
* Commit your modifications: `git commit -m "useful message"`
* Push your branch to the remote repository: `git push -u origin user/username/description`

In case you are running into problems with the submodules such as TTHbbAnalysis you can try
* `git submodule update –init`
* `git submodule update –remote`
to update the submodule from remote.

Once you're done with your modifications and you tested it locally, you can then request to merge the
branch into the master branch using the gitlab web interface. 
It is recommended to create a merge request with `WIP:` in front of the title as soon as possible 
so that other people can already see and discuss the changes being made. 
This reduces merge problems and it's generally a nice starting point for good discussions.

**Important**: since we are trying to separate the SM4tops code from the TTHbb framework as much as possible
 in order to be as independent as possible from the TTHbb team, we ask every analyzer to modify the TTHbb framework 
as little as possible, preferably not at all.

## Updating the TTHbb submodule
:warning: **TO BE UPDATED**

In the current set-up, the TTHbb submodule points to our fork of that repository, so we are responsible for updating it, although it is not necessary to do this too often. There are multiple workflows of how to do it..here's the one I've been using:

1. get into the right directory (so that we're using the git of the submodule): `cd common-framework/Framework/TTHbbAnalysis`
2. check that you've already added the "upstream" original package (only once) `git remote -v`, which should result in
```
origin	ssh://git@gitlab.cern.ch:7999/atlasphys-top/xs/4tops/frameworks/TTHbbAnalysis.git (fetch)
origin	ssh://git@gitlab.cern.ch:7999/atlasphys-top/xs/4tops/frameworks/TTHbbAnalysis.git (push)
upstream	ssh://git@gitlab.cern.ch:7999/atlasHTop/TTHbbAnalysis.git (fetch)
upstream	ssh://git@gitlab.cern.ch:7999/atlasHTop/TTHbbAnalysis.git (push)
```
If you're missing the `upstream` part, do `git remote add upstream ssh://git@gitlab.cern.ch:7999/atlasHTop/TTHbbAnalysis.git`.

3. We want to update master so let's go there: `git checkout master`
4. Update it: `git pull upstream master`. This should pull everything nicely from the original TTHbb master branch, which will **very likely** result in a merge conflict which we need to resolve because we changed the CMakeLists a bit. Resolve the merge conflict in your preferred way [like so](https://help.github.com/articles/resolving-a-merge-conflict-using-the-command-line/) or through gitLab, for example.
5. Make sure the merged and updated master is in our remote fork (on gitlab), so if you resolved the merge conflict locally, you need to push the changes to `origin`: `git push origin master`, if you did it through gitLab it should already be there.
6. Create a merge request to merge the newly updated master with the branch we're actually using `group/4Top/master`. (This step might be removed as we might just use the master branch directly in the future)
7. Checkout and pull the group branch locally: `git checkout group/4Top/master` and `git pull origin group/4Top/master`
8. Done

This sounds fairly "elaborate", but it really does not take much time and might even be simplified quite a bit in the future. On the other hand, this allows us to be very independent from the TTHbb team, do all the merge conflict resolution on our side and it allows us to use their framework without us disturbing them.

<!--
** Important **: Because we use the TTHbbAnalysis framework as a submodule, one need to keep in mind that all
the commits will be separate based on which repository they are applied to. That means that any modification
inside the TTHbb framework will be propagated in the TTHbb repository, not in the 4tops repository.

One can update the TTHbb submodule to a newer version inside the 4tops repository. To do so, just pull the git
project inside the TTHbbAnalysis subdirectory. Then go to the parent directory and commit the update by doing
`git add TTHbbAnalysis`.

You can also modify the TTHbb framework by creating a branch as described above, inside the TTHbbAnalysis
subdirectory. The merging request would then be approved by the TTHbb team.
-->