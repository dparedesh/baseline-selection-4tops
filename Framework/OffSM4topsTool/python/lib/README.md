# Offline production list

`listMaker.py` is used to prepare the list for the post-production. To prepare the list, you can run the following commands. Please remember to replace the path for `--mainfolder` by your path of the online samples.

 nominal offline production
```bash
python listMaker.py  --tag HBSM4t-212120 -c mc16a -s nominal --channel SSML --mainfolder /lustre/umt3/user/metsai/analysis/4tops/samples/common-fw_tag212120/online/ --preprod_file ProdList_ForPostProduction/mc16a_SM4tops_ForPostProduction.py
python listMaker.py  --tag HBSM4t-212120 -c mc16d -s nominal --channel SSML --mainfolder /lustre/umt3/user/metsai/analysis/4tops/samples/common-fw_tag212120/online/ --preprod_file ProdList_ForPostProduction/mc16d_SM4tops_ForPostProduction.py
python listMaker.py  --tag HBSM4t-212120 -c mc16e -s nominal --channel SSML --mainfolder /lustre/umt3/user/metsai/analysis/4tops/samples/common-fw_tag212120/online/ --preprod_file ProdList_ForPostProduction/mc16e_SM4tops_ForPostProduction.py
```
# syst offline production
```bash
python listMaker.py  --tag HBSM4t-212120 -c mc16a -s syst --channel SSML --mainfolder /lustre/umt3/user/metsai/analysis/4tops/samples/common-fw_tag212120/online/ --preprod_file ProdList_ForPostProduction/mc16a_SM4tops_ForPostProduction.py
python listMaker.py  --tag HBSM4t-212120 -c mc16d -s syst --channel SSML --mainfolder /lustre/umt3/user/metsai/analysis/4tops/samples/common-fw_tag212120/online/ --preprod_file ProdList_ForPostProduction/mc16d_SM4tops_ForPostProduction.py
python listMaker.py  --tag HBSM4t-212120 -c mc16e -s syst --channel SSML --mainfolder /lustre/umt3/user/metsai/analysis/4tops/samples/common-fw_tag212120/online/ --preprod_file ProdList_ForPostProduction/mc16e_SM4tops_ForPostProduction.py
```
