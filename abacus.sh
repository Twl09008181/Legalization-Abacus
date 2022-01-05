
make clean
make

#for test in "adaptec1" "newblue5"
for test in "adaptec1"
do
	echo "Run $test" 
	echo "Run $test" >> $test.log
	for i in 10 20 30
	do
		for j in 0 1 2 4 6 8 16 
		do
			echo "rowRange: $i threadNum: $j" 
			echo "rowRange: $i threadNum: $j" >> $test.log
			./Lab4 $test.aux -r $i -t $j | grep -E 'time|cost' >> $test.log
		done 
	done
done
