# SM4t-offline wrapper code 

## Introduction

This python wrapper has be developed to give a benchmark for the 4tops analysis in the production of offline ntuples. This wrapper should help the users to have an harmonized offline ntuples production.

The main task is to launch the production of offline ntuples without giving in input any list or configuration file to `SM4t-offline` command. Given in input which kind of process (online sample) you want to process, the code:
* checks whether all the samples for the required process are stored in the input directory
  * if the directory does not exist, or it is not specified it creates one in your `/eos/` space for the online ntuples storage
  * if ntuples are missing in the path it downloads them
* Create configuration files based on the requirements in input
  * variables to store
  * regions
  * tools output to add
  * systematics
* Run it locally or on the grid


## Instructions

You should just setup the usual offline environment in the `build/` directory
```
asetup --restore
cmake ../Framework
cmake --build ./
source */setup.sh
```
Then setup, from `common-framework` directory, the path to find the python executable in `Framework/OffSM4topsTool/python`
```
export PATH=$PATH:$PWD/Framework/OffSM4topsTool/python
```
and run (from any directory)
```
offline-wrapper.py --samples <sample> --typology  <type> --analysis <analysis> --input-path <input-path>  --output-path <input-path>
```
To know the default settings and the meaning of the settigs, read the comments in `offline-wrapper.py` file,or just type

```
offline-wrapper.py -h
```
**IMPORTANT** the input datasets are taken from files in the format `<analysis>_<typology>.py` in the `lib` folder. 

### Available options
Here is a list of the possible options to be used in running the code:

