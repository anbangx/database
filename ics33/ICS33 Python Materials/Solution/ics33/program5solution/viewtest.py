#Use only functions supplied by tkinter and the controller

from tkinter import *
#from photomosaicmvc.controller import number_button, place_call_button, clear_button, special_entry


#Lay out the main GUI (Phone) left->right as two frames and a Button
root = Tk()
root.title("Photomosaic")

main = Frame(root)
main.pack(side=TOP,anchor=W)

#Row 1
metric = StringVar()
metric.set("Metric")
OptionMenu(main, metric,  *["Average_BW","Average_RGB"], command=None).grid(row=1,column=1,sticky=EW)

Entry(main,width=30).grid(row=1,column=2,sticky=W)

compose = StringVar()
compose.set("Compose")
OptionMenu(main, compose, "Quad_Metric", *[],command=None).grid(row=1,column=3,sticky=EW)
Button(main,text="Compose").grid(row=1,column=4)


#Row 2
Label(main,text="Picture Width x Height").grid(row=2,column=1)
f = Frame(main)
f.grid(row=2,column=2,sticky=W)
s=Spinbox(f,from_=0,to=100,width=4)
s.pack(side=LEFT)
Label(f,text="x").pack(side=LEFT)
Spinbox(f,from_=0,to=100,width=4).pack(side=LEFT)
Label(f,text="0x0 means None").pack(side=LEFT)

#Button(main,text="Display DB").grid(row=2,column=3)
Button(main,text="Change Metric").grid(row=2,column=3)


#Row 3
db = StringVar()
db.set("Database")
OptionMenu(main, db, "DB_Pictures", *["DB_Pictures_Used"], command=None).grid(row=3,column=1,sticky=EW)

Entry(main,width=30).grid(row=3,column=2,sticky=W)

Button(main,text="Load").grid(row=3,column=3)
Button(main,text="Append").grid(row=3,column=4)


#Row 4
pm = StringVar()
pm.set("Photomosaic")
OptionMenu(main, pm, "Photomosaic", *["Photomosaic_Limited_Repeat"], command=None).grid(row=4,column=1,sticky=EW)

Entry(main,width=30).grid(row=4,column=2,sticky=W)

Button(main,text="Make").grid(row=4,column=3)
Button(main,text="Save").grid(row=4,column=4)


#Row 5
filter = StringVar()
filter.set("Filter")
OptionMenu(main, filter, "gray_scale", *["intensity"], command=None).grid(row=5 ,column=1,sticky=EW)

Entry(main,width=30).grid(row=5,column=2,sticky=W)

Button(main,text="Filter DB").grid(row=4,column=3)
Button(main,text="Filter Picture").grid(row=4,column=4)


#Row 6
class Scrolled_Text(Frame):
  def __init__(self,parent):
    Frame.__init__(self,parent)
    self.row = 1
    sbar = Scrollbar(self)
    text = Text(self,height=10)
    sbar.config(command=text.yview)
    text.config(yscrollcommand=sbar.set)
    sbar.pack(side=RIGHT,fill=Y)
    text.pack(side=LEFT,expand=YES,fill=BOTH)
    self.text = text

  def add(self,text):
    self.text.insert(str(self.row)+".0",str(self.row)+": "+text+"\n")
    self.text.see(str(self.row)+".0")
    self.row += 1

s = Scrolled_Text(root)
s.pack(side=TOP,expand=YES,fill=X)
for i in range(100):
  s.add(str(i))

#freeze size

#Label(main,text="").grid(row=0,column=5)
#main.columnconfigure(5,weight=100)
#for r in range(1):
#  main.rowconfigure(r,weight=1)
#for c in range(5):
#  main.columnconfigure(c,weight=1)

root.mainloop()