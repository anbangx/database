from tkinter import *



#ScrolledText: with width in characters and height in lines
#add is a push function called to add a line to the text
#lines are numbered for clarity

class ScrolledText45(Frame):
    def __init__(self,parent,width=80,height=30,**config):
        Frame.__init__(self,parent,**config)
        self.parent=parent
        self.row = 1
        sbar = Scrollbar(self)
        text = Text(self,width=width,height=height)
        sbar.config(command=text.yview)
        text.config(yscrollcommand=sbar.set)
        sbar.pack(side=RIGHT,fill=Y)
        text.pack(side=LEFT,expand=YES,fill=BOTH)
        self.text = text
      
    def add(self,text):
        self.text.insert(str(self.row)+".0",str(self.row)+": "+text+"\n")
        self.text.see(str(self.row)+".0")
        self.row += 1