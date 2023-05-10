# Libraries
from multiprocessing import Pool
import tqdm
import os
from os.path import dirname, isdir

# Other Modules
import xml_parser


def find_article_links(FILES):
    pool = Pool(processes=os.cpu_count())
    results = []

    # Change directory to Articles-p
    MAIN_DIR = dirname(dirname(dirname(__file__)))
    CHECK_DIR = MAIN_DIR + "/data/load/Articles-p/"
    # If directory doesn't exist, make it
    if not isdir(CHECK_DIR):
        os.mkdir(CHECK_DIR)

    print("\nFinding article links from dumps...")
    # Run partitions in parallel using a threadpool with tqdm progress bar
    for x in tqdm.tqdm(pool.imap_unordered(xml_parser.find_articles, FILES), total=len(FILES)):
        results.append(x)

    pool.close()
    pool.join()
    os.chdir(MAIN_DIR)
    return