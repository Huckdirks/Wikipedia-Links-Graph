# Libraries
import requests
import os
from os.path import dirname, isdir
from bs4 import BeautifulSoup

# Other Modules
import wiki_downloader
import multiprocessor
from timeit import default_timer as timer


def main():
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
    MAIN_DIR = dirname(dirname(dirname(__file__)))
    ARTICLES_P_DIR = MAIN_DIR + "/data/load/Articles-p/"
    WIKI_DUMPS_DIR = MAIN_DIR + "/data/load/Wiki_Dumps/"

    # Create data directories if they don't exist
    if not isdir(MAIN_DIR + "/data/"):
        os.makedirs(MAIN_DIR + "/data/load/")

    # Check if a file in Articles-p already exists, and remove from files_to_download if it does
    if isdir(ARTICLES_P_DIR):
        for FILE in files_to_download[:]:
            for CHECK in os.listdir(ARTICLES_P_DIR):
                if FILE.split('-')[-1].split('.')[-2] in CHECK:
                    files_to_download.remove(FILE)
    # Check if a file in Wiki_Dumps already exists, and remove from files_to_download if it does
    elif isdir(WIKI_DUMPS_DIR):
        for FILE in files_to_download[:]:
            for CHECK in os.listdir(WIKI_DUMPS_DIR):
                if FILE in CHECK:
                    files_to_download.remove(FILE)


    if files_to_download:
        START_TIME = timer()
        wiki_downloader.download_wiki_dumps(files_to_download, DUMP_URL)
        multiprocessor.find_article_links(files_to_download)
        print(f"\n{timer() - START_TIME} seconds to download Wikipedia & parse it.")
    else:
        print("All files from Wikipedia have already been downloaded & parsed!\n")



if __name__ == "__main__":
    main()