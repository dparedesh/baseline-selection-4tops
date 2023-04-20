import TopExamples.grid as grid
import TopExamples.ami

## COMMENTS
# AUTOMATIC COMMENT: DSID 411068 (ttbar_PhPy8OL_ttbb4F_AFII) a875 missing
# AUTOMATIC COMMENT: DSID 411069 (ttbar_PhPy8OL_ttbb4F_AFII) a875 missing
# AUTOMATIC COMMENT: DSID 410250 (ttbar_Sherpa221) s3126 missing
# AUTOMATIC COMMENT: DSID 410251 (ttbar_Sherpa221) s3126 missing
# AUTOMATIC COMMENT: DSID 410252 (ttbar_Sherpa221) s3126 missing
# AUTOMATIC COMMENT: DSID 412115 (tttt_MadGraphPy8LO_FS) a875 missing
# AUTOMATIC COMMENT: DSID 361100 (Vjets_PhPy8) s3126 missing
# AUTOMATIC COMMENT: DSID 361101 (Vjets_PhPy8) s3126 missing
# AUTOMATIC COMMENT: DSID 361102 (Vjets_PhPy8) s3126 missing
# AUTOMATIC COMMENT: DSID 361104 (Vjets_PhPy8) s3126 missing
#
#
#

grid.Add("mc16e_ttH_tttt").datasets = [
	 "mc16_13TeV.312440.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m400.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
	 "mc16_13TeV.312441.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m500.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
	 "mc16_13TeV.312442.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m600.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
	 "mc16_13TeV.312443.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m700.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
	 "mc16_13TeV.312444.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m800.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
	 "mc16_13TeV.312445.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m900.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
	 "mc16_13TeV.312446.MadGraphPythia8EvtGen_noallhad_ttH_tttt_m1000.deriv.DAOD_TOPQ1.e7743_a875_r10724_p4031",
]

grid.Add("mc16e_tttt_aMcAtNloPy8_AFII").datasets = [
	 "mc16_13TeV.412043.aMcAtNloPythia8EvtGen_A14NNPDF31_SM4topsNLO.deriv.DAOD_TOPQ1.e7101_a875_r10724_p4174",
]

grid.Add("mc16e_tttt_MadGraphPy8LO_NNPDF31_AFII").datasets = [
    "mc16_13TeV.412115.MadGraphPythia8EvtGen_A14NNPDF31_SM4topsLO.deriv.DAOD_TOPQ1.e7424_a875_r10724_p4174",
]

grid.Add("mc16e_tttt_Sherpa2210_FS").datasets = [
	 "mc16_13TeV.700046.Sh_2210_tttt.deriv.DAOD_TOPQ1.e8253_s3126_r10724_p4174",
	 "mc16_13TeV.700075.Sh_2210_tttt_muQHT2.deriv.DAOD_TOPQ1.e8253_s3126_r10724_p4174",
]

grid.Add("mc16e_tttt_aMcAtNloHerwig7_AFII").datasets = [
	 "mc16_13TeV.412044.aMcAtNloHerwig7EvtGen_H7UE_SM4topsNLO.deriv.DAOD_TOPQ1.e7102_a875_r10724_p4174",
]

grid.Add("mc16e_ttWNLO").datasets = [
	 "mc16_13TeV.413008.Sherpa_221_NN30NNLO_ttW_multilegNLO.deriv.DAOD_TOPQ1.e7286_s3126_r10724_p4174",
]

grid.Add("mc16e_ttWSh228").datasets = [
	 "mc16_13TeV.700000.Sh_228_ttW.deriv.DAOD_TOPQ1.e7793_s3126_r10724_p4174",
]

grid.Add("mc16e_ttWSh2210").datasets = [
	 "mc16_13TeV.700168.Sh_2210_ttW.deriv.DAOD_TOPQ1.e8273_s3126_r10724_p4174",
]

grid.Add("mc16e_ttW").datasets = [
	 "mc16_13TeV.410155.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttW.deriv.DAOD_TOPQ1.e5070_s3126_r10724_p4174",
]

