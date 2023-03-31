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
def process_wiki(title, text):
    wiki = mwparserfromhell.parse(text)
    links = [x.title.strip_code().strip() for x in wiki.filter_wikilinks()]
    return title, links


# Load data from XML files into the parser
def find_articles(file, save=True):
    # Set the directory to save the files to & filename
    original_dir = os.path.dirname(__file__)
    parent_dir = os.path.dirname(original_dir)
    main_dir = os.path.dirname(parent_dir)
    file_name = file.split('-')[-1].split('.')[-2]
    file_name = f"{file_name}.ndjson"
    out_dir = main_dir + "/data/load/Articles-p/" + file_name

    # If the file already exists, don't parse it again
    if os.path.isfile(out_dir):
        return

    dumps_dir = main_dir + "/data/load/Wiki_Dumps"
    os.chdir(dumps_dir)

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

    if save:
        with open(out_dir, 'w') as fout:
            # Write as json
            for page in handler._pages:
                fout.write(json.dumps(page) + '\n')

    # Delete the file after it's been parsed
    # ONLY KEEP THIS LINE IF YOU WANT TO DELETE THE FILES AFTER THEY'RE PARSED!!! IT'S A BITCH TO REDOWNLOAD THEM
    os.remove(file)

    os.chdir(original_dir)
    # Memory management
    del handler
    del parser
    gc.collect()
    return