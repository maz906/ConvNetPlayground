import matplotlib.pyplot as plt
import operator
import math

def plot(iter_list, data, option):
	plt.plot(iter_list, data)
	plt.xlabel('iteration number')
	plt.ylabel(option + ' values')
	plt.show()

def plot_avg(option):
	result = []
	iter_list = None
	length = 10
	min_length = 0
	for i in range(1, length + 1):
		if i != 8:
			if not result:
				iter_list, data = get_data(option, data_filename(i, option))
				min_length = len(iter_list)	
				result = data
			else:
				_, data = get_data(option, data_filename(i, option))
				min_length = min(len(result), len(data), min_length)
				result = [x + y for x, y in zip(result[0:min_length], data[0:min_length])]

	result = [x/length for x in result]
	plot(iter_list[0:min_length], result, option)
def single_crov_plot(idx, option):
	iter_list, data = get_data(option, data_filename(idx, option))	
	plot(iter_list, data, option)

def data_filename(idx, option):
	return 'crov_' + str(idx) + '/' + 'output_' + option + '_' + str(idx) + '.txt'


def get_data(option, data_file):
	iter_list = data = []
	if option == 'accuracy':
		with open(data_file) as crov_data:
			data = [float(line.rstrip('\n')) for line in crov_data]
			iter_list = [1000*x for x in range(0, sum(1 for _ in data))]
		return (iter_list, data)	
	elif option == 'loss':
		with open(data_file) as loss_data:
			lines = [line for line in loss_data]
			iter_list = [int(line.split('/')[0]) for line in lines]
			data = [float(line.split('/')[1].strip('\n')) for line in lines]
		return (iter_list, data)