grid.Add("mc16e_ttH").datasets = [
	 "mc16_13TeV.346344.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_semilep.deriv.DAOD_TOPQ1.e7148_s3126_r10724_p4174",
	 "mc16_13TeV.346345.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_dilep.deriv.DAOD_TOPQ1.e7148_s3126_r10724_p4174",
]

grid.Add("mc16e_ttHMGPhy").datasets = [
	"mc16_13TeV.345940.aMcAtNloPythia8EvtGen_ttH_dilep.deriv.DAOD_TOPQ1.e6654_a875_r10724_p4174",
	"mc16_13TeV.345941.aMcAtNloPythia8EvtGen_ttH_semilep.deriv.DAOD_TOPQ1.e6654_a875_r10724_p4174",
	"mc16_13TeV.345942.aMcAtNloPythia8EvtGen_ttH_allhad.deriv.DAOD_TOPQ1.e6654_a875_r10724_p4031",

]

grid.Add("mc16e_ttZ").datasets = [
     "mc16_13TeV.410156.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttZnunu.deriv.DAOD_TOPQ1.e5070_s3126_r10724_p4174",
     "mc16_13TeV.410157.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttZqq.deriv.DAOD_TOPQ1.e5070_s3126_r10724_p4174",
     "mc16_13TeV.410218.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee.deriv.DAOD_TOPQ1.e5070_s3126_r10724_p4174",
     "mc16_13TeV.410219.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu.deriv.DAOD_TOPQ1.e5070_s3126_r10724_p4174",
     "mc16_13TeV.410220.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau.deriv.DAOD_TOPQ1.e5070_s3126_r10724_p4174",
]

grid.Add("mc16e_ttll").datasets = [
     "mc16_13TeV.410276.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee_mll_1_5.deriv.DAOD_TOPQ1.e6087_s3126_r10724_p4174",
     "mc16_13TeV.410277.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu_mll_1_5.deriv.DAOD_TOPQ1.e6087_s3126_r10724_p4174",
     "mc16_13TeV.410278.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau_mll_1_5.deriv.DAOD_TOPQ1.e6087_s3126_r10724_p4174",
]

grid.Add("mc16e_ttZSherpa").datasets = [
	"mc16_13TeV.410142.Sherpa_NNPDF30NNLO_ttll_mll5.deriv.DAOD_TOPQ1.e4686_s3126_r10724_p4174",
]

grid.Add("mc16e_ttbar").datasets = [
     "mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e6337_s3126_r10724_p4174",
     "mc16_13TeV.407342.PhPy8EG_A14_ttbarHT1k5_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e6414_s3126_r10724_p4062",
     "mc16_13TeV.407343.PhPy8EG_A14_ttbarHT1k_1k5_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e6414_s3126_r10724_p4062",
     "mc16_13TeV.407344.PhPy8EG_A14_ttbarHT6c_1k_hdamp258p75_nonallhad.deriv.DAOD_TOPQ1.e6414_s3126_r10724_p4062",
]

grid.Add("mc16e_SingleTop").datasets = [
     "mc16_13TeV.410658.PhPy8EG_A14_tchan_BW50_lept_top.deriv.DAOD_TOPQ1.e6671_s3126_r10724_p4174",
     "mc16_13TeV.410659.PhPy8EG_A14_tchan_BW50_lept_antitop.deriv.DAOD_TOPQ1.e6671_s3126_r10724_p4174",
     "mc16_13TeV.410646.PowhegPythia8EvtGen_A14_Wt_DR_inclusive_top.deriv.DAOD_TOPQ1.e6552_s3126_r10724_p4174",
     "mc16_13TeV.410647.PowhegPythia8EvtGen_A14_Wt_DR_inclusive_antitop.deriv.DAOD_TOPQ1.e6552_s3126_r10724_p4174",
     "mc16_13TeV.410644.PowhegPythia8EvtGen_A14_singletop_schan_lept_top.deriv.DAOD_TOPQ1.e6527_s3126_r10724_p4174",
     "mc16_13TeV.410645.PowhegPythia8EvtGen_A14_singletop_schan_lept_antitop.deriv.DAOD_TOPQ1.e6527_s3126_r10724_p4174",
     "mc16_13TeV.410560.MadGraphPythia8EvtGen_A14_tZ_4fl_tchan_noAllHad.deriv.DAOD_TOPQ1.e5803_s3126_r10724_p4174",
     "mc16_13TeV.410408.aMcAtNloPythia8EvtGen_tWZ_Ztoll_minDR1.deriv.DAOD_TOPQ1.e6423_s3126_r10724_p4174",
]

