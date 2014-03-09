# A Floater is Prey; it updates by moving mostly in
#   a straight line, but with random changes to its
#   angle and speed, and displays as ufo.gif (whose
#   dimensions (width and height) are computed by
#   calling .width()/.height() on the PhotoImage


from PIL.ImageTk import PhotoImage
from prey import Prey
from random import random


class Floater(Prey):
    def __init__(self,x,y):   
        self._image = PhotoImage(file='ufo.gif')
        Prey.__init__(self,x,y,self._image.width(),self._image.height(),0,5)
        self.randomize_angle()

        
    def update(self,model):
        if random() <= .3:
            new_speed = min(7,max(3,self.get_speed() + random()-.5))
            self.set_velocity(new_speed, self.get_angle()+random()-.5)
        self.move()
 

    def display(self,the_canvas):
        the_canvas.create_image(self._x,self._y,image=self._image)
