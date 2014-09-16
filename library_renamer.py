#!/usr/bin/python
from Tkinter import *
import std_os
tool=std_os.files()
tool2=std_os.files()
tool3=std_os.files()
root=Tk()
directory=tool.askfolder("/media/RAID/Video")

#setup some stuff
tool.walk(top=directory,recursive=False)
tool.directoryTree.sort(key=lambda a: a[1])
tool.folders.sort(key=lambda a: a[1])
'''
print "**Shows in this Location**"
for dirs in tool.folders:
	print dirs[1]
'''

#boxes with the structures in them
temp_width=35 #common width for the stuffs
temp_height=25 #height for the listboxes
list_series=Listbox(root,width=35,height=temp_height)
list_series.grid(row=0,column=0,rowspan=3)
list_seasons=Listbox(root,width=20,height=temp_height)
list_seasons.grid(row=0,column=2,rowspan=3)
list_episodes=Listbox(root,width=temp_width,selectmode=EXTENDED,height=temp_height)
list_episodes.grid(row=0,column=4,rowspan=3)
del temp_height

#buttons to move selections of episodes up and down
button_up=Button(root,text="UP")
button_up.grid(row=0,column=6)
button_ignore=Button(root,text="IGNORE")
button_ignore.grid(row=1,column=6)
button_down=Button(root,text="DOWN")
button_down.grid(row=2,column=6)

#scrollbars
list_series_scroll=Scrollbar(root)
list_series_scroll.grid(row=0,column=1,rowspan=3,sticky=N+S)
list_series.config(yscrollcommand=list_series_scroll.set)
list_series_scroll.config(command=list_series.yview)
list_seasons_scroll=Scrollbar(root)
list_seasons_scroll.grid(row=0,column=3,rowspan=3,sticky=N+S)
list_seasons.config(yscrollcommand=list_seasons_scroll.set)
list_seasons_scroll.config(command=list_seasons.yview)
list_episodes_scroll=Scrollbar(root)
list_episodes_scroll.grid(row=0,column=5,rowspan=3,sticky=N+S)
list_episodes.config(yscrollcommand=list_episodes_scroll.set)
list_episodes_scroll.config(command=list_episodes.yview)

#entries and buttons for the acctual values of renaming
#this means that any changes by the user in the boxes are used
#but the boxes start with their own values as given by the folders
entry_series=Entry(root,width=temp_width)
entry_series.grid(row=3,column=0)
entry_season=Entry(root,width=20)
entry_season.grid(row=3,column=2)
entry_season.insert(0,1) #default
entry_episodeCountStart=Entry(root,width=temp_width)
entry_episodeCountStart.grid(row=3,column=4)
entry_episodeCountStart.insert(0,1) #default
button_startRename=Button(root,text="Rename")
button_startRename.grid(row=3,column=6)
Label(root,text="Series Name").grid(row=4,column=0)
Label(root,text="Season Number").grid(row=4,column=2)
Label(root,text="Episode Starting Number").grid(row=4,column=4)
del temp_width

#Functions to update the listings of the shows
def update_seasons_call(ignore=""):root.after(1,update_seasons)
def update_seasons(ignore=""):
	selection=int(list_series.curselection()[0])
	entry_series.delete(0,END)
	entry_series.insert(0,tool.folders[selection][1])
	list_seasons.delete(0,END)
	tool2.walk(tool.folders[selection][0]+"/"+tool.folders[selection][1],recursive=False)
	tool2.directoryTree.sort(key=lambda a: a[1])
	tool2.folders.sort(key=lambda a: a[1])
	for dirs in tool2.folders:
		list_seasons.insert(END,dirs[1])
	list_seasons.selection_set(0) #setup for the first folder
	update_episodes() #cascade into updateing the episodes of the first folder
list_series.bind("<Button-1>",update_seasons_call)

def update_episodes_call(ignore=""):root.after(1,update_episodes)
def update_episodes(ignore=""):
	try:
		selection=int(list_seasons.curselection()[0])
	except:
		selection=0
	list_episodes.delete(0,END)
	tool3.walk(tool2.folders[selection][0]+"/"+tool2.folders[selection][1],recursive=False,typeWanted='video')
	tool3.directoryTree.sort(key=lambda a: a[1])
	tool3.folders.sort(key=lambda a: a[1])
	for dirs in tool3.directoryTree:
		list_episodes.insert(END,dirs[1])
list_seasons.bind("<Button-1>",update_episodes_call)

#populate the root of the library and then start the cascade of updates
for dirs in tool.folders:
	#dirs comes as [top,name]	
	list_series.insert(END,dirs[1])
list_series.selection_set(0)
update_seasons() #CASCADE!! WHOO!!

#moves items in both the visable episode box and the file list
def episodes_ignore(ignore=""):
	selection=[]
	for sel in list_episodes.curselection():
		selection.append(int(sel))
	selection.reverse()
	for sel in selection:
		del tool3.directoryTree[sel]
		list_episodes.delete(sel)
button_ignore.config(command=episodes_ignore)

def episodes_up(ignore=""):
	selection=[]
	for sel in list_episodes.curselection():
		selection.append(int(sel))
	#selection.reverse()
	for sel in selection:
		try:
			list_episodes.insert(sel-1,tool3.directoryTree[sel][1])
			list_episodes.delete(sel+1)
		except:
			print "top of list"
			continue
		temp=tool3.directoryTree[sel]
		tool3.directoryTree[sel]=tool3.directoryTree[sel-1]
		tool3.directoryTree[sel-1]=temp
	#special treatment to let you keep pressing up untill it is correct
	for sel in selection:
		sel-=1
		list_episodes.selection_set(sel)
