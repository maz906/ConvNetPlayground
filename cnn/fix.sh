#!/bin/bash
for i in `seq 1 10`; do
	name="crov_$i.txt"
	sed -i 's&200_BRANDS_FINAL&images&g' $name
done
