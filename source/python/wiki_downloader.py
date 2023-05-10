# Libraries
import os
from os.path import dirname, isdir, isfile
import requests
import tqdm


def download_wiki_dumps(FILES_TO_DOWNLOAD, DUMP_URL):
    # Check if directory to store files exists & create it if it doesn't
    """ ORIGINAL_DIR = dirname(__file__)
    PARENT_DIR = dirname(ORIGINAL_DIR)
    MAIN_DIR = dirname(PARENT_DIR) """
    MAIN_DIR = dirname(dirname(dirname(__file__)))
    CHECK_DIR = MAIN_DIR + "/data/load/Articles-p/"
    NEW_DIR = MAIN_DIR + "/data/load/Wiki_Dumps"
    if not isdir(NEW_DIR):
        os.mkdir(NEW_DIR)
    os.chdir(NEW_DIR)

    os.system("clear")
    print(f"Downloading all of Wikipedia in {len(FILES_TO_DOWNLOAD)} segments...")
    for FILE in tqdm.tqdm(FILES_TO_DOWNLOAD):
        downloaded = False
        # Download a file if it's missing
        if not isfile(FILE):
            # Check if the file is already downloaded and parsed into JSON
            if isdir(CHECK_DIR):
                for CHECK in os.listdir(CHECK_DIR):
                    if FILE.split('-')[-1].split('.')[-2] in CHECK:
                        downloaded = True
                        break
            if not downloaded:
                URL = DUMP_URL + FILE

                # Download the file in 50 MB chunks
                download = requests.get(URL, stream=True)
                with open(FILE, "wb") as bz2:
                    # 50 MB
                    for CHUNK in download.iter_content(chunk_size=1024*1024*50):
                        if CHUNK:
                            bz2.write(CHUNK)

    if len(FILES_TO_DOWNLOAD) == 0:
        os.system("clear")
        print("All files have already been downloaded!")
    else:
        print("\nAll files have been downloaded\n")

    os.chdir(MAIN_DIR)
    return