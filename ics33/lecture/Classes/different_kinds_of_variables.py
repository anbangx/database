'''
Created on Dec 18, 2013

@author: anbangx
'''

global_var = 0
class C:
    class_var = 0

    def __init__(self,init_instance_var):
        self.instance_var = init_instance_var

    def bump(self,name):
        print(name,'bumped')
        #global_var = 100    # comment out this line or the next
        global global_var    # comment out this line or the previous 
        global_var += 1 
        C.class_var += 1
        self.instance_var += 1

    def report(self,var):
        print('instance referred to by', var,
              ': global_var =', global_var,
              '/class_var =', C.class_var,
              '/instance_var=', self.instance_var)

if __name__ == '__main__':
    x=C(10)
    x.report('x')
    x.bump('x')
    x.report('x')
    
    y = x
    print('y = x')
    y.bump('y')
    x.report('x')
    y.report('y')
    
    print('y = C(20)')
    y=C(20)
    y.bump('y')
    y.report('y')
    
    C.report(x,'x')       # same as x.report('x') by the Fundamental Equation of OOP
    type(x).report(x,'x') # ditto: the meaning of the Fundamental Equation of OOP
    
    print(C.class_var, x.class_var)  # discussed below
    print(x.instance_var)