'''
Created on Dec 20, 2013

@author: anbangx
'''

if __name__ == '__main__':
    def outer(some_func):
        def inner():
            print("before some_func")
            ret = some_func()
            return ret + 1
        return inner
    
    def foo():
        return 1
    decorated = outer(foo)
    decorated()
    
    foo = outer(foo)
    print(foo)