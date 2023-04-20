####################################################################################
###                                                                              ###
### Goal: printout/generate/validate production scripts                          ###
###                                                                              ###
### [PRINT] Print the block for a given sample and campaign                      ###
###  Use: python updateTOPQ1helper.py --print <block_name>,<campaign>            ###
###      ex. python updateTOPQ1helper.py --print Vjets_Sherpa221,mc16a           ###
###                                                                              ###
### [GENERATE] Generate the TOPQ1 MC sample list for a given campaign            ###
###  Use: python updateTOPQ1helper.py --generate <campaign>                      ###
###      ex. python updateTOPQ1helper.py --generate mc16a                        ###
###                                                                              ###
### [VALIDATE] Validate the TOPQ1 MC lists and scripts for agiven campaign       ###
###  Use: python updateTOPQ1helper.py --validate <campaign>                      ###
###      ex. python updateTOPQ1helper.py --validate mc16a                        ###
###  Step 1: check campaign/rTag in lists                                        ###
###  Step 2: check main bkg MC samples on rucio                                  ###
###  Step 3: check campaign in submission script                                 ###
###  Step 4: check whether samples called in submission script are in lists      ###
###                                                                              ###
####################################################################################

import os,sys

dic_sample = dict()
MAXPTAG = 5000
SKIPTTBARALLHAD = True

def writeIntro (f):
    f.write (
        'import TopExamples.grid as grid \n'
        'import TopExamples.ami \n ' 
        '\n'
        '## COMMENTS \n# \n# \n# \n'
    )
    
    


def get_sample (dsid, rtag, stag):

    cmd = 'ami list datasets mc16_13TeV% --dataset-number '+str(dsid)+' --type DAOD_TOPQ1'

    ami_output = os.popen(cmd).read()
    file_list = ami_output.split("\n")
    good_list = list()
    highest_ptag = 0
    for f in file_list:
        if f == '': continue
        pass_condition = ('_'+rtag+'_' in f and '_'+stag+'_' in f)
        if pass_condition: 
            if int(f[-4:]) > MAXPTAG: continue
            good_list.append(f)
            if int(f[-4:]) > highest_ptag: highest_ptag = int(f[-4:]) 
    
    highest_ptag_list = list()
    duplicate_list = list()
    for f in good_list: 
        if str(highest_ptag) not in f: continue
        if f.count('_r') > 1: duplicate_list.append(f)
        elif f.count('_p') > 1: duplicate_list.append(f)
        else: highest_ptag_list.append(f)
        
    if len(highest_ptag_list) != 0: return  highest_ptag_list[0]
    elif len(duplicate_list) != 0:
        for f in duplicate_list: 
            if f.count('_p') > 1: return f
        for f in duplicate_list: 
            if f.count('_r') > 1: return f
    else: return ""

