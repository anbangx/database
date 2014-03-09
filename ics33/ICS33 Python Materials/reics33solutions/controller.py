from tkinter import Button,Label,Canvas

# import model to refer to functions that buttons call via command=...
import model


# The simulation_canvas/progress widgets, when called, set these before they return
# The model uses this information in the world and display_all functions
the_canvas   = None
the_progress = None
 
 
# Buttons/Canvas are called in the view and call methods in the model 
def reset_button  (parent,**config):
    return Button(parent,command=model.reset,**config)
 
  
def start_button  (parent,**config):
    return Button(parent,command=model.start,**config)
 
  
def stop_button  (parent,**config):
    return Button(parent,command=model.stop,**config)
 
  
def step_button  (parent,**config):
    return Button(parent,command=model.step,**config)
 
  
def object_button  (parent,**config):
    def doit():
        model.select_object(config['text'])
    return Button(parent,command=doit,**config)

  
def simulation_canvas  (parent,**config):
    global the_canvas
    the_canvas = Canvas(parent,**config)
    the_canvas.bind("<ButtonRelease>", lambda event : model.mouse_click(event.x,event.y))
    return the_canvas


def progress  (parent,**config):
    global the_progress
    the_progress = Label(parent,**config)
    return the_progress


# By the script calling this function, the update_all/display_all in the model
#   is called every 100 milliseconds in the GUI's/root thread, and then this
#   function reschedules itself to be called in 100 milliseconds
# This makes the simulation update itself every .1 seconds
def repeater(root):
    model.update_all()
    model.display_all()
    root.after(100,repeater,root)
