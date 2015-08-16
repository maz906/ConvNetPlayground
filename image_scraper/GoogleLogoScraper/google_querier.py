import urllib
import urllib.request
from urllib.error import HTTPError
import json
import os
import os.path
import threading
import time
import sys

class GoogleQuerier:
	"""This class makes a query to Google Custom Search Engine with a fixed
	API key and engine ID, which is hard coded into the constructor. The fields
	which are open to being set are the search type (image or text) and the number
	of results to be returned.

	There are methods to do queries, save the JSON file from the query, and extract
	images from the websites listed in the JSON file.

	6/16/2015: We've found out that the Google Custom Search API terms of service forbids 
	using a script to do queries (from https://cse.google.com/cse/docs/tos.html):

		1.4 Appropriate Conduct. You shall not, and shall not allow any third party to:...
		(f) directly or indirectly access, launch and/or activate the Service through or from, or
		otherwise incorporate the Service in, any Web site or other means other than the Site, 
		and then only to the extent expressly permitted herein;... (i) directly or indirectly 
		generate queries, or impressions of or clicks on Results, through any automated, 
		deceptive, fraudulent or other invalid means (including, but not limited to, click 
		spam, robots, macro programs, and Internet agents);
	"""
	image_count = 0
	api_to_engineID = {'AIzaSyA2gulOuoO-zW79Pzt-xz97wMQuSSZ3t1s':'012049976034714812121:zz6ggm1nvo8', \
						'AIzaSyCwAFypweqyrX0qS45dUGY44BOkzxE1g9E': '004183340223802383764:62a1o0gt7ss', \
						'AIzaSyAWZhp9t9xRfLD0gHVIre_HinJjQRAE3qk': '012912476253704493779:3ln4zw6oxwi', \
						'AIzaSyBajizWik4KT9mjt7WPdHvDpHyO4RZLYKY': '017305718505888063765:1r7rlj9v5rw'}
	default_ext = '.png'
	default_searchType = 'image'
	default_num = 10 #i think 10 is the maximum possible
	#TODO: create a set of four engine ID's and API keys

	def __init__(self, searchType, num):
		self.search_type = searchType
		self.num = num
		self.pathname = 'images/'


	def __build_url(self, api_key, query, exclude):
		url = 'https://www.googleapis.com/customsearch/v1?'
		url += 'key=' + api_key
		url += '&cx=' + MassQuerier.api_to_engineID[api_key]
		url += '&num=' + str(self.num)
		url += '&searchType=' + self.search_type
		url += '&q=' + query
		url += '&exclude=' + exclude
		return url

	def __save_query(self, returned, filename):
		"""Write the data (returned) from a query to Google CSE to filename."""
		save_file = open(filename, 'wb+') #wb since we need to write bytes
		save_file.write(returned)
		save_file.close()

	def __get_json(self, filename, brand, exclude):
		#TODO: 'street view' needs to be changed to something that makes more sense.
		for api_key in MassQuerier.api_to_engineID.keys():
			link = self.__build_url(api_key, brand + '+street+view', exclude)
			print('Querying: ' + link)
			try:
				url = urllib.request.urlopen(link)
			except HTTPError as e:
				pass
		if url == None:
			print('...query failed. Error: ' + str(e))
			sys.exit(0)
		self.__save_query(url.read(), filename)


	def __save_images(self, filename, brand, exclude):
		"""Saves images from the links in items in filename (assumed to be a JSON 
		file from a Google CSE query) to pathname"""
		self.__get_json(filename, brand, exclude)
		with open(filename) as data_file:    
			data = json.load(data_file)
		results = data["items"]
		for result in results:
			link = result["link"]
			print('Downloading from: ' + link)
			#get rid of everything after the first ?
			web_file = os.path.basename(link.split('?')[0])
			#if there's no extension on the basename of the link, add a default 
			if len(os.path.splitext(web_file)) < 2:
				web_file += MassQuerier.default_ext
			try:
				#tried to make the filenames nicer :(
				# urllib.request.urlretrieve(link, self.pathname + 'image' + \
					# str(MassQuerier.image_count) + web_file_ext)
				urllib.request.urlretrieve(link, self.pathname + web_file)
				# image_count += 1
			except Exception as e:
				print('...download failed. Error: ' + str(e))
				pass		
		data_file.close()

	def __downloader(self, brand, exclude, count):
		"""Downloads an image of a single brand. Purpose is for multi-threading."""
		filename = 'query'+str(count)+'.json'
		self.__save_images(filename, brand, exclude)
		os.remove(filename)

	def download(self, brands, exclude):
		for i in range(0, len(brands)):
			#threaded version:
			t = threading.Thread(None, self.__downloader, args=(brands[i],exclude, i))
			t.start()
			#non-threaded version:
			# self.downloader(brands[i], exclude, i)
			
def main():
	temp = MassQuerier('image', 10)
	temp.download(['microsoft', 'starbucks', 'mcdonalds', 'jiro', \
		'michelin', '3M', 'einsteins+bros+bagels', 'oracle'], 'map+google')

if __name__ == "__main__":
	main()

