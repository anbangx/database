from goody import type_as_str


class DimensionError(Exception):
    def __init__(self, message=None):
        Exception.__init__(self, message)

class Dimensional:
    def __init__(self, value, l=0, m=0, t=0):
        assert type(l) is int, 'Dimensional.__init: argument l(' + str(l) + ') must be an int'  
        assert type(m) is int, 'Dimensional.__init: argument l(' + str(m) + ') must be an int'  
        assert type(t) is int, 'Dimensional.__init: argument l(' + str(t) + ') must be an int'  
        self.value, self.l, self.m, self.t = value, l, m, t
        
    def __repr__(self):
        return 'Dimensional(' + str(self.value) + \
                             ('' if self.l == 0 else ',l=' + str(self.l)) + \
                             ('' if self.m == 0 else ',m=' + str(self.m)) + \
                             ('' if self.t == 0 else ',t=' + str(self.t)) + ')' 
    
    def __str__(self):
        return str(self.value) + '(' + str(self.l) + ',' + str(self.m) + ',' + str(self.t) + ')'
            
    def __getitem__(self, arg):
        return (self.l, self.m, self.t) if arg == 'd' else self.__dict__[arg]
    
    def format(self, args):
        def f(c, p):
            return str(c) if abs(p) == 1 else str(c) + '**' + str(abs(p))
        val = str(self.value)
        num = "".join(f(c, p) for c, p in zip(args, self['d']) if p > 0)
        denom = "".join(f(c, p) for c, p in zip(args, self['d']) if p < 0)
        return val + (' 1' if num == '' and denom != '' else ' ' + num if num != '' else '') + ('' if denom == '' else '/' + denom) 
        
    def __pos__(self):
        return self
    
    def __neg__(self):
        return Dimensional(-self.value, self.l, self.m, self.t)
    
    @staticmethod
    def _validate_muldiv(l,r,op):
        if type(l) not in [int,float,Dimensional] or type(r) not in [int,float,Dimensional]:
            raise TypeError('unsupported operand type(s) for '+op+
                            ': \''+type_as_str(l)+'\' and \''+type_as_str(r)+'\'')
             
    @staticmethod
    def _validate_relational(l,r,op):
        Dimensional._validate_muldiv(l,r,op)
        if type(r) in [int, float] and l['d'] != (0, 0 ,0):
            raise DimensionError('') 
        if type(l) in [int, float] and r['d'] != (0, 0 ,0):
            raise DimensionError('') 
        if type(l) is Dimensional and type(r) is Dimensional:
            if l.l != r.l or l.m != r.m or l.t != r.t:
                raise DimensionError('')
        
    def __add__(self, r):
        Dimensional._validate_relational(self, r, "+")
        if type(r) in [int, float]:
            return Dimensional(self.value + r, self.l, self.m, self.t);
        else:
            return Dimensional(self.value + r.value, self.l, self.m, self.t);
    
    def __radd__(self, l):
        Dimensional._validate_relational(l, self, "+")
        if type(l) in [int, float]:
            return Dimensional(l + self.value, self.l, self.m, self.t);
        else:
            return Dimensional(l.value + self.value, self.l, self.m, self.t);    
    
    def __sub__(self, r):
        Dimensional._validate_relational(self, r, "-")
        if type(r) in [int, float]:
            return Dimensional(self.value - r, self.l, self.m, self.t);
        else:
            return Dimensional(self.value - r.value, self.l, self.m, self.t);
        
    def __rsub__(self, l):
        Dimensional._validate_relational(l, self, "-")
        if type(l) in [int, float]:
            return Dimensional(l - self.value, self.l, self.m, self.t);
        else:
            return Dimensional(l.value - self.value, self.l, self.m, self.t);   
    
    def __mul__(self, r):
        Dimensional._validate_muldiv(self, r, "*")
        if type(r) in [int, float]:
            return Dimensional(self.value * r, self.l, self.m, self.t);
        else:
            return Dimensional(self.value * r.value, self.l + r.l, self.m + r.m, self.t + r.t);
        
    def __rmul__(self, l):
        Dimensional._validate_muldiv(l, self, "*")
        if type(l) in [int, float]:
            return Dimensional(l * self.value, self.l, self.m, self.t);
        else:
            return Dimensional(self.value * l.value, self.l + l.l, self.m + l.m, self.t + l.t);
        
    def __pow__(self, p):
        if type(p) not in [int, Dimensional]:
            raise TypeError()
        if type(p) is Dimensional and p['d'] != (0, 0, 0):
            raise DimensionError()
        if type(p) is Dimensional:
            p = p.value
        return Dimensional(self.value ** p, self.l * p, self.m * p, self.t * p)
    
    def __eq__(self, r):
        Dimensional._validate_muldiv(self, r, '==')
        if type(r) in [int, float]:
            return self.value == r
        else:
            return self.value == r.value and self.l == r.l and self.m == r.m and self.t == r.t 
    
    def __abs__(self):
        return Dimensional(abs(self['value']),*self['d'])
    
    def __bool__(self):
        return self.value != 0
          
