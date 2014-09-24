from Tkinter import *
from os import walk
import os
import shutil
from errorWindow import PopupErrorBox
from ConfirmationWindow import confirmationWindow

class BaseWindow:
	shows=[]
	def  __init__(self,location):
		self.root=Tk()
		self.location=location

		self.body()
		self.populateShows()

		self.root.rowconfigure(20,weight=1)
		self.root.columnconfigure(0,weight=1)
		self.root.columnconfigure(2,weight=1)
		self.root.columnconfigure(4,weight=1)

		self.root.protocol("WM_DELETE_WINDOW",self.delete)
		self.root.title("Library Renamer")
		self.root.mainloop()

	def body(self):
		self.elements={}

		self.elements['headingShow']=Label(self.root,text="Show List")
		self.elements['headingShow'].grid(row=0,column=0)
		self.elements['showList']=Listbox(self.root,selectmode=EXTENDED,exportselection=0)
		self.elements['showList'].grid(row=1,column=0,rowspan=20,sticky=N+E+S+W)
		self.elements['showNameEntry']=Entry(self.root)
		self.elements['showNameEntry'].grid(row=21,column=0,sticky=E+W)
		self.elements['showRenameButton']=Button(self.root,text="Rename Show",command=self.renameShow_call)
		self.elements['showRenameButton'].grid(row=22,column=0)
		self.elements['addFilesToShowButton']=Button(self.root,text="Add Files To Show")
		self.elements['addFilesToShowButton'].grid(row=23,column=0)
		self.elements['showList'].bind("<Button-1>",self.showInfo_call)
		self.elements['showList'].bind("<Key>",self.showInfo_call)

		self.elements['headingSeason']=Label(self.root,text="Season List")
		self.elements['headingSeason'].grid(row=0,column=2)
		self.elements['seasonsList']=Listbox(self.root,selectmode=EXTENDED,exportselection=0)
		self.elements['seasonsList'].grid(row=1,column=2,rowspan=20,sticky=N+E+S+W)
		self.elements['seasonNameEntry']=Entry(self.root)
		self.elements["seasonNameEntry"].grid(row=21,column=2,sticky=E+W)
		self.elements['seasonsRenameButton']=Button(self.root,text="Rename Season",command=self.renameSeason_call)
		self.elements['seasonsRenameButton'].grid(row=22,column=2)
		self.elements['addFilesToSeasonButton']=Button(self.root,text="Add Files To Season",command=self.moveFilesToSeason_call)
		self.elements['addFilesToSeasonButton'].grid(row=23,column=2)
		self.elements['seasonsList'].bind("<Button-1>",self.seasonInfo_call)
		self.elements['seasonsList'].bind("<Key>",self.seasonInfo_call)

		self.elements['headingEpisodes']=Label(self.root,text="Episode List")
		self.elements['headingEpisodes'].grid(row=0,column=4)
		self.elements['episodesList']=Listbox(self.root,selectmode=EXTENDED,exportselection=0)
		self.elements['episodesList'].grid(row=1,column=4,rowspan=20,sticky=N+E+S+W)
		self.elements['episodesMovetoSeasonButton']=Button(self.root,text="Move To Season",command=self.moveFilesFromSeason_call)
		self.elements['episodesMovetoSeasonButton'].grid(row=21,column=4)

		self.elements['showNameManualLabel']=Label(self.root,text="Show Name")
		self.elements['showNameManualLabel'].grid(row=1,column=6)
		self.elements['showNameManualEntry']=Entry(self.root)
		self.elements['showNameManualEntry'].grid(row=2,column=6)
		self.elements['seasonStartNumberLabel']=Label(self.root,text="Season Starting Number")
		self.elements['seasonStartNumberLabel'].grid(row=3,column=6)
		self.elements['seasonStartNumberEntry']=Entry(self.root)
		self.elements['seasonStartNumberEntry'].grid(row=4,column=6)
		self.elements['episodesStartNumberLabel']=Label(self.root,text="Episode Starting Number")
		self.elements['episodesStartNumberLabel'].grid(row=5,column=6)
		self.elements['episodesStartNumberEntry']=Entry(self.root)
		self.elements['episodesStartNumberEntry'].grid(row=6,column=6)

		self.elements['moveUpEpisode']=Button(self.root,text="Move Up Episode(s)",command=self.moveEpisodesUp)
		self.elements['moveUpEpisode'].grid(row=10,column=6)
		self.elements['moveIgnoreEpisode']=Button(self.root,text="Ignore Episode(s)",command=self.moveEpisodesIgnore)
		self.elements['moveIgnoreEpisode'].grid(row=11,column=6)
		self.elements['moveDownEpisode']=Button(self.root,text="Move Down Episode(s)",command=self.moveEpisodesDown)
		self.elements['moveDownEpisode'].grid(row=12,column=6)

		self.elements["convertButton"]=Button(self.root,text="Rename All The Season",command=self.massRenameSeason)
		self.elements['convertButton'].grid(row=21,column=6)

		self.elements['settingsButton']=Button(self.root,text="Change Settings")
		self.elements['settingsButton'].grid(row=23,column=6)

	def populateShows(self):
		files=[]
		folders=[]
		BaseWindow.shows=[]
		BaseWindow.seasons=[]
		BaseWindow.episodes=[]
		for x in walk(self.location):
			folders=x[1]
			files=x[2]
			break

		#clean the mess that may exist here
		self.elements['showList'].delete(0,END)
		self.elements['seasonsList'].delete(0,END)
		self.elements['episodesList'].delete(0,END)
		self.curtshow=None #so we don't change a show we haven't clicked yet
		self.curtseason=None #so that we cant change a season that we don't want
		self.elements['showNameEntry'].delete(0,END)
		self.elements['seasonNameEntry'].delete(0,END)

		folders.sort()
		for x in folders:
			self.elements['showList'].insert(END,x)
			BaseWindow.shows.append({'name':x,'type':'folder'})
		files.sort()
		for x in files:
			self.elements['showList'].insert(END,x)
			self.elements['showList'].itemconfig(END,bg='red')
			BaseWindow.shows.append({'name':x,'type':'file'})
	def populateSeasons(self,showName):
		files=[]
		folders=[]
		BaseWindow.seasons=[]
		BaseWindow.episodes=[]
		for x in walk(self.location+'/'+showName):
			folders=x[1]
			files=x[2]
			break

		#clean the mess that may exist here
		self.elements['seasonsList'].delete(0,END)
		self.elements['episodesList'].delete(0,END)
		self.curtseason=None #so we don't change a season we haven't clicked yet
		self.elements['seasonNameEntry'].delete(0,END)

		folders.sort()
		for x in folders:
			self.elements['seasonsList'].insert(END,x)
			BaseWindow.seasons.append({'name':x,'type':'folder'})
		files.sort()
		for x in files:
			self.elements['seasonsList'].insert(END,x)
			self.elements['seasonsList'].itemconfig(END,bg='red')
			BaseWindow.seasons.append({'name':x,'type':'file'})

	def populateEpisodes(self,seasonName):
		files=[]
		folders=[]
		BaseWindow.episodes=[]
		for x in walk(self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+seasonName):
			folders=x[1]
			files=x[2]
			break

		self.elements['episodesList'].delete(0,END)
		folders.sort()
		for x in folders:
			self.elements['episodesList'].insert(END,x)
			self.elements['episodesList'].itemconfig(END,bg='red')
			BaseWindow.episodes.append({'name':x,'type':'folder'})
		files.sort()
		for x in files:
			self.elements['episodesList'].insert(END,x)
			BaseWindow.episodes.append({'name':x,'type':'file'})
	def redrawEpisodesList(self):
		self.elements['episodesList'].delete(0,END)
		for x in BaseWindow.episodes:
			self.elements['episodesList'].insert(END,x['name'])
			if x['type']=='folder':self.elements['episodesList'].itemconfig(END,bg='red')

	def showInfo_call(self,ignore=''):self.root.after(1,self.showInfo)
	def showInfo(self,ignore=""):
		index=0
		try: index=int(self.elements['showList'].curselection()[0])
		except:pass
		self.curtshow=index
		if BaseWindow.shows[index]['type']=='folder':self.populateSeasons(self.shows[index]['name'])
		else:
			#clean the mess that may exist here
			self.elements['seasonsList'].delete(0,END)
			self.elements['episodesList'].delete(0,END)
			self.curtseason=None #so that we cant change a season that we don't want
			self.elements['seasonNameEntry'].delete(0,END)
		self.elements['showNameEntry'].delete(0,END)
		self.elements['showNameEntry'].insert(0,BaseWindow.shows[index]['name'])

	def seasonInfo_call(self,ignore=''):self.root.after(1,self.seasonInfo)
	def seasonInfo(self,ignore=''):
		index=0
		try: index=int(self.elements['seasonsList'].curselection()[0])
		except:pass
		self.curtseason=index
		if BaseWindow.seasons[index]['type']=='folder':self.populateEpisodes(self.seasons[index]['name'])
		else:
			#clean the mess that may exist here
			self.elements['episodesList'].delete(0,END)
		self.elements['seasonNameEntry'].delete(0,END)
		self.elements['seasonNameEntry'].insert(0,BaseWindow.seasons[index]['name'])

	def renameShow_call(self,ignore=''):self.root.after(1,self.renameShow)
	def renameShow(self,ignore=''):
		newName=self.elements['showNameEntry'].get()
		os.rename(self.location+'/'+BaseWindow.shows[self.curtshow]['name'],self.location+'/'+newName)
		self.populateShows()

	def renameSeason_call(self,ignore=''):self.root.after(1,self.renameSeason)
	def renameSeason(self,ignore=''):
		newName=self.elements['seasonNameEntry'].get()
		os.rename(self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+BaseWindow.seasons[self.curtseason]['name'],self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+newName)
		self.populateSeasons(BaseWindow.shows[self.curtshow]['name'])

	def moveFilesToSeason_call(self,ignore=""):self.root.after(1,self.moveFilesToSeason)
	def moveFilesToSeason(self,ignore=""):
		seasonName=self.elements['seasonNameEntry'].get()
		for x in self.elements['seasonsList'].curselection():
			index=(int(x))
			src=self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+BaseWindow.seasons[index]['name']
			dest=self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+seasonName
			if not os.path.isdir(dest):os.makedirs(dest)
			shutil.move(src,dest)
		self.populateSeasons(BaseWindow.shows[self.curtshow]['name'])
	def moveFilesFromSeason_call(self,ignore=""):self.root.after(1,self.moveFilesFromSeason)
	def moveFilesFromSeason(self,ignore=""):
		print self.elements['episodesList'].curselection()
		for x in self.elements['episodesList'].curselection():
			index=(int(x))
			src=self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+BaseWindow.seasons[self.curtseason]['name']+'/'+BaseWindow.episodes[x]['name']
			dest=self.location+'/'+BaseWindow.shows[self.curtshow]['name']
			shutil.move(src,dest)
		self.populateSeasons(BaseWindow.shows[self.curtshow]['name'])

	def moveEpisodesUp(self,ignore=''):
		indexes=[]
		for x in self.elements['episodesList'].curselection():
			indexes.append(int(x))
		#we have the indecies already in assending order
		if indexes[0]==0: return
		for x in indexes:
			temp=BaseWindow.episodes[x-1]
			BaseWindow.episodes[x-1]=BaseWindow.episodes[x]
			BaseWindow.episodes[x]=temp
		self.redrawEpisodesList()
		for x in indexes:
			self.elements['episodesList'].selection_set(x-1) #reselects the old items so we can mash the upp button
	def moveEpisodesDown(self,ignore=''):
		indexes=[]
		for x in self.elements['episodesList'].curselection():
			indexes.append(int(x))
		indexes.sort(key=lambda x: 99999-x) #sort into desending order so we move from bottom up
		if indexes[-1]==len(BaseWindow.episodes)-1: return
		for x in indexes:
			temp=BaseWindow.episodes[x+1]
			BaseWindow.episodes[x+1]=BaseWindow.episodes[x]
			BaseWindow.episodes[x]=temp
		self.redrawEpisodesList()
		for x in indexes:
			self.elements['episodesList'].selection_set(x+1) #reselects the old items so we can mash the down button
	def moveEpisodesIgnore(self,ignore=''):
		indexes=[]
		for x in self.elements['episodesList'].curselection():
			indexes.append(int(x))
		indexes.sort(key=lambda x: 99999-x) #sort into desending order so we move from bottom up
		for x in indexes:
			del BaseWindow.episodes[x]
		self.redrawEpisodesList()

	def massRenameSeason(self,ignore=""):
		#series name
		show=self.elements['showNameManualEntry'].get()
		if len(show)==0:show=BaseWindow.shows[self.curtshow]['name']
		#season number
		season=self.elements['seasonStartNumberEntry'].get()
		if len(season)==0:
			season=self.elements['seasonNameEntry'].get().split()[-1]
		if len(season)==0:
			season=1
		try:
			season=int(season)
		except:
			PopupErrorBox(self.root,"Unable to use \n"+str(season)+"\nas the season NUMBER")
			return
		#episode number
		episode=self.elements['episodesStartNumberEntry'].get()
		if len(episode)==0:
			episode=1
		try:
			episode=int(episode)
		except:
			PopupErrorBox(self.root,"Unable to use \n"+str(episode)+"\nas the episode NUMBER")
			return

		#get the folder location
		base=self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/'+BaseWindow.seasons[self.curtseason]['name']+'/'
		print base

		#make the episode names
		names=[]
		info=open(self.location+'/'+BaseWindow.shows[self.curtshow]['name']+'/changes.info','a')
		for x in BaseWindow.episodes:
			if x["type"]=='folder':continue
			names.append([x["name"],show+'.s'+self.__zeroPad(season,5)+'e'+self.__zeroPad(episode,5)+'.'+self.__filetype(x['name'])])
			print names[-1][0]+'  -->  '+names[-1][1]
			info.write(names[-1][0]+'  -->  '+names[-1][1]+'\n')
			episode+=1
		info.close()

		#confirmation Window Here
		confirm=confirmationWindow(self.root,names)
		if confirm.result==False or confirm.result==None: return

		for x in names:
			shutil.move(base+x[0],base+x[1])
		self.populateEpisodes(BaseWindow.seasons[self.curtseason]['name'])
	def __zeroPad(self,number,places):
		#makes a zero-padded number in string form with the total length of places
		#EXAMPLE: number=27 places=6  returns  "000027"
		number=str(number)
		while len(number)<places: number='0'+number
		return number
	def __filetype(self,name):
		#returns things like mkv or avi
		return name.split('.')[-1]

	def delete(self,ignore=""):
		self.root.quit()