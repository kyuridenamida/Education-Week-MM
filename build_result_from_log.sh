set -eu
log_dir=$1
for file in `ls $log_dir | sort -n`
do
	final_score=`cat $log_dir/$file | grep final_score | sed -e "s/^.* \([0-9]\+\)/\1/g"`
	n=`cat $log_dir/$file | grep " N " | sed -e "s/^.* \([0-9]\+\)/\1/g"`
	k=`cat $log_dir/$file | grep " K " | sed -e "s/^.* \([0-9]\+\)/\1/g"`
	final_t=`cat $log_dir/$file | grep "final_t" | sed -e "s/^.* \([0-9]\+\)/\1/g"`
	last_updated=`cat $log_dir/$file | grep "last_updated " | sed -e "s/^.* \([0-9]\+\)/\1/g"`
	last_updated_by_probability=`cat $log_dir/$file | grep "last_updated_by_probability " | sed -e "s/^.* \([0-9]\+\)/\1/g"`
	
	echo $final_score, $n, $k, $final_t, $last_updated, $last_updated_by_probability
done