def setup_dic_sample ():      
     dic_sample ["tttt_aMcAtNloPy8_AFII"] = [412043]
     dic_sample ["tttt_aMcAtNloHerwig7_AFII"] = [412044]
     dic_sample ["tttt_MadGraphPy8LO_FS"] = [412115]
     dic_sample ["ttH_tttt"] = range (312440,312446+1)
     dic_sample ['ttbar_PhPy8_FS'] = [410470, 410472] 
     dic_sample ['ttbar_PhPy8_AFII'] = [410470, 410472]
     dic_sample ['ttbar_PhPy8_Rad_AFII'] = [410480, 410482]
     dic_sample ['ttbar_PhHerwig_AFII'] = range (410557,410558+1)
     dic_sample ['ttbar_PhHerwig713_AFII'] = range (411233,411234+1)
     dic_sample ['ttbar_Sherpa221'] = range (410250,410252+1)
     dic_sample ['ttbar_aMcAtNloPy8_AFII'] = range (410464,410465+1)
     dic_sample ['ttbar_PhPy8OL_ttbb4F_AFII'] = range (411068,411069+1)
     dic_sample ['ttbar_PhPy8_FS_HT_sliced'] = range (407342, 407344+1)
     dic_sample ['ttbar_PhPy8_AFII_HT_sliced'] = range (407342,407344+1)
     dic_sample ['ttbar_PhPy8_AFII_HF_filt'] = range (411073,411078+1)
     dic_sample ['ttbar_PhHerwig_AFII_HF_filt'] = range (411082,411087+1)
     dic_sample ['ttbar_PhHerwig_AFII_HT_sliced'] = range (407354, 407356+1)
     dic_sample ['ttbar_aMcAtNloPy8_AFII_HF_filt'] = range (412066,412071+1)
     dic_sample ['ttbar_aMcAtNloPy8_AFII_HT_sliced'] = range (407348, 407350+1)
     dic_sample ['SingleTop'] = [410658, 410659, 410646, 410647, 410644, 410645, 410560, 410408]
     dic_sample ['SingleTop_Alt'] = [410658, 410659, 411032, 411033, 410646, 410647, 410654, 410655, 411036, 411037, 410644, 410645, 411034, 411035, 412004, 412002, 412005]
     dic_sample ['ttH'] = [346344, 346345]
     dic_sample ['ttW'] = [410155]
     dic_sample ['ttWNLO'] = [413008]
     dic_sample ['ttX'] = [410156, 410157, 410218, 410219, 410220, 410276, 410277, 410278]
     dic_sample ['OtherTop'] = [410560, 410408, 410081, 304014]
     dic_sample ['ttGamma'] = [410389]
     dic_sample ['Vjets_PhPy8'] = range (361100,361108+1)
     dic_sample ['Vjets_Sherpa221'] = range (364170,364183+1)
     dic_sample ['Vjets_Sherpa221'].extend ( range (364156, 364169+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364184, 364197+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364114, 364127+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364100, 364113+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364128, 364141+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364204, 364209+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364198, 364203+1) )
     dic_sample ['Vjets_Sherpa221'].extend ( range (364210, 364215+1) )
     dic_sample ['VV'] = [364250, 364253, 364254, 364255, 364283, 364284, 364285, 364287, 364288, 364289, 364290, 345705, 345706, 345723, 363355, 363356, 363357, 363358, 363359, 363360, 363489, 363494]
     dic_sample ['VVV'] = [364242, 364243, 364244, 364245, 364246, 364247, 364248, 364249]
     dic_sample ['VH'] = [342284, 342285]
     dic_sample ['Vgamma'] = range (366140, 366154+1)
     # dic_sample ['TEST'] = [999999]
     



def get_dsids(sample):
     if sample not in dic_sample.keys():
         print "ERROR not recognised sample %s " % (sample)
         exit (1)
         
     else: 
         return dic_sample [sample]
    
def get_rtag (campaign):
    if campaign == "mc16a": return "r9364"
    elif campaign == "mc16d": return "r10201"
    elif campaign == "mc16e": return "r10724"
    else:
        print "ERROR not recognised campaign %s " % (sample)
        exit (1)
        

def get_stag (sample, campaign, dsid):
    if 'AFII' in sample: return 'a875'
    elif 'tttt' in sample:
        return 'a875'
    elif 'SingleTop_Alt' in sample:
        if dsid in dic_sample ['SingleTop']: return 'a875'
        elif dsid in [411032, 411033, 411036, 411037, 412002, 411034, 411035, 412004, 412002, 412005]: 
            return 'a875'
    return 's3126'

