## Example to run the weight calculation to successfully combine ttbar samples (HF Filtered, HT sliced, nominal).

After successfully building and setting up the offline tool, you will need to designate a list of input files that you want to run over. This file should contain the full path and different files should be separated by a newline. Given that you called the file `inputs.txt`, you can from this directory, i.e. `postProd/Examples/SSML_ttbarCombinations` run:

```
SM4t-offline config.txt inputs.txt
```

which should create a `test_dir` in this directory which should in turn contain your output root file. To understand what you can change, please have a look at the `config.txt`, where especially the last line is interesting. In order to alter the map between ttbar type and dsids, have a look at `Framework/OffSM4topsTool/share/ttbarStitchingDsidMap.json`.
