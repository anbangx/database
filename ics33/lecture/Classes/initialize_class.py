'''
Created on Dec 18, 2013

@author: anbangx
'''

class C:
    def __init__(self, init_x, init_y):
        print('C object created')
        self.x = init_x        # value depends on the argument matching init_x
        self.y = init_y        # value depends on the argument matching init_y
        self.z = 3        # always the same object: 3

        
if __name__ == '__main__':
    x = C(1, 'two')
    print(x.__dict__)