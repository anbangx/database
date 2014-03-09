#tkinter imports
from tkinter import Entry, Button, Spinbox
from tkinter.filedialog import askopenfilename
import _thread as thread

#controller class imports
from photomosaicmvc.controller.optionmenu45   import OptionMenu45, OptionMenu45Whiten, OptionMenu45ToEntry
from photomosaicmvc.controller.entryaskopen   import FileEnabledEntry45, FolderEnabledEntry45
from photomosaicmvc.controller.scrolledtext45 import ScrolledText45

#view function import
from photomosaicmvc.view.displayimage import display_image 

#model function imports
from photomosaicmvc.model import metric_options_list
from photomosaicmvc.model import compose_options_list
from photomosaicmvc.model import filter_options_list
from photomosaicmvc.model import db_options_list
from photomosaicmvc.model import photomosaic_options_list

from photomosaicmvc.model.metrics     import * #direct access to all; for use in eval
from photomosaicmvc.model.dbtiles     import * #direct access to all; for use in eval
from photomosaicmvc.model.photomosaic import * #direct access to all; for use in eval
from photomosaicmvc.model.filters     import * #direct access to all; for use in eval


# Names storing current database of pictures and photomosaic to make
db = None
pm = None

#Textbox Information

#Widget(s) to remember (needed in other widgets)
the_message_textbox = None

def message_textbox(parent,**config):
    global the_message_textbox
    the_message_textbox = ScrolledText45(parent,**config)
    return the_message_textbox


#Metric Information

#Widget(s) to remember (needed in other widgets)
the_metric_entry   = None
the_compose_option = None


def metric_option(parent,**config):
    return OptionMenu45ToEntry(parent,"Choose Metric",metric_options_list,the_metric_entry,**config)


def metric_entry(parent,**config):
    global the_metric_entry
    the_metric_entry = Entry(parent,**config)
    the_metric_entry.bind("<KeyRelease>",lambda event : the_metric_entry.config(bg="white"))
    return the_metric_entry


def compose_option(parent,**config):
    global the_compose_option
    the_compose_option = OptionMenu45(parent,"Choose Function",compose_options_list,**config)
    return the_compose_option

def compose_button(parent,**config):
    def compose():
        if the_metric_entry.get() == "":
            the_message_textbox.add("Error: Must choose metric to compose first")
        elif the_compose_option.get() == "Choose Function":
            the_message_textbox.add("Error: Must choose composition function first")
        else:
            the_metric_entry.insert(0,the_compose_option.get()+"(")
            the_metric_entry.insert(len(the_metric_entry.get()),")")
            the_metric_entry.config(bg="white")
    return Button(parent,command=compose,**config)



#Picture/Metric Information

#Widget(s) to remember (needed in other widgets)
the_width_spinbox        = None
the_height_spinbox       = None

def width_spinbox(parent,**config):
    global the_width_spinbox
    the_width_spinbox = Spinbox(parent,from_=0,to=100,**config)
    return the_width_spinbox

def height_spinbox(parent,**config):
    global the_height_spinbox
    the_height_spinbox = Spinbox(parent,from_=0,to=100,**config)
    return the_height_spinbox

def change_metric_button(parent,**config):
    def change():
        if db == None:
            the_message_textbox.add("Error: Must load database pictures first")
        elif the_metric_entry.get() == "":
            the_message_textbox.add("Error: Must specify valid metric first")
        else:
            metric = eval(the_metric_entry.get())
            db.change_metric(metric)
            the_metric_entry.config(bg="green")

    return Button(parent,command=change,**config)



#Database Information

#Widget(s) to remember (needed in other widgets)
the_db_option = None
the_db_entry  = None


def db_option(parent,**config):
    global the_db_option
    the_db_option = OptionMenu45Whiten(parent,"Choose Database Class",db_options_list,the_db_entry,**config)
    return the_db_option


def db_entry(parent,**config):
    global the_db_entry
    the_db_entry = FolderEnabledEntry45(parent,**config)
    return the_db_entry


def db_load_button(parent,**config):
    import glob
    def load_from_scratch():
        try:
            global db
            if the_db_option.get() == "Choose Database Class":
                the_message_textbox.add("Error: Must specify Database option first")
            elif the_metric_entry.get() == "":
                the_message_textbox.add("Error: Must specify Metric first")
            elif the_db_entry.get() == "":
                the_message_textbox.add("Error: Must specify files to use first")
            else: 
                pixel_size  = eval(the_width_spinbox.get()+","+the_height_spinbox.get())
                if pixel_size == (0,0):
                    pixel_size = None
                metric      = eval(the_metric_entry.get())
                pixel_files = glob.glob(the_db_entry.get())
                if pixel_files == []:
                    raise Exception # only exception? avoid try/except
                db          = eval(the_db_option.get())(pixel_size,metric)
                the_db_entry.config(bg="green")
                the_metric_entry.config(bg="green")
                db.load(pixel_files,True)
                db.display()
                the_message_textbox.add("Database files read")
        except:
            the_message_textbox.add("Database files unknown")
    return Button(parent,command=load_from_scratch,**config)


