#!/bin/bash
root="."
logos="$root/data/logos"
train_DB="$root/data/logos/train_DB"
test_DB="$root/data/logos/test_DB"
train_mean="$root/data/logos/train_logos_image_mean.binaryproto"
test_mean="$root/data/logos/test_logos_image_mean.binaryproto"

#assemble all the training data
for i in `seq 1 10`; do
	cat $logos/crov_$i.txt >> $logos/train.txt
	cat $logos/cnn/new.txt >> $logos/train.txt
done

# copy the test data

cat $logos/FINAL_TEST_SET.txt >> $logos/test.txt
#build DB and compute means
./build/tools/convert_imageset / $logos/train.txt $train_DB	
./build/tools/compute_image_mean $train_DB $train_mean 
./build/tools/convert_imageset / $logos/test.txt $test_DB
./build/tools/compute_image_mean $test_DB $test_mean

# train google net
solver="$root/models/google_logonet/quick_solver.prototxt" 
weights="$root/models/bvlc_googlenet/bvlc_googlenet.caffemodel"
snapshots="$root/models/google_logonet/snapshots"
final_folder="$snapshots/final"
./$root/build/tools/caffe train -solver $solver -weights $weights -gpu 0 2>&1 | tee $final_folder/caffe-train.log

# train alex net
solver="$root/models/finetune_logos/solver.prototxt" 
weights="$root/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel"
snapshots="$root/models/finetune_logos/snapshots"
final_folder="$snapshots/final"
./$root/build/tools/caffe train -solver $solver -weights $weights -gpu 0 2>&1 | tee $final_folder/caffe-train.log








