# Libraries
import os
import requests
import tqdm


def download_wiki_dumps(files_to_download, download_num, DUMP_URL):
    # Check if directory to store files exists & create it if it doesn't
    original_dir = os.path.dirname(__file__)
    parent_dir = os.path.dirname(original_dir)
    main_dir = os.path.dirname(parent_dir)
    check_dir = main_dir + "/data/load/Articles-p/"
    new_dir = main_dir + "/data/load/Wiki_Dumps"
    if not os.path.isdir(new_dir):
        os.mkdir(new_dir)
    # Change directory to the new one
    os.chdir(new_dir)

    already_downloaded = 0

    os.system("clear")
    print(f"Downloading all of Wikipedia in {download_num} segments...\nIf already downloaded, it won't download repeats again\n")
    for file in tqdm.tqdm(files_to_download):
        downloaded = False
        # Break & download the files if one of them is missing
        if not os.path.isfile(file):
            # Check if the file is already downloaded and parsed into JSON
            if os.path.isdir(check_dir):
                for check in os.listdir(check_dir):
                    if file.split('-')[-1].split('.')[-2] in check:
                        already_downloaded += 1
                        downloaded = True
                        break
            if not downloaded:
                url = DUMP_URL + file
                file_name = file

                # Download the file in 50 MB chunks
                download = requests.get(url, stream=True)
                with open(file_name, "wb") as bz2:
                    # 50 MB
                    for chunk in download.iter_content(chunk_size=1024*1024*50):
                        if chunk:
                            bz2.write(chunk)
        else:
            already_downloaded += 1

    if already_downloaded == len(files_to_download):
        os.system("clear")
        print("All files have already been downloaded!")
    else:
        print("\nAll files have been downloaded\n")

    os.chdir(original_dir)
    return