def db_append_button(parent,**config):
    import glob
    def append():
        if db == None:
            the_message_textbox.add("Error: Must have already loaded a Database of pictures first")
        elif the_db_entry.get() == "":
            the_message_textbox.add("Error: Must specify files to use first")
        else: 
            pixel_files = glob.glob(the_db_entry.get())
            db.load(pixel_files,False)
            the_db_entry.config(bg="green")
            db.display()
    return Button(parent,command=append,**config)



#Sort Information

#Widget(s) to remember (needed in other widgets)
the_sort_entry = None


def sort_setup_button(parent,**config):
    def setup():
        the_sort_entry.delete(0,len(the_sort_entry.get()))
        the_sort_entry.insert(0,"lambda x : ")
        the_sort_entry.config(bg="white")
        the_sort_entry.focus_set()
    return Button(parent,command=setup,**config)


def sort_entry(parent,**config):
    global the_sort_entry
    the_sort_entry = Entry(parent,**config)
    the_sort_entry.bind("<KeyRelease>",lambda event : the_sort_entry.config(bg="white"))
    return the_sort_entry


def sort_button(parent,**config):
    def sort():
        if db == None:
            the_message_textbox.add("Error: Must load Database of pictures first")
        elif the_sort_entry.get() == "":
            the_message_textbox.add("Error: Must specify valid lambda first")
        else:
            db.sort(eval(the_sort_entry.get()))
            the_sort_entry.config(bg="green")
            db.display()
    return Button(parent,command=sort,**config)



#Photomosaic Information

#Widget(s) to remember (needed in other widgets)
the_photomosaic_option = None
the_photomosaic_entry  = None


def photomosaic_option(parent,**config):
    global the_photomosaic_option
    the_photomosaic_option = OptionMenu45Whiten(parent,"Choose Photomosaic Class",photomosaic_options_list,the_photomosaic_entry,**config)
    return the_photomosaic_option


def photomosaic_entry(parent,**config):
    global the_photomosaic_entry
    the_photomosaic_entry = FileEnabledEntry45(parent,**config)
    return the_photomosaic_entry


def photomosaic_load_button(parent,**config):
    def load_picture():
        try:
            global pm
            if the_photomosaic_option.get() == "Choose Photomosaic Class":
                the_message_textbox.add("Error: Must specify Photomosaic option first")
            elif the_photomosaic_entry.get() == "":
                the_message_textbox.add("Error: Must specify Photomosaic file first")
            else:
                pm = eval(the_photomosaic_option.get())(the_photomosaic_entry.get())
                the_photomosaic_entry.config(bg="green")
                display_image("Picture",pm.image)
                the_message_textbox.add("Photomosaic file read")
        except:
            the_message_textbox.add("Photomosaic file unknown")
    return Button(parent,command=load_picture,**config)


def photomosaic_make_button(parent,**config):
    def make():
        if db == None:
            the_message_textbox.add("Error: Must load Database of pictures first")
        elif pm == None:
            the_message_textbox.add("Error: Must load picture first")
        else:
            def todo():
                photomosaic = pm.make(db)
                # Don't worry about these syntax errors!
                photomosaicmvc.view.root.after(0,display_image,"Photomosaic",photomosaic)
                photomosaicmvc.view.root.after(0,photomosaic.save,"photomosaic.jpg")
            thread.start_new_thread(todo, ())
    return Button(parent,command=make,**config)


#Filter Information

#Widget(s) to remember (needed in other widgets)
the_filter_entry = None

def filter_option(parent,**config):
    return OptionMenu45ToEntry(parent,"Choose Filter",filter_options_list,the_filter_entry,**config)


def filter_entry(parent,**config):
    global the_filter_entry
    the_filter_entry = Entry(parent,**config)
    the_filter_entry.bind("<KeyRelease>",lambda event : the_filter_entry.config(bg="white"))
    return the_filter_entry


def filter_db_button(parent,**config):
    def filter_db():
        if db == None:
            the_message_textbox.add("Error: Must load Database of pictures first")
        elif the_filter_entry.get() == "":
            the_message_textbox.add("Error: Must specify valid filter first")
        else:
            f = eval(the_filter_entry.get())
            db.filter(f)
            db.display()
            the_filter_entry.config(bg="green")
    return Button(parent,command=filter_db,**config)


def filter_picture_button(parent,**config):
    def filter_picture():
        if pm == None:
            the_message_textbox.add("Error: Must load picture first")
        elif the_filter_entry.get() == "":
            the_message_textbox.add("Error: Must specify valid filter first")
        else:
            f = eval(the_filter_entry.get())
            pm.filter(f)
            display_image("Picture",pm.image)
            the_filter_entry.config(bg="green")
    return Button(parent,command=filter_picture,**config)





