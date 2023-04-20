
# Definition of the required classes and modules for the code
# Two main classes are defined:
# - Job: contains all the options to run ttH-offline, generate the config files and eventually the batch script
# - Tool: it contains just the option specified for the tool we want to include

import os, shutil
import sys
import fileinput

TOOLS_ORDERED_LIST = ["DoTTbarDileptonFilter","DoTTbarBfilter","MVAVariables","Add1LOSVar", "AddSSMLVar", "TtTRF",  "AddMVAVar", "AddBDT1LOS"]

# Definition of the preferred sites, definition here to be global:
# - eosatlas.cern.ch:1094 : either CERN-PROD_PHYS-TOP or any CERN group disk
# - /pnfs/ifh.de/ : DESY-ZN
preferredSites = ['eosatlas.cern.ch:1094','/pnfs/ifh.de/']

# Definition of Tool class, in order to pass the list of tools with their option
# to the job to create the configuration files.
# The class has as attributes the options we pass at the execution phase. This way for each tool we can loop
# over the attributes and add them to the config file
class Tool:
    name = ""
    # It needs the name of the tool and the passed string
    def __init__(self, name, str_config):
        self.name = name
        if str_config == "": return
        for c in str_config.split(","):
            head = c.split(":")[0]
            opt = c.split(":")[1]
            # The substring before ':' is the attribute and on the right the associated value
            self.__dict__[head] = opt

# Look up in the libraries files in run_offline/lib and create the list of
# dataset corresponding at the requested process
def setup_sample_list(main_path, sample, typ, analysis, channel, syst):
    print 'Retrieving the input list for %s (%s) in %s ntuples for %s channel' % (sample
, typ, analysis, channel)
    if channel is not "":
        channel += "_"
    elif channel == "BSM": channel = "1LOS"
    file_name = main_path+"/lib/"+analysis+'_'+channel+typ
    if syst == "nominal": syst = ""
    elif analysis == "HBSM4t-212120":
	print 'Systematics: %s' % syst
        file_name = file_name + "_syst"
        syst = " syst"
    f = open (file_name+'.py', "r")
    sampleFound = False
    with open(file_name+'.py', 'r') as f:
        line=f.readlines()
    line = [x for x in line if x!="\n"]
    line = [x.strip() for x in line]
    i_start = line.index("## "+sample+syst)
    line = [x for i,x in enumerate(line) if i > i_start]
    i_end = [i for i,x in enumerate(line) if x.startswith("##")]
    line = [x for i,x in enumerate(line) if i < i_end[0]]
    return line

# Check whether the given path exists, if not it creates the folders
def check_path(input_path):
    print "\t > Checking  directories"
    if not os.path.exists(input_path):
        os.system("mkdir -p "+input_path)
        print "\t >> directory %s has been created" % input_path
    else:
        print "\t >> directory %s already exists" % input_path

# Needed to well get the input folder in case we are in the eos_path
def getDetailedType (dataset):
    if "mc16a" in dataset:
        return "mc16a"
    if "mc16c" in dataset:
        return "mc16c"
    if "mc16d" in dataset:
        return "mc16d"
    if "mc16e" in dataset:
        return "mc16e"
    if "data" in dataset:
        return "data"

# Check whether the samples corresponding to the jobs are at the input path if some is missing they are downloaded
# IMPLEMENTATION: rucio is looking up whether the files exist, rucio setup is required before
def check_samples(input_path, analysis, list_sample, typ):
    print "\t > Checking the input samples"
    for dataset in list_sample:
        if not os.path.exists(dataset):
            input_path_copy = input_path
            scope = ""
            # In case we are in /eos/ we have to modify the path to get the right structure
            if '/eos/atlas/atlascerngroupdisk/phys-top/' or '/lustre/' in input_path:
                eos_type = getDetailedType(dataset)
                print eos_type, dataset
                input_path_copy = input_path.replace('<type>',eos_type)
            # Here the scopes are different based on the type of input ntuple we use: 4tops or ttHbb
            scope = dataset.split('.')[0]+"."+dataset.split('.')[1]
            os.system("rucio download --ndownloader 5 --dir "+input_path_copy+" "+scope+":"+dataset)

# Build the input file list given the list of samples that have been downloaded, this way in case some downloads
# fail the file is not included in the list and the code does not crash
def setup_input_list(input_path, list_sample):
    with open('input_list.txt', 'w') as f:
        for sample in list_sample:
            path_dir = input_path+""+sample
            # In case we are in /eos/ we have to modify the path to get the right structure
            if '/eos/atlas/atlascerngroupdisk/phys-top/' or '/lustre/'  in input_path:
                eos_type = getDetailedType(sample)
                path_dir = path_dir.replace('<type>',eos_type)
            for currfile in os.listdir(path_dir):
                f.write(path_dir+"/"+currfile+"\n")
        f.close()

# Build the input file list in case one wants to read the files with XRootD from the grid
# Here the selection of better location is done
def getBetterDuplicate (f1, f2):

    if preferredSites[0] in f1 : return f1
    if preferredSites[0] in f2 : return f2

    f1_in_pref_site = any ( site in f1 for site in preferredSites)
    f2_in_pref_site = any ( site in f2 for site in preferredSites)

    if f1_in_pref_site and not f2_in_pref_site: return f1
    elif not f1_in_pref_site and f2_in_pref_site: return f2
    else:
        import random
        return random.choice ( [f1,f2] )

# Here the warnings in case of some file missing or not preferred sites
def check_lists_xrootd(input_list):
    list_job_nr = []
    for f in input_list:
        job_nr = f.split('/')[-1].split('.')[2]
        if job_nr not in list_job_nr: list_job_nr.append (job_nr)

    for j in list_job_nr:
        files = [x for x in input_list if j in x]

        ## Here wanted to check if numbers are consecutive, but it happens more often than expected
        ## not really a problem, if it is not listed it does not exist on rucio!
        # ids = [int(x.split('/')[-1].split('.')[3][1:]) for x in files ]
        # for index in range (0, len (ids)-1):
        #     if ids[index+1] != ids[index]+1:
        #         print '\t[WARNING] File for JobNr %s and Nr %s missing' % (str(j), str(ids[index]+1))
        #####################################################################

        for f in files:
            if not any ( site in f for site in preferredSites):
                print '\t[WARNING] Using %s: not in preferred site' % (f)


# Here the actual list is defined
def setup_input_list_xrootd( list_sample, opening_mode = "w"):
    input_list = list()
    for sample in list_sample:
        rucio_cmd = 'rucio list-file-replicas --protocol root --pfns '+sample
        rucio_output = os.popen (rucio_cmd).read()

        for f in list(rucio_output.split('\n')):
            if 'root' not in f: continue
            curr_job_nr = f.split('/')[-1].split('.')[2]
            curr_file_nr = f.split('/')[-1].split('.')[3]
            duplicate = [x for x in input_list if '.'+curr_job_nr+'.' in x and '.'+curr_file_nr+'.' in x]
            if len (duplicate)!=0:
                best_file = f
                for d in duplicate:
                    best_file = getBetterDuplicate (best_file, d)
                    input_list.remove (d) # = [best_file if d in duplicate else x for x in input_list]
                input_list.append(best_file)
            else:
                input_list.append (f)

    check_lists_xrootd (input_list)

    f = open ('input_list.txt',opening_mode)
    for item in input_list :
        item = item.replace('root://eosatlas.cern.ch:1094/', '')
        item = "%s\n"%item
        f.write(item)
    #f.writelines(["%s\n" % item  for item in input_list])
    f.close()

