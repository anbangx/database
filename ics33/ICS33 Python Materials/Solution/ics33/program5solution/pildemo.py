# This file illustrates how to use the PIL.Image class, specifically
#   i = Image.open(file-name)
#   i = Image.new ('RGB',(horizontal-pixels, vertical-pixels))
#   i.size returns (horizontal-pixels, vertical-pixels)
#   i.resize((new-horizontal-pixels,new_vertical-pixels))
#   i.paste(image, (row-in-i,column
#   i.get_pixel((x,y)) returns a (r,g,b) tuple
#   i.put_pixel((x,y),(r,g,b))


import prompt
from tkinter import Toplevel,Label,Tk,Frame,TOP,W

from PIL import Image
from PIL import ImageTk

#display_image: with window title and image to display
#This is a push function called to display any image

def display_image(title,image):
    win = Toplevel(root)
    photo = ImageTk.PhotoImage(image)
    win.title(title)
    label = Label(win,image=photo)
    label.image = photo #Kludge: keep a reference to avoid garbage collection!
    label.pack()


# Script

# Setup root
root = Tk()
root.title('main')
main = Frame(root)
main.pack(side=TOP,anchor=W)

# Get a jpg file name, get the image from it, display the image
i_file = prompt.for_string_via_index('Ener File name of JPG',legal=['uci.jpg','smallavengers.jpg'],default='uci.jpg')
i      = Image.open(i_file)
display_image('image',i)

# Get multiplicity for result image and
M = prompt.for_int('Enter # repeats in rows,columns (1-10)',default=2,is_legal=lambda x : type(x) is int and 1<=x<=10)

# Create image MxM times the original size, fill it with images, display the enlarged image
ixM = Image.new('RGB',tuple(M*x for x in i.size))
for r in range(M+1):
    for c in range(M+1):
        ixM.paste(i,(r*i.size[0],c*i.size[1]))
display_image('imageX'+str(M*M),ixM)

# Create image the same size as i, filling it with smaller multiplicities of i
i_small = i.resize(tuple(x//M for x in i.size))
i_smallxM = Image.new('RGB',i.size)
for r in range(M):
    for c in range(M):
        i_smallxM.paste(i_small,(r*i_small.size[0],c*i_small.size[1]))
display_image('image-squeezed'+str(M*M),i_smallxM)

# Play around with the pixels in an image
for y in range(i.size[0]):
    for x in range(i.size[1]):
        r,b,g = i.getpixel((y,x))
        i.putpixel((y,x),(g,r,b))
display_image('pixelated',i)


root.mainloop()  