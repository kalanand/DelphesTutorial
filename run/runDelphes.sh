#!/bin/bash

if [[ "$#" -ne 1 ]]; then
    echo "You forgot to specify a detector card."
    exit
fi

card=${1}

for i in `seq 25`; do
    cat > condor/Delphes.${i}.job <<EOF
universe = vanilla
Executable = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/Delphes.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Output = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/Delphes.${i}.out
Error = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/Delphes.${i}.err
Log = ${CMSSW_BASE}/src/DelphesTutorial/run/condor/Delphes.${i}.log
Notification = Never
Arguments = ${i} ${CMSSW_BASE}/src/DelphesTutorial/Delphes-3.0.12 ${PWD}/${card} ${CMSSW_BASE}/src/DelphesTutorial/run/MadGraphOutput

Queue 1
EOF
    
    cd DelphesOutput
    condor_submit ../condor/Delphes.${i}.job
    cd -
done
    