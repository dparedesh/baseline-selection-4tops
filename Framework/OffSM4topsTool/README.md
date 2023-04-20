# README


Compile the tool... 
-------------------

Just follow instructions for the offline code, BUT using the PackageFilter inside this folder:
```
cd common-framework #!!
mkdir build  #!!
cd build  #!!
asetup AnalysisTop,21.2.56 || echo ignore alrb  #!! (or --restore from the 2nd  time)
cmake -DATLAS_PACKAGE_FILTER_FILE=../Framework/OffSM4topsTool/PackageFilters/offline_packages.txt ../Framework #!!
cmake --build ./ #!!
source */setup.sh #!!
```

SM4t-offline
------------
Like ttH-offline, but with additional dictionary entries specific to our own tools (AddVar1LOS settings, TtTRF settings, ...).
Note that some other configuration files have been added in the folder "share". 


TtTRF
----
   Usual TtTRF tool, used together with SM4t-offline to produce ttTRF ntuples. 
   Use with configuration and weigths in the subfolder "share" of the TtTRF subfolder (e.g. link them  under your favourite run folder ).
   
   Efficiencies
   ------------

   Efficiencies are derived using the script util/Get_eff_ttTRF.C

   Ntuples
   -------
   Ntuples are currently produced under three choices of parametrisations (when 2 variables are present, assuming 1D x 1D)

                      Description                                      Weight name                     Number of b-tagged jets in event
            ----------------------------------------------------------------------------------------------------------------------------
            a) Default: pt of the jet vs  dR_min^jet.                 weight_ttTRF                            nBTags_MV2c10_77
            b)  pt of the jet                                       weight_ttTRF_pt                         nBTags_MV2c10_77_pt
            c)  pt of the jet vs  <dR> within excluded jets.       weight_ttTRF_ptdRtj                     nBTags_MV2c10_77_ptdRtj
  
   Ntuples characterised by the string "allpar" alone contains these last variables, where the evolution of efficiencies against nj is not taken into account. 
   Ntuples characterised by the string "allparNjdist",  contains these last variables, where the evolution of efficiencies against nj is taken into account, but
   the weight/variables names are kept the same, for easing the way they can be compared with the previous set of (non corrected) variables. 


