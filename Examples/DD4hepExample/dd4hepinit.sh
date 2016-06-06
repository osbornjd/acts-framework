# later implement that used only when needed
# Root needed for geoDisplay
source /afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/bin/thisroot.sh

# DD4hep needed for DD4hep Plugin and example - decouple later
export inithere=$PWD
cd /afs/cern.ch/user/j/jhrdinka/DD4hep/
source cmake/thisdd4hep.sh
#cd  /afs/cern.ch/exp/fcc/sw/0.5/DD4hep/20152711/x86_64-slc6-gcc49-opt
#source bin/thisdd4hep.sh
cd $inithere

export LD_LIBRARY_PATH=/afs/cern.ch/work/j/jhrdinka/ACTS/ACTS/a-common-tracking-sw/build/installed/lib:/afs/cern.ch/work/j/jhrdinka/ACTS/ACTS/a-common-tracking-sw/build/installed/lib64:/afs/cern.ch/user/j/jhrdinka/DD4hep/lib:$LD_LIBRARY_PATH


#needed for detector building in DD4hep
export LD_LIBRARY_PATH=$ACTFW/build/installed/lib:$LD_LIBRARY_PATH