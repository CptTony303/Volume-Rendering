import os

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
    for file, values in mean_values.items():
        mean_value = sum(values) / len(values)
        mean_results[file] = mean_value

    return mean_results

def write_mean_values_to_file(mean_results, root_folder):
    # Create a new text file to store mean values
    output_file_path = os.path.join(root_folder, 'mean_values.txt')

    # Write mean values to the output file
    with open(output_file_path, 'w') as f:
        for file, mean_value in mean_results.items():
            f.write(f'{file}: {mean_value}\n')

if __name__ == '__main__':
    import sys

    # Check if the root folder path is provided as a command-line argument
    if len(sys.argv) < 2:
        print("Usage: python script.py <root_folder_path>")
        sys.exit(1)

    # Get the root folder path from command-line argument
    root_folder = sys.argv[1]

    # Compute mean values for files in 'flip' folders
    mean_results = compute_mean_value(root_folder)

    # Write mean values to a text file in the root folder
    write_mean_values_to_file(mean_results, root_folder)

    print("Mean values written to mean_values.txt in the root folder.")