#!/usr/bin/python
import time
import sys

temp_nodes = []
fo = open("/root/scripts/mobilityfile", "a", 0)

time_val=0

with open("/root/scripts/mobility", "r") as f:
        lines = f.readlines()
        for l in lines: 
                entries = l.split(' ')
		while (time_val<=int(entries[1])):
			if int(entries[1]) == time_val:
				fo.write('1 '+entries[0]+'\n')
				#print('1 '+entries[0]+'\n')
			time.sleep(10)
			time_val = time_val+10
			
		time.sleep(10)
		time_val = time_val+10
		#fo.write('1 '+str(entries[0])+'\n')
		#print('1 '+str(entries[0])+'\n')
		
f.close()
fo.close()