# Find_file method list creator
def setup_input_list_find_file (input_path, list_sample):

    # First check whether the dataset is downloaded or symlinks are there!
    for sample in list_sample:
        path_dir = input_path+""+sample
        # In case we are in /eos/ we have to modify the path to get the right folder structure
        if '/eos/atlas/atlascerngroupdisk/phys-top/' or '/lustre/' in input_path:
            eos_type = getDetailedType(sample)
            path_dir = path_dir.replace('<type>',eos_type)
        if os.path.exists (path_dir) and len (os.listdir(path_dir)) != 0:
            # In case the dataset is there and not empty
            f = open ("input_list.txt","a+")
            for ntuple in os.listdir(path_dir):
                f.write(path_dir+"/"+ntuple+"\n")
            f.close()
        else :
            # If dataset not found there, then open with XRootD
            setup_input_list_xrootd ( [sample], "a+")

# Computation of number of jobs, if split-and-combine is enabled
#
def computeNumberOfJobs (isSystematic = False):
    nTrees = 1 if not isSystematic else 95
    total_size = 0
    size_per_1M = 7500e6
    from ROOT import TFile
    lines = open ('input_list.txt').readlines()
    for l in lines:
        f = TFile.Open(l.replace ('\n',''))
        total_size += f.GetSize()
    num_jobs = int (total_size / size_per_1M / nTrees)
    return num_jobs if num_jobs > 0 else 1


# Definition of the the dictionary for the variables to be read and saved, to be changed in case we want to add
# other variables. To redefine a new set of variables, just add:
# - dictionary['<label to be called at runtime>'] = ["<type> <name>", ...]
# In case it is a vector to be stored, easily add it to any of jet, rcjet, lepton or change also createConfigs() below
def setup_variables_dictionary(tag):
    dictionary = {}
    dictionary["jet"] =  ["float jet_pt","float jet_eta","float jet_phi","float jet_e","float jet_jvt", "char jet_passfjvt", "float jet_DL1","float jet_DL1r", "char jet_isbtagged_DL1r_60","char jet_isbtagged_DL1r_70","char jet_isbtagged_DL1r_77","char jet_isbtagged_DL1r_85","int jet_tagWeightBin_DL1r_Continuous","char jet_isbtagged_DL1_60","char jet_isbtagged_DL1_70","char jet_isbtagged_DL1_77","char jet_isbtagged_DL1_85","int jet_tagWeightBin_DL1_Continuous"]
    dictionary["rcjet"] = ["float rcjet_pt","float rcjet_eta","float rcjet_phi","float rcjet_e","float rcjet_d12", "float rcjet_d23"]
    dictionary["lepton"] = ["float el_pt","float el_eta","float el_phi","float el_e","float el_charge","char el_isTight","float mu_pt","float mu_eta","float mu_phi","float mu_e","float mu_charge","char mu_isTight"]
    dictionary["met"] = ["float met_met","float met_phi"]
    # mcChannelNumber is not int! using uint causes warning in TTHbbOffline but will render the correct variable type unsigned int - otherwise data in mcChannelNumber could corrupt
    dictionary["event"] = ["uint mcChannelNumber","int eventNumber","int runNumber","int randomRunNumber","int nPrimaryVtx","int nBTags_DL1r_60","int nBTags_DL1r_70","int nBTags_DL1r_77","int nBTags_DL1r_85","int nBTags_DL1_60","int nBTags_DL1_70","int nBTags_DL1_77","int nBTags_DL1_85","int nElectrons","int nMuons","int nJets","float HT_all","float HT_jets","int TopHeavyFlavorFilterFlag","float GenFiltHT"]
    dictionary ["truth"] = ["int truth_barcode","int truth_pdgid","int truth_status", "float truth_pt","float truth_phi","float truth_eta","float truth_tbar_pt", "float truth_top_pt","float truth_ttbar_pt","int el_true_origin","int el_true_firstEgMotherTruthType","int el_true_firstEgMotherTruthOrigin","int el_true_isPrompt","int el_true_pdg","int el_true_pt","int el_true_eta","int mu_true_type","int mu_true_origin","int mu_true_isPrompt","int mu_true_pdg","int mu_true_pt","int mu_true_eta","int jet_truthflav","int jet_truthPartonLabel","int jet_truthflavExtended","int jet_isTrueHS"]
    dictionary ["lepton-iso"] = ["float el_topoetcone20", "float el_ptvarcone20", "char el_LHMedium","char el_LHTight","int el_isPrompt","char el_isoFixedCutLoose","char el_isoFixedCutTight","char el_isoFixedCutTightTrackOnly","char el_isoGradient","char el_isoGradientLoose","char el_isoLoose","char el_isoLooseTrackOnly","float mu_topoetcone20", "float mu_ptvarcone30","char mu_Medium", "char mu_Tight","int mu_isPrompt","char mu_isoFixedCutLoose","char mu_isoFixedCutTightTrackOnly","char mu_isoGradient","char mu_isoGradientLoose","char mu_isoLoose","char mu_isoLooseTrackOnly",]
    dictionary ["jet-btag"] = ["float jet_MV2c10mu","float jet_MV2c10rnn","float jet_DL1mu","float jet_DL1rnn","float jet_MV2c100","float jet_MV2cl100","float jet_DL1_pu","float jet_DL1_pc","float jet_DL1_pb","float jet_DL1mu_pu","float jet_DL1mu_pb","float jet_DL1mu_pc","float jet_DL1rnn_pu","float jet_DL1rnn_pb","float jet_DL1rnn_pc"]
    dictionary ["qg"] = ["float jet_nTrkPt1000","float jet_nTrkPt500","float jet_TrackWidthPt1000","float jet_TrackWidthPt500","float jet_Charge","float jet_N90Constituents","float jet_Width","float jet_WidthPhi"]
    dictionary ["common"] = ["int nRCJets", "float mJSum"]
    dictionary ["ss3l-var"] = ["float chargeIDBDTLoose","float chargeIDBDTMedium","float chargeIDBDTTight"]
    dictionary ["1los-var"] = ['int HF_SimpleClassification', 'float mtw',"int el_true_isPrompt","int mu_true_isPrompt","int el_isPrompt","int mu_isPrompt","char truth_top_dilep_filter","int TopHeavyFlavorFilterFlag","float truth_tbar_pt","float truth_top_pt","float truth_ttbar_pt","float mu","int ttbb_categories","float truth_ht","int HF_SimpleClassificationGhost","int HF_Classification","int HF_ClassificationGhost"]
    dictionary ["ssml"] = ["float HT_jets","float HT_all","float mJSum","int nRCJets","int SSee","int SSmm","int SSem","int eee","int eem","int eem","int emm","int mmm","int eee_Zveto","int eem_Zveto","int eem_Zveto","int emm_Zveto","int mmm_Zveto","char    el_isTight","char    el_ECIDS","int el_true_pdg","int     el_true_origin", "int     el_true_type","int     el_true_firstEgMotherTruthType","int     el_true_firstEgMotherTruthOrigin","int     el_true_firstEgMotherPdgId","int el_truthPdgId","float   el_MeeCO","float   el_PromptLeptonVeto","float   el_RadiusCO","float   el_bestmatchSiTrackD0","float   el_bestmatchSiTrackEta","float   el_bestmatchSiTrackPhi","float   el_bestmatchSiTrackPt","float   el_bestmatchSiTrackQ","float el_bestmatchSiTrackZ0","float   el_closestSiTrackD0","float   el_closestSiTrackEta","float   el_closestSiTrackPhi","float   el_closestSiTrackPt","float   el_closestSiTrackQ","float   el_closestSiTrackZ0","float   el_closestSiTrackeIL","float   el_closestSiTracknIL","float   el_closestSiTracknNIL","float   el_deltaPhi2","float   el_leadSubleadDeta","float   el_leadSubleadDphi","float el_leadSubleadDr","float   el_leadTrackPt","float   el_separationMinDCT","float el_subleadTrackPt","float   el_sumTrackPt","char mu_isTight","int mu_true_pdg","int mu_true_type","int mu_true_origin","float mu_PromptLeptonVeto","float lep_0_pt","float lep_0_eta","float lep_0_phi","float lep_0_charge","float lep_0_e","float lep_1_pt","float lep_1_eta","float lep_1_phi","float lep_1_charge","float lep_1_e","float lep_2_pt","float lep_2_eta","float lep_2_phi","float lep_2_charge","float lep_2_e","float el_ECIDSResult",'float mu_delta_z0_sintheta', 'float mu_d0sig', 'float el_delta_z0_sintheta','float el_d0sig']
    #for the loose, just add the extra vars that you need: don't repeat the same vars as above.
    dictionary ["ssml_loose"] = ["int loose_SSee","int loose_SSmm","int loose_SSem","int loose_eee","int loose_eem","int loose_eem","int loose_emm","int loose_mmm","int OSee","int OSem","int el_firstEgMotherTruthType","int el_firstEgMotherTruthOrigin","int el_firstEgMotherPdgId",'float weight_indiv_SF_EL_ChargeMisID','float weight_indiv_SF_EL_ChargeID']
    dictionary ["trigger"] = ["char HLT_2mu14","char HLT_e24_lhmedium_L1EM20VH","char HLT_e60_lhmedium","char HLT_e120_lhloose","char HLT_2e12_lhloose_L12EM10VH","char HLT_e7_lhmedium_mu24","char HLT_mu50","char HLT_2mu10","char HLT_mu18_mu8noL1","char HLT_e60_lhmedium_nod0","char HLT_e140_lhloose_nod0","char HLT_2e17_lhvloose_nod0","char HLT_e17_lhloose_nod0_mu14","char HLT_e7_lhmedium_nod0_mu24","char HLT_2e24_lhvloose_nod0","char HLT_e17_lhloose_nod0_mu14","char HLT_e7_lhmedium_nod0_mu24","char HLT_e60_lhmedium_nod0","char HLT_2e17_lhvloose_nod0_L12EM15VHI","char HLT_e17_lhloose_nod0_mu14","char HLT_e7_lhmedium_nod0_mu24","char HLT_e7_lhmedium_nod0_mu24","char mu_isDLTMatch","char mu_isSLTMatch","char el_trigMatch_HLT_e24_lhmedium_L1EM20VH","char el_trigMatch_HLT_e60_lhmedium","char el_trigMatch_HLT_e120_lhloose","char el_trigMatch_HLT_e60_lhmedium_nod0","char el_trigMatch_HLT_e140_lhloose_nod0","char el_isDLTMatch"]

    # Name changed for mass-tagged reclustered jets and truth_ht from SM4t-212750 on
    if tag == "SM4t-212750" or tag=="HBSM4t-212120":
        for label, variables in dict (dictionary).iteritems():
            if 'int nRCJets' in variables:
                dictionary [label].append ('int nRCJetsM100')
            if 'float truth_ht' in variables:
                dictionary [label].append ('float GenFiltHT')




