# to be stored in the /var/www/html/ folder
# as of 7/31/2015, it is necessary to first switch to root by "sudo su"
# due to the sudo call below (which is necessary since a json file is created
# by www-data and cannot be modified by rips_tc)

import time
import os
from datetime import datetime as dt
import json
import codecs

import caffe
import numpy as np

######## to be replaced with Django? ########
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler

################## CAFFE INFORMATION ##################
DEFAULT_GPU = 0
DEFAULT_DEVICE = "gpu"
DEFAULT_DLAYER = "data"
DEFAULT_PROB = "prob"
caffe_root = '/home/rips_tc/caffe/'

################## LABEL INFORMATION ##################
labels = np.loadtxt(caffe_root + 'data/logos/index-brand.txt', str, delimiter='\t')

################### PROCESSING INFORMATION ##################
script_root = '/var/www/html/'
upload_path = script_root + 'data/runtime/upload/'
processed_path = script_root + 'data/runtime/output/processed_'
metadata_path = script_root + 'data/runtime/metadata/'
INFO_FORMAT = '.json'
INFO_ENCODING = 'utf-8'


def initialize_device(device = DEFAULT_DEVICE, id = DEFAULT_GPU):
        if device == DEFAULT_DEVICE:
                caffe.set_device(id)
                caffe.set_mode_gpu()
        else:
                caffe.set_mode_cpu()

def initialize_transform(mean_file_path):
        transformer = caffe.io.Transformer({DEFAULT_DLAYER : net.blobs[DEFAULT_DLAYER].data.shape})
        transformer.set_transpose(DEFAULT_DLAYER, (2, 0, 1))
        transformer.set_mean(DEFAULT_DLAYER, np.load(mean_file_path).mean(1).mean(1))
        transformer.set_raw_scale(DEFAULT_DLAYER, 255)
        transformer.set_channel_swap(DEFAULT_DLAYER, (2,1,0))
        return transformer

class CNN:
	def __init__(self, solver, snapshot, mean_file):
		self.solver = solver
		self.snapshot = snapshot
		self.mean_file = mean_file
		self.net = caffe.Net(self.solver, self.snapshot, caffe.TEST)
		self.transformer = initialize_transform(self.mean_file)

	def classify_image(self, image_path, top = 5):
                """Classifies an image and returns the top five labels. Necessary to call initialize_device first."""
		prob = final_output(image_path)
		return labels[prob.argsort()[-1:-(top + 1):-1]]

	def final_output(self, image_path):
		self.net.blobs[DEFAULT_DLAYER_NAME].data[...] = self.transformer.preprocess(DEFAULT_DLAYER, caffe.io.load_image(image_path))
		out = self.net.forward()
		return self.net.blobs[DEFAULT_PROB].data[0].flatten()

class EnsembleCNN:

	"""Constants to read the JSON file with neural net information. Format
	is assumed to be 
		{
			solver_name : {
				SOLVER : "path/to/deploy.prototxt"
				SNAPSHOT : "path/to/snapshot.caffemodel"
				MEAN_FILE : "path/to/mean_file"
			}
		}
	"""
	SOLVER = "solver"
	SNAPSHOT = "snapshot"
	MEAN_FILE = "mean_file"

	def __init__(self, net_info_file, device = DEFAULT_DEVICE, id = DEFAULT_GPU):
		initialize_device(device, id)	

		with open(net_info_file) as net_info:
			nets_data = json.load(net_info)
	
		self.nets = []
		for net_name in nets_data.keys():	
			self.nets.append(CNN(nets_data[net_name][SOLVER], \
				nets_data[net_name][SNAPSHOT], nets_data[net_name][MEAN_FILE]))
	
	

	def classify_image(self, image_path, top = 5):
		final_predict = np.zeros(len(labels)) 
		for net in self.nets:
			final_predict += net.final_output(image_path)
		final_predict /= len(labels)
		return labels[final_predict.argsort()[-1:-(top + 1):-1]]

class ImageHandler(PatternMatchingEventHandler):
	patterns = ["*.jpg", "*.png"]

	
	def __init__(self, ensemble):
		super(ImageHandler, self).__init__()
		self.ensemble = ensemble

	def on_created(self, event):
		"""Upon creation of a .jpg file, classify the image, store the interaction,
		and delete the file."""
		print(event.src_path)
		try:
			top_labels = self.ensemble.classify_image(event.src_path)
			self.serv_store_interaction(event.src_path, top_labels)
		except Exception as e:
			log(str(e))	

	def log(msg, logfile = "error.log"):
		if not os.path.isfile(logfile):
			open(logfile, 'a').close()

		with open(logfile, 'w') as outfile:
			outfile.write(str(dt.now()) + ' ' + msg + '\n')

	def db_store_interaction(image_path, results):
		"""Planned method to write interaction to database."""
		#save image to data/runtime/data_backup/<datetime>_test_<android_id>.jpg
		os.remove(image_path)

	def serv_store_interaction(self, image_path, top_labels):
		name = os.path.splitext(os.path.basename(image_path))[0]

		result_info = []	
		for i, label in enumerate(top_labels):
			label = str(label.split(' ')[1])
			with codecs.open(metadata_path + label + INFO_FORMAT, 'r', INFO_ENCODING) as fil:
				result_info.append(json.load(fil,encoding = INFO_ENCODING))
				print(metadata_path + label + INFO_FORMAT) 		

		os.system('sudo chmod a+rw ' +  processed_path + name + INFO_FORMAT)
		with open(processed_path + name + INFO_FORMAT, 'a') as outfile:
			json.dump(result_info, outfile)


def main():
	initialize_net(model_path, snapshot_path)
	initialize_transform(mean_file_path)
	observer = Observer()
	observer.schedule(ImageHandler(EnsembleCNN("nets.json")), path=upload_path)
	observer.start()
	try:
		while True:
			time.sleep(1)
	except KeyboardInterrupt:
		observer.stop()

	observer.join()

if __name__ == "__main__":
	main()












