
variable=$1
variable2=$2
variable3=$3

echo $variable
echo $variable2
echo $variable3

for j in $(echo $variable2 | sed "s/,/ /g")
do
    for i in $(echo $variable | sed "s/,/ /g")
    do
	rucio list-dids group.phys-exotics.$i*HBSM4t-212120*$j*1LOS${variable3}v*_output_root
    done
done
