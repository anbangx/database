# A Hunter is both a Mobile_Simulton and Pulsator; it updates
#   as a Pulsator, but also moves (either in a straight line
#   or in pursuit of Prey), and displays as a Pulsator.


from pulsator import Pulsator
from mobilesimulton import Mobile_Simulton
from prey import Prey
from math import atan2


class Hunter(Pulsator,Mobile_Simulton):
    see_distance = 200   # used in this class only; here for easy changing
    
    def __init__(self,x,y):   
        Pulsator.__init__(self,x,y)
        w,h = self.get_dimension()
        Mobile_Simulton.__init__(self,x,y,w,h,0,5)
        self.randomize_angle()
        
        
    def update(self,model):
        eaten = Pulsator.update(self,model)
        seen  = model.find(lambda x : isinstance(x,Prey) and self.distance(x.get_location()) <= Hunter.see_distance)
        if seen:
            min_dist,who = min([(self.distance(s.get_location()),s) for s in seen])
            sx,sy = self.get_location()
            x,y   = who.get_location()
            self.set_angle(atan2(y-sy,x-sx))
        self.move()
        return eaten
        