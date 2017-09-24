recentdir=`ls ./results_localtest/ | sort -rn | head -n 1`
./build_result_from_log.sh results_localtest/$recentdir/log/

