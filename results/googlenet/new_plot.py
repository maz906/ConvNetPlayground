import csv
data_handles = [csv.DictReader(open('google_val.csv', 'r'), fieldnames = ['iter', 'acc', 'lr', 'loss'])]
data = [ [(row['acc'], row['lr'], row['loss']) for row in data] for data in data_handles]
length = min([len(data_list) for data_list in data])
data = [data_list[:length] for data_list in data]
err = [ 1 - sum([float(data_list[j][0]) for data_list in data]) for j in range(length)]
lrs = [ sum([float(data_list[j][1]) for data_list in data]) for j in range(length)]
loss = [ sum([float(data_list[j][2]) for data_list in data]) for j in range(length)]
iters = range(4, 328, 4)
