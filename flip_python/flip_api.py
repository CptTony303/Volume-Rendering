import argparse
import os
import flip
import cv2
import numpy as np

def compare_images(reference_path, test_path):
    # Berechnung des FLIP-Fehlers und der Parameter
    flipErrorMap, meanFLIPError, parameters = flip.evaluate(reference_path, test_path, "LDR")

    # Berechnung des MSE
    mse = meanFLIPError ** 2

    # Speichern des MSE in einer Datei
    mse_file = test_path.replace(".png", "_mse.txt")
    with open(mse_file, "w") as f:
        f.write(str(mse))

    # Speichern des Differenzbildes
    diff_image_path = test_path.replace(".png", "_diff.png")
    flip.save(diff_image_path, flipErrorMap)


# Funktion zum Speichern des meanFLIPError in eine Textdatei
def save_mean_flip_error(mean_flip_error, scaled_mse, results_folder, filename):
    output_path = os.path.join(results_folder, filename)
    mse = mean_flip_error ** 2
    with open(output_path, "w") as file:
        file.write("mean flip error"+str(mean_flip_error)+"\n")
        file.write("mse: "+str(mse)+"\n")
        file.write("scaled mse: "+str(scaled_mse)+"\n")
    print(f"Mean FLIP Error wurde in '{output_path}' gespeichert.")

if __name__ == '__main__':
    # Definition der Kommandozeilenargumente
    parser = argparse.ArgumentParser(description="Calculate FLIP error and save difference images for images in subfolders.")
    parser.add_argument("root_folder", type=str, help="Path to the root folder containing subfolders with images to compare.")

    # Parsing der Kommandozeilenargumente
    args = parser.parse_args()

    # Durchlaufen aller Unterordner im Stammverzeichnis
    for subdir in os.listdir(args.root_folder):
        subdir_path = os.path.join(args.root_folder, subdir)
        output_folder = os.path.join(subdir_path, "flip")
        os.makedirs(output_folder, exist_ok=True)
        if os.path.isdir(subdir_path):
            # Durchlaufen aller Dateien im Unterordner
            for filename in os.listdir(subdir_path):
                if filename.endswith(".png"):
                    # Vergleich der Bilder, wenn es sich um F.png, F_STAR.png oder F1.png handelt
                    if filename.startswith("F") and filename.endswith(".png"):
                        if(filename == "F_ACC.png"):
                            continue
                        ref = os.path.join(subdir_path, "F_ACC.png")
                        test = os.path.join(subdir_path, filename)
                        try:
                            flipErrorMap, meanFLIPError, parameters = flip.evaluate(ref, test, "LDR")
                        except RuntimeError as e:
                            print(f"Error evaluating FLIP for {filename}: {e}")
                            continue 
                        save_png = filename.replace(".png", "_diff.png")
                        mse = meanFLIPError ** 2
                        scaled_flipErrorMap = (flipErrorMap ** 2 - flipErrorMap.min() ** 2) / (flipErrorMap.max() ** 2 - flipErrorMap.min() ** 2) * 255
                        scaled_flipErrorMap = scaled_flipErrorMap.astype(np.uint8)
                        cv2.imwrite(os.path.join(output_folder,save_png), scaled_flipErrorMap)
                        save_mse = filename.replace(".png", "_mse.txt")
                        scaled_flipError = (meanFLIPError ** 2 - flipErrorMap.min() ** 2) / (flipErrorMap.max() ** 2 - flipErrorMap.min() ** 2)
                        save_mean_flip_error(meanFLIPError,scaled_flipError, output_folder, save_mse)
                        #print(f"FLIP error map saved to {output_path}")
                        