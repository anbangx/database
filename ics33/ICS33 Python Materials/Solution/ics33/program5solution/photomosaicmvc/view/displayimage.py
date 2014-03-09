from tkinter import Toplevel,Label

from PIL import ImageTk

import photomosaicmvc.view  #Cannot import root explicitly, so use this form




#display_image: with window title and image to display
#This is a push function called to display any image

def display_image(title,image):
    win = Toplevel(photomosaicmvc.view.root)
    photo = ImageTk.PhotoImage(image)
    win.title(title)
    label = Label(win,image=photo)
    label.image = photo #Kludge: keep a reference to avoid garbage collection!
    label.pack()