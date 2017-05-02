import os

initialDirectory = "/home/rajeev/worm/skeletons/"
newDirectory = "/home/rajeev/worm/skeleton/"

# format of input is: x y z radius nucleus seed_id parent_id

radiusExists = 0
with open(newDirectory + "new_seeds.txt", "w") as f:
    for filename in os.listdir(initialDirectory):
        if filename == "README":
            continue
        print filename
        seed = filename[:len(filename) - 4]
        with open(initialDirectory + filename, "r") as g:
            for line in g:
                line_ints = [int(round(float(each))) for each in line.split()]

                coords = line_ints[:3]
                coords[0] = coords[0] / 2
                coords[1] = coords[1] / 2
                coords[2] = coords[2] / 30
                coords = [str(each) for each in coords]
                coords = [seed] + coords

                rad = 7
                if line_ints[3] != -1:
                    rad = line_ints[3]/2
                    radiusExists += 1
                coords.append(str(rad))

                f.write(" ".join(coords) + "\n")

print radiusExists
