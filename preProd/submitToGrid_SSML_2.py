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
config.excludedSites = 'INFN-ROMA1_DATADISK'  
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
#  - Part: 2                                                                       #
#  - Campaigns: Mc16d                                                              #
####################################################################################

version = 'v5'
channel = 'SSML'
FWTag = 'HBSM4t-212120'

################################################################
## Step1:  4top (nominal) + main backgrounds (nominal) ##
################################################################

# Mc16d 4tops nominal + main background
config.suffix       = FWTag+'_mc16d_'+channel+'_'+version
config.settingsFile = 'cuts_mc16d_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names =  ["mc16d_Sh_2210_tttt","mc16d_ttH_tttt","mc16d_tttt_aMcAtNloPy8_AFII"]
#samples = grid.Samples(names)
#grid.submit(config, samples)

##############################################################
## Step2: other backgrounds (nominal)                       ##
##############################################################

# Mc16d other backgrounds
config.suffix       = FWTag+'_mc16d_'+channel+'_'+version
config.settingsFile = 'cuts_mc16d_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names =  ["mc16d_ttVV","mc16d_ttWW","mc16d_tZ-tWZ","mc16d_3top","mc16d_SingleTop","mc16d_ttbar_PhPy8_FS","mc16d_ttbar_PhPy8_FS_HT_sliced","mc16d_ttGamma","mc16d_ttH","mc16d_ttW_Sh_2210","mc16d_ttX","mc16d_Vgamma","mc16d_VH","mc16d_Vjets_Sherpa221","mc16d_VV","mc16d_VVV"]
#samples = grid.Samples(names)
#grid.submit(config, samples)

############################################################
## Step3: sliced + alternative backgrounds (nominal)       ##
############################################################

# Mc16d sliced and alternative bkg
config.suffix       = FWTag+'_mc16d_'+channel+'_'+version
config.settingsFile = 'cuts_mc16d_SM4tops_'+channel+'.txt'
config.extFile      = '.so'
names = ["mc16d_Sh_2210_tttt_muQHT2","mc16d_SM4topsLOInclusive","mc16d_ttH_syst","mc16d_ttZ_syst","mc16d_tttt_aMcAtNloHerwig7_AFII","mc16d_ttW","mc16d_ttWNLO","mc16d_ttW_Sh_228","mc16d_Vjets_PhPy8"]
samples = grid.Samples(names)
grid.submit(config, samples)

############################################################
## Step4: Systematics on nominal                          ##
############################################################

# Mc16d systematics
config.suffix       = FWTag+'_mc16d_'+channel+'_syst_'+version
config.settingsFile = 'cuts_mc16d_SM4tops_'+channel+'_syst.txt'
config.extFile      = '.so'
names =  ["mc16d_Sh_2210_tttt","mc16d_ttH_tttt","mc16d_tttt_aMcAtNloPy8_AFII","mc16d_ttVV","mc16d_ttWW","mc16d_tZ-tWZ","mc16d_3top","mc16d_SingleTop","mc16d_ttbar_PhPy8_FS","mc16d_ttbar_PhPy8_FS_HT_sliced","mc16d_ttGamma","mc16d_ttH","mc16d_ttW_Sh_2210","mc16d_ttX","mc16d_Vgamma","mc16d_VH","mc16d_Vjets_Sherpa221","mc16d_VV","mc16d_VVV"]
#samples = grid.Samples(names)
#grid.submit(config, samples)
