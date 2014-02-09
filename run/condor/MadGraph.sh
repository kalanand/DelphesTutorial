#!/bin/bash

n=${1}
mg=${2}

sed s/SEED/${RANDOM}/ ${mg}/Cards/run_card.dat

./${mg}/bin/generate_events --laststep=parton -f

mv ${mg}/Events/run_01/unweighted_events.lhe.gz  events.{n}.lhe.gz