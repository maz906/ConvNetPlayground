i=8
for j in `seq 1 10`; do
       if [ "$i" -ne "$j" ]; then
           #conCATenate
           cat crov_$j.txt >> train.txt
           cat cnn/new.txt >> train.txt
       else
           cat crov_$j.txt >> test.txt
       fi
done

