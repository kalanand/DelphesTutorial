#!/bin/bash

i=${1}
mg=${2}

scratch=${PWD}
scramv1 project CMSSW CMSSW_5_3_6
cd CMSSW_5_3_6/src
eval `scramv1 runtime -sh`

cp -r $mg/* .
sed s/SEED/${RANDOM}/ ttbar/Cards/run_card.dat --in-place

ttbar/bin/generate_events --laststep=pythia -f

#mv ttbar/Events/run_01/unweighted_events.lhe.gz ${scratch}/madgraph_events.${i}.lhe.gz
mv ttbar/Events/run_01/tag_1_pythia_events.hep.gz  ${scratch}/pythia_events.${i}.hep.gz
gunzip ${scratch}/pythia_events.${i}.hep.gz