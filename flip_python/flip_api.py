import argparse
import os
import flip
import cv2
import numpy as np


# Funktion zum Speichern des meanFLIPError in eine Textdatei
def save_mean_flip_error(mean_flip_error, results_folder, filename):
    output_path = os.path.join(results_folder, filename)
    with open(output_path, "w") as file:
        file.write(str(mean_flip_error))

def berechne_gradienten_gewichtungen(bild):
    # Konvertiere das Bild in Graustufen
    graustufen = cv2.cvtColor(bild, cv2.COLOR_BGR2GRAY)

    # Berechne Gradienten in x- und y-Richtung
    gradientX = cv2.Sobel(graustufen, cv2.CV_64F, 1, 0, ksize=3)
    gradientY = cv2.Sobel(graustufen, cv2.CV_64F, 0, 1, ksize=3)

    # Berechne die Magnitude des Gradienten
    gradientMagnitude = np.sqrt(gradientX**2 + gradientY**2)

    # Normalisiere den Gradienten auf den Bereich [0, 1]
    weightMap = gradientMagnitude / np.max(gradientMagnitude)

    return weightMap

def compare_all_testcases(root_folder):
    # Durchlaufen aller Unterordner im Stammverzeichnis
    for subdir in os.listdir(root_folder):
        subdir_path = os.path.join(root_folder, subdir)
        output_folder = os.path.join(subdir_path, "flip")
        os.makedirs(output_folder, exist_ok=True)
        if os.path.isdir(subdir_path):
            # Durchlaufen aller Dateien im Unterordner
            files = ["F_STAR.png", "F1.png", "F.png", "CV.png", "H1.png"]
            ref = os.path.join(subdir_path, "F_ACC.png")
            for filename in files:
                    # Vergleich der Bilder, wenn es sich um F.png, F_STAR.png oder F1.png handelt
                test = os.path.join(subdir_path, filename)
                try:
                    flipErrorMap, meanFLIPError, parameters = flip.evaluate(ref, test, "LDR")
                    if filename == "H1.png":
                        flipErrorMap, meanFLIPError, parameters = flip.evaluate(os.path.join(subdir_path, "CV.png"), test, "LDR")
                except RuntimeError as e:
                    print(f"Error evaluating FLIP for {filename}: {e}")
                    continue 
                save_png = filename.replace(".png", "_diff.png")
                scaled_flipErrorMap = (flipErrorMap **2 - flipErrorMap.min() **2) / (flipErrorMap.max() **2 - flipErrorMap.min() **2) * 255
                scaled_flipErrorMap = scaled_flipErrorMap.astype(np.uint8)
                # weighted_flipErrorMap = flipErrorMap
                cv2.imwrite(os.path.join(output_folder,save_png), scaled_flipErrorMap)
                # cv2.imwrite(os.path.join(output_folder,save_png), flipErrorMap)
                save_mfe = filename.replace(".png", "_mfe.txt")
                save_mean_flip_error(meanFLIPError, output_folder, save_mfe)
    
                        
if __name__ == '__main__':
    # Definition der Kommandozeilenargumente
    parser = argparse.ArgumentParser(description="Calculate FLIP error and save difference images for images in subfolders.")
    parser.add_argument("root_folder", type=str, help="Path to the root folder containing subfolders with images to compare.")
    parser.add_argument("-r", "--recursive", action="store_true", help="Setzen Sie dieses Flag, um die Schleife durchlaufen zu lassen.")
    

    # Parsing der Kommandozeilenargumente
    args = parser.parse_args()
    if args.recursive == True:
        for root, dirs, files in os.walk(args.root_folder):
            for dir in dirs:
                compare_all_testcases(os.path.join(root, dir))
    else:
        compare_all_testcases(args.root_folder)