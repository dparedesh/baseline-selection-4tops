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
#  - Part: 3                                                                       #
#  - Campaigns: Mc16e                                                              #
####################################################################################

version = 'v1'
channel = '1LOS'
FWTag = 'SM4t-212750'

################################################################
## Step1:  4top (nominal) + main backgrounds (nominal) ##
################################################################

# Mc16e 4tops nominal + main background
config.suffix       = FWTag+'_mc16e_'+channel+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['mc16e_tttt_aMcAtNloPy8_AFII', 'mc16e_ttbar_PhPy8_FS']
samples = grid.Samples(names)
grid.submit(config, samples)

##############################################################
## Step2: other backgrounds (nominal)                       ##
##############################################################

# Mc16e other backgrounds
config.suffix       = FWTag+'_mc16e_'+channel+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['mc16e_SingleTop','mc16e_ttX', 'mc16e_ttW', 'mc16e_ttH', 'mc16e_ttH_tttt', 'mc16e_ttGamma', 'mc16e_Vjets_Sherpa221', 'mc16e_VV', 'mc16e_VVV', 'mc16e_VH', 'mc16e_Vgamma']
samples = grid.Samples(names)
grid.submit(config, samples)

############################################################
## Step3: sliced + alternative backgrounds (nominal)       ##
############################################################

# Mc16e sliced and alternative bkg
config.suffix       = FWTag+'_mc16e_'+channel+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ['mc16e_4tops_Alt', 'mc16e_ttbar_PhPy8_AFII', 'mc16e_ttbar_PhPy8_Rad_AFII', 'mc16e_ttbar_PhHerwig_AFII', 'mc16e_ttbar_aMcAtNloPy8_AFII', 'mc16e_ttbar_PhPy8_FS_HT_sliced', 'mc16e_ttbar_PhPy8_AFII_HF_filt', 'mc16e_ttbar_PhHerwig_AFII_HF_filt', 'mc16e_ttbar_aMcAtNloPy8_AFII_HF_filt', 'mc16e_ttbar_PhHerwig_AFII_HT_sliced', 'mc16e_ttbar_aMcAtNloPy8_AFII_HT_sliced', 'mc16e_SingleTop_Alt']
samples = grid.Samples(names)
grid.submit(config, samples)

############################################################
## Step4: Systematics on nominal                          ##
############################################################

# Mc16e systematics
config.suffix       = FWTag+'_mc16e_'+channel+'_syst_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'_syst.txt'
config.extFile      = '.so'
names = ['mc16e_tttt_aMcAtNloPy8_AFII','mc16e_ttbar_PhPy8_FS','mc16e_SingleTop','mc16e_ttX', 'mc16e_ttW', 'mc16e_ttH', 'mc16e_ttH_tttt', 'mc16e_Vjets_Sherpa221','mc16e_VV','mc16e_VVV', 'mc16e_VH', 'mc16e_ttbar_PhPy8_FS_HT_sliced']
samples = grid.Samples(names)
grid.submit(config, samples)
