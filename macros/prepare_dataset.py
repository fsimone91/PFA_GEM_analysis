#!/bin/env python2
import sys
import os
import argparse

import pickle
import root_pandas
import numpy            as np
import pandas           as pd
import matplotlib.cm    as cm
import root_numpy
import matplotlib.pyplot as plt

input_file = [
    '/lustre/cms/store/user/gmilella/ExpressCosmics/CRAB3_gem_dpg_ntuple_mwgr2_2021_run340220_express/210306_100351/0000/MuDPGNTuple_MWGR2_2021_EXP_run340220_1.root',
]

#branches to be taken for analysis
main_branches = [
    'event_runNumber',
    'event_eventNumber',
    'event_lumiBlock',
    'gemRecHit_nRecHits',
    'gemRecHit_cluster_size',
    'gemRecHit_region',
    'gemRecHit_chamber',
    'gemRecHit_layer',
]

#main ntuples
ntuple = pd.DataFrame( root_numpy.root2array(input_file  , 'muNtupleProducer/MuDPGTree', branches = main_branches) )

print(ntuple)

ntuple_lumi = ntuple.groupby(['event_lumiBlock', 'gemRecHit_region'])
print(ntuple_lumi)


#data_common = pd.merge(UL, rereco, on=['run', 'lumi', 'evt'])
#data_common['massreso_diff'] = data_common.tripletMassReso_x - data_common.tripletMassReso_y
#data_common['chi2_diff'] = data_common.fv_nC_x - data_common.fv_nC_y
#print(data_common)
#
#fig, axs = plt.subplots(2)
#data_common.hist(column='chi2_diff', bins = 100, grid=False, figsize=(20,20), color='#11bf91', zorder=2, rwidth=0.9, ax=axs[0])
#axs[0].set_yscale('log')
#axs[0].set_xlabel('fv_nC (UL - rereco)', horizontalalignment='left')
#data_common.hist(column='massreso_diff', bins = 100, grid=False, figsize=(20,20), color='#86bf91', zorder=2, rwidth=0.9, ax=axs[1])
#axs[1].set_yscale('log')
#axs[1].set_xlabel('m3m_reso (UL - rereco) (GeV)', horizontalalignment='left')
#fig.suptitle('common events')
#fig.savefig('common.png')
