import os

with open('blackimage_test_list', 'r') as excludefile:
	avoid_lines = { '/home/rips_tc/caffe/data/logos/images/' + line.strip('\n') for line in excludefile.readlines() }


for j in range(1, 11):
	with open('crov_' + str(j) + '.txt', 'r') as crovfile:
		images = crovfile.readlines()

	with open('crov_' + str(j) + '.txt', 'w') as crovfile:
		for image in images:
			if image.split(' ')[0] not in avoid_lines:
				crovfile.write(image)
			else:
				print 'ignoring ' + image
						
with open('FINAL_TEST_SET.txt', 'r') as testfile:
	images = testfile.readlines()

print images

with open('FINAL_TEST_SET.txt', 'w') as testfile:
	for image in images:
		if image.split(' ')[0] not in avoid_lines:
			testfile.write(image)
		else:
			print 'ignoring ' + image	
