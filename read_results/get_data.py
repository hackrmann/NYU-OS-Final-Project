import os
import json

# Get the list of all files and directories
path = "/home/thejus/Desktop/OS/final/read_results/"
dir_list = os.listdir(path)

# prints all files
# print(dir_list)

def get_lines(filepath):
    with open(filepath) as f:
        contents = f.read()
    return contents.split("\n")

data = {"block_size": 0, "number_blocks":0, "size":0, "time":0, "rate":0, "xor":0}
experiment = {}
for file in dir_list:
    experiment[file] = []
    if "txt" in file:
        contents = get_lines(path+"/"+file)
        # print(contents)
        for line in contents:
            lin = line.split(" ")
            # print("lin", lin)
            if len(lin)>3:
                if lin[0] == "block_size":
                    data["block_size"] = lin[2]
                elif lin[0] == "Number":
                    data["number_blocks"] = lin[-2]
                elif lin[0] == "Size":
                    data["size"] = lin[-2]
                elif lin[0] == "Time":
                    data["time"] = lin[-2]
                elif lin[0] == "Rate":
                    data["rate"] = lin[-1][:len(lin[-1])-7]
                elif lin[0] == "Xor":
                    data["xor"] = lin[-1]
                    experiment[file].append(data.copy())
# print(experiment)
json_object = json.dumps(experiment, indent = 4)
print(json_object)

for i in experiment:
    try:
        print(i,"-",experiment[i][0]["size"],"MB")
        x = experiment[i]
        block_sizes = [i["block_size"] for i in x]
        rates = [i["rate"] for i in x]
        print("block sizes:")
        for k in block_sizes:
            print(k)
        print("rates---")
        for k in rates:
            print(k)
        print("-------------------------")
    except:
        pass
