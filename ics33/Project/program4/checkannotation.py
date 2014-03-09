from goody import type_as_str
import inspect
from imp import reload

class Check_All_OK:
    """
    Check_ALl_OK implements __check_annotation__ by checking whether all the
      annotations passed to its constructor are OK; the first one that
      fails (raises AssertionError) prints its problem, with a list of all
      annotations being tried at the end of the check_history.
    """
       
    def __init__(self,*args):
        self._annotations = args
        
    def __repr__(self):
        return 'Check_All_OK('+','.join([str(i) for i in self._annotations])+')'

    def __check_annotation__(self, check, param, value,check_history):
        for annot in self._annotations:
            check(param, annot, value, check_history+'Check_All_OK check: '+str(annot)+' while trying: '+str(self)+'\n')


class Check_Any_OK:
    """
    Check_Any_OK implements __check_annotation__ by checking whether at least
      one of the annotations passed to its constructor is OK; if all fail 
      (raise AssertionError) this classes raises AssertionError and prints its
      failure, along with a list of all annotations tried followed by the check_history.
    """
    
    def __init__(self,*args):
        self._annotations = args
        
    def __repr__(self):
        return 'Check_Any_OK('+','.join([str(i) for i in self._annotations])+')'

    def __check_annotation__(self, check, param, value, check_history):
        failed = 0
        for annot in self._annotations: 
            try:
                check(param, annot, value, check_history)
            except AssertionError:
                failed += 1
        if failed == len(self._annotations):
            assert False, repr(param)+' failed annotation check(Check_Any_OK): value = '+repr(value)+\
                         '\n  tried '+str(self)+'\n'+check_history                 



class Check_Annotation():
    # must be True for checking to occur
    checking_on  = True
  
    # self._checking_on must also be true for checking to occur
    def __init__(self,f):
        self._f = f
        self.checking_on = True
        
    # Check whether param's annot is correct for value, adding to check_history
    #    if recurs; defines many local function which use it parameters.  
    def check(self,param,annot,value,check_history=''):
        
        # Define local functions for checking, list/tuple, dict, set/frozenset,
        #   lambda/functions, and str (str for extra credit)
        # Many of these local functions called by check, call check on their
        #   elements (thus are indirectly recursive)
        def check_sequence(kind, kind_text):
            assert isinstance(value, kind), 'Wrong whole type: ' + type_as_str(value) + '... should be ' + kind_text
            
            if len(annot) == 1:
                i = 0
                for v in value:
                    self.check(param,annot[0],v,check_history+kind_text+'['+str(i)+'] check: '+str(annot[0])+'\n')
                    i += 1
            else:
                assert len(annot) == len(value), 'Different length of value and annot!'
                for v,k in zip(value, annot):
                    assert isinstance(v, k), 'Wrong single type: ' + type_as_str(v) + '... should be ' + str(k)
        
        def check_dict(kind, kind_text):
            assert isinstance(value, kind), 'Wrong whole type: ' + type_as_str(value) + '... should be ' + kind_text
            
            if(len(annot) != 1):
                assert False
            else:
                for annot_k,annot_v in annot.items(): # get first and only
                    pass
                for k,v in value.items():
                    self.check(param,annot_k,k, check_history+'dict key check: '  +str(annot_k)+'\n')
                    self.check(param,annot_v,v, check_history+'dict value check: '+str(annot_v)+'\n')
                
        def check_set(kind, kind_text):
            assert isinstance(value, kind), 'Wrong whole type: ' + type_as_str(value) + '... should be ' + kind_text
            
            if(len(annot) != 1):
                assert False
            else:
                for annot_v in annot:
                    pass
                for v in value:
                    self.check(param,annot_v, v, check_history+'set key check: '  +str(annot_v)+'\n')
        
        
        # Decode annotation and check it 
        if annot == None:
            pass
        elif type(annot) is type:
            assert isinstance(value,annot), repr(param)+' failed annotation check(wrong type): value = '+repr(value)+\
                                        '\n  was type '+type_as_str(value)+' ...should be type '+str(annot)[8:-2]+'\n'+check_history
        elif isinstance(annot,list):      check_sequence(list,'list')  
        elif isinstance(annot,tuple):      check_sequence(tuple,'tuple')
        elif isinstance(annot,dict):      check_dict(dict,'dict')    
        else:
            pass
    # Return result of calling decorated function call, checking present
    #   parameter/return annotations if required
    def __call__(self, *args, **kargs):
        
        # Return a dictionary of the parameter/argument bindings (actually an
        #    ordereddict, in the order parameters occur in the function's header)
        def param_arg_bindings():
            f_signature  = inspect.signature(self._f)
            bound_f_signature = f_signature.bind(*args,**kargs)
            for param in f_signature.parameters.values():
                if param.name not in bound_f_signature.arguments:
                    bound_f_signature.arguments[param.name] = param.default
            return bound_f_signature.arguments

        # If annotation checking is turned off at the class or function level
        #   just return the result of calling the decorated function
        # Otherwise do annotation checking
        if not (Check_Annotation.checking_on and self.checking_on):
            return self._f(*args,**kargs)
        
        self._args = param_arg_bindings()
        annot = self._f.__annotations__
        print('self._args: ' + str(self._args))
        print('annot: ' + str(annot))
        try:
            # Check the annotation for every parameter (if there is one)
            for p in self._args.keys():
                if p in annot:
                    self.check(p,annot[p],self._args[p])
                    
            # Compute/remember the value of the decorated function
            
            # If 'return' is in the annotation, check it
            
            # Return the decorated answer
            
            pass #remove after adding real code in try/except
            
        # On first AssertionError, print the source lines of the function and reraise 
        except AssertionError:
            print(80*'-')
            for l in inspect.getsourcelines(self._f)[0]: # ignore starting line #
                print(l.rstrip())
            print(80*'-')
            raise




  
if __name__ == '__main__':     
    # and example of testing a simple annotation
    @Check_Annotation
    def f(x : [int]): pass
    
    f(1)
#     f([1,2])
#     f({1,2}) #-->AssertionError
#     f(['a',2]) #-->AssertionError
#     f([1,'b']) #-->AssertionError
#     f(['a','b']) #-->AssertionError
#     
    @Check_Annotation
    def f(x : [int,str]): pass
    f([1,'b'])
    
    #Check: dict
    @Check_Annotation
    def f(x : {str : int}): pass
    f({'a':1,'b':2})
#     f([('a',1),('b',2)]) #-->AssertionError
#     f({0:1,'b':2}) #-->AssertionError
#     f({'a':'x','b':2}) #-->AssertionError
#     f({'a':1,0:2}) #-->AssertionError
#     f({'a':1,'b':'x'}) #-->AssertionError
#     
#     @Check_Annotation
#     def f(x : {str : int, int : str}): pass
#     f({'a':1}) #-->AssertionError
