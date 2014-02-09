#!/bin/bash



for i in `seq 25`; do
    cat > condor/MadGraph.${i}.job <<EOF
universe = vanilla
Executable = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/MadGraph.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Output = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/MadGraph.${i}.out
Error = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/MadGraph.${i}.err
Log = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/MadGraph.${i}.log
Notification = Never
Arguments = ${i} ${CMSSW_BASE}/src/DelphesTutorial/MadGraph

Queue 1
EOF
    
    cd MadGraphOutput
    condor_submit ../condor/MadGraph.${i}.job
    cd -
done
    