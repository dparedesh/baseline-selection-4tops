if [[ "$5"=="m$5" ]]
then
  savetree=false
else
  savetree=$5
fi
root -b -l <<EOF
.include ./
.L util/Get_ttTRF_2018_C.so
Get_ttTRF_2018("$1","$2","$3", false, true,"$4","${savetree}")
.q
EOF