#
    return dictionary

# Here is the definition of the regions dictionary: to add any other region to be called at the exectution time
# just add:
# - dictionary ['<label>'] = "<string-definition>"
def setup_regions_dictionary(channel,analysis,typology,sample):

    dictionary = {}

    years = list()
    if typology == "mc16a": years = ["_2015","_2016"]
    elif typology == "mc16cd": years = ["_2017"]
    elif typology == "mc16e": years = ["_2018"]
    elif typology == "data" and sample != "data18": years = ["_2015","_2016","_2017"]
    elif typology == "data" and sample == "data18": years = ["_2018"]
    if analysis == "HBSM4t-212120": years = [""]
    cond_1l = "("
    for sel in ["ejets","mujets"]:
        for y in years:
            for b in ["_DL1r", "_DL1"]:
                cond_1l += sel+y+b + " || "
    cond_1l = cond_1l[:-3] + ")"
    cond_os2l = "("
    for sel in ["ee","mumu","emu"]:
        for y in years:
            for b in ["_DL1r", "_DL1"]:
                cond_os2l += sel+y+b + " || "
    cond_os2l = cond_os2l[:-3] + ")"
    cond_1los2l = cond_1l[:-1] + " || "+cond_os2l[1:]
    if channel == "BSM":
        cond_1l = cond_1l.replace("ejets","BSM_ejets")
        cond_1l = cond_1l.replace("mujets","BSM_mujets")

    cond_2lss = "("
    for sel in ["SSee","SSmm","SSem"]:
        for y in years:
            cond_2lss += sel+y + " || "
    cond_2lss = cond_2lss[:-3] + ")"
    cond_3l = "("
    for sel in ["eee","eem","emm","mmm"]:
        for y in years:
            for z in ["","_Zveto"]:
                cond_3l += sel+z+y + " || "
    cond_3l = cond_3l[:-3] + ")"
    cond_2lss3l = cond_2lss[:-1] + " || "+cond_3l[1:]

    if channel == "LooseSSML":
        cond_os2l = "("
        for sel in ["OSee","OSem"]:
            for y in years:
                cond_os2l += sel+y + " || "
        cond_os2l = cond_os2l[:-3] + ")"
        cond_2lss = cond_2lss.replace("SS","loose_SS")
        cond_3l = cond_3l.replace("eee","loose_eee")
        cond_3l = cond_3l.replace("eem","loose_eem")
        cond_3l = cond_3l.replace("emm","loose_emm")
        cond_3l = cond_3l.replace("mmm","loose_mmm")

    incl_loose = cond_os2l[:-1] + " || " + cond_2lss[1:-1] + " || "+cond_3l[1:]

    dictionary ["1l"] = cond_1l
    dictionary ["os2l"] = cond_os2l
    dictionary ["1los2l"] = cond_1los2l

    njets = range (0,11)
    for operator in ["==", ">="]:
        label = ""
        if operator == ">=": label = "ge"
        for j in njets:
            dictionary [label+str(j)+"j"] = "nJets "+operator+" "+str(j)
            dictionary ["ljets"+label+str(j)+"j"] = cond_1l+" && nJets "+operator+" "+str(j)
            dictionary ["os2l"+label+str(j)+"j"] = cond_os2l+" && nJets "+operator+" "+str(j)
            dictionary ["ljetsos2l"+label+str(j)+"j"] = cond_1los2l+" && nJets "+operator+" "+str(j)
            dictionary ["2lss"+label+str(j)+"j"] = cond_2lss+" && nJets "+operator+" "+str(j)
            dictionary ["3l"+label+str(j)+"j"] = cond_3l+" && nJets "+operator+" "+str(j)
            dictionary ["2lss3l"+label+str(j)+"j"] = cond_2lss3l+" && nJets "+operator+" "+str(j)
            dictionary ["2lss3l"+label+str(j)+"DL1r"] = cond_2lss3l+" && nBTags_DL1r_77 "+operator+" "+str(j)
            dictionary [label+str(j)+"DL1r"] = "nBTags_DL1r_77 "+operator+" "+str(j)
            if channel == "LooseSSML":
                dictionary ["looseSSML"] = incl_loose

    #for key, value in dictionary.iteritems():
    #     print key, value
    #     print "\n\n"

    return dictionary

