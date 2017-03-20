import h5py
import numpy as np
import os

def convert(prefix):
    f = h5py.File(matDirectory + prefix + ".mat")
    f = f['Buffer']
    np.savetxt(txtDirectory + prefix + ".txt", f, delimiter = ' ', fmt = "%d")

matDirectory = "/Users/Rajeev/Documents/worm/data/probs_by_z/"
txtDirectory = "/Users/Rajeev/Documents/worm/data/probs_txt/"

for filename in os.listdir(matDirectory):
    prefix = filename[:len(filename) - 4]
    convert(prefix)
