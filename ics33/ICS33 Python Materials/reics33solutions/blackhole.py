# A Black_Hole is a Simulton; it updates by removing
#   any Prey whose center is contained within its radius
#   (returning a set of all eaten simultons), and
#   displays as a black circle with a radius of 10
#   (width/height 20).
# Calling get_dimension for the width/height (for
#   containment and displaying) will facilitate
#   update and inheritance in Pulsator and Hunter

from simulton import Simulton
from prey import Prey


class Black_Hole(Simulton):
    radius = 10 # used in this class only; here for easy size-changing
    
    def __init__(self,x,y):   
        Simulton.__init__(self,x,y,2*Black_Hole.radius,2*Black_Hole.radius)
        
    def update(self,model):
#        def decide(s):
#            print('checking bh@',self.get_location(),'and object@',s.get_location(),type(s))
#            return isinstance(s,Prey) and self.contains(s.get_location())
#        eaten = model.find(decide)
        eaten = model.find(lambda s : isinstance(s,Prey) and self.contains(s.get_location()))
        for s in eaten:
            model.remove(s)
        return eaten
 
 
    def contains(self,xy):
        return self.distance(xy) <= self.get_dimension()[0]/2

               
    def display(self,the_canvas):
        w,h = self.get_dimension()
        the_canvas.create_oval(self._x-w/2, self._y-h/2,
                               self._x+w/2, self._y+h/2,
                               fill='black')