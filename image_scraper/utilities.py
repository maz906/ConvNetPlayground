import os
import math
import random

def averageFileSize():
	"""Determines the average size of an image in ./images/"""
	brands = [f for f in os.listdir('./images')]

	avg = 0
	count = 0
	for brand in brands:
		images = [i for i in os.listdir('./images/' + brand)]
		count += len(images)
		for image in images:
			avg += os.path.getsize('./images/' + brand + '/' + image)

	print(avg/count)

def separateTrainingFromTesting(propForTesting):
	brands = [f for f in os.listdir('./images')]
	maxInts = {}
	# construct a list of ints which represent the number of
	# image files in each brand folder.
	for brand in brands:
		images = [i for i in os.listdir('./images'+brand)]
		images = sorted(images, key = lambda x : intPart(x))
		maxInts[brand] = intPart(images[0]))
	
	# decide who will be used for testing. construct a dictionary
	# from brand to the image numbers that will be tested.
	total = float(sum(maxInts.values()))
	testSize = total*propForTesting
	brandImagesToTest = {}
	for brand in brands:
		# find the contribution of that brand to the total test set
		brandContrib = int(maxInts[brand]/total * float(testSize))
		# shuffle for randomness
		nums = range(0, brandContrib)
		random.shuffle(range(0, brandContrib))
		# grab the first part of the array
		brandImagesToTest[brand] = nums[0:brandContrib]

	














def intPart(filename):
	try:
		intVersion = -1*os.path.splitext(i)[0].split('_')[1]
	except:
		return 0
	return intVersion