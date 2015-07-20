from Tkinter import *
from std_os import files

class SettingsWindow(Toplevel):
	def __init__(self,parent):
		Toplevel.__init__(self,parent.root) #outdated style as the parent is not inheriting from some tkinter widget
		self.parent=parent
		self.body()
		Toplevel.title(self,"Settings Window")
		Toplevel.grab_set(self)
		Toplevel.protocol(self,"WM_DELETE_WINDOW",self.exit)

	def browse(self,ignore=None):
		temp=files()
		newLoc=temp.askfolder(self.entryLocation.get())
		if(len(newLoc)==0):return
		self.entryLocation.delete(0,END)
		self.entryLocation.insert(0,newLoc)
	def confirm(self):
		self.parent.location=self.entryLocation.get()
		self.parent.populateShows() #restart the process from scratch
		self.exit()
	def exit(self):
		Toplevel.grab_release(self)
		Toplevel.destroy(self)
	def body(self):
		self.labelLocation=Label(self,text="Library Location: ")
		self.entryLocation=Entry(self)
		self.entryLocation.insert(0,self.parent.location)
		self.entryLocation.bind("<Button-1>",self.browse) #use a file dialog to get the new location
		self.labelLocation.grid(row=0,column=0)
		self.entryLocation.grid(row=0,column=1)

		self.confirmButton=Button(self,text="Confirm",command=self.confirm)
		self.confirmButton.grid(row=2,column=0,columnspan=2)