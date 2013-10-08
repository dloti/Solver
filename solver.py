import subprocess
import shutil
import sys

instances_num = int(sys.argv[2])
for i in range(1,instances_num+1):
	subprocess.call(["./solver", str(sys.argv[1]), str(i)])


