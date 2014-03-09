#Use only functions supplied by tkinter and the controller

from tkinter import *

#imports: for all rows
import photomosaicmvc.controller as pmc
 

standard_width = 50
cr             =  0  #incremented before each new row in the main grid


#Lay out the main GUI (Phone) left->right as two frames and a Button
root = Tk()
root.title("Photomosaic")

main = Frame(root)
main.pack(side=TOP,anchor=W)

#Row 1: Metric Choices
cr+=1
pmc.metric_entry(main,width=standard_width).grid(row=cr,column=2,sticky=W)   #Caution must come before metric_option
pmc.metric_option (main)                   .grid(row=cr,column=1,sticky=EW)  #See code in controller.__init__
pmc.compose_option(main)                   .grid(row=cr,column=3,sticky=EW)
pmc.compose_button(main,text="Compose")    .grid(row=cr,column=4)


#Row 2: Database Picture Width/Height
cr+=1
Label(main,text="Database Picture Width x Height").grid(row=2,column=1)
f = Frame(main)
f.grid(row=cr,column=2,sticky=W)
pmc.width_spinbox(f,width=4).pack(side=LEFT)
Label(f,text="x").pack(side=LEFT)
pmc.height_spinbox(f,width=4).pack(side=LEFT)
Label(f,text="0x0 means None").pack(side=LEFT)
pmc.change_metric_button(main,text="Change Metric").grid(row=cr,column=3)


#Row 3: Database Loading (must be in this order: entry before option)
cr+=1
pmc.db_entry(main,width=standard_width) .grid(row=cr,column=2,sticky=W)
pmc.db_option(main)                     .grid(row=cr,column=1,sticky=EW)
pmc.db_load_button(main,text="Load")    .grid(row=cr,column=3)
pmc.db_append_button(main,text="Append").grid(row=cr,column=4)


# #Row 4: Sorting Database
# cr+=1
# pmc.sort_setup_button(main,text="Sort Setup").grid(row=cr,column=1,sticky=EW)
# pmc.sort_entry(main,width=standard_width)    .grid(row=cr,column=2,sticky=W)
# pmc.sort_button(main,text="Sort")            .grid(row=cr,column=3)
 
#Row 5: Photomosaic (must be in this order: entry before option)
cr+=1
pmc.photomosaic_entry(main,width=standard_width).grid(row=cr,column=2,sticky=W)
pmc.photomosaic_option(main)                    .grid(row=cr,column=1,sticky=EW)
pmc.photomosaic_load_button(main,text="Load")   .grid(row=cr,column=3)
pmc.photomosaic_make_button(main,text="Make")   .grid(row=cr,column=4)


# #Row 6: Filtering
# cr+=1
# pmc.filter_entry(main,width=standard_width)          .grid(row=cr,column=2,sticky=W)   #Caution must come before filter_option
# pmc.filter_option(main)                              .grid(row=cr,column=1,sticky=EW)  #See code in controller.__init__
# pmc.filter_db_button(main,text="Filter DB")          .grid(row=cr,column=3)
# pmc.filter_picture_button(main,text="Filter Picture").grid(row=cr,column=4)


#Row 7: Progress/Messages
pmc.message_textbox(root).pack(side=TOP,expand=YES,fill=X)