button_up.config(command=episodes_up)

def episodes_down(ignore=""):
	selection=[]
	for sel in list_episodes.curselection():
		selection.append(int(sel))
	selection.reverse()
	for sel in selection:
		try:
			list_episodes.delete(sel+1)
			list_episodes.insert(sel,tool3.directoryTree[sel+1][1])
		except: 
			print "end of list"
			break
		temp=tool3.directoryTree[sel]
		tool3.directoryTree[sel]=tool3.directoryTree[sel+1]
		tool3.directoryTree[sel+1]=temp
button_down.config(command=episodes_down)

#This is the workhorse area for renaming the files
#it attempts the format of SHOW.s001.e001.ext
def rename_call(ignore=""):root.after(1,rename)
def rename(ignore=""):
	#toplevel confirmation box
	global confirmation
	confirmation=Toplevel(root)
	confirmation.protocol("WM_DELETE_WINDOW",renameCanceled)
	button_confirm=Button(confirmation,text="Confirm",command=renameConfirmed)
	button_confirm.grid(row=0,column=0)
	button_cancel=Button(confirmation,text="Cancel",command=renameCanceled)
	button_cancel.grid(row=0,column=1)
	list_orig=Listbox(confirmation)
	list_orig.grid(row=1,column=0)
	list_new=Listbox(confirmation)
	list_new.grid(row=1,column=1)

	#lock out main window so user cant mess things up
	list_series.config(state="disabled")
	list_seasons.config(state="disabled")
	list_episodes.config(state="disabled")
	entry_series.config(state="disabled")
	entry_season.config(state="disabled")
	entry_episodeCountStart.config(state="disabled")
	button_startRename.config(state="disabled")
	button_up.config(state="disabled")
	button_ignore.config(state="disabled")
	button_down.config(state="disabled")

	#preconfigure the names to put into both the renamer function and confirmation toplevel
	tool3.directoryTreeNew=[] #will be moved to a var in the class of THIS prog, no hijacking
	SERIES=entry_series.get()
	SEASON=entry_season.get()
	EPISODE_START=entry_episodeCountStart.get()
	episodeCount=int(EPISODE_START)
	for ep in tool3.directoryTree:
		if not tool3.filetype(ep[1])=="video": continue #one last check that this is a video
		top=ep[0]
		nameDone=makeName(SERIES,SEASON,episodeCount,tool3.file_extension(ep[1]))
		tool3.directoryTreeNew.append([ep,[ep[0],nameDone]])
		episodeCount+=1
	#NOTE: tool3 has directoryTreeNew
	#it is a list of [origFile,newFile] in my standard file notation
	# so this is [ [top,name] , [top,nameNew] ]

	#put names into the confirmation
	maxWidth_new=0
	maxWidth_orig=0
	for files in tool3.directoryTreeNew:
		list_orig.insert(END,files[0][1])
		list_new.insert(END,files[1][1])
		if len(files[0][1])>maxWidth_orig:maxWidth_orig=len(files[0][1])
		if len(files[1][1])>maxWidth_new:maxWidth_new=len(files[1][1])
	#make the listboxes larger so the whole things are readable
	list_orig.config(height=len(tool3.directoryTreeNew),width=maxWidth_orig)
	list_new.config(height=len(tool3.directoryTreeNew),width=maxWidth_new)

def makeName(Series,seasonNumber,episodeNumber,extension,extra=None):
	seasonNumber=str(seasonNumber)
	while len(seasonNumber)<4:
		seasonNumber='0'+seasonNumber
	episodeNumber=str(episodeNumber)
	while len(episodeNumber)<4:
		episodeNumber='0'+episodeNumber
	if extra==None:return Series+".s"+seasonNumber+"e"+episodeNumber+"."+extension
	return Series+".s"+str(seasonNumber)+"e"+str(episodeNumber)+"."+extra+"."+extension

def renameCanceled(ignore=""):
	#gets called by the renamedConfirmed function as well so as to take care of the same unlocking stuff
	list_series.config(state="normal")
	list_seasons.config(state="normal")
	list_episodes.config(state="normal")
	entry_series.config(state="normal")
	entry_season.config(state="normal")
	entry_episodeCountStart.config(state="normal")
	button_startRename.config(state="normal")
	button_up.config(state="normal")
	button_ignore.config(state="normal")
	button_down.config(state="normal")
	global confirmation
	confirmation.destroy()

def renameConfirmed(ignore=""):
	#renaming function here
	#use the preconfigure tool3.directeryTreeNew
	output=file(tool2.folders[0][0]+"/renames.txt",'a') #backup text file for renaming that is put in the series's folder
	output.write('\n\nSeason '+entry_season.get()+'\n')
	for files in tool3.directoryTreeNew:
		print files[0][1]+' ---> '+files[1][1]+'\n'
		output.write(files[0][1]+' ---> '+files[1][1]+'\n')
		tool3.rename(files[0],files[1])
	renameCanceled()
	update_episodes()
button_startRename.config(command=rename)

#called when the program is to quit and exit
def quit(ignore=""):
	print
	print
	print "Exiting..."
	root.quit()

root.title("Glop's Library Renamer")
root.protocol("WM_DELETE_WINDOW", quit) #make it acctualy stop when closed
root.mainloop()
