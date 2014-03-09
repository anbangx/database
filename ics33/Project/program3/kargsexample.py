# kargs example

print('Demonstrating how kargs works in functions')
def f(**kargs):
    # prints dict
    print ('\nkargs =',kargs)
    
    # raises exception
    # equivalent to calling print(a=1,b=2) order of arguments based on dict
    d = dict(a=1,b=2)
    print (**d)

#Call successfully (watch what is printed)
f(a=1,b=True,c=['a','b','c'],d=print)