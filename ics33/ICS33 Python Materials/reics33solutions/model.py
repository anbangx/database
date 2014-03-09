import controller, sys
#import model   #strange, but we need a reference to this module to pass this module to update

from ball      import Ball
from floater   import Floater
from blackhole import Black_Hole
from pulsator  import Pulsator
from hunter    import Hunter


# Global variables: declare them global in functions that assign to them: e.g., ... = or +=
cycle_count     = 0;
simultons      = set();
running        = False
stop_after_one  = False
click_object    = None

#return a 2-tuple of the width and height of the canvas (defined in the controller)
def world():
    return (controller.the_canvas.winfo_width(),controller.the_canvas.winfo_height())

#reset all module variables to represent an empty/stopped simulation
def reset ():
    global running,cycle_count,simultons,stop_after_one
    running = False
    cycle_count = 0;
    simultons   = set()
    stop_after_one = False


#start running the simulation
def start ():
    global running
    running = True


#stop running the simulation (freezing it)
def stop ():
    global running
    running = False


#tep just one update in the simulation
def step ():
    global running,stop_after_one
    running = True
    stop_after_one = True


#remember the kind of object to add to the simulation when an (x,y) coordinate in the canvas
#  is clicked next (or remember to remove an object by such a click)   
def select_object(kind):
    global click_object
    click_object = kind
    print(kind,'is now selected object for clicking')


#add the kind of remembered object to the simulation (or remove any objects that contain the
#  clicked (x,y) coordinate
def mouse_click(x,y):
    if click_object == None:
        print('Select an object to create first')
    elif click_object == 'Remove':
        for s in find(lambda s : s.contains((x,y))):
            simultons.discard(s)                             
    else:
        simultons.add(eval(click_object+'('+str(x)+','+str(y)+')'))


#add simulton s to the simulation
def add(s):
    global simultons
    simultons.add(s)
    

# remove simulton s from the simulation    
def remove(s):
    global simultons
    simultons.discard(s)
    

#find/return a set of simultons that each satisfy predicate p    
def find(p):
    return {s for s in simultons if p(s)}


#call update for every simulton in the simulation
def update_all():
    global running, stop_after_one
    if running:
        global cycle_count,world
        cycle_count += 1
        
        # if a simulton has been removed. don't call update on it
        original_simultons = set(simultons)
        for s in original_simultons:
            if s in simultons:
                s.update(sys.modules[__name__])#s.update(model)
        if stop_after_one:
            running = False
            stop_after_one = False


#delete from the canvas every simulton in the simulation, and then call display for every
#  simulton in the simulation to add it back to the canvas possibly in a new location: to
#  animate it; also, update the progress label defined in the controller
def display_all():
    # Easier to delete all and display all; could use move with more thought
    for o in controller.the_canvas.find_all():
        controller.the_canvas.delete(o)
    
    for s in simultons:
        s.display(controller.the_canvas)
    
    controller.the_progress.config(text=str(cycle_count)+' cycles/'+str(len(simultons))+' simultons')