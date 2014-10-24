#!/usr/bin/python
import random

team = "ABCDE"
fixed = ["Tom", "Dylan", "Patrick", "Yifan", "Cloud"]
players = ["Joey", "Wei", "Lee", "Range", "Eric", "Vincent", "Benny", "Zhitao", "Warren", "Frank"]
scores = {}

random.shuffle(players)

fo = open("result.txt", "w");
for i in range(0, len(fixed)):
    print "Team " + str(team[i]) + ":" 
    print fixed[i] + ", " + players[i*2] + ", " + players[i*2+1] + "\n"
    # write to file
    fo.write("Team " + str(team[i]) + ":\n")
    fo.write(fixed[i] + ", " + players[i*2] + ", " + players[i*2+1] + "\n\n")
print "Result has been written to 'result.txt'"
fo.close()

raw_input("Press key to exit...")
