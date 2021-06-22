import sys
import subprocess
import time

sleepytime = 20

if(len(sys.argv) == 1):
    print("Usage: ./{} <num_dd> <sleeptime>".format(sys.argv[0]))
    exit()

if(len(sys.argv) == 3):
    sleepytime = int(sys.argv[2])

for i in range(int(sys.argv[1])/2):
    args = ["likwid-pin", "-c", str(i), "dd", "if=/dev/zero", "of=/dev/null"]
    print("calling", args)
    subprocess.Popen(args)
    time.sleep(sleepytime/2)
    subprocess.call(["likwid-setFrequencies", "-p"])
    time.sleep(sleepytime/2)

    args = ["likwid-pin", "-c", str(i+4), "dd", "if=/dev/zero", "of=/dev/null"]
    print("calling", args)
    subprocess.Popen(args)
    time.sleep(sleepytime/2)
    subprocess.call(["likwid-setFrequencies", "-p"])
    time.sleep(sleepytime/2)



try:
    time.sleep(1)
    input("\nPress any key to continue...\n")
except SyntaxError:
    pass
subprocess.call(["killall", "dd"])