<dl>
  <dt><b> --samples, -s </b> SAMPLES</dt>
  <dd>Comma separated list of the samples to process. The list of ntuples for each sample is specified in files of the type '<analysis>_<type>.py' in 'lib/' 
  <br />Available: 4tops, ttbar, single-top, ttX, vjets, vv,  all, single-top-alt, ttbar-alt.
  </dd>

  <dt><b> --analysis, -a </b> ANALYSIS</dt>
  <dd> Specifies the ntuples to be used, from which analysis they belong.
  <br />Available: SM4t-212290, SM4t-212380 (Default), ttHbb, SM4t-212560.
  </dd>

  <dt><b> --channel </b> CHANNEL</dt>
  <dd> Specifies the selection channel.
  <br />Available: SSML, 1LOS, BSM.
  </dd>
  
  <dt><b> --type, -t </b> TYPE</dt>
  <dd> Type of the samples: period of MC production or data
  <br />Available: mc16a, mc16cd, mc16e, mc16acd, mc16acde, data.
  </dd>

  <dt><b> --batch, -b </b> BATCH </dt>
  <dd> Flag the execution on the batch, to choose between 'lsf' and 'condor' (recommended)
       <dl>  
       <dt><b> --batch-queue, -q </b> BATCH-QUEUE</dt>
       <dd> Batch queue to use (just for LSF)</dd>	     
       <dt><b> --batch-ncores, -n </b> NCORES</dt>
       <dd> Number of cores to use (just for LSF)</dd>	     
       <dt><b> --batch-mail </b> MAIL</dt>
       <dd> Enable notifications and specify the e-mail (just for HTCondor)</dd>	     
       </dl>
  </dd>
  
  <dt><b> --eos-area</dt>
  <dd> Use as input the our eos area and make everything compatible with the folder structure. The default output is still kept to '/eos/user' area
  </dd>


  <dt><b> --input-path, -i </b> INPUT-PATH</dt>
  <dd> Absolute path to the folder containing the online samples in input
  <br /> Default: '/your/eos/area/analysis_ntuples/online/channel'
  </dd>

  <dt><b> --output-path, -o </b> OUTPUT-PATH</dt>
  <dd> Absolute path to the folder where to store the output
  <br /> Default: '/your/eos/area/analysis_ntuples/offline/channel`
  </dd>

  <dt><b> --variables-to-store, -v </b> VARIABLES </dt>
  <dd> Comma-separated list of sets of variables you want to store in your output file. Default: event.
       <dl>  
       <dt> event</dt>
       <dd> 'eventNumber', 'runNumber', 'nBTags_(MV2c10/DL1)_(60/70/77/85)', 'nMuons', 'nElectrons', 'nJets', 'HF_SimpleClassification', 'HT_all', 'HT_jets'  </dd>
       <dt>jet</dt>
       <dd> 'pt', 'eta', 'phi', 'e', 'mv2c10/dl1' scores, 'passfjvt', 'isbtagged_(MV2c10/DL1)_(60/70/77/85)', 'tagWeightBin_(MV2c10/DL1)_Continuous' </dd>
       <dt>lepton</dt>
       <dd> electrons and muons: 'pt', 'eta', 'phi', 'e', 'charge', 'isTight' </dd>
      <dt>rcjet</dt>
       <dd> 'pt', 'eta', 'phi', 'e,' 'd12', 'd23' </dd>
      <dt>met</dt>
       <dd> 'met', 'phi' </dd>
      <dt>truth</dt>
       <dd> 'el_true_type', 'el_true_origin', 'el_true_firstEgMotherTruthType', 'el_true_firstEgMotherTruthOrigin', 'el_true_isPrompt', 'el_true_pdg', 'el_true_pt', 'el_true_eta',  'mu_true_type', 'mu_true_origin', 'mu_true_isPrompt', 'mu_true_pdg', 'mu_true_pt', 'mu_true_eta', 'jet_truthflav', 'jet_truthPartonLabel', 'jet_trutflavExtended', 'jet_isTreuHS' </dd>
      <dt>lepton-iso</dt>
       <dd> 'el_topoetcone20', 'el_ptvarcone20', 'el_LH*', 'el_isPrompt', 'el_iso*', 'mu_topoetcone20', 'mu_ptvarcone30', 'mu_LH*', 'mu_isPrompt', 'mu_iso*'   </dd>
       <dt>jet-btag</dt>
       <dd>  'jet_MV2c10mu', 'jet_MV2c10rnn', 'jet_DL1mu. 'jet_DL1rnn', 'jet_MV2c100', 'jet_MV2cl100', 'jet_DL1_p*', 'jet_DL1mu_p*', 'jet_DL1rnn_p*'</dd> 
       <dt> qg </dt>
        <dd> 'jet_nTrkPt*', 'jet_TrackWidthPt*', 'jet_Charge', 'jet_N90Constituents', 'jet_Width*' </dd>
  	<dt> common </dt>
	<dd>  'nRCJets', 'mJSum'</dd>
	<dt> 1los-var </dt>
	<dd> Currently empty  </dd>
	<dt> ss3l-var </dt>
	<dd> 'chargeIDBDTLoose', 'chargeIDBDTMedium', 'chargeIDBDTTight' </dd>
	
      </dl>
  </dd>
  
  <dt><b> --regions, -r </b> REGIONS</dt>
  <dd> Comma-separated list of regions
  <br /> Available: Nj (nJets == N), geNj (nJets >= N), 1l (OR{'ejets*', 'mujets*}), 2los (OR{'ee*', 'mumu*','emu*'} or OR{'OSee*', 'OSem*'} if LooseSSML), 1l2los (OR{1l, 2los}), 2lss (OR{'SSee*','SSem*','SSmm*'}), 3l (OR{eee,eem,emm,mmm}), 2lss3l (OR{2lss,3l}) ljetsNj (1l & Nj), ljetsgeNj (1l & geNj), 2losNj (2los & Nj), 2losgeNj (2los & geNj), ljets2losNj (1l2los & Nj), ljets2losgeNj (1l & geNj), 2lss (2lss & Nj), 2lssgeNj (2lss & geNj), 3lNj (3l & Nj),3lgeNj (3l & geNj), 2lss3lNj (2lss3l & Nj), 2lss3lgeNj (2lss3l & geNj), looseSSML (2los or 2lss3l).      
  </dd>

  <dt><b> --config, -c </b> CONFIG</dt>
  <dd> Run the code with an external `configuration.txt`. The absolute path of the config file is required. The other files (`VariablesToSave.txt`, `weights.txt` etc) are copied from paths in the given config.
  </dd>

  <dt><b> --systematics </b> SYSTEMATICS</dt>
  <dd> Comma-separated list of the systematics to be included. This changes also the file to read, that are in dedicated `*_sys.py` files. The list of systematics for each specified type are read from an internal dictionary of the class. Very dummy at the moment, shaped on ttHbb ntuples. 
  <br /> Available: nominal, sys, jer (SM4t-212380), jet (SM4t-212560)
  </dd>
  
  <dt><b> --tools </b> TOOLS</dt>
  <dd> Comma separated list of tools to be included.
  <br /> Available: Add1LOSVar, TtTRF, DoTTbarDileptonFilter, DoTTbarBfilter (in general all tools from TTHbbAnalysis and TtTRF can be ran if the default configs are set properly)
  </dd>
  
  <dt><b> --Add1LOSVar-opt </b> OPTIONS</dt>
  <dd> Comma separated list of option to be passed to the SM4topAnalysis tool (if included). The format is of the type <option>:<vallue>. The default options are:
       <dl>  
       <dt>MeVToGeV</dt>
       <dd> Converts the values from MeV to GeV</dd>	     
       </dl>
  </dd>


  <dt><b> --TtTRF-opt </b> OPTIONS</dt>
  <dd> Comma separated list of option to be passed to the TtTRF tool (if included). The format is of the type <option>:<value>. The default options are:
       <dl>  
       <dt>nBTags</dt>
       <dd> Number of btags in the signal region. Default: 3b</dd>
       <dt>Path</dt>
       <dd> Path to the efficiency files. Default: /eos/atlas/atlascerngroupdisk/phys-top/4tops/ljetsOS/DDbkgEstimate/ttTRF/data/2.0.5/dd>
       <dt>Sample</dt>
       <dd> On which sample the method has to be applied. Default: data_mc16a</dd>
       <dt>Selection</dt>
       <dd> Region for the efficiency extraction. Default: 5j</dd>
       <dt>Systematic</dt>
       <dd> Which systematic to be processed. Default: nominal</dd>
       <dt>Parametrisation</dt>
       <dd> Parametrisation of the efficiecies. Default: pt/dRj</dd>
       <dt>Debug</dt>
       <dd> Option for debugging. Default: False</dd>
       </dl>
  </dd>

  <dt><b> --OfflineFakes-opt </b> OPTIONS</dt>
  <dd> Comma separated list of option to be passed to the OfflineFakes tool (if included). The format is of the type <option>:<value>. **NOTE** not recently tested. The default options are:
       <dl>  
       <dt>DataPath2015</dt>
       <dd> Path to the 2015 efficiencies. Default: /eos/user/p/psabatin/Rel2_efficiencies_20180615</dd>
        <dt>DataPath2016>/dt>
       <dd> Path to the 2016 efficiencies. Default: /eos/user/p/psabatin/Rel2_efficiencies_20180615</dd>
        <dt>Use2016PrescaledTruggers</dt>
       <dd> Flag to use the 2016 prescaled triggers</dd>
       </dl>
  </dd>


## Status of the code
Here the to-do list for the code:
- [X] Reading sample lists from file
- [X] Download the requested ntuples
- [X] Write configuration files for `ttH-offline`
  - [X] use default
  - [X] run with given config file
  - [X] variables to read
  - [X] regions
  - [X] tools to add
  - [X] systematics 
- [ ] Running `SM4t-offline` 
  - [X] Locally
  - [X] On the batch

## How to add a flag for your output variables
Here a short tutorial on how to add your output variable selection.

Assume I want to just to have just jet and leptons pTs and HT_jets in your output files and I want to call this selection `myvar`. I add these lines in the `lib/Class.py` in the `setup_variables_dictionary()` module:
```python
# Definition of the the dictionary for the variables to be read and saved, to be changed in case we want to add                                                                           
# other variables. To redefine a new set of variables, just add:                                                                                                                         
# - dictionary['<label to be called at runtime>'] = ["<type> <name>", ...]                                                                                                               
def setup_variables_dictionary():
    dictionary = {}
    # Adding variables in the format [ "<type> <variable>", ... ]
    dictionary["myvar"] = ["float jet_pt", "float el_pt", "float mu_pt", "float HT_jets"]
    dictionary["jet"] =  ["float jet_pt","float  jet_eta", "..."]

```
In case you want to add vectors, you also should change accordingly the `createConfigs()` module:
```python

with open("VariablesToRead.txt", "w") as ftw:
     for var in variables: 
     	 for item in dictionary[var]:
	      if var == "jet" or var == "rcjet" or var == "qg":  
	      	  ftw.write("jet "+item+"\n")     
...
``` 
The you can call your variables by indicating `-v myvar` at the running time.

## Useful information

Here are some useful infos and tips:
* If running TtTRF does not work, give in input all the configurations that you want in your config at the end: nBTags, Path, Splitting (split4b), Selection, Parametrisation
* In case of seg-fault due to `free()`: it means that a variable has been requested double, check the generated `configuration.txt` and `VariablesToRead.txt` files. Try to reduce the variables in the output to see which is the problematic (via changing `VariablesToRead.txt` and launch `SM4t-offline` from the job directory, or via `-r` option at running time).
* A good tip is: if you want to try something new out, you can add options to the `Framework/OffSM4topsTool/python/configs/configuration.txt` and run the offline. This config file is copied and customised at running time with the options you give. Be careful: output name, sample type, variables to read and regions are *always* overwritten at running time. 

## Contact and links
Contacts:
* Paolo Sabatini `paolo.sabatini@cern.ch`