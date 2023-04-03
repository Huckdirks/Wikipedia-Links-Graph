# Libraries
import os
import requests
import tqdm


def download_wiki_dumps(FILES_TO_DOWNLOAD, DOWNLOAD_NUM, DUMP_URL):
    # Check if directory to store files exists & create it if it doesn't
    ORIGINAL_DIR = os.path.dirname(__file__)
    PARENT_DIR = os.path.dirname(ORIGINAL_DIR)
    MAIN_DIR = os.path.dirname(PARENT_DIR)
    CHECK_DIR = MAIN_DIR + "/data/load/Articles-p/"
    NEW_DIR = MAIN_DIR + "/data/load/Wiki_Dumps"
    if not os.path.isdir(NEW_DIR):
        os.mkdir(NEW_DIR)
    # Change directory to the new one
    os.chdir(NEW_DIR)

    already_downloaded = 0

    os.system("clear")
    # I need to figure out how to only show how many files are left to download instead of the total number of files
    print(f"Downloading all of Wikipedia in {DOWNLOAD_NUM} segments...\nIf already downloaded, it won't download repeats again")
    for FILE in tqdm.tqdm(FILES_TO_DOWNLOAD):
        downloaded = False
        # Break & download the files if one of them is missing
        if not os.path.isfile(FILE):
            # Check if the file is already downloaded and parsed into JSON
            if os.path.isdir(CHECK_DIR):
                for CHECK in os.listdir(CHECK_DIR):
                    if FILE.split('-')[-1].split('.')[-2] in CHECK:
                        already_downloaded += 1
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
        else:
            already_downloaded += 1

    if already_downloaded == len(FILES_TO_DOWNLOAD):
        os.system("clear")
        print("All files have already been downloaded!")
    else:
        print("\nAll files have been downloaded\n")

    os.chdir(ORIGINAL_DIR)
    return