if __name__ == '__main__':
#     import driver
#     driver.driver()
    
    # Test init
#     Dimensional(0,'a',0,0)
#     Dimensional(0,0,1.0,0)
#     Dimensional(0,0,0,True)
    g = Dimensional(9.8,1,0,-2)
     
    # Test repr/str
    print(repr(g))
    print(str(g))
     
    # Test getitem
    print(g['value'])
    print(g['l'])
    print(g['m'])
    print(g['t'])
    print(g['d'])
     
    # Test format
    print(g.format('mgs')) #-->9.8 m/s**2
    print(Dimensional(1,2,3,-1).format('mgs')) #-->1 m**2g**3/s
    print(Dimensional(1,0,0,-1).format('mgs')) #-->1 1/s
    print(Dimensional(1,0,1,0).format('mgs'))  #-->1 g
    print(Dimensional(1,0,0,0).format('mgs'))  #-->1
     
    # Test pos/neg
    print(str(+g)) #-->9.8(1,0,-2)
    print(str(-g)) #-->-9.8(1,0,-2)
     
    # Test add/radd
    a = g + Dimensional(5,1,0,-2)  #-->14.8(1,0,-2)
    print(str(a))
    b = g + Dimensional(5.,1,0,-2)  #-->14.8(1,0,-2)
    print(str(b))
#     g + Dimensional(5,0,0,-2) #-->DimensionError
#     g + Dimensional(5,1,1,-2) #-->DimensionError
#     g + Dimensional(5,1,0,-1) #-->DimensionError
#     g + 1 #-->DimensionError
#     g + 1. #-->DimensionError
#     1 + g #-->DimensionError
#     1. + g #-->DimensionError
    dless = Dimensional(5)
    a = 1 + dless  #-->6(0,0,0)
    print(str(a))
    b = 1. + dless  #-->6.0(0,0,0)
    print(str(b))
    c = dless + 1  #-->6(0,0,0)
    print(str(c))
    d = dless + 1.  #-->6.0(0,0,0)
    print(str(d))
#     dless + 'a' #-->TypeError
#     'a' + dless #-->TypeError

    # Test sub/rsub
    a = g - Dimensional(5,1,0,-2) #-->4.800000000000001(1,0,-2)
    b = g - Dimensional(5.,1,0,-2) #-->4.800000000000001(1,0,-2)
    print(str(a))
    print(str(b))
#     g - Dimensional(5,0,0,-2) #-->DimensionError
#     g - Dimensional(5,1,1,-2) #-->DimensionError
#     g - Dimensional(5,1,0,-1) #-->DimensionError
#     g - 1 #-->DimensionError
#     g - 1. #-->DimensionError
#     1 - g #-->DimensionError
#     1. - g #-->DimensionError
    a = 1 - dless #-->-4(0,0,0)
    b =1. - dless #-->-4.0(0,0,0)
    c = dless - 1 #-->4(0,0,0)
    d = dless - 1. #-->4.0(0,0,0)
    print(str(a))
    print(str(b))
    print(str(c))
    print(str(d))
#     dless - 'a' #-->TypeError
#     'a' - dless #-->TypeError

    # Test mul/rmul
    a = g * Dimensional(5,0,0,-2) #-->49.0(1,0,-4)
    b = g * Dimensional(5,1,1,-2) #-->49.0(2,1,-4)
    c = g * Dimensional(5,1,0,-1) #-->49.0(2,0,-3)
    d = g * 1 #-->9.8(1,0,-2)
    x = g * 1. #-->9.8(1,0,-2)
    y = 1 * g #-->9.8(1,0,-2)
    z = 1. * g #-->9.8(1,0,-2)
    print(str(a))
    print(str(b))
    print(str(c))
    print(str(d))
    print(str(x))
    print(str(y))
    print(str(z))
    
    # Test pow
    a = g ** 2 #-->96.04000000000002(2,0,-4)
    b = g ** -2 #-->0.010412328196584756(-2,0,4)
    c = g ** Dimensional(2,0,0,0) #-->96.04000000000002(2,0,-4)
    print(str(a))
    print(str(b))
    print(str(c))
#     g ** 1. #-->TypeError
#     g ** Dimensional(1,1,0,0) #-->DimensionError
#     g ** Dimensional(1,0,1,0) #-->DimensionError
#     g ** Dimensional(1,0,0,1) #-->DimensionError
#     dless ** 'a' #-->TypeError
#     'a' ** dless #-->TypeError
    
    # Test ==
    a = g == 9.8 #-->True
    b = g == 9.7 #-->False
    c = 9.8 == g #-->True
    d = 9.7 == g #-->False
    x = g == Dimensional(9.8,1,0,-2) #-->True
    print(str(a))
    print(str(b))
    print(str(c))
    print(str(d))
    print(str(x))
    y = g == Dimensional(9.8,0,0,0) #-->DimensionError
    z = Dimensional(9.8,0,0,0) == g #-->DimensionError
    
    