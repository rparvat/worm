import os

initialDirectory = "/home/heather/worm/skeleton/"
newDirectory = "/home/rajeev/worm/skeleton/"

with open(newDirectory + "seeds.txt", "w") as f:
    for filename in os.listdir(initialDirectory):
        if filename == "README":
            continue
        print filename
        seed = filename[:len(filename) - 4]
        with open(initialDirectory + filename, "r") as g:
            for line in g:
                coords = [int(round(float(each))) for each in line.split()]
                coords[0] = coords[0] / 2
                coords[1] = coords[1] / 2
                coords[2] = coords[2] / 30
                coords = [str(each) for each in coords]
                coords = [seed] + coords
                f.write(" ".join(coords) + "\n")
