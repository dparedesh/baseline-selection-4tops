#!/bin/env python


import os, sys

if len (sys.argv) < 3:
    print 'ERROR At least 2 parameter needed'
    print 'INFO usage MergeSample.py <output_file> <input_file_1> <input_file_2> .. <input_file_N> '
    exit (1)

import ROOT

## First hadd the files
##

files = str()
for f in sys.argv: 
    if "MergeSample.py" not in f: files += " "+f

print "INFO Removing spurious file: merged files and *_post.root files"
if os.path.exists (sys.argv[1]): os.system ("rm "+sys.argv[1])
if os.path.exists (sys.argv[1].replace (".root","_post.root")): os.system ("rm "+sys.argv[1].replace (".root","_post.root"))
print "INFO hadd "+files
os.system ("hadd "+files) 


## Fill the dictionaries for DSIDs for the computation
##

from ROOT import TFile, TTree

output_file = sys.argv [1]
norm_dictionary = dict()


f = TFile (output_file, "r+")
for key in f.GetListOfKeys():
    if key.GetClassName() != "TTree": continue
    tree_name =  key.GetName()
    tree = eval ("f."+tree_name)
    for evt in tree:
        label = tree_name+"__"+str(evt.mcChannelNumber)
        
        if label not in norm_dictionary:
            norm_dictionary [label] = list()
            norm_dictionary [label].append (evt.weight_normalise)
        else: 
            if evt.weight_normalise not in norm_dictionary [label]:
                norm_dictionary [label].append (evt.weight_normalise)
f.Close()

print "INFO Pre-merging normalisations:"
for label, list_norm in norm_dictionary.iteritems():
    print "     - "+label+" = "+str(list_norm)

## Computation of the normalisation
##

for label, list_norm in list( norm_dictionary.iteritems() ):
    norm = 0
    for val in list_norm: norm += 1./val
    norm = 1./norm
    norm_dictionary [label + "_post"] = norm

print "INFO Post-merging normalisations:"
for label, list_norm in norm_dictionary.iteritems():
    if "_post" not in label: continue
    print "     - "+label+" = "+str(list_norm)

## Refill the tree
##    

print "INFO Refilling the weight_normalise branch"

from array import array

f_old = TFile(output_file, "READ")
f_new = TFile(output_file.replace (".root", "_post.root"), "RECREATE")

for key in f_old.GetListOfKeys():
    if key.GetClassName() != "TTree": continue
    tree_name =  key.GetName()
    tree_old = eval ("f_old."+tree_name)
    tree_old.SetBranchStatus("*",1)
    tree_old.SetBranchStatus("weight_normalise",0)
    tree_new = tree_old.CloneTree()

    weight_normalise = array( 'f', [ 0 ] )
    b = tree_new.Branch ("weight_normalise", weight_normalise, "weight_normalise/F")
    
    for evt in tree_new:
        dsid = evt.mcChannelNumber
        weight_normalise[0] = norm_dictionary [tree_name+"__"+str(dsid)+"_post"]
        b.Fill()

    f_new.cd()
    tree_new.Write()
    f_old.cd()

f_old.Close()
f_new.Close()

print "INFO Replacing the merged (with original normalisation) with the *_post.root (with merged)"
os.system ("mv "+output_file.replace (".root", "_post.root")+" "+output_file)

print "INFO Removing the splitted files"
for f in list (sys.argv) :
    if f == sys.argv[0] or f == sys.argv[1]: continue
    print "     - Removing %s" % f
    os.system("rm "+f)
