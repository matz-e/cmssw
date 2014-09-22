# Getting the code

Get the right CMSSW release:

    cmsrel CMSSW_6_2_0_SLHC18
    cd CMSSW_6_2_0_SLHC18/src
    cmsenv

Get the topic from github:

    git cms-merge-topic matz-e:l1hcal
    scram b -j 8

**This branch being periodically rebased and forcefully updated.**

# Running it

This will most likely fail at the end, but the HCAL TP algo runs through
without any issues.  Verification of TP energies still to be done:

    cd SimCalorimetry/HcalTrigPrimAlgos/test
    cmsRun run_algo.py
