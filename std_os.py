#! /env/python
import os
import math #just so i can use the dang floor function
import tkFileDialog

class output:
	def table(self,rows,width):
		#makes a table at regular column widtsh and is dynamic to length of info in each column and row
		for row in rows: #for each row...
			output="" #is what is added to for printing
			for data in row: #for each column data for that row...
				output+=data #put that data to the output
				temp=math.floor(len(data)/8) #get a tab length of the data
				while(temp<width): #dynamicly add tabs untill it is brought to the column width specified
					output+="\t" #add a tab since it is not wide wnough yet
					temp+=1 #specify that the tab width has increased by 1 since we just added a tab
			print output #print this row and then repeat

class files:
	
	directoryTree=[] #holds all the found files, and only the files specified. This means that filters apply on this
			 #this gets reset for every new "walk". save this externally is you want to remember it
	folders=[]
	types={ "video":["avi","mkv","flv","wmv","mp4","mpeg","mpg","xvid","divx"],
		"audio":["mp3","flac","ogg","wma","aac","wav"],
		"programming":["py","c","cpp","h","pyc","jar","json","html","htm","cfg","ino","pde","css","ini","desktop"],
		"data":["config","xml","srt","part","exe","iso","md","so","o","csv","cbp","depend","layout","dll","lib","dat","bdc","d",
			"pbm","ppm","pil","bdf","pm","icns","pem","manifest","bin","inf","out","lck","mus","uid","old","lang","mca","prop",
			"pls","mcmeta","mrot"],
		"image":["jpg","jpeg","png","gif","bmp","dib","ico"],
		"text":["txt","nfo","readme","info","log","properties","conf","recipes"],
		"office":["doc","docx","ppt","pptx","odt","pdf","odp"],
		"archive":["zip","gz","bz","cbz"],
		"other":["xml~","py~","pth",None,"pl~","bak","gcode~","disabled","lock","tcn"],
		"modeling":["stl","gcode","dwg","scad"]
		} #oh my god, that is a lot...............
	
	def file_extension(self,filename):
		#returns the file extension, such as avi
		return filename.split('.')[-1] #return the last part of the name after the dot
	def filetype(self,filename):
		#return the type it is as defined in the dictionary above
		#typical returns are video or audio
		extension=self.file_extension(filename) #get the extension to check what type it is
		for x in self.types: #seperate out each possible type
			if extension in self.types[x]: return x #see if the extension is in that type definition
		return "unknown" #it has not been defined, so, say we don't know
	def filename_noExtension(self,filename):
		for x in xrange(1,len(filename)): #for the length of the filename, count up to use going backwards
			if filename[-x]==".":return filename[:-x] #if it is the period ending the extension, retun the extension
	
	def walk_files_print(self,top,space=" "):
		#does the same as walk_files but prints what it is doing
		#after you can call tool.directoryTree to have access to a list of files
		self.directoryTree=[]
		self.__walk_files_print(top, space)
		return self.directoryTree
	def __walk_files_print(self,top,space):
		#workhorse for walk_files_print
		for x in os.walk(top):
			for dirs in x[1]:
				print space+'**'+dirs
				self.__walk_files_print(top+'/'+dirs,space+"  ")
			for files in x[2]:
				print space+files
				self.directoryTree.append([top,files])
			break
	
	def walk_files(self,top):
		#DEPRICATED
		#walkes the directory given into 'top' and recursivly catalogs ALL the files
		#after you can call tool.directoryTree to have access to a list of files
		#tool.directoryTree is a list of files in the form [ [top,filename] , [top,filename] ]
		self.directoryTree=[]
		self.__walk_files(top)
		return self.directoryTree
	def __walk_files(self,top):
		#workhorse for walk_files
		for x in os.walk(top):
			for dirs in x[1]:
				self.__walk_files(top+'/'+dirs)
			for files in x[2]:
				self.directoryTree.append([top,files])
			break

	def walk_files_ofType(self,top,typeWanted):
		#DEPRICATED
		# does the same thing as walk_files but filters by filetype
		#after you can call tool.directoryTree to have access to a list of files
		self.directoryTree=[]
		self.__walk_files_ofType(top,typeWanted)
		return self.directoryTree
	def __walk_files_ofType(self,top,typeWanted):
		#workhorse for walk_files_ofType
		for x in os.walk(top):
			for dirs in x[1]:
				self.__walk_files_ofType(top+'/'+dirs,typeWanted)
			for files in x[2]:
				if self.filetype(files)==typeWanted:
					self.directoryTree.append([top,files])
			break

	def walk(self,top,typeWanted=None,output=False,ignoreDirs=[],readHidden=False,recursive=True):
		#the new and improved file walk mehod which allows mixing and matching all the options i had before
		#after you can call tool.directoryTree to have access to a list of files in the form [location,filename]
		self.directoryTree=[] #clear the directoryTree incase it has been populated once
		self.folders=[]
		self.__walk(top,typeWanted,output,ignoreDirs,readHidden,recursive,space=" ") #do the work
		return self.directoryTree #just in case someone doesn't know the wonder of just using the variable from here
	def __walk(self,top,typeWanted,output,ignoreDirs,readHidden,recursive,space=" "):
		#workhorse for walk. do not call unless you know what you are doing!
		for x in os.walk(top): #grab the info from os.walk in the only way i know how to propperly...
			for dirs in x[1]: #for every directory there is reported
				if dirs[0]==".": #if the folder is hidden...
					if readHidden==False:continue #and is told to NOT read hidden, then skip it
				if output:
					print space+"**"+dirs #if output is not False then print everything it is doing
					if dirs in ignoreDirs: #if told not to go into this directory...
						print space+"  "+"Skipping Directory!"+top+"/"+dirs #say that it got skipped and...
						continue #then move to the next folder.
				if dirs in ignoreDirs: #if told not to go into this directory and wasn't intercepted by the output sequence
					print " "+"Skipping Directory! "+top+"/"+dirs #tell about skipping the folder
					continue #and then skip to the next one
				#if the folder has not been skipped then descend into it recursivly
				self.folders.append([top,dirs])
				if recursive==True:self.__walk((top+"/"+dirs),typeWanted,output,ignoreDirs,readHidden,recursive,(space+"  "))
			for files in x[2]: #for every file reported
				if files[0]==".": #if it is a hidden file...
					if readHidden==False:continue #skip it if told to NOT read it
				if output: print space+files #verbose output of what is happening if 'output' is NOT False
				if not typeWanted==None: #if they specified a filetype to find...
					if self.filetype(files)==typeWanted: #if the type wanted is the file's type...
						self.directoryTree.append([top,files]) #add it to our list
						continue #just found where it belongs, so for efficency, stop the checking and continue
					continue #dont add the file as underneith as we just checked weither to add it now
				self.directoryTree.append([top,files]) #the generic add everything to our list line for if not using filters
			break

	def askfile(self,top=None):
		if top==None:
			return tkFileDialog.askopenfilenames()
		else:
			return tkFileDialog.askopenfilenames(initialdir=top)
	def askfolder(self,top=None):
		if top==None:
			return tkFileDialog.askdirectory()
		else:
			return tkFileDialog.askdirectory(initialdir=top)

	def rename(self,inputFile,outputFile):
		if type(inputFile)==list:
			inputFile=inputFile[0]+'/'+inputFile[1]
		if type(outputFile)==list:
			outputFile=outputFile[0]+'/'+outputFile[1]
		os.rename(inputFile,outputFile)

if __name__ == "__main__":
	print "This is a library for doing some functions that \ni was constantly reimplenting in other programms"
	print
	print "To use this simply make the object such as 'tool=std_os.files()'"
	print "then simply use 'tool.walk(top)' to parse the directory for all files in 'top'"
	print "after it has finished, you can acces the files via 'tool.directoryTree'"
	print
	print "There is also a small ouput class that will do some basic formated output"
	print "this small class is under develoupment and may be removed in the future"
	print
	print
	print "Thanks for using Alex's Standard OS Modual"
