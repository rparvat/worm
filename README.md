#MIT CCG Worm Reconstruction

Uses parallel Dijkstra algorithm to segment image.

##Requirements:
###probability images
1. must be 1024x1024
1. named as <probDir>/<z>/<y_index>_<x_index>.png
###skeleton of worm
1. place all skeletons, as provided by Harvard, in ./skeletons/
1. run python ./newSeedsToTxt.py

##MAIN COMPONENTS
###worm
1. Binary that performs segmentation
1. Use "make" to compile
1. run ./worm --help to see usage of the binary
####Important options
    1. Option to output seed image
    1. Option to output grayscale image of each pixel's distance from the closest seed
    1. Option to synthesize the whole probability map and output it
    1. Option to output seed image either as one big image, or as many smaller ones
    1. Configurable directories for inputs and outputs


###z\_recon
1. One level higher: used to run the binary on specified layers of the worm
1. Creates a directory corresponding to the specific z layer prior to running the algorithm and producing output images
###Options
    1. datadir: where the inputs are stored
    1. logdir: location of a log to write to
    1. probdir: where the outputs should be written



