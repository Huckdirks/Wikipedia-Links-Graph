# Libraries
import xml.sax
import subprocess
import mwparserfromhell
import os
import json
import gc


# Content handler for Wiki XML data using SAX
# Lmao I just copied this from https://github.com/WillKoehrsen/wikipedia-data-science/blob/master/notebooks/Downloading%20and%20Parsing%20Wikipedia%20Articles.ipynb and barely modified it
class WikiXmlHandler(xml.sax.handler.ContentHandler):
    def __init__(self):
        xml.sax.handler.ContentHandler.__init__(self)
        self._buffer = None
        self._values = {}
        self._current_tag = None
        self._pages = []
        self._article_count = 0

    def characters(self, content):
        # Characters between opening and closing tags
        if self._current_tag:
            self._buffer.append(content)

    def startElement(self, name, attrs):
        # Opening tag of element
        if name in ("title", "text"):
            self._current_tag = name
            self._buffer = []

    def endElement(self, name):
        # Closing tag of element
        if name == self._current_tag:
            self._values[name] = ' '.join(self._buffer)

        if name == "page":
            self._article_count += 1
            article = process_wiki(self._values.get(
                "title"), self._values.get("text"))
            if article:
                self._pages.append(article)


# Parse the wiki pages
def process_wiki(TITLE, TEXT):
    WIKI = mwparserfromhell.parse(TEXT)
    LINKS = [x.title.strip_code().strip() for x in WIKI.filter_wikilinks()]
    return TITLE, LINKS


# Load data from XML files into the parser
# I would declare file as const but since it gets deleted I made it normal (even tho file is technically a const string, not the file itself, but this is python so I'll just use the loose typing)
def find_articles(file):
    # Set the directory to save the files to & filename
    ORIGINAL_DIR = os.path.dirname(__file__)
    PARENT_DIR = os.path.dirname(ORIGINAL_DIR)
    MAIN_DIR = os.path.dirname(PARENT_DIR)
    FILE_NAME = file.split('-')[-1].split('.')[-2]
    FILE_NAME = f"{FILE_NAME}.ndjson"
    OUT_DIR = MAIN_DIR + "/data/load/Articles-p/" + FILE_NAME

    # If the file already exists, don't parse it again
    if os.path.isfile(OUT_DIR):
        return

    DUMPS_DIR = MAIN_DIR + "/data/load/Wiki_Dumps"
    os.chdir(DUMPS_DIR)

    # Object for handling xml
    handler = WikiXmlHandler()

    # Parsing object
    parser = xml.sax.make_parser()
    parser.setContentHandler(handler)

    # Uncompress the file line by line and parse it
    for line in subprocess.Popen(['bzcat'], stdin=open(file), stdout=subprocess.PIPE).stdout:
        try:
            parser.feed(line)
        except StopIteration:
            break

    with open(OUT_DIR, 'w') as fout:
        # Write as json
        for PAGE in handler._pages:
            fout.write(json.dumps(PAGE) + '\n')

    # Delete the file after it's been parsed
    # ONLY KEEP THIS LINE IF YOU WANT TO DELETE THE FILES AFTER THEY'RE PARSED!!! IT'S A BITCH TO REDOWNLOAD THEM
    os.remove(file)

    os.chdir(ORIGINAL_DIR)
    # Memory management
    del handler
    del parser
    gc.collect()
    return