import os
import shutil
import glob
from pathlib import Path

# Quick Guide
# This script converts .png.txt files to .flxspritesheet files
# and copies the corresponding .png files to a new directory.
# 1. Download the Art folder from the Chrono_Drift Google Drive
# 2. Unzip the Art folder
# 3. Place this script in the Art folder
# 4. Open a terminal and navigate to the Art folder using the cd command
# 5. Change the "append" variable to the path of the spritesheets directory in your project
# 6. Run the script by typing "python convert.py" in the terminal
# 7. The script will create a new directory called "out" with the converted files, move the contents from the "out" directory to your project's "images/spritesheets" directory

# Settings
append = "/images/spritesheets/"  # Path to add to the beginning of each .flxspritesheet file

def process_files():
    out_dir = "out" # Output directory

    # Clean up the output directory
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)

    # Create output directory if it doesn't exist
    os.makedirs(out_dir, exist_ok=True)
    
    # Find all *.png.txt files recursively
    captured_values = []
    for txt_file in glob.glob("**/*.png.txt", recursive=True):
        # Extract the captured value (everything before .png.txt)
        captured_value = Path(txt_file).name[:-8]  # Remove .png.txt
        captured_values.append(captured_value)
        
        # Copy the .txt file to out directory with new extension
        new_filename = f"{captured_value}.flxspritesheet"
        shutil.copy2(txt_file, os.path.join(out_dir, new_filename))
        
        # Find and copy corresponding .png file
        png_file = txt_file[:-4]  # Remove .txt from the path
        if os.path.exists(png_file):
            shutil.copy2(png_file, os.path.join(out_dir, os.path.basename(png_file)))
    
    # Add the prefix to the beginning of each .flxspritesheet file
    for sheet_file in glob.glob(os.path.join(out_dir, "*.flxspritesheet")):
        with open(sheet_file, 'r') as f:
            content = f.read()
        
        captured_value = Path(sheet_file).stem  # Get filename without extension
        prefix = f"{append}{captured_value}.png "
        
        with open(sheet_file, 'w') as f:
            f.write(prefix + content)

if __name__ == "__main__":
    process_files()