import wget
from random import sample
import os
import sys

s3_root_path = "https://commoncrawl.s3.amazonaws.com/"
wet_path = "_data/wet.paths"
out_path = "_data/common_crawl_data"

def downloadCrawlData(num_pages):
	with open(wet_path) as fPath:
		paths = fPath.read().splitlines()

	sampled_paths = sample(paths, num_pages)

	for fpath in sampled_paths:
		url = s3_root_path + fpath
		print("Downloading {}".format(url))
		wget.download(url, out_path)

if __name__ == "__main__":
	try:
		num_pages = sys.argv[1]
	except:
		num_pages = 4
	downloadCrawlData(num_pages)