grid.Add("mc16e_ttt").datasets = [
     "mc16_13TeV.304014.MadGraphPythia8EvtGen_A14NNPDF23_3top_SM.deriv.DAOD_TOPQ1.e4324_s3126_r10724_p4174",
]

grid.Add("mc16e_ttWW").datasets = [
     "mc16_13TeV.410081.MadGraphPythia8EvtGen_A14NNPDF23_ttbarWW.deriv.DAOD_TOPQ1.e4111_s3126_r10724_p4174",
]

grid.Add("mc16e_ttVV").datasets = [
         "mc16_13TeV.500460.MGPy8EG_A14NNPDF23LO_ttHH.deriv.DAOD_TOPQ1.e8041_a875_r10724_p4174",
         "mc16_13TeV.500461.MGPy8EG_A14NNPDF23LO_ttWH.deriv.DAOD_TOPQ1.e8041_a875_r10724_p4174",
         "mc16_13TeV.500462.MGPy8EG_A14NNPDF23LO_ttZZ.deriv.DAOD_TOPQ1.e8041_a875_r10724_p4174",
         "mc16_13TeV.500463.MGPy8EG_A14NNPDF23LO_ttWZ.deriv.DAOD_TOPQ1.e8041_a875_r10724_p4174",
]

grid.Add("mc16e_VH").datasets = [
	 "mc16_13TeV.342284.Pythia8EvtGen_A14NNPDF23LO_WH125_inc.deriv.DAOD_TOPQ1.e4246_s3126_r10724_p4172",
	 "mc16_13TeV.342285.Pythia8EvtGen_A14NNPDF23LO_ZH125_inc.deriv.DAOD_TOPQ1.e4246_s3126_r10724_p4172",
]

