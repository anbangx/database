'''
Created on Dec 20, 2013

@author: anbangx
'''

if __name__ == '__main__':
    def outer():
        x = 1
        def inner():
           print(x) # 1
        return inner
        
    foo = outer()
    foo.func_closure