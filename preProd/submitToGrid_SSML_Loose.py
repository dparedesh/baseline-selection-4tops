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
#  - Channel: SSML Loose                                                           #
#  - Part: 1                                                                       #
#  - Campaigns: Full Data + All campaigns ttbar/Vjets                              #
####################################################################################

version = 'v4'
channel = 'SSML'
FWTag = 'HBSM4t-212120'
leptonID="loose"


################################################################
## Step1: Data 2015/16 + MC16a                                ##
################################################################

# Data 2015/16
config.suffix       = FWTag+'_data_'+channel+'_'+leptonID+'_'+version
config.settingsFile = 'cuts_data_SM4tops_'+channel+'_'+leptonID+'.txt'
config.extFile      = '.so'
names = ['data2015', 'data2016']
#samples = grid.Samples(names)
#grid.submit(config, samples)

# MC16a ttbar/Vjets
config.suffix       = FWTag+'_mc16a_'+channel+'_'+leptonID+'_'+version
config.settingsFile = 'cuts_mc16a_SM4tops_'+channel+'_'+leptonID+'.txt'
config.extFile      = '.so'
names = ["mc16a_Sh_2210_tttt","mc16a_ttVV","mc16a_ttWW","mc16a_tZ-tWZ","mc16a_3top","mc16a_SingleTop","mc16a_ttbar_PhPy8_FS","mc16a_ttbar_PhPy8_FS_HT_sliced","mc16a_ttGamma","mc16a_ttH","mc16a_ttW_Sh_2210","mc16a_ttX","mc16a_Vgamma","mc16a_VH","mc16a_Vjets_Sherpa221","mc16a_VV","mc16a_VVV"]
#samples = grid.Samples(names)
#grid.submit(config, samples)

################################################################
## Step2: Data 2017 + MC16d                                   ##
################################################################

# Data 2017
config.suffix       = FWTag+'_data_'+channel+'_'+leptonID+'_'+version
config.settingsFile = 'cuts_data_SM4tops_'+channel+'_'+leptonID+'.txt'
config.extFile      = '.so'
names = ['data2017']
#samples = grid.Samples(names)
#grid.submit(config, samples)

# MC16d ttbar/Vjets
config.suffix       = FWTag+'_mc16d_'+channel+'_'+leptonID+'_'+version
config.settingsFile = 'cuts_mc16d_SM4tops_'+channel+'_'+leptonID+'.txt'
config.extFile      = '.so'
names =  ["mc16d_Sh_2210_tttt","mc16d_ttVV","mc16d_ttWW","mc16d_tZ-tWZ","mc16d_3top","mc16d_SingleTop","mc16d_ttbar_PhPy8_FS","mc16d_ttbar_PhPy8_FS_HT_sliced","mc16d_ttGamma","mc16d_ttH","mc16d_ttW_Sh_2210","mc16d_ttX","mc16d_Vgamma","mc16d_VH","mc16d_Vjets_Sherpa221","mc16d_VV","mc16d_VVV"]
#samples = grid.Samples(names)
#grid.submit(config, samples)

################################################################
## Step3: Data 2018 + MC16e                                   ##
################################################################

# Data 2017
config.suffix       = FWTag+'_data_'+channel+'_'+leptonID+'_'+version
config.settingsFile = 'cuts_data_SM4tops_'+channel+'_'+leptonID+'.txt'
config.extFile      = '.so'
names = ['data2018']
#samples = grid.Samples(names)
#grid.submit(config, samples)

# MC16e ttbar/Vjets
config.suffix       = FWTag+'_mc16e_'+channel+'_'+leptonID+'_'+version
config.settingsFile = 'cuts_mc16e_SM4tops_'+channel+'_'+leptonID+'.txt'
config.extFile      = '.so'
names =  ["mc16e_Sh_2210_tttt","mc16e_ttVV","mc16e_ttWW","mc16e_tZ-tWZ","mc16e_3top","mc16e_SingleTop","mc16e_ttbar_PhPy8_FS","mc16e_ttbar_PhPy8_FS_HT_sliced","mc16e_ttGamma","mc16e_ttH","mc16e_ttW_Sh_2210","mc16e_ttX","mc16e_Vgamma","mc16e_VH","mc16e_Vjets_Sherpa221","mc16e_VV","mc16e_VVV"]
samples = grid.Samples(names)
grid.submit(config, samples)