grid.Add("mc16e_Vjets_Sherpa221").datasets = [
	 "mc16_13TeV.364170.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV0_70_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364171.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364172.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364173.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV70_140_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364174.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV70_140_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364175.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV70_140_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364176.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364177.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV140_280_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364178.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV140_280_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364179.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV280_500_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364180.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV280_500_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364181.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV280_500_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364182.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV500_1000.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364183.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV1000_E_CMS.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364156.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV0_70_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364157.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364158.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364159.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV70_140_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364160.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV70_140_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364161.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV70_140_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364162.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364163.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV140_280_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364164.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV140_280_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364165.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV280_500_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364166.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV280_500_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364167.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV280_500_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364168.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV500_1000.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364169.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV1000_E_CMS.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364184.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV0_70_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364185.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364186.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364187.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV70_140_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364188.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV70_140_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364189.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV70_140_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364190.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364191.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV140_280_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364192.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV140_280_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364193.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV280_500_CVetoBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364194.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV280_500_CFilterBVeto.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364195.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV280_500_BFilter.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364196.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV500_1000.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364197.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV1000_E_CMS.deriv.DAOD_TOPQ1.e5340_s3126_r10724_p4172",
	 "mc16_13TeV.364114.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV0_70_CVetoBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364115.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364116.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364117.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV70_140_CVetoBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364118.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV70_140_CFilterBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364119.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV70_140_BFilter.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364120.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364121.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV140_280_CFilterBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364122.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV140_280_BFilter.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364123.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV280_500_CVetoBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364124.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV280_500_CFilterBVeto.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364125.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV280_500_BFilter.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364126.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV500_1000.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364127.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV1000_E_CMS.deriv.DAOD_TOPQ1.e5299_s3126_r10724_p4172",
	 "mc16_13TeV.364100.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV0_70_CVetoBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364101.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364102.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364103.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV70_140_CVetoBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364104.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV70_140_CFilterBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364105.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV70_140_BFilter.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364106.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364107.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV140_280_CFilterBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364108.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV140_280_BFilter.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364109.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV280_500_CVetoBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364110.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV280_500_CFilterBVeto.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364111.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV280_500_BFilter.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364112.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV500_1000.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364113.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV1000_E_CMS.deriv.DAOD_TOPQ1.e5271_s3126_r10724_p4172",
	 "mc16_13TeV.364128.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV0_70_CVetoBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364129.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364130.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364131.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV70_140_CVetoBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364132.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV70_140_CFilterBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364133.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV70_140_BFilter.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364134.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364135.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV140_280_CFilterBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364136.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV140_280_BFilter.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364137.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV280_500_CVetoBVeto.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364138.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV280_500_CFilterBVeto.deriv.DAOD_TOPQ1.e5313_s3126_r10724_p4172",
	 "mc16_13TeV.364139.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV280_500_BFilter.deriv.DAOD_TOPQ1.e5313_s3126_r10724_p4172",
	 "mc16_13TeV.364140.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV500_1000.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364141.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV1000_E_CMS.deriv.DAOD_TOPQ1.e5307_s3126_r10724_p4172",
	 "mc16_13TeV.364204.Sherpa_221_NN30NNLO_Zee_Mll10_40_MAXHTPTV0_70_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364205.Sherpa_221_NN30NNLO_Zee_Mll10_40_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364206.Sherpa_221_NN30NNLO_Zee_Mll10_40_MAXHTPTV70_280_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364207.Sherpa_221_NN30NNLO_Zee_Mll10_40_MAXHTPTV70_280_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364208.Sherpa_221_NN30NNLO_Zee_Mll10_40_MAXHTPTV280_E_CMS_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364209.Sherpa_221_NN30NNLO_Zee_Mll10_40_MAXHTPTV280_E_CMS_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364198.Sherpa_221_NN30NNLO_Zmm_Mll10_40_MAXHTPTV0_70_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364199.Sherpa_221_NN30NNLO_Zmm_Mll10_40_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364200.Sherpa_221_NN30NNLO_Zmm_Mll10_40_MAXHTPTV70_280_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364201.Sherpa_221_NN30NNLO_Zmm_Mll10_40_MAXHTPTV70_280_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364202.Sherpa_221_NN30NNLO_Zmm_Mll10_40_MAXHTPTV280_E_CMS_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364203.Sherpa_221_NN30NNLO_Zmm_Mll10_40_MAXHTPTV280_E_CMS_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364210.Sherpa_221_NN30NNLO_Ztt_Mll10_40_MAXHTPTV0_70_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364211.Sherpa_221_NN30NNLO_Ztt_Mll10_40_MAXHTPTV0_70_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364212.Sherpa_221_NN30NNLO_Ztt_Mll10_40_MAXHTPTV70_280_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364213.Sherpa_221_NN30NNLO_Ztt_Mll10_40_MAXHTPTV70_280_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364214.Sherpa_221_NN30NNLO_Ztt_Mll10_40_MAXHTPTV280_E_CMS_BVeto.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
	 "mc16_13TeV.364215.Sherpa_221_NN30NNLO_Ztt_Mll10_40_MAXHTPTV280_E_CMS_BFilter.deriv.DAOD_TOPQ1.e5421_s3126_r10724_p4172",
]

