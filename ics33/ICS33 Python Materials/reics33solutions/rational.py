from goody import irange


class Rational:
    def _gcd(x,y):
        while y != 0:
            x, y = y, x % y
        return x
    
    def _validate_arithmetic(v,t,op,lt,rt):
        if type(v) not in t:
            raise TypeError('unsupported operand type(s) for '+op+
                            ': \''+lt+'\' and \''+rt+'\'')        

    def _validate_relational(v,t,op,rt):
        if type(v) not in t:
            raise TypeError('unorderable types: '+
                            ': Rational() '+op+' '+rt+'()')        

    def __setattr__(self,name,value):
        if name in self.__dict__:
            raise NameError('Rational is immutable; attempted to change instance variable: '+name)
        self.__dict__[name] = value
        
    def __init__(self,num=0,denom=1):
        assert type(num)   is int, 'Rational.__init__ numerator is not int: '+str(num)
        assert type(denom) is int, 'Rational.__init__ denominator is not int: '+str(denom)
        assert denom != 0,'Rational.__init__ denominator is 0'
        if denom < 0:
            num, denom = -num, -denom
        if num == 0:
            denom = 1
        gcd = Rational._gcd(abs(num), denom)
        self.num = num // gcd
        self.denom = denom // gcd
        
    def __repr__(self):
        return 'Rational('+str(self.num)+','+str(self.denom)+')'
    
    def __str__(self):
        return str(self.num)+'/'+str(self.denom)
    
    def __pos__(self):
        return self
    
    def __neg__(self):
        return Rational(-self.num,self.denom)
    
    def __add__(self,right):
        Rational._validate_arithmetic(right, [Rational,int],'+','Rational',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return Rational(self.num*right.denom+right.num*self.denom,self.denom*right.denom)
    
    def __radd__(self,left):
        Rational._validate_arithmetic(left, [Rational,int],'+',str(type(left))[8:-2],'Rational')
        return self.__add__(left)

    def __sub__(self,right):
        Rational._validate_arithmetic(right, [Rational,int],'-','Rational',str(type(right))[8:-2])
        return self + -right
    
    def __rsub__(self,left):
        Rational._validate_arithmetic(left, [Rational,int],'-',str(type(left))[8:-2],'Rational')
        return left + -self 
    
    def __mul__(self,right):
        Rational._validate_arithmetic(right, [Rational,int],'*','Rational',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return Rational(self.num*right.num,self.denom*right.denom)

    def __rmul__(self,left):
        Rational._validate_arithmetic(left, [Rational,int],'*',str(type(left))[8:-2],'Rational')
        return self.__mul__(left)
    
    def __truediv__(self,right):
        Rational._validate_arithmetic(right, [Rational,int],'/','Rational',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return Rational(self.num*right.denom,self.denom*right.num)

    def __rtruediv__(self,left):
        Rational._validate_arithmetic(left, [Rational,int],'/',str(type(left))[8:-2],'Rational')
        if type(left) is int:
            left = Rational(left)
        return left.__truediv__(self)

    def __pow__(self,right):
        Rational._validate_arithmetic(right, [int],'**','Rational',str(type(right))[8:-2])
        if right >= 0:
            return Rational(self.num**right,self.denom**right)
        else:
            return Rational(self.denom**-right,self.num**-right)

    def __eq__(self,right):
        Rational._validate_relational(right,[Rational,int],'==',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return self.num==right.num and self.denom==right.denom
    
    def __neq__(self,right):
        Rational._validate_relational(right,[Rational,int],'!=',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return not (self == right)
    
    def __lt__(self,right):
        Rational._validate_relational(right,[Rational,int],'<',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return self.num*right.denom < right.num*self.denom
    
    def __gt__(self,right):
        Rational._validate_relational(right,[Rational,int],'>',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return right.__lt__(self)
    
    def __le__(self,right):
        Rational._validate_relational(right,[Rational,int],'<=',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return not right.__lt__(self)
    
    def __ge__(self,right):
        Rational._validate_relational(right,[Rational,int],'>=',str(type(right))[8:-2])
        if type(right) is int:
            right = Rational(right)
        return not self.__lt__(right)
    
    def __abs__(self):
        return Rational(abs(self.num),self.denom)
    
    def __bool__(self):
        return self.num != 0
    
    # no iadd, isub imul itrudiv ipow
    
    def approximate(self, decimal_places):
        answer = ''
        num = self.num
        denom = self.denom
    
        # handle negative values
        if num < 0:
            num, answer = -num, '-'
    
        # handle integer part
        if num >= denom:
            answer += str(num//denom)
            num     = num - num//denom*denom
#
#        # handle decimal part
#        answer += '.'
#        num    *= 10
#        for i in irange(1,decimal_places):
#            answer += str(num//denom)
#            num     = 10*(num - num//denom*denom)
            
        answer += '.'+str(num*10**decimal_places//denom)
        return answer
    
    def best_approximation(self,n):
        best = Rational(0)
        for num in irange(0,10**n-1):
            for denom in irange(1,10**n-1):
                if abs(self-Rational(num,denom)) < abs(self-best):
                    best = Rational(num,denom)
        return best