import sys
import subprocess
import time

freqs = []

for f in sys.argv[1:]:
    freqs.append(f)
freqs.reverse()

for f in freqs:
    args = ["likwid-setFrequencies", "-f", f]
    print("calling", args)
    time.sleep(5)
    subprocess.call(args)
    args = ["likwid-setFrequencies", "-p"]
    subprocess.call(args)
    time.sleep(25)


