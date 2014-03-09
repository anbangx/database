'''
Created on Dec 20, 2013

@author: anbangx
'''

if __name__ == '__main__':
    x = eval('5')              # x <- 5
    x = eval('%d + 6' % x)     # x <- 11
    x = eval('abs(%d)' % -100) # x <- 100
    x = eval('print 5')        # INVALID; print is a statement, not an expression (in Python 2.x).
    x = eval('if 1: x = 4')    # INVALID; if is a statement, not an expression.