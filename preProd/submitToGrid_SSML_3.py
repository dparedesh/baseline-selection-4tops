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
config.groupProduction = True
config.excludedSites = ''
# config.excludedSites = 'ANALY_CONNECT,ANALY_CONNECT_SHORT'#'ANALY_GOEGRID'
config.noSubmit      = False
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

version = 'v5'
channel = 'SSML'
FWTag = 'HBSM4t-212120'

################################################################
## Step1:  4top (nominal) + main backgrounds (nominal) ##
################################################################

# Mc16e 4tops nominal + main background
config.suffix       = FWTag+'_mc16e_'+channel+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names =  ["mc16e_Sh_2210_tttt","mc16e_ttH_tttt","mc16e_tttt_aMcAtNloPy8_AFII"]
#samples = grid.Samples(names)
#grid.submit(config, samples)

##############################################################
## Step2: other backgrounds (nominal)                       ##
##############################################################

# Mc16e other backgrounds
config.suffix       = FWTag+'_mc16e_'+channel+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names =  ["mc16e_ttVV","mc16e_ttWW","mc16e_tZ-tWZ","mc16e_3top","mc16e_SingleTop","mc16e_ttbar_PhPy8_FS","mc16e_ttbar_PhPy8_FS_HT_sliced","mc16e_ttGamma","mc16e_ttH","mc16e_ttW_Sh_2210","mc16e_ttX","mc16e_Vgamma","mc16e_VH","mc16e_Vjets_Sherpa221","mc16e_VV","mc16e_VVV"]
#samples = grid.Samples(names)
#grid.submit(config, samples)

############################################################
## Step3: sliced + alternative backgrounds (nominal)       ##
############################################################

# Mc16e sliced and alternative bkg
config.suffix       = FWTag+'_mc16e_'+channel+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ["mc16e_Sh_2210_tttt_muQHT2","mc16e_SM4topsLOInclusive","mc16e_ttH_syst","mc16e_ttZ_syst","mc16e_tttt_aMcAtNloHerwig7_AFII","mc16e_ttW","mc16e_ttWNLO","mc16e_ttW_Sh_228","mc16e_Vjets_PhPy8"]
samples = grid.Samples(names)
grid.submit(config, samples)

############################################################
## Step4: Systematics on nominal                          ##
############################################################

# Mc16e systematics
config.suffix       = FWTag+'_mc16e_'+channel+'_syst_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'_syst.txt'
config.extFile      = '.so'
names =  ["mc16e_Sh_2210_tttt","mc16e_ttH_tttt","mc16e_tttt_aMcAtNloPy8_AFII","mc16e_ttVV","mc16e_ttWW","mc16e_tZ-tWZ","mc16e_3top","mc16e_SingleTop","mc16e_ttbar_PhPy8_FS","mc16e_ttbar_PhPy8_FS_HT_sliced","mc16e_ttGamma","mc16e_ttH","mc16e_ttW_Sh_2210","mc16e_ttX","mc16e_Vgamma","mc16e_VH","mc16e_Vjets_Sherpa221","mc16e_VV","mc16e_VVV"]
#samples = grid.Samples(names)
#grid.submit(config, samples)
