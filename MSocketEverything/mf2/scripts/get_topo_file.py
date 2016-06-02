import time
import sys

temp_nodes = []
fo = open("mobility_topo", "a")
f2 = open("is_BR", "a")

time_val=0
ledge=[]
redge=[]
as_list=[]

with open("uniq_anodes", "r") as f1:
	lines = f1.readlines()
	for l in lines:
		entries = l.split(' ')
		as_list.append(entries[0])	

with open("SF_links_numbered", "r") as f:
        lines = f.readlines()
        for l in lines:
                entries = l.split(' ')
		ledge.append(entries[0])
		redge.append(entries[1].split('\n')[0])
f.close()

for i in range(53):
	temp_vec=[]
	for j in range(len(ledge)):
		if int(ledge[j])==i:
			temp_vec.append(str(int(redge[j])+1))
		if int(redge[j])==i:
			temp_vec.append(str(int(ledge[j])+1))
	temp_str=''
	var=0

	for k in range(len(temp_vec)):
		temp_str = temp_str+temp_vec[k]+' '
		if int(as_list[i]) != as_list[int(temp_vec[k])-1]:
			var=1
	f2.write(str(i+1)+' '+str(var)+' '+as_list[i]+'\n')

	#print temp_str
	temp_str=str(i+1)+' '+str(len(temp_vec))+' '+temp_str
	fo.write(temp_str+'\n')
	del temp_vec[:]

fo.close()
