import sys
import subprocess
import time

freqs = []

for f in range(int(float(sys.argv[1])*10), int(float(sys.argv[2])*10+1), 1):
    args = ["likwid-setFrequencies", "-f", str(f/10)]
    print("calling", args) 
    subprocess.call(args)
    time.sleep(5)
    subprocess.call(["likwid-setFrequencies","-p"])
    time.sleep(20)


