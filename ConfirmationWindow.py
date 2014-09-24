from Tkinter import *

class confirmationWindow(Toplevel):
	def __init__(self,ROOT,files):
		self.files=files

		self.root=ROOT
		Toplevel.__init__(self, self.root)
		self.transient(self.root)
		self.protocol("WM_DELETE_WINDOW",self.delete)
		self.title("Error Window")
		self.result=None

		self.body()
		self.grab_set()

		self.geometry("+%d+%d" % (self.root.winfo_rootx(),self.root.winfo_rooty()))

		self.wait_window(self)

	def body(self):
		wwww=50
		hhhh=20
		self.elements={}
		self.elements['leftLabel']=Label(self,text="Original")
		self.elements['leftLabel'].grid(row=0,column=0)
		self.elements['leftList']=Listbox(self,width=wwww,height=hhhh)
		self.elements['leftList'].grid(row=1,column=0)
		self.elements['confirmButton']=Button(self,text="Confirm",command=self.confirm)
		self.elements['confirmButton'].grid(row=2,column=0)

		self.elements['rightLabel']=Label(self,text="New Names")
		self.elements['rightLabel'].grid(row=0,column=1)
		self.elements['rightList']=Listbox(self,width=wwww,height=hhhh)
		self.elements['rightList'].grid(row=1,column=1)
		self.elements['confirmButton']=Button(self,text="Cancel",command=self.cancel)
		self.elements['confirmButton'].grid(row=2,column=1)

		for x in self.files:
			self.elements['leftList'].insert(END,x[0])
			self.elements['rightList'].insert(END,x[1])

	def confirm(self):
		self.result=True
		self.delete()
	def cancel(self):
		self.result=False
		self.delete()
	def delete(self):
		self.grab_release()
		self.destroy()