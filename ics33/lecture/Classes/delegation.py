'''
Created on Dec 18, 2013

@author: anbangx
'''

class C:
    def __init__(self,init_instance_var):
        self.instance_var = init_instance_var

    def report(self,var):
        print('instance referred to by', var,
              '/instance_var=', self.instance_var)

    def bump(self,name):
        try:
            self.object_bump(self,name)
        except AttributeError:
            print('could not bump',name) # or just pass to handle the exception
            
if __name__ == '__main__':
    x = C(10)
    y = C(20)
    z = C(30)
    
    def bump1(self,name):
        print('bumped',name)
        self.instance_var += 1
        
    def bump2(self,name):
        print('bumped',name)
        self.instance_var += 2
        
    x.object_bump = bump1
    y.object_bump = bump2
    
    x.report('x')
    x.bump('x')
    x.report('x')
    
    y.report('y')
    y.bump('y')
    y.report('y')
    
    z.report('z')
    z.bump('z')
    z.report('z')