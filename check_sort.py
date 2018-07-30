#!/usr/bin/python
import argparse

parser = argparse.ArgumentParser(description='Check file sorting')

parser.add_argument('--file-path')

args = parser.parse_args()

previous_string = ''
line_qty = 0

with open(args.file_path, 'r') as f:
    for line in f:
        assert line >= previous_string
        line_qty += 1

print '%s is sorted and contains %d lines' % (args.file_path, line_qty)

