#!/bin/bash

cd MadGraphOutput

for i in `seq 25`; do
    cat > MadGraph.${i}.job <<EOF
universe = vanilla
Executable = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/MadGraph.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Output = condor/MadGraph.${i}.out
Error = condor/MadGraph..${i}.err
Log = condor/MadGraph.${i}.log
Notification = Never
Arguments = ${i} ${CMSSW_BASE}/src/MadGraph

Queue 1
EOF
    
    condor_submit MadGraph.{i}.job

done
    