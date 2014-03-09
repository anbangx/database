'''
Created on Dec 20, 2013

@author: anbangx
'''

if __name__ == '__main__':
    exec 'print 5'           # prints 5.
    exec 'print 5\nprint 6'  # prints 5{newline}6.
    exec 'if True: print 6'  # prints 6.
    exec '5'                 # does nothing and returns nothing.