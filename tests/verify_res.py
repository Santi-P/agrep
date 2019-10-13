
import sys

grep_file_name = sys.argv[1]
my_file_name = sys.argv[2]
grep_file_lines = []
my_file_lines = []

with open(grep_file_name, "r") as gf:
    for line in gf:
        line = line.split(':')
        try: 
            grep_file_lines.append(line[0])
        except:
            pass
with open(my_file_name, "r") as gf:
    for line in gf:
        line = line.split(':')
        
        try: 
            my_file_lines.append(line[0])
        except:
            pass


if (set(my_file_lines) == set(grep_file_lines)):
    print("Test Passed: grep results are equal to agrep results" )
else:
    print("Test failed")