# Libraries
from multiprocessing import Pool
import tqdm
import os
from timeit import default_timer as timer

# Other Modules
import XML_Parser


def find_article_links(files):
    pool = Pool(processes=os.cpu_count())
    results = []

    # Change directory to Articles-p
    original_dir = os.path.dirname(__file__)
    parent_dir = os.path.dirname(original_dir)
    main_dir = os.path.dirname(parent_dir)
    new_dir = main_dir + "/data/load/Articles-p/"
    # If directory doesn't exist, make it
    if not os.path.isdir(new_dir):
        os.mkdir(new_dir)

    print("\nFinding article links from dumps...\n")
    start = timer()
    # Run partitions in parallel using a threadpool with tqdm progress bar
    for x in tqdm.tqdm(pool.imap_unordered(XML_Parser.find_articles, files), total=len(files)):
        results.append(x)

    pool.close()
    pool.join()
    os.chdir(original_dir)

    end = timer()
    print(f'{end - start} seconds elapsed.')
    return end - start