# Here the systematics to be picked for each type of systematics have been specified
# At the moment just two different systematics types are available, shaped on ttHbb ntuples
# - sys: egamma syst (dummy)
# - jer: empty
def setup_syst_dictionary():
    dictionary = {}
    dictionary ['egFS'] = {"EG_RESOLUTION_ALL__1up_Loose","EG_RESOLUTION_ALL__1down_Loose", "EG_SCALE_AF2__1up_Loose","EG_SCALE_AF2__1down_Loose","EG_SCALE_ALL__1up_Loose","EG_SCALE_ALL__1down_Loose"}

    dictionary ['egAFII'] = {"EG_RESOLUTION_ALL__1down_Loose","EG_RESOLUTION_ALL__1up_Loose","EG_SCALE_AF2__1down_Loose","EG_SCALE_AF2__1up_Loose","EG_SCALE_ALL__1down_Loose","EG_SCALE_ALL__1up_Loose"}

    dictionary ['mu0'] = {"MUON_ID__1down_Loose","MUON_ID__1up_Loose","MUON_MS__1down_Loose","MUON_MS__1up_Loose","MUON_SAGITTA_RESBIAS__1down_Loose","MUON_SAGITTA_RESBIAS__1up_Loose"}

    dictionary ['mu1'] = {"MUON_SAGITTA_RHO__1down_Loose","MUON_SAGITTA_RHO__1up_Loose","MUON_SCALE__1down_Loose","MUON_SCALE__1up_Loose"}

    dictionary ["met"] = {"MET_SoftTrk_ResoPara_Loose","MET_SoftTrk_ResoPerp_Loose","MET_SoftTrk_ScaleDown_Loose","MET_SoftTrk_ScaleUp_Loose"}

    dictionary ['jes0'] = {"CategoryReduction_JET_BJES_Response__1down_Loose","CategoryReduction_JET_BJES_Response__1up_Loose","CategoryReduction_JET_EffectiveNP_Detector1__1down_Loose","CategoryReduction_JET_EffectiveNP_Detector1__1up_Loose","CategoryReduction_JET_EffectiveNP_Detector2__1down_Loose","CategoryReduction_JET_EffectiveNP_Detector2__1up_Loose"}

    dictionary ['jes1'] = {"CategoryReduction_JET_EffectiveNP_Mixed1__1down_Loose","CategoryReduction_JET_EffectiveNP_Mixed1__1up_Loose","CategoryReduction_JET_EffectiveNP_Mixed2__1down_Loose","CategoryReduction_JET_EffectiveNP_Mixed2__1up_Loose","CategoryReduction_JET_EffectiveNP_Mixed3__1down_Loose","CategoryReduction_JET_EffectiveNP_Mixed3__1up_Loose"}

    dictionary ['jes2'] = {"CategoryReduction_JET_EffectiveNP_Modelling1__1down_Loose","CategoryReduction_JET_EffectiveNP_Modelling1__1up_Loose","CategoryReduction_JET_EffectiveNP_Modelling2__1down_Loose","CategoryReduction_JET_EffectiveNP_Modelling2__1up_Loose"}

    dictionary ['jes2a'] = {"CategoryReduction_JET_EffectiveNP_Modelling3__1down_Loose","CategoryReduction_JET_EffectiveNP_Modelling3__1up_Loose","CategoryReduction_JET_EffectiveNP_Modelling4__1down_Loose","CategoryReduction_JET_EffectiveNP_Modelling4__1up_Loose"}

    dictionary ['jes3'] = {"CategoryReduction_JET_EffectiveNP_Statistical1__1down_Loose","CategoryReduction_JET_EffectiveNP_Statistical1__1up_Loose","CategoryReduction_JET_EffectiveNP_Statistical2__1down_Loose","CategoryReduction_JET_EffectiveNP_Statistical2__1up_Loose"}

    dictionary ['jes3a'] = {"CategoryReduction_JET_EffectiveNP_Statistical3__1down_Loose","CategoryReduction_JET_EffectiveNP_Statistical3__1up_Loose","CategoryReduction_JET_EffectiveNP_Statistical4__1down_Loose","CategoryReduction_JET_EffectiveNP_Statistical4__1up_Loose"}

    dictionary ['jes3b'] = {"CategoryReduction_JET_EffectiveNP_Statistical5__1down_Loose","CategoryReduction_JET_EffectiveNP_Statistical5__1up_Loose","CategoryReduction_JET_EffectiveNP_Statistical6__1down_Loose","CategoryReduction_JET_EffectiveNP_Statistical6__1up_Loose"}

    dictionary ['jes4'] = {"CategoryReduction_JET_EtaIntercalibration_Modelling__1down_Loose","CategoryReduction_JET_EtaIntercalibration_Modelling__1up_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_highE__1down_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_highE__1up_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_negEta__1down_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_negEta__1up_Loose"}

    dictionary ['jes4a'] = {"CategoryReduction_JET_EtaIntercalibration_NonClosure_posEta__1down_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_posEta__1up_Loose","CategoryReduction_JET_EtaIntercalibration_TotalStat__1down_Loose","CategoryReduction_JET_EtaIntercalibration_TotalStat__1up_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_2018data__1down_Loose","CategoryReduction_JET_EtaIntercalibration_NonClosure_2018data__1up_Loose"}

    dictionary ['jes5'] = {"CategoryReduction_JET_Flavor_Composition__1down_Loose","CategoryReduction_JET_Flavor_Composition__1up_Loose","CategoryReduction_JET_Flavor_Response__1down_Loose","CategoryReduction_JET_Flavor_Response__1up_Loose"}

    dictionary ['jes6'] = {"CategoryReduction_JET_Pileup_OffsetMu__1down_Loose","CategoryReduction_JET_Pileup_OffsetMu__1up_Loose","CategoryReduction_JET_Pileup_OffsetNPV__1down_Loose","CategoryReduction_JET_Pileup_OffsetNPV__1up_Loose"}

    dictionary ['jes6a'] = {"CategoryReduction_JET_Pileup_PtTerm__1down_Loose","CategoryReduction_JET_Pileup_PtTerm__1up_Loose","CategoryReduction_JET_Pileup_RhoTopology__1down_Loose","CategoryReduction_JET_Pileup_RhoTopology__1up_Loose"}

    dictionary ['jesFS'] = {"CategoryReduction_JET_PunchThrough_MC16__1down_Loose","CategoryReduction_JET_PunchThrough_MC16__1up_Loose","CategoryReduction_JET_SingleParticle_HighPt__1down_Loose","CategoryReduction_JET_SingleParticle_HighPt__1up_Loose"}

    dictionary ['jesAFII'] = {"CategoryReduction_JET_SingleParticle_HighPt__1down_Loose","CategoryReduction_JET_SingleParticle_HighPt__1up_Loose","CategoryReduction_JET_PunchThrough_AFII__1down_Loose","CategoryReduction_JET_PunchThrough_AFII__1up_Loose","CategoryReduction_JET_RelativeNonClosure_AFII__1down_Loose","CategoryReduction_JET_RelativeNonClosure_AFII__1up_Loose"}

    dictionary['jerFS'] = {"CategoryReduction_JET_JER_DataVsMC_MC16__1down_Loose","CategoryReduction_JET_JER_DataVsMC_MC16__1up_Loose"}

    dictionary['jerAFII'] = {"CategoryReduction_JET_JER_DataVsMC_AFII__1down_Loose","CategoryReduction_JET_JER_DataVsMC_AFII__1up_Loose"}

    dictionary ['jer0'] = {"CategoryReduction_JET_JER_EffectiveNP_1__1up_Loose","CategoryReduction_JET_JER_EffectiveNP_1__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_2__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_2__1up_Loose","CategoryReduction_JET_JER_EffectiveNP_3__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_3__1up_Loose"}

    dictionary ['jer1'] = {"CategoryReduction_JET_JER_EffectiveNP_4__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_4__1up_Loose","CategoryReduction_JET_JER_EffectiveNP_5__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_5__1up_Loose"}

    dictionary ['jer2'] = {"CategoryReduction_JET_JER_EffectiveNP_6__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_6__1up_Loose","CategoryReduction_JET_JER_EffectiveNP_7restTerm__1down_Loose","CategoryReduction_JET_JER_EffectiveNP_7restTerm__1up_Loose"}

