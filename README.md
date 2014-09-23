# Getting the code

Get the right CMSSW release:

    cmsrel CMSSW_6_2_0_SLHC19
    cd CMSSW_6_2_0_SLHC19/src
    cmsenv

Get the topic from github:

    git cms-merge-topic matz-e:l1hcal
    scram b -j 8

**This branch being periodically rebased and forcefully updated.**

# Running it

This will most likely fail at the end, but the HCAL TP algo runs through
without any issues.  Verification of TP energies still to be done:

    cmsRun SimCalorimetry/HcalTrigPrimAlgos/test/run_algo.py

# Changes

The TP algo will now produce `HcalUpgradeTriggerPrimitives` found in

    DataFormats/HcalDigi/interface/HcalUpgradeTriggerPrimitiveDigi.h
    DataFormats/HcalDigi/src/HcalUpgradeTriggerPrimitiveDigi.cc

which have been resurrected from CVS.  Currently, the method
`SOI_depth_linear` takes a depth level (0 - 4) and returns depth
information in linearized ADC counts.
