#!/usr/bin/env python
import TopExamples.grid as grid
import data_SM4tops
import mc16a_SM4tops
import mc16d_SM4tops
import mc16e_SM4tops
import os

config = grid.Config()
config.CMake         = True
config.code          = 'top-xaod'
config.gridUsername  = 'phys-exotics'
config.groupProduction = False
config.excludedSites = ''
# config.excludedSites = 'ANALY_CONNECT,ANALY_CONNECT_SHORT'#'ANALY_GOEGRID'
config.noSubmit      = True
# config.memory	     = '4000'
config.maxNFilesPerJob = '10'

config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
# config.destSE        = 'BNL-OSG2_LOCALGROUPDISK'
config.destSE        = ''

####################################################################################
#  SubmitToGrid script                                                             # 
#  - Channel: 1LOS                                                                 #
#  - Part: 1                                                                       #
#  - Campaigns: Full Data + MC16a                                                  #
####################################################################################

version = 'v1'
channel = '1LOS'
FWTag = 'SM4t-212750'

################################################################
## Step1: Data +  4top (nominal) + main backgrounds (nominal) ##
################################################################

# Full Data
config.suffix       = FWTag+'_data_'+channel+'_'+version
config.settingsFile = 'cuts_data_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['data2015', 'data2016', 'data2017', 'data2018']
samples = grid.Samples(names)
grid.submit(config, samples)

# MC16a 4tops nominal + main background
config.suffix       = FWTag+'_mc16a_'+channel+'_'+version
config.settingsFile = 'cuts_mc16a_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['mc16a_tttt_aMcAtNloPy8_AFII', 'mc16a_ttbar_PhPy8_FS']
samples = grid.Samples(names)
grid.submit(config, samples)

##############################################################
## Step2: other backgrounds (nominal)                       ##
##############################################################

# MC16a other backgrounds
config.suffix       = FWTag+'_mc16a_'+channel+'_'+version
config.settingsFile = 'cuts_mc16a_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['mc16a_SingleTop','mc16a_ttX', 'mc16a_ttH', 'mc16a_ttH_tttt', 'mc16a_ttW', 'mc16a_ttGamma', 'mc16a_Vjets_Sherpa221', 'mc16a_VV', 'mc16a_VVV', 'mc16a_VH', 'mc16a_Vgamma']
samples = grid.Samples(names)
grid.submit(config, samples)

############################################################
## Step3: sliced + alternative backgrounds (nominal)       ##
############################################################

# MC16a sliced and alternative bkg
config.suffix       = FWTag+'_mc16a_'+channel+'_'+version
config.settingsFile = 'cuts_mc16a_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['mc16a_tttt_aMcAtNloHerwig7_AFII', 'mc16a_ttbar_PhPy8_AFII', 'mc16a_ttbar_PhPy8_Rad_AFII', 'mc16a_ttbar_PhHerwig_AFII', 'mc16a_ttbar_Sherpa221', 'mc16a_ttbar_aMcAtNloPy8_AFII', 'mc16a_ttbar_PhPy8OL_ttbb4F_AFII', 'mc16a_ttbar_PhPy8_FS_HT_sliced', 'mc16a_ttbar_PhPy8_AFII_HF_filt', 'mc16a_ttbar_PhHerwig_AFII_HF_filt', 'mc16a_ttbar_aMcAtNloPy8_AFII_HF_filt', 'mc16a_ttbar_PhHerwig_AFII_HT_sliced', 'mc16a_ttbar_aMcAtNloPy8_AFII_HT_sliced', 'mc16a_SingleTop_Alt']
samples = grid.Samples(names)
grid.submit(config, samples)

############################################################
## Step4: Systematics on nominal                          ##
############################################################

# MC16a systematics
config.suffix       = FWTag+'_mc16a_'+channel+'_syst_'+version
config.settingsFile = 'cuts_mc16a_SM4tops_'+channel+'_syst.txt'
config.extFile      = '.so'
names = ['mc16a_tttt_aMcAtNloPy8_AFII',	'mc16a_ttbar_PhPy8_FS','mc16a_SingleTop', 'mc16a_ttX', 'mc16a_ttH', 'mc16a_ttH_tttt', 'mc16a_ttW', 'mc16a_Vjets_Sherpa221', 'mc16a_VV', 'mc16a_VVV', 'mc16a_VH', 'mc16a_ttbar_PhPy8_FS_HT_sliced']
samples = grid.Samples(names)
grid.submit(config, samples)