grid.Add("mc16e_VV").datasets = [
	 "mc16_13TeV.364250.Sherpa_222_NNPDF30NNLO_llll.deriv.DAOD_TOPQ1.e5894_s3126_r10724_p4172",
	 "mc16_13TeV.364253.Sherpa_222_NNPDF30NNLO_lllv.deriv.DAOD_TOPQ1.e5916_s3126_r10724_p4172",
	 "mc16_13TeV.364254.Sherpa_222_NNPDF30NNLO_llvv.deriv.DAOD_TOPQ1.e5916_s3126_r10724_p4172",
	 "mc16_13TeV.364255.Sherpa_222_NNPDF30NNLO_lvvv.deriv.DAOD_TOPQ1.e5916_s3126_r10724_p4172",
	 "mc16_13TeV.364283.Sherpa_222_NNPDF30NNLO_lllljj_EW6.deriv.DAOD_TOPQ1.e6055_s3126_r10724_p4172",
	 "mc16_13TeV.364284.Sherpa_222_NNPDF30NNLO_lllvjj_EW6.deriv.DAOD_TOPQ1.e6055_s3126_r10724_p4172",
	 "mc16_13TeV.364285.Sherpa_222_NNPDF30NNLO_llvvjj_EW6.deriv.DAOD_TOPQ1.e6055_s3126_r10724_p4172",
	 "mc16_13TeV.364287.Sherpa_222_NNPDF30NNLO_llvvjj_ss_EW6.deriv.DAOD_TOPQ1.e6055_s3126_r10724_p4172",
	 "mc16_13TeV.364288.Sherpa_222_NNPDF30NNLO_llll_lowMllPtComplement.deriv.DAOD_TOPQ1.e6096_s3126_r10724_p4172",
	 "mc16_13TeV.364289.Sherpa_222_NNPDF30NNLO_lllv_lowMllPtComplement.deriv.DAOD_TOPQ1.e6133_s3126_r10724_p4172",
	 "mc16_13TeV.364290.Sherpa_222_NNPDF30NNLO_llvv_lowMllPtComplement.deriv.DAOD_TOPQ1.e6096_s3126_r10724_p4172",
	 "mc16_13TeV.345705.Sherpa_222_NNPDF30NNLO_ggllll_0M4l130.deriv.DAOD_TOPQ1.e6213_s3126_r10724_p4172",
	 "mc16_13TeV.345706.Sherpa_222_NNPDF30NNLO_ggllll_130M4l.deriv.DAOD_TOPQ1.e6213_s3126_r10724_p4172",
	 "mc16_13TeV.345723.Sherpa_222_NNPDF30NNLO_ggllvvZZ.deriv.DAOD_TOPQ1.e6213_s3126_r10724_p4172",
	 "mc16_13TeV.363355.Sherpa_221_NNPDF30NNLO_ZqqZvv.deriv.DAOD_TOPQ1.e5525_s3126_r10724_p4172",
	 "mc16_13TeV.363356.Sherpa_221_NNPDF30NNLO_ZqqZll.deriv.DAOD_TOPQ1.e5525_s3126_r10724_p4172",
	 "mc16_13TeV.363357.Sherpa_221_NNPDF30NNLO_WqqZvv.deriv.DAOD_TOPQ1.e5525_s3126_r10724_p4172",
	 "mc16_13TeV.363358.Sherpa_221_NNPDF30NNLO_WqqZll.deriv.DAOD_TOPQ1.e5525_s3126_r10724_p4172",
	 "mc16_13TeV.363359.Sherpa_221_NNPDF30NNLO_WpqqWmlv.deriv.DAOD_TOPQ1.e5583_s3126_r10724_p4172",
	 "mc16_13TeV.363360.Sherpa_221_NNPDF30NNLO_WplvWmqq.deriv.DAOD_TOPQ1.e5983_s3126_r10724_p4172",
	 "mc16_13TeV.363489.Sherpa_221_NNPDF30NNLO_WlvZqq.deriv.DAOD_TOPQ1.e5525_s3126_r10724_p4172",
	 "mc16_13TeV.363494.Sherpa_221_NNPDF30NNLO_vvvv.deriv.DAOD_TOPQ1.e5332_s3126_r10724_p4172",
]

grid.Add("mc16e_VVV").datasets = [
	 "mc16_13TeV.364242.Sherpa_222_NNPDF30NNLO_WWW_3l3v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364243.Sherpa_222_NNPDF30NNLO_WWZ_4l2v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364244.Sherpa_222_NNPDF30NNLO_WWZ_2l4v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364245.Sherpa_222_NNPDF30NNLO_WZZ_5l1v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364246.Sherpa_222_NNPDF30NNLO_WZZ_3l3v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364247.Sherpa_222_NNPDF30NNLO_ZZZ_6l0v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364248.Sherpa_222_NNPDF30NNLO_ZZZ_4l2v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
	 "mc16_13TeV.364249.Sherpa_222_NNPDF30NNLO_ZZZ_2l4v_EW6.deriv.DAOD_TOPQ1.e5887_s3126_r10724_p4172",
]
