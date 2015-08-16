#!/bin/bash
#assume you have already run CreateIndexResize.m for 10 validation parts

#root to the caffe directory
root="."
#path to the solver file
solver="$root/models/google_logonet/quick_solver.prototxt" 
#path to the AlexNet pretrained weights file
weights="$root/models/google_logonet/bvlc_googlenet.caffemodel"
#path to the snapshots folder
snapshots="$root/models/google_logonet/snapshots"
#prefix in the solver.prototxt file to the path where snapshots go
prefix="snapshot_prefix: "
#path to the logos folder
logos="$root/data/logos"
train_DB="$root/data/logos/train_DB"
test_DB="$root/data/logos/test_DB"
train_mean="$root/data/logos/train_logos_image_mean.binaryproto"
test_mean="$root/data/logos/test_logos_image_mean.binaryproto"

function finish {
	if [ -f $logos/train.txt ]; then
                rm $logos/train.txt
        fi

        if [ -f $logos/test.txt ]; then
                rm $logos/test.txt
        fi

        if [ -f $train_mean ]; then
                rm $train_mean
        fi

        if [ -f $test_mean ]; then
                rm $test_mean
        fi
 
        if [ -d $train_DB ]; then
                sudo rm -r $train_DB
        fi      

        if [ -d $test_DB ]; then
                sudo rm -r $test_DB
        fi	
	
	echo "Force terminated. Cleanup of train/test files completed. To free memory, close current terminal window."
}
trap finish EXIT

#for each of the 5 validation pieces...
for i in `seq 1 5`; do
	#make the folder to store snapshots for the i-th round of cross validation
	crov_folder="$snapshots/crov_$i"
	if [ ! -d $crov_folder ]; then
		mkdir $crov_folder
	fi
	#replace in solver.prototxt the path to which the snapshots are saved
	sed -i "s&$prefix.*&$prefix\"$crov_folder/\"&g" $solver
	#gather all but the i-th piece into train.txt.
	#put the i-th piece into test.txt
	for j in `seq 1 5`; do
		if [ "$i" -ne "$j" ]; then
			#conCATenate
			#cat $logos/crov_$j.txt >> $logos/train.txt
			#cat $logos/cnn/new.txt >> $logos/train.txt
			cat $logos/crov_$((2*j-1)).txt >> $logos/train.txt
			cat $logos/cnn/new.txt >> $logos/train.txt
			cat $logos/crov_$((2*j)).txt >> $logos/train.txt
			cat $logos/cnn/new.txt >> $logos/train.txt
		else
			#cat $logos/crov_$j.txt >> $logos/test.txt
			cat $logos/crov_$((2*j-1)).txt >> $logos/test.txt
			cat $logos/cnn/new.txt >> $logos/test.txt
			cat $logos/crov_$((2*j)).txt >> $logos/test.txt
		fi
	done
	
	./build/tools/convert_imageset / $logos/train.txt $train_DB	
	./build/tools/compute_image_mean $train_DB $train_mean 
	./build/tools/convert_imageset / $logos/test.txt $test_DB
	./build/tools/compute_image_mean $test_DB $test_mean	

	#train!!!!!!!!!!!!!!! and save to log file in $crov_folder
	./$root/build/tools/caffe train -solver $solver -weights $weights -gpu 0 2>&1 | tee $crov_folder/caffe-train.log
	#./$root/build/tools/caffe train -solver $solver -snapshot models/google_logonet/snapshots/crov_1/_iter_280000.solverstate -gpu 0 2>&1 | tee $crov_folder/caffe-train.log 

	finish

	#switch the snapshot folder in solver.prototxt back to
	#$snapshots
	sed -i "s&$prefix.*&$prefix\"$snapshots\"&g" $solver
done