#    dictionary ['jer'] = {"JET_JER_CROSS_CALIB_FORWARD__1up_Loose","JET_JER_NOISE_FORWARD__1up_Loose","JET_JER_NP0__1up_Loose","JET_JER_NP0__1down_Loose","JET_JER_NP1__1up_Loose","JET_JER_NP1__1down_Loose","JET_JER_NP2__1up_Loose","JET_JER_NP2__1down_Loose","JET_JER_NP3__1up_Loose","JET_JER_NP3__1down_Loose","JET_JER_NP4__1up_Loose","JET_JER_NP4__1down_Loose","JET_JER_NP5__1up_Loose","JET_JER_NP5__1down_Loose","JET_JER_NP6__1up_Loose","JET_JER_NP6__1down_Loose","JET_JER_NP7__1up_Loose","JET_JER_NP7__1down_Loose"}

     ## Single systematic processing
    for category in dict (dictionary):
        for systematic in dictionary[category]:
            dictionary [ systematic ] = []
            dictionary [ systematic ].append(systematic)


    return dictionary

# Fill the string for the region definition in the configuration.txt file
def getRegionString(dictionary, regions):
    string = "Regions "
    for reg in regions:
        string += reg+":"+'"'+dictionary[reg]+'",'
    string = string[:-1]
    return string

# Fill the string for the systematics to be included in the configuration.txt file
def getSystString(dictionary, syst):
    string = "Systematics "
    for s in dictionary[syst]:
        string += s+","
    string = string[:-1]+"\n"
    return string

# Adding ve
def getStreamFlags(channel, analysis, typology, sample):
    d = dict()
    if channel == "1LOS":
        d [channel] = ["int ejets_DL1r","int mujets_DL1r","int ee_DL1r","int emu_DL1r","int mumu_DL1r","int ejets_DL1","int mujets_DL1","int ee_DL1","int emu_DL1","int mumu_DL1"]
    elif channel == "BSM":
        d [channel] =  ["int BSM_ejets_DL1r","int BSM_mujets_DL1r","int BSM_ejets_DL1","int BSM_mujets_DL1"]
    elif channel == "SSML":
        d [channel] = ["int SSee","int SSmm","int SSem","int eee","int eem","int emm","int mmm","int eee_Zveto","int eem_Zveto","int emm_Zveto","int mmm_Zveto"]
    elif channel == "LooseSSML":
        d [channel] = ["int OSee","int OSem","int loose_SSee","int loose_SSmm","int loose_SSem","int loose_eee","int loose_eem","int loose_emm","int loose_mmm","int loose_eee_Zveto","int loose_eem_Zveto","int loose_emm_Zveto","int loose_mmm_Zveto"]
    else:
        print "[ERROR] Channel %s not found, giving empty stream flag dictionary" % channel
        return d


    return d


# Setup the configuration file to be compatible with TtTRF
def setupTtTRFConfig(tool):
    for line in fileinput.input("configuration.txt", inplace=1):
            if line.startswith("Regions"):
                if "os2l" in tool.Selection:
                    line = line.replace(':"',':"{pass_sf_cut} ')
                else:
                    line = line.replace(':"',':"{pass_triangle} ')
            if line.startswith("CreateOutputFileFirst"):
                line = line + "BTaggingWP DL1r_60,DL1r_77,DL1r_70,DL1r_85,DL1_60,DL1_77,DL1_70,DL1_85,PC\n"
            sys.stdout.write(line)

# Setup the configuration file to be compatible with BDT 1LOS
def addVarsToBDT1LOS(tool):
    version_1l = "190524" if 'BDT_240519_1l_set1' in tool.XMLpath_1l else "AZ_test" if "test" in tool.XMLpath_1l else  "190510"
    version_2l = "190524" if 'BDT_240519_2l_set1' in tool.XMLpath_2l else "AZ_test" if "test" in tool.XMLpath_2l else  "190510"
    vars_1l = "HT_jets,mJSum,nBTags_DL1r_77/I,nJets/I,nRCJets/I,jet_pt0:jet_pt[0],jet_pt1:jet_pt[1],jet_pt2:jet_pt[2],jet_pt3:jet_pt[3]" if version_1l == "AZ_test" else "HT_all,nJets/I,Centrality_all:centrality,nBTags_DL1r_70/I,jet_DL1r_ordered_discrete_2:jet_DL1r_ordered_discrete[2],jet_DL1r_ordered_discrete_3:jet_DL1r_ordered_discrete[3],jet_DL1r_ordered_discrete_4:jet_DL1r_ordered_discrete[4],nRCJets/I,dRjj_Avg:deltaR_avg_jets,dRbb_MindR_DL1r_70:deltaR_min_bjets,dRbl_MindR_DL1r_70:deltaR_min_bjet_lepton,Mjjj_MinM:inv_trijet_mass_min_jets,Mbbb_MaxM_DL1r_70:inv_trijet_mass_max_bjets,Mjjj_Avg:inv_trijet_mass_avg_jets,dPhijj_Avg:deltaPhi_avg_jets,rcjet_d12_0,rcjet_d23_0,rcjet_pt_0,ptW,mtw,met_met"
    vars_2l = "HT_jets,mJSum,nBTags_DL1r_77/I,nJets/I,nRCJets/I,jet_pt0:jet_pt[0],jet_pt1:jet_pt[1],jet_pt2:jet_pt[2],jet_pt3:jet_pt[3]" if version_1l == "AZ_test" else "HT_all,nJets/I,Centrality_all:centrality,nBTags_DL1r_70/I,jet_DL1r_ordered_discrete_2:jet_DL1r_ordered_discrete[2],jet_DL1r_ordered_discrete_3:jet_DL1r_ordered_discrete[3],jet_DL1r_ordered_discrete_4:jet_DL1r_ordered_discrete[4],nRCJets/I,dRjj_Avg:deltaR_avg_jets,dRbb_MindR_DL1r_70:deltaR_min_bjets,dRbl_MindR_DL1r_70:deltaR_min_bjet_lepton,Mjjj_MinM:inv_trijet_mass_min_jets,Mbbb_MaxM_DL1r_70:inv_trijet_mass_max_bjets,Mjjj_Avg:inv_trijet_mass_avg_jets,dPhijj_Avg:deltaPhi_avg_jets,rcjet_d12_0,rcjet_d23_0,rcjet_pt_0,met_met"
    prefix_1l = "TMVA_1l" if version_1l == "AZ_test" else "TMVAMultiBkg_sm4tops" if version_1l == "190510" else "TMVA_4tops_1l"
    prefix_2l = "TMVA_2l" if version_2l == "AZ_test" else "TMVAMultiBkg_sm4tops" if version_2l == "190510" else "TMVA_4tops_2l"
    tool.__dict__["Prefix_1l"] = prefix_1l
    tool.__dict__["Prefix_2l"] = prefix_2l
    tool.__dict__["Vars_1l"] = vars_1l
    tool.__dict__["Vars_2l"] = vars_2l
    print version_1l, prefix_1l

