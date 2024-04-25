import argparse
import os
import flip

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

if __name__ == '__main__':
    # Definition der Kommandozeilenargumente
    parser = argparse.ArgumentParser(description="Calculate FLIP error and save difference images for images in subfolders.")
    parser.add_argument("root_folder", type=str, help="Path to the root folder containing subfolders with images to compare.")

    # Parsing der Kommandozeilenargumente
    args = parser.parse_args()

    # Durchlaufen aller Unterordner im Stammverzeichnis
    for subdir in os.listdir(args.root_folder):
        subdir_path = os.path.join(args.root_folder, subdir)
        if os.path.isdir(subdir_path):
            # Durchlaufen aller Dateien im Unterordner
            for filename in os.listdir(subdir_path):
                if filename.endswith(".png"):
                    # Vergleich der Bilder, wenn es sich um F.png, F_STAR.png oder F1.png handelt
                    if filename.startswith("F") and filename.endswith(".png"):
                        reference_path = os.path.join(subdir_path, "F_ACC.png")
                        test_path = os.path.join(subdir_path, filename)
                        compare_images(reference_path, test_path)