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

def success_example():
    x=C(10)
    x.report('x')
    
    def bump(self,name):
        print(name,'bumped')
        self.instance_var += 1
    
    bump(x,'x')
    x.report('x')
    
    C.cbump = bump;
    
    x.cbump('x')
    x.report('x')
    
    y=C(20)
    y.cbump('y')
    y.report(y)
    
def fail_example():
    def bump(self,name):
        print(name,'bumped')
        self.instance_var += 1

    x = C(0)
    y = C(100)
    
    x.bump = bump;
    
    x.bump(x,'x')
    x.bump(x,'x')
    x.report('x')
    
    y.bump(y,'y')     # fails because there is no bump attribute in the object y
    y.report('y')     #    refers to, either directly in y or in y's class C
    
if __name__ == '__main__':
    success_example()
    fail_example()
