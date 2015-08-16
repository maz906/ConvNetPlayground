import os

brands = [f for f in os.listdir('./images')]

avg = 0
count = 0
for brand in brands:
	images = [i for i in os.listdir('./images/' + brand)]
	count += len(images)
	for image in images:
		avg += os.path.getsize('./images/' + brand + '/' + image)

print(avg/count)
