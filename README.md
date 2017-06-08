MIT CCG Worm Reconstruction

Uses parallel Dijkstra algorithm to segment image.

requirements:
    probability images
        must be 1024x1024
        named as <probDir>/<z>/<y_index>_<x_index>.png

MAIN COMPONENTS
worm
    Binary that performs segmentation
    Use "make" to compile
    run ./worm --help to see usage of the binary
    Highlighted options
        Option to output seed image
        Option to output grayscale image of each pixel's distance from the closest seed
        Option to synthesize the whole probability map and output it
        Option to output seed image either as one big image, or as many smaller ones
        Configurable directories for inputs and outputs


z\_recon
    One level higher: used to run the binary on specified layers of the worm
    Creates a directory corresponding to the specific z layer prior to running the algorithm and producing output images
    Options
        datadir: where the output should be stored
        logdir: location of a log to write to