# This method create the configurations files to run the code: three are created VariablesToRead.txt (specifies which variable
# has to be read from the ntuple), VariablesToSave.txt (not mandatory, specifies what to save: with the default SaveAllObjectDecorations
# in the configuration.txt all it is read is stored) and configuration.txt that shapes the job (at the moment we just change the path to
# absolute paths to be ran on the grid)
# In case we want to add other vector storing, just add another block when VariablesToRead.txt is filled
# In case configuration file is specified to be used, everything is skipped and:
# - the config file is copied in the job folder
# - the other files are copied taking the paths from the configuration file (also local path from run_offline)
def createConfigs(main_path,configs,dictionary,variables, region_dictionary, regions,tools,syst_dictionary, syst):
    # In case the config is directly passed to the code, configuration.txt is copied in the job directory, then the input and output variables
    # files are copied from the paths in the configuration file, if not commented out.
    if not configs == "NONE":
        shutil.copy2("../"+configs, "configuration.txt")
        for line in open("configuration.txt","r"):
            if "VariableListToDecorate" in line and not "#" in line:
                shutil.copy2("../"+line.split(" ")[1][:-1],"VariablesToRead.txt")
            if "VariablesToSaveFile" in line and not "#" in line:
                shutil.copy2("../"+line.split(" ")[1][:-1],"VariablesToSave.txt")
            if "WeightsFile" in line and not "#" in line:
                shutil.copy2("../"+line.split(" ")[1][:-1],"weights.txt")
            if "ObjectDecorationToSaveFile" in line and not "#" in line:
                shutil.copy2("../"+line.split(" ")[1][:-1],"DecoToSave.txt")
        if "NONE" not in tools:
            with open("configuration.txt","a") as f:
                for toolname,tool in tools.iteritems():
                    if "DoTTbar" in toolname:
                        line = toolname+" true\n"
                    f.write(line)

        reg_string = getRegionString(region_dictionary, regions)
        for line in fileinput.input("configuration.txt", inplace=1):
            if not syst == "nominal" and "Systematics " in line:
                syst_string =getSystString(syst_dictionary, syst)
                line = syst_string
            if line.startswith("Regions"):
                line = reg_string
            sys.stdout.write(line)

    # In case the config is not passed to the code, they are generated based on passed options (or default). The configs files are all generated
    # in the job folder
    else:
        with open("VariablesToSave.txt", "w") as ftr:
            for var in variables:
                for item in dictionary[var]:
                    ftr.write(item+"\n")
        ftr.close()

        list_default_vars = str()

        with open("VariablesToRead.txt", "w") as ftw:
            for var in variables:
                for item in dictionary[var]:

                    isStreamFlag = True if "ejets" in item or "mujets" in item or "emu_" in item or "mumu_" in item or "ee_" in item else False
                    if var == "1LOS" or var == "SSML" or var == "BSM" or var == "LooseSSML":
                        ftw.write(item+"\n")
                    if "eventNumber" in item or "mcChannelNumber" in item or "runNumber" in item:
                        continue
                    elif var == "qg":
                        ftw.write("jet "+item+"\n")
                    elif "rcjet_" in item:
                        ftw.write("rcjet "+item+"\n")
                    elif "truth_" in item and "top" not in item and "tbar" not in item:
                        if "barcode" in item or "pdgid" in item or "status" in item or "pt" in item or "eta" in item or "phi" in item:
                            ftw.write("jet "+item+"\n")
                        else:
                            ftw.write(item+"\n")
                    elif "jet_" in item:
                        #if " jet_pt" in item or " jet_eta" in item or " jet_phi" in item  or " jet_e" in item or " jet_DL1r" in item or " jet_jvt" in item:   ## Daniela OUT
                        if " jet_pt" in item or " jet_eta" in item or " jet_phi" in item  or " jet_e" in item or " jet_jvt" in item:  ## Daniela IN
                            list_default_vars += item.split(" ")[1]+","
                        elif "isbtagged" in item and "TtTRF" in tools:
                            continue
                        else:
                            ftw.write("jet "+item+"\n")
                    elif "el_" in item:
                        if " el_pt" in item or " el_eta" in item or " el_phi" in item  or " el_e" in item or " el_charge" in item:
                            list_default_vars += item.split(" ")[1]+","
                        else: ftw.write("electron "+item+"\n")
                    elif "mu_" in item and not isStreamFlag:
                        if  " mu_pt" in item or " mu_eta" in item or " mu_phi" in item or " mu_e" in item or " mu_charge" in item:
                            list_default_vars += item.split(" ")[1]+","
                        else: ftw.write("muon "+item+"\n")
                    elif "met_met" in item or "met_phi" in item:
                        list_default_vars += item.split(" ")[1]+","
                    else:
                        ftw.write(item+"\n")

            # Block checking whether the important variables for the Tool are included, otherwise broken output
            if "Add1LOSVar" in tools:
                for item in ["float mll", "int lepFlavChannel", "int nRCJets", "int nJets", "float mtw","float met_met","int nBTags_DL1r_77"]:
                    included = False
                    for var in variables:
                        if item in dictionary[var]:
                            included = True
                            break
                    if included == False:
                        if "met_" in item: list_default_vars += item.split(" ")[1]+","
                        else: ftw.write(item+"\n")
            # Block checking whether the important variables for the Tool are included
            if "DoTTbarBfilter" in tools:
                included = False
                for var in variables:
                    if "TopHeavyFlavorFilterFlag" in dictionary[var]:
                        included = True
                        break
                if included == False:
                    ftw.write("int TopHeavyFlavorFilterFlag\n")
            if "DoTTbarDileptonFilter" in tools:
                included = False
                for var in variables:
                    if "truth_top_dilep_filter" in dictionary[var]:
                        included = True
                        break
                if included == False:
                    ftw.write("int truth_top_dilep_filter\n")

        ftw.close()
        shutil.copy2(main_path+"/configs/configuration.txt",".")
        reg_string = getRegionString(region_dictionary, regions)
        for line in fileinput.input("configuration.txt", inplace=1):
            if line.startswith("Regions"):
                line = reg_string
            if not syst == "nominal" and "Systematics " in line:
                syst_string =getSystString(syst_dictionary, syst)
                line = syst_string
            if line.startswith("SaveDefaultVariables") and list_default_vars != "":
                defvars_string = line.replace("\n","")+","+list_default_vars[:-1]+"\n"
                line = defvars_string
            sys.stdout.write(line)
        if "NONE" not in tools:
            if "TtTRF" in tools: setupTtTRFConfig(tools["TtTRF"])
            if "AddBDT1LOS" in tools: addVarsToBDT1LOS(tools["AddBDT1LOS"])
            with open("configuration.txt","a") as f:
                for toolname in TOOLS_ORDERED_LIST:
                    if toolname in list (tools.keys() ):
                        tool = tools [toolname]
                        if "DoTTbar" in toolname:
                            line = toolname+" true\n"
                        else:
                            line = toolname+".TurnOn true\n"
                        f.write(line)
                        for c in [a for a in dir(tool) if not a.startswith('__')]:
                            if c == "name": continue
                            opt = tool.__dict__[c]
                            opt = opt.replace ("_AND_",",").replace ("&",",").replace ("%",",")
                            line = toolname+"."+c+" "+opt+"\n"
                            f.write(line)
                    f.write("\n")
        shutil.copy2(main_path+"/configs/weights.txt",".")

    # Now that all the configs are generated in the current directory, the paths are modified to be absolute to run on batch
    for line in fileinput.input("configuration.txt", inplace=1):
        if line.startswith("VariableListToDecorate"):
            line = line.replace(line.split(" ")[1][:-1],os.getcwd()+"/VariablesToRead.txt")
        if line.startswith("VariablesToSaveFile"):
            line = line.replace(line.split(" ")[1][:-1],os.getcwd()+"/VariablesToSave.txt")
        if line.startswith("WeightsFile"):
            line = line.replace(line.split(" ")[1][:-1],os.getcwd()+"/weights.txt")
        if line.startswith("ObjectDecorationToSaveFile"):
            line = line.replace(line.split(" ")[1][:-1],os.getcwd()+"/DecoToSave.txt")
        # To add the CustomObject definition for rcjets, otherwise not filled properly
        if line.startswith("CreateOutputFileFirst") and "rcjet" in variables:
            line = line + "\n" + "CustomObjects rcjet"+"\n"
        sys.stdout.write(line)

