import os
import numpy as np

diffs = []

def subtract_lists(input_dict, list1_key, list2_key):
    # Get the lists from the input dictionary based on the provided keys
    list1 = input_dict[list1_key]
    list2 = input_dict[list2_key]

    # Ensure the lists are of the same length
    assert len(list1) == len(list2), "Lists must be of the same length"

    # Subtract corresponding elements of the lists and create a new list of booleans
    diff_list = [1 if (list1[i] - list2[i] > 0) else 0 for i in range(len(list1))]
    # Create a list of indices where the difference is greater than 0
    index_list = [i for i in range(len(diff_list)) if diff_list[i] == 1]

    return diff_list, index_list, [list1[i] - list2[i] for i in range(len(list1))]

def compute_mean_value(folder_path):
    # Dictionary to store mean values for each file
    mean_values = {}

    # Traverse through the root folder
    for subdir, dirs, files in os.walk(folder_path):
        # Check if the current directory is a 'flip' folder
        if os.path.basename(subdir) == 'flip':
            # Process each file in the 'flip' folder
            for file in files:
                file_path = os.path.join(subdir, file)
                if file.endswith('.txt'):
                    # Read the float value from the file
                    with open(file_path, 'r') as f:
                        value = float(f.read().strip())

                    # Store the value in the dictionary based on file name
                    if file not in mean_values:
                        mean_values[file] = []
                    mean_values[file].append(value)

    # Calculate the mean for each file
    mean_results = {}
    var_results = {}
    for file, values in mean_values.items():
        mean_value = sum(values) / len(values)
        mean_results[file] = np.mean(mean_values[file])
        var_results[file] = np.var(mean_values[file])
    ereignisse = []
    ereignisse.append(subtract_lists(mean_values, 'F1_mfe.txt', 'F_mfe.txt'))
    ereignisse.append(subtract_lists(mean_values, 'F1_mfe.txt', 'F_STAR_mfe.txt'))
    ereignisse.append(subtract_lists(mean_values, 'F_mfe.txt', 'F_STAR_mfe.txt'))
    return (mean_results, var_results, ereignisse)

def write_mean_values_to_file(mean_results, var_results, ereignisse, root_folder):
    # Create a new text file to store mean values
    output_file_path = os.path.join(root_folder, 'mean_values.txt')

    # Write mean values to the output file
    with open(output_file_path, 'w') as f:
        for file, mean_value in mean_results.items():
            f.write(f'{file}: mean: {mean_value} - variance: {var_results[file]} \n')
        count = 0
        for results, idices, diffs in ereignisse:
            percentage = np.mean(results)
            diff_mean = np.mean(diffs)
            if(count == 0):
                f.write(f'Improvement with F: {percentage} \n')
            elif(count == 1):
                f.write(f'Improvement with F*: {percentage} \n')
            else:
                f.write(f'F worse than F*: {percentage} \n')
            f.write(f'mean of diffs: {diff_mean} \n')
            f.write(f'Indices: {idices} \n')
            count+=1

def call_util(root_folder):

    # Compute mean values for files in 'flip' folders
    mean_results, var_results, ereignisse = compute_mean_value(root_folder)

    # Write mean values to a text file in the root folder
    write_mean_values_to_file(mean_results, var_results, ereignisse, root_folder)

    print("Mean values written to mean_values.txt in the root folder.")
    
if __name__ == '__main__':
    import sys

    # Check if the root folder path is provided as a command-line argument
    if len(sys.argv) < 2:
        print("Usage: python script.py <root_folder_path>")
        sys.exit(1)
call_util(sys.argv[1])