# this script is to setup the production when you don't have production role
# at this point we assume you did setupATLAS and setup AnalysisTop and TTHbbAnalysis
lsetup rucio panda pyami
voms-proxy-init -voms atlas
