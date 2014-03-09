# Mobile_Simultons have angles (in radians) and speeds
#   (in pixels/update); they inherit locations and
#   dimensions (and methods) from Simulton.
# These are initialized in __init__ and can be
#    queried and set using methods.
# Three useful methods help mobile simultons move on their
#   canvas, by updating their location and changing their
#   angle (bouncing) if they hit a wall.


# The angle unit is radians. If you are unfamiliar with this
# unit, examine the diagram below in which all units are in radians. 
#
#                   pi/2
#                     |
#                     |
#                     | 
#                     |
#                     |
# pi (or -pi) --------+-------- 0 (or 2pi)
#                     |
#                     |
#                     |
#                     |
#                     |
#                  3pi/2 (or -pi/2)


import math,random
from simulton import Simulton
import model


class Mobile_Simulton(Simulton):
    def __init__(self,x,y,width,height,angle,speed):
        Simulton.__init__(self,x,y,width,height)
        self._angle = angle
        self._speed = speed
        
    def get_angle(self):
        return self._angle
    
    
    def set_angle(self,angle):
        self._angle = angle
    
    
    def get_speed(self):
        return self._speed
    
    
    def set_speed(self,speed):
        self._speed = speed
        
        
    # velocity includes speed and angle    
    def set_velocity(self,speed,angle):
        self.set_speed(speed)
        self.set_angle(angle)

        
    def randomize_angle(self):
        self._angle = 2*math.pi*random.random()

    
    def move(self):
        self.change_location(self._speed*math.cos(self._angle),
                             self._speed*math.sin(self._angle))
        self.wall_bounce()

        
    def bounce(self,barrier_angle):
        self._angle = 2*barrier_angle - self._angle

        
    def wall_bounce(self):
        x,y      = self.get_location()
        w,h      = self.get_dimension()
        mw,mh    = model.world()
        
        left_x   = x - w/2
        right_x  = x + w/2
        top_y    = y - h/2
        bottom_y = y + h/2

        if left_x < 0:
            self.bounce(math.pi/2)
            self.change_location(-2*left_x,0)
        elif right_x > mw:
            self.bounce(math.pi/2);
            self.change_location(2*(mw-right_x),0);

        if top_y < 0:
            self.bounce(0);
            self.change_location(0,-2*top_y);
        elif bottom_y > mh:
            self.bounce(0);
            self.change_location(0,2*(mh-bottom_y));