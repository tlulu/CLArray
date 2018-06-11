#!/bin/python

import argparse
import random

def get_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("-q", "--num_rows", type=int)
	parser.add_argument("-w", "--max_clause_size", type=int)
	parser.add_argument("-e", "--min_clause_size", type=int)
	parser.add_argument("-r", "--max_val", type=int)
	parser.add_argument("-y", "--clause_fname")
	parser.add_argument("-u", "--assign_fname")
	return parser.parse_args()

def main():
	args = get_args()
	rows = []
	acc = 0
	for _ in range(int(args.num_rows)):
		row = [str(random.randint(0, args.max_val - 1)) for _ in range(random.randint(args.min_clause_size, int(args.max_clause_size)))]
		acc += len(row)
		rows.append(row)
	with open(args.clause_fname, 'w') as f:
		for row in rows:
			line = ' '.join(row) + '\n'
			f.write(line)
	assign = [str(random.randint(-1, 0)) for _ in range(args.max_val)]
	if args.assign_fname:	
		with open(args.assign_fname, 'w') as g:
			line = ' '.join(assign) + '\n'
			g.write(line)
	return acc

if __name__ == '__main__':
	main()
