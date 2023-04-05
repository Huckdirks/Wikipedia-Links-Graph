# Libraries
import requests
import os
from bs4 import BeautifulSoup

# Other Modules
import wiki_downloader
import multiprocessor
from timeit import default_timer as timer


if __name__ == "__main__":
    # Get the HTML of the page
    DUMP_URL = "https://dumps.wikimedia.org/enwiki/latest/"
    DUMP_HTML = requests.get(DUMP_URL).text

    # Convert HTML to a soup
    SOUP_DUMP = BeautifulSoup(DUMP_HTML, "html.parser")

    # Search through all files & get the ones with "pages-articles" in the name
    files_to_download = []
    download_num = 0
    for FILE in SOUP_DUMP.find_all('a'):
        if "pages-articles" in FILE.get("href") and "xml-p" in FILE.get("href") and FILE.get("href").endswith(".bz2"):
            files_to_download.append(FILE.get("href"))
            download_num += 1

    # Change directory to Articles-p
    ORIGINAL_DIR = os.path.dirname(__file__)
    PARENT_DIR = os.path.dirname(ORIGINAL_DIR)
    MAIN_DIR = os.path.dirname(PARENT_DIR)
    CHECK_DIR = MAIN_DIR + "/data/load/Articles-p/"

    # Create data directories if they don't exist
    if not os.path.isdir(MAIN_DIR + "/data/"):
        os.mkdir(MAIN_DIR + "/data/")
    if not os.path.isdir(MAIN_DIR + "/data/load/"):
        os.mkdir(MAIN_DIR + "/data/load/")

    # Check if all a file in Articles-p already exists, and remove from files_to_download if they do
    if os.path.isdir(CHECK_DIR):
        for FILE in files_to_download[:]:
            FILE_NAME = FILE.split('-')[-1].split('.')[-2]
            CHECK_FILE_LIST = os.listdir(CHECK_DIR)
            for CHECK in CHECK_FILE_LIST:
                if FILE_NAME in CHECK:
                    files_to_download.remove(FILE)

    if files_to_download:
        START_TIME = timer()
        wiki_downloader.download_wiki_dumps(files_to_download, DUMP_URL)
        multiprocessor.find_article_links(files_to_download)
        print(f"\n{timer() - START_TIME} seconds to download Wikipedia & parse it.")
    else:
        print("All files from Wikipedia have already been downloaded & parsed!\n")