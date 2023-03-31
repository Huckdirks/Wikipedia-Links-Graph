# Libraries
import requests
import os
from bs4 import BeautifulSoup

# Other Modules
import Wiki_Downloader
import Multiprocessor


if __name__ == "__main__":
    # Get the HTML of the page
    DUMP_URL = "https://dumps.wikimedia.org/enwiki/latest/"
    dump_html = requests.get(DUMP_URL).text

    # Convert HTML to a soup
    soup_dump = BeautifulSoup(dump_html, "html.parser")

    # Search through all files & get the ones with "pages-articles" in the name
    files_to_download = []
    download_num = 0
    for file in soup_dump.find_all('a'):
        if "pages-articles" in file.get("href") and "xml-p" in file.get("href") and file.get("href").endswith(".bz2"):
            files_to_download.append(file.get("href"))
            download_num += 1

    # Change directory to Articles-p
    original_dir = os.path.dirname(__file__)
    parent_dir = os.path.dirname(original_dir)
    main_dir = os.path.dirname(parent_dir)
    check_dir = main_dir + "/data/load/Articles-p/"

    # Create data directories if they don't exist
    if not os.path.isdir(main_dir + "/data/"):
        os.mkdir(main_dir + "/data/")
    if not os.path.isdir(main_dir + "/data/load/"):
        os.mkdir(main_dir + "/data/load/")

    downloaded_num = 0
    # Check if all the files in Articles-p already exist, and exit if they do
    if os.path.isdir(check_dir):
        for file in files_to_download:
            file_name = file.split('-')[-1].split('.')[-2]
            check_file_list = os.listdir(check_dir)
            for check in check_file_list:
                if file_name in check:
                    downloaded_num += 1

    if downloaded_num == download_num:
        print("All files have already been downloaded & parsed!\n")
        exit()
    else:
        Wiki_Downloader.download_wiki_dumps(files_to_download, download_num, DUMP_URL)
        Multiprocessor.find_article_links(files_to_download)
