#!/usr/bin/python
import argparse
import random
import string

parser = argparse.ArgumentParser(description='Generate file with random strings')

parser.add_argument('--max-length', type=int)
parser.add_argument('--max-size', type=int)
parser.add_argument('--output')
parser.add_argument('--max-buffer-size', help='buffer size in bytes', type=int)

args = parser.parse_args()

symbols = string.ascii_uppercase + string.digits

buffer_strings = []
buffer_size = 0
size = 0
iteration_qty = 0

with open(args.output, 'w') as f:
    while size < args.max_size:
        string_size = random.choice(range(10, args.max_length)) + 1
        s = ''.join(random.choice(symbols) for _ in range(string_size))
        size += string_size
        buffer_size += string_size
        buffer_strings.append(s)

        if buffer_size >= args.max_buffer_size:
            f.write('\n'.join(buffer_strings))
            f.write('\n')
            buffer_size = 0
            buffer_strings = []
            iteration_qty += 1

print 'Wrote %d symbols in %d iterations' % (size, iteration_qty)