import argparse

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser(
        description='Generate (print) the TOPQ1 list (or part of it).  To generate the full list for a given <campaign>, type:  python updateTOPQ1helper.py generate <camapign> To print out just a block (checking or fast updating): python updateTOPQ1helper.py print <block>,<campaign>  ')
    
    parser.add_argument('--generate', dest="generate", action='store_true',
                    help='Generate the list for the given campaign')
    parser.add_argument('--print', dest="printout", action='store_true',
                    help='Print the up-to-date block passed as argument')
    parser.add_argument('input_pars', action='store',
                    help='Input string')
    parser.add_argument('--validate', action='store_true',dest='validate',
                    help='Validate the lists/submission scripts')
    parser.add_argument('--test', action='store_true',dest='test',
                        help='Testing: if --generate a file with "TEST_*"  is created', default=False)
    
    args = parser.parse_args()

    haveToPrint = args.printout
    haveToGenerate = args.generate
    haveToValidate = args.validate
    inputString = args.input_pars
    nSubPars = len( inputString.split(',' ) )

    setup_dic_sample ()

    if haveToPrint and nSubPars != 2:
        print ("\033[1;31;40m ERROR: option --print is set but no correct option is passed, the code does not know which block to print! \n \t Usage: updateTOPQ1helper --print <block>,<campaign> \033[0m")
        exit (1)


    elif haveToValidate and nSubPars != 1:
        print ("\033[1;31;40m ERROR: option --validate is set but no correct option is passed, the code does not know which list to generate! \n \t Usage: updateTOPQ1helper --validate <campaign> \033[0m")
        exit (1)

    elif haveToGenerate and nSubPars != 1:
        print ("\033[1;31;40m ERROR: option --generate is set but no correct option is passed, the code does not know which list to generate! \n \t Usage: updateTOPQ1helper --generate <campaign> \033[0m")
        exit (1)

    elif haveToPrint and nSubPars == 2:
        sample = inputString.split(',')[0]
        campaign = inputString.split(',')[1]
                
        list_dsids = get_dsids(sample)
        rtag = get_rtag (campaign)
        
        ## Here we print the block to put in the list
        print  '\033[1;32;40m === Block to be added in the list === \033[0m \n'
        print 'grid.Add("'+campaign+'_'+sample+'").datasets = ['
        
        for d in list_dsids:
            if SKIPTTBARALLHAD and d == 410471: continue 
            stag = get_stag (sample, campaign, d)
            sam = get_sample (d, rtag, stag)
            if sam != "": print '\t "'+sam+'",'
        print ']\n'

        print  '\033[1;32;40m === End of the block ===\033[0m \n'


    elif haveToGenerate and nSubPars == 1:
        comments = list()
        prefix = "TEST_" if args.test else ""
        campaign = inputString
        list_name = prefix+campaign+"_SM4tops.py"
        
        print  '\033[1;32;40m INFO Generating '+list_name+'\033[0m \n'
        
        f = open (list_name, "w")
        writeIntro (f)
        f.write("\n")
        
        
        
        index = 0
        for sample in sorted( dic_sample.keys(), key=str.lower ):
            
            list_dsids = get_dsids(sample)
            
            rtag = get_rtag (campaign)
        
            f.write( 'grid.Add("'+campaign+'_'+sample+'").datasets = [ \n' )
            print "Block %s: # DSIDs = %d" %(sample, len(list_dsids))
            for d in list_dsids:
                if SKIPTTBARALLHAD and d == 410471: continue 
                stag = get_stag (sample, campaign, d)
                sam = get_sample (d, rtag, stag)
                if sam == "":
                    comments.append ("# AUTOMATIC COMMENT: DSID "+str(d)+" ("+str(sample)+") "+str(stag)+" missing")
                    continue
                f.write ( '\t "'+sam+'", \n' )
            f.write ('] \n \n' )
            
            # if index > 3 : break
            index = index + 1

        f.close()

        if len (comments):
            print '\033[1;33;40m WARNING automatic comments created\033[0m \n'
            comments_string = ""
            for c in comments: comments_string += c+"\n"
            import fileinput            
            for line in fileinput.FileInput(list_name,inplace=1):
                if "COMMENTS" in line:
                    line=line.replace(line,line+comments_string)
                sys.stdout.write(line)

    elif haveToValidate and nSubPars == 1:
        
        errors_step1 = list()

        prefix = "TEST_" if args.test else ""
        campaign = inputString
        list_name = prefix+campaign+"_SM4tops.py"
        

        print  '\033[1;32;40m STEP 1: checking rTags/blocks names in '+list_name+'\033[0m \n'
        
        f = open (list_name, "r")
        lines = f.readlines()
        f.close()
        
        rtag = get_rtag (campaign)
        
        index = 1
        for l in lines:
            if 'grid.Add' in l and campaign not in l:
                errors_step1.append (l)
                print '\033[1;31;40m ERROR line '+str(index)+' wrong campaing in block definition\033[0m \n'
            if 'mc16_13TeV' in l and '_'+rtag+'_' not in l:
                errors_step1.append (l)
                print '\033[1;31;40m ERROR line '+str(index)+' wrong rtag in the list\033[0m \n'
            index = index+1

        if len(errors_step1) != 0:
            print '\033[1;31;40m \t STEP 1 Failed! please check the errors above\033[0m \n'
            exit(1)
        else: print '\033[1;32;40m \t STEP 1 OK!\033[0m' 
        

        print  '\033[1;32;40m STEP 2: checks on rucio on main MC bkg\033[0m \n'
        ttbar_list = [x for x in lines if 'mc16_13TeV' in x and '41047' in x]
        tttt_list = [x for x in lines if 'mc16_13TeV' in x and '41204' in x]
        ttV_list = [x for x in lines if 'mc16_13TeV' in x and ('41015' in x or '41021' in x)]
        error_step2 = list()
        
        print  '\033[1;32;40m \t ttbar samples\033[0m \n'
        for x in ttbar_list:
            x = x.replace(" ","").replace('"',"").replace('\n','').replace('\t','').replace(',','')
            cmd = 'rucio ls '+x+' --filter type=CONTAINER'
            rucio_output  = os.popen(cmd).read()
            if x not in rucio_output: error_step2.append(x)
        
        print  '\033[1;32;40m \t 4tops samples\033[0m \n'
        for x in tttt_list:
            x = x.replace(" ","").replace('"',"").replace('\n','').replace('\t','').replace(',','')
            cmd = 'rucio ls '+x+' --filter type=CONTAINER'
            rucio_output  = os.popen(cmd).read()
            if x not in rucio_output: error_step2.append(x)
        
        print  '\033[1;32;40m \t ttV samples\033[0m \n'
        for x in ttV_list:
            x = x.replace(" ","").replace('"',"").replace('\n','').replace('\t','').replace(',','')
            cmd = 'rucio ls '+x+' --filter type=CONTAINER'
            rucio_output  = os.popen(cmd).read()
            if x not in rucio_output: error_step2.append(x)
            
        if len( error_step2 ) != 0:
            print  '\033[1;31;40m ERROR The following samples not found on rucio: \n'
            for s in error_step2:
                print '\t %s' % s
            print '\n \033[0m'
            exit (1)
        else: print '\033[1;32;40m \t STEP 2 OK!\033[0m' 
                
        print  '\033[1;32;40m STEP 3: consistency of submission scripts\033[0m \n'
        stream_number = 1 if campaign == "mc16a" else 2 if campaign == "mc16d" else 3
        errors_step3 = list()
        for channel in ['1LOS','SSML']:
            sub_script = 'submitToGrid_'+channel+'_'+str(stream_number)+'.py'
            f = open (sub_script, "r")
            lines = f.readlines()
            f.close()
            index = 1
            for l in lines:
                if 'data' in l: continue
                if 'names = ' in l and campaign not in l:
                    errors_step3.append (l)
                    print '\033[1;31;40m ERROR '+sub_script+' in line '+str(index)+' wrong campaign (sample blocks)\033[0m \n'
                if 'config.suffix' in l and campaign not in l:
                    errors_step3.append (l)
                    print '\033[1;31;40m ERROR '+sub_script+' in line '+str(index)+' wrong campaign (config suffix)\033[0m \n'
                if 'config.settingsFile' in l and campaign not in l:
                    errors_step3.append (l)
                    print '\033[1;31;40m ERROR '+sub_script+' in line '+str(index)+' wrong campaign (config settingsFile)\033[0m \n'

                index = index +1

        if len(errors_step3) != 0:
            print '\033[1;31;40m \t STEP 3 Failed! please check the errors above\033[0m \n'
            exit(1)
        else: print '\033[1;32;40m \t STEP 3 OK!\033[0m' 

        print  '\033[1;32;40m STEP 4: cross validation between submission script and list \033[0m \n'
        errors_step4 = list()
        flist  = open (list_name, "r")
        list_lines = flist.readlines()
        flist.close()
        
        for channel in ['1LOS','SSML']:
            sub_script = 'submitToGrid_'+channel+'_'+str(stream_number)+'.py'
            f = open (sub_script, "r")
            lines = f.readlines()
            f.close()
            index = 1
            for l in lines:
                if 'data' in l: continue
                if 'names = ' in l:
                    l = l.replace('names = [','').replace(']','').replace("'","").replace(" ","").replace('"','').replace('\n','')
                    
                    for block in l.split(","):
                        
                        entries_in_list = [x for x in list_lines if  str('grid.Add("'+block+'")') in x ]
                        if len(entries_in_list) == 0:
                            errors_step4.append (l)
                            print '\033[1;31;40m ERROR '+sub_script+' in line '+str(index)+': '+block+' not found in list '+list_name+'\033[0m \n'
                    index = index +1

        if len(errors_step4) != 0:
            print '\033[1;31;40m \t STEP 4 Failed! please check the errors above\033[0m \n'
            exit(1)
        else: print '\033[1;32;40m \t STEP 4 OK!\033[0m' 

        


    else:
        print '\033[1;31;40m ERROR Something weird in the settings, instructions with "-h" option\033[0m \n'
        exit (1)
