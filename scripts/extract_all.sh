#!/bin/bash

repeat=25
fileExt=vec
statType=vector
OPTIONS=hc:r:e:t:
DIR_RESULT="./CRAN2/CRAN2/simulations/results"
#DIR_RESULT="D:/PECSN_simulation_results/verification/continuity"
DIR_OUT="./CRAN2/csv_results"

statistics=(bbuPacketInQueue bbuQueueingTimeStat bbuResponseTimeStat bbuOccupationQueueStat rrhQueueingTimeStat rrhResponseTimeStat rrhPacketInQueueStat delayStat)

usage() {
cat << EOF
Collect statistics from OMNET++

-h          help
-c          name of the configuration
-r          number of repetitions (default 25)
-e          extension of the file (default vec)
-t          type of the statistics (defaul vector)

EOF
}

# Parse command line
while getopts $OPTIONS opt ; do
	case $opt in
	h ) usage ; exit 0 ;;
	c ) configName=$OPTARG ;;
	r ) repeat=$OPTARG ;;
	e ) fileExt=$OPTARG ;;
    t ) statType=$OPTARG ;;
	esac
done
shift $(($OPTIND-1))

if [ -z $configName ] ; then
	echo "ERROR: you must provide a configuration name"
	usage
	exit 1
fi

for statistic in ${statistics[@]} ; do
	#echo $statistic
	path=${DIR_RESULT}"/${configName}"

	#echo "scavetool export --type v -o ${DIR_OUT}/${configName}_${i}_${statistic}.csv -F CSV-S -v -x precision=14 -x separator=semicolon -f ${statistic}:$statType $path"
	scavetool export --type v -o ${DIR_OUT}/${configName}_${statistic}.csv -F CSV-S -v -x precision=14 -x separator=semicolon -f ${statistic}:$statType $path*.${fileExt}
	#fix_csv ${DIR_OUT}/${configName}_${file_id}_delay.csv
done

exit 0