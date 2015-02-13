from BaseWindow import BaseWindow
from sys import path

temp=open(path[0]+'/settings.info','r')

location=''
for x in temp: #for every line
	if len(x.split('='))<2:continue
	if x.split('=')[0]=='location':
		location=x.split('=')[1][:-1]

BaseWindow(location)