# Class definition
# - sample: process to be processed (eg ttbar)
# - typology: which samples to be processed (eg data, mc16a, mc16cd or both)
# - analysis: flag used to specify the input samples: 4tops or ttHbb
# - input_path: path where samples are stored
# - output_path: path where the output ntuples are stored
# - vars_dict: dictionary containing the variables to be read/stored corresponding to each setting
# The methods are:
# - __init__ : constructor, that fill the features
# - executeLocal : launch the offline code locally
# - createBatchScript : generate the bash script to be used when the offline code is launched on batch
# - executeBatch : launch the offline code on the batch
# - splitAndCombineeBatch : launch the offline code on the batch (with split-and-combine enabled
class Job:
    sample = ""
    typology = ""
    input_path = ""
    output_path = ""
    analysis = ""
    channel = ""
    vars_dict = {}
    regions_dict = {}
    tools_dict = {}
    syst_dict = {}

    # Constructor function for Job class
    # One job is relative to one set of process, type, analysis, systematic and tools
    def __init__ (self, exe_path,sample, typ, analysis, channel, inpath, outpath,tools,syst, access_method, build_area):
        self.exe_path = exe_path
        self.sample = sample
        self.channel = channel
        self.typology = typ
        self.analysis = analysis
        self.input_path = inpath
        self.output_path = outpath
        self.tools_dict = tools
        self.access_method = access_method
        self.build_area = build_area
        # The list of input sample are generated from lib files in lib/
        if typ == 'mc16acd':
            list_sample = setup_sample_list (exe_path,sample, 'mc16a', analysis, channel, syst)
            list_sample += setup_sample_list (exe_path,sample, 'mc16cd', analysis, channel, syst)
        elif typ == 'mc16acde':
            list_sample = setup_sample_list (exe_path,sample, 'mc16a', analysis, channel, syst)
            list_sample += setup_sample_list (exe_path,sample, 'mc16cd', analysis, channel, syst)
            list_sample += setup_sample_list (exe_path,sample, 'mc16e', analysis, channel, syst)
        else: list_sample = setup_sample_list (exe_path,sample, typ, analysis, channel, syst)
        # Directories and required samples are checked and in case downloaded
        inpath = inpath.replace("BSM","1LOS")
        if '/eos/atlas/atlascerngroupdisk/phys-top/' or '/lustre/' in inpath:
            for t in ['mc16a','mc16c','mc16d','mc16e','data']:
                input_path_copy = inpath.replace('/<type>/',"/"+t+"/")
                if access_method == 'standard': check_path(input_path_copy)
        elif access_method == 'standard':
            check_path(inpath)
        check_path(outpath)
        if access_method == 'xrootd':
            setup_input_list_xrootd (list_sample)
        elif access_method == 'find_file':
            setup_input_list_find_file (inpath, list_sample)
        else:
            check_samples(inpath, analysis, list_sample, typ)
            setup_input_list(inpath, list_sample)

        # Variables, regions and systematics dictionaries are defined
        # These give the actual string to be included in the configuration file starting from the labels specified in the
        # execution phase
        self.vars_dict = setup_variables_dictionary(self.analysis)
        self.regions_dict = setup_regions_dictionary(self.channel,self.analysis,self.typology,self.sample)
        self.syst_dict = setup_syst_dictionary()
        # Adding stream flags to variables dictionary
        self.vars_dict.update(getStreamFlags(self.channel, self.analysis, self.typology, self.sample))



    # Execute ttH-offline, locally
    # The config files is generated and the ttH-offline is launched
    def executeLocal(self,configs,variables,regions,syst, norun):
        #Add the channel variables to selected variables
        variables.append(self.channel)
        createConfigs(self.exe_path,configs,self.vars_dict,variables,self.regions_dict,regions,self.tools_dict,self.syst_dict,syst)
        out_dir = self.typology
        # The folder for histogramming is generated by default
        check_path(self.output_path+"/"+out_dir+"/histo")
        out_file = self.sample
        if not syst == "nominal": out_file += "_"+syst
        config = "configuration.txt"
        offlineCmd = "SM4t-offline" # SM4t-offline or ttH-offline
        cmd = offlineCmd+ " " +config+" input_list.txt -o"
        cmd += " SampleName "+out_file
        cmd += " OutputDirectory "+self.output_path+"/"+out_dir
        if self.typology == "data":
            cmd += " SampleType data"
        if norun == False:
            print("###### Debug: Running command -> ",cmd) #Daniela
            os.system (cmd)

    # Generation of HTCondor submission script
    def createMergeScript(self,mail, input_files_raw, regions):
        input_files_refined = ""
        input_files_transfer = ""
        for r in regions:
            input_files_refined += " "+input_files_raw.replace ("REGION",r)
            input_files_transfer += " "+input_files_raw.replace (input_files_raw.split(" ")[0]+" ", "").replace ("REGION",r)
        with open("job_merge.sub","w") as f:
             f.write('universe = vanilla\n')
             f.write("executable\t\t = merge.sh\n")
             f.write("arguments\t\t = "+input_files_refined+"\n")
             f.write("output\t\t = out/merge.out\n")
             f.write("error\t\t = err/merge.err\n")
             f.write("log\t\t = log/merge.log\n")
             f.write('requirements = (OpSysAndVer =?= "CentOS7")\n')
             if mail != "none":
                 f.write('notification\t\t = always\n')
                 f.write('notify_user\t\t = '+mail+'\n')
             f.write('+JobFlavour\t\t = "tomorrow"\n')
             f.write('transfer_input_files\t\t = '+input_files_transfer.replace (' ',', ')[1:]+'\n')
             f.write("queue \n")
             f.close()


    # Generation of HTCondor submission script
    def createCondorMultiScript(self,mail):
        with open("job.sub","w") as f:
            f.write('universe = vanilla\n')
            f.write("executable\t\t = $(filename)\n")
            f.write("arguments\t\t = $Fnx(filename)\n")
            f.write("output\t\t = out/$Fn(filename).$(ClusterId).$(ProcId).out\n")
            f.write("error\t\t = err/$Fn(filename).$(ClusterId).$(ProcId).err\n")
            f.write("log\t\t = log/$(ClusterID).log\n")
            f.write('requirements = (OpSysAndVer =?= "CentOS7")\n')
            if mail != "none":
                f.write('notification\t\t = always\n')
                f.write('notify_user\t\t = '+mail+'\n')
            f.write('+JobFlavour\t\t = "tomorrow"\n')
            f.write("queue filename matching ("+self.sample+"*.sh)\n")
            f.close()



    # Generation of HTCondor submission script
    def createCondorScript(self,exe,mail):
        with open("job.sub","w") as f:
            f.write('universe = vanilla\n')
            f.write("executable\t\t = "+exe+"\n")
            f.write("output\t\t = job.out\n")
            f.write("error\t\t = job.err\n")
            f.write("log\t\t = job.log\n")
            f.write('requirements = (OpSysAndVer =?= "CentOS7")\n')
            if mail != "none":
                f.write('notification\t\t = always\n')
                f.write('notify_user\t\t = '+mail+'\n')
            f.write('+JobFlavour\t\t = "tomorrow"\n')
            f.write("queue\n")
            f.close()

    # Generation of batch script file to run ttH-offline
    def createBatchScript(self,syst):
        curr_path = os.getcwd()
        fw_path = ""
        for p in curr_path.split("/") :
            fw_path += p + "/"
            if "common-framework" in p: break
        build_area = fw_path+"/build"
        if self.build_area != "" :
            build_area = self.build_area
        out_dir = self.typology
        # The folder for histogramming is generated by default
        check_path(self.output_path+"/"+out_dir+"/histo")
        out_file = self.sample
        if not syst == "nominal": out_file += "_"+syst
        config = "configuration.txt"
        offlineCmd = "SM4t-offline" # SM4t-offline or ttH-offline
        cmd = offlineCmd +" "+curr_path+"/"+config+" "+curr_path+"/input_list.txt -o"
        cmd += " SampleName "+out_file
        cmd += " OutputDirectory "+self.output_path+"/"+out_dir
        if self.typology == "data":
            cmd += " SampleType data"
        with open("batch.sh","w") as f:
            f.write("#!/bin/bash"+"\n")
            f.write("path="+curr_path+"\n")
            f.write("export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"+"\n")
            f.write("source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh"+"\n")
            f.write("cd "+build_area+"\n")
            f.write("asetup --restore"+"\n")
            # FIXME: doesn't this cause conflict in case of more than 1 jobs?
            #f.write("cmake ../Framework"+"\n")
            #f.write("cmake --build ./"+"\n")
            f.write("source */setup.sh"+"\n")
            if self.access_method == "xrootd" or self.access_method == "find_file" :
                f.write("lsetup -f \"xrootd 4.7.0-x86_64-slc6-gcc62-opt\""+"\n")
                voms_in_tmp = os.popen ('echo $X509_USER_PROXY').read().replace ('\n','')
                afs_path = os.popen ('echo ~/').read().replace ('\n','')
                os.system ('cp '+voms_in_tmp+' '+afs_path+'/')
                f.write ('export X509_USER_PROXY='+afs_path+voms_in_tmp.split('/')[-1]+'\n')
            f.write(cmd+"\n")
            f.close()




    # Execute ttH-offline, on batch
    # The config files is generated, the batch script is generated and the ttH-offline is launched
    def executeBatch(self, configs,variables,regions,syst,queue, ncores, norun,batch,mail):
        #Add the channel variables to selected variables
        variables.append(self.channel)
        createConfigs(self.exe_path, configs,self.vars_dict,variables,self.regions_dict,regions,self.tools_dict,self.syst_dict, syst)
        self.createBatchScript(syst)
        if batch == "lsf":
            batch_cmd = "bsub -J "+self.sample+"_"+syst+" -q "+queue+" -n "+str(ncores)+" < batch.sh"
            print "\t > Submission of %s on the lxbatch on %s w/ %s cores " % (self.sample, queue,ncores)
            if norun == False:
                os.system (batch_cmd)
        elif batch == "condor":
            script_name = self.sample+"_"+syst+".sh"
            os.system("mv batch.sh "+script_name)
            self.createCondorScript(script_name, mail)
            batch_cmd = "condor_submit job.sub"
            print "\t > Submission of %s on the HTCondor" % (script_name)
            if norun == False:
                os.system (batch_cmd)


    # Execute ttH-offline, on batch with split-and-combine
    # The config files is generated, the batch script is generated and the ttH-offline is launched
    def splitAndCombineBatch(self, configs,variables,regions,syst,queue, ncores, norun,batch,mail):
        #Add the channel variables to selected variables
        variables.append(self.channel)
        createConfigs(self.exe_path, configs,self.vars_dict,variables,self.regions_dict,regions,self.tools_dict,self.syst_dict, syst)
        self.createBatchScript(syst)
        script_name = self.sample+"_"+syst+".sh"
        os.system("mv batch.sh "+script_name)

        # get the output file name
        out_file = self.sample
        if not syst == "nominal": out_file += "_"+syst

        ## Now splitting and multi job
        ## Setting up the scripts
        num_lines = sum(1 for line in open('input_list.txt'))
        argument_files = ""

        num_jobs = computeNumberOfJobs ( syst!='nominal' )
        num_lines_per_job = (num_lines) / num_jobs

        lines = open ("input_list.txt").readlines()
        for j in range (0,num_jobs):

            rest =  (num_lines - (j+1)*num_lines_per_job) if (j+1)*num_lines_per_job != num_lines and j == (num_jobs-1) else 0
            skimmed_list = [i for n, i in enumerate(lines) if n in range (j*num_lines_per_job, (j+1)*num_lines_per_job+rest) ]
            f = open("input_list_"+str(j)+".txt","w+")
            f.writelines(["%s" % item  for item in skimmed_list])
            f.close()
            os.system ("cp "+script_name+" "+script_name.replace(".sh","_"+str(j)+".sh") )
            for l in fileinput.input( script_name.replace(".sh","_"+str(j)+".sh") , inplace = 1):
                l = l.replace("/input_list.txt", "/input_list_"+str(j)+".txt").replace ("SampleName "+out_file, "SampleName "+out_file+"_"+str(j))
                sys.stdout.write(l)
            argument_files += " "+self.output_path+self.typology+"/REGION/"+out_file+"_"+str(j)+".root"

        os.system ("rm "+script_name)
        os.system ("rm input_list.txt")
        os.system ("mkdir out err log")
        self.createCondorMultiScript(mail)


        ## Now merging the ntuples
        ## Setting up the scripts
        argument_files = self.output_path+self.typology+"/REGION/"+out_file+".root"+argument_files

        curr_path = os.getcwd()
        fw_path = ""
        for p in curr_path.split("/") :
            fw_path += p + "/"
            if "common-framework" in p: break

        merging_job = open ("merge.sh", "w+")
        merging_job.write ("#!/bin/bash \n")
        merging_job.write ("export PATH=$PATH:"+fw_path+"Framework/OffSM4topsTool/python\n")

        index = 0
        for reg in regions:
            cmd = "MergeSample.py "+" ".join ('${'+str(x)+'}' for x in range (index*(num_jobs+1)+1, index*(num_jobs+1)+1+num_jobs+1))
            merging_job.write (cmd.replace ("REGION",reg)+"\n")
            index = index + 1

        self.createMergeScript(mail,argument_files,regions)

        dag_file = open ("job.dag","w+")
        dag_file.write ("JOB A job.sub\n")
        dag_file.write ("JOB B job_merge.sub\n")
        dag_file.write ("PARENT A CHILD B\n")
        dag_file.close()

        cmd_submit = "condor_submit_dag job.dag"
        if mail != "none": cmd_submit = cmd_submit.replace ( "_dag", "_dag -dont_suppress_notification" )
        if norun == False:
            os.system (cmd_submit)
        else :
            print "\t > Launch " + cmd_submit
