from Tkinter import *

class PopupErrorBox(Toplevel):
	def __init__(self,ROOT,message):
		self.message=message

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
		self.elements={}
		self.elements['messageLabel']=Label(self,text=self.message)
		self.elements['messageLabel'].grid(row=0,column=0)
		self.elements['closeButton']=Button(self,text="Close",command=self.delete)
		self.elements['closeButton'].grid(row=1,column=0)

	def delete(self):
		self.grab_release()
		self.destroy()