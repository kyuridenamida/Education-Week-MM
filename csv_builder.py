#! /bin/sh
""":"
exec python "$0" ${1+"$@"}
"""

import sys

definition = {
	"program_name" : ["program_name"],
	"sa_finish_time" : ["sa_finish_time"],
	"final_t" : ["final_t"],
	"final_score" : ["final_score"],
	"compiled_datetime" : ["month", "date", "year", "time", "timezone"],
	"updated" : ["score","t", "elapsed"],
	"score_incomplete" : ["score", "t", "elapsed"],
	"N" : ["N"],
	"K" : ["K"],
}

if __name__ == "__main__":
	args = sys.argv[1:]
	if len(args) != 1:
		raise Exception("# of arguments must be 1")

	group_by = {}	
	with open(args[0]) as f:	
		for l_ in f:
			l = l_.strip()
			if l.find("[ANALYSIS]") == -1:
				continue
			
			lst = l.split()[2:]
			name = lst[0]
			log_part = lst[1:]
			if name not in group_by:
				group_by[name] = []
			group_by[name].append(log_part)

	dict_group_by = {}
	for key in definition.keys():
		for e in group_by[key]:
			dct = dict(zip(definition[key], e))
			if key not in dict_group_by:
				dict_group_by[key] = []
			dict_group_by[key].append(dct)

	single = lambda key:dict_group_by[key][0][key]

	print "Seed", args[0].split(".")[-2].split("-")[-1]
	print "N", single('N')
	print "K", single('K')
	print "sa_finish_time", single('sa_finish_time')
	print "final_t", single('final_t')
	
	print "t", "score"
	for row in dict_group_by["score_incomplete"]:
		print row["t"], row["score"]
