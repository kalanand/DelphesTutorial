#!/bin/bash

i=${1}
delphes=${2}
card=${3}
mgOutput=${4}

scratch=${PWD}
scramv1 project CMSSW CMSSW_5_3_6
cd CMSSW_5_3_6/src
eval `scramv1 runtime -sh`

cp -r $delphes/* .

./DelphesSTDHEP $card $scratch/delphes.${i}.root ${mgOutput}/pythia_events.${i}.hep

