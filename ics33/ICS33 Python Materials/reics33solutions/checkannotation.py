from goody import type_as_str
import inspect

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
        
        # Check list/tuple: type, len=1 (same check) or match (respective matches)
        def check_sequence(kind,kind_text):
            assert isinstance(value,kind), repr(param)+' failed annotation check(wrong type): value = '+repr(value)+\
                                           '\n  was type '+type_as_str(value)+' ...should be type '+kind_text+'\n'+check_history                 
            if len(annot) == 1:
                i = 0
                for v in value:
                    self.check(param,annot[0],v,check_history+kind_text+'['+str(i)+'] check: '+str(annot[0])+'\n')
                    i += 1
            else:
                assert len(annot) == len(value), repr(param)+' failed annotation check(wrong number of elements): value = '+repr(value)+\
                                                 '\n  annotation had '+str(len(annot))+ ' elements'+str(annot)+'\n'+check_history                 
                i = 0
                for a,v in zip(annot,value):
                    self.check(param,a,v, check_history+kind_text+'['+str(i)+'] check: '+str(annot[i])+'\n')
                    i += 1

        # Check dict: type, len=1, all keys (same check) and all values (same check)
        def check_dict():
            assert isinstance(value,dict), repr(param)+' failed annotation check(wrong type): value = '+repr(value)+\
                                           '\n  was type '+type_as_str(value)+' ...should be type dict\n'+check_history                 
            if len(annot) != 1:
                assert False, repr(param)+' annotation inconsistency: dict should have 1 item but had '+str(len(annot))+\
                              '\n  annotation = '+str(annot)+'\n'+check_history                 
            else:
                for annot_k,annot_v in annot.items(): # get first and only
                    pass
                for k,v in value.items():
                    self.check(param,annot_k,k, check_history+'dict key check: '  +str(annot_k)+'\n')
                    self.check(param,annot_v,v, check_history+'dict value check: '+str(annot_v)+'\n')
        
        # Check set/frozenset: type, len=1, all keys (same check) and all values (same check)
        def check_set(kind,kind_text):
            assert isinstance(value,kind), repr(param)+' failed annotation check(wrong type): value = '+repr(value)+\
                                           '\n  was type '+type_as_str(value)+' ...should be type ' +kind_text+'\n'+check_history                 
            if len(annot) != 1:
                assert False, repr(param)+' annotation inconsistency: '+kind_text+' should have 1 value but had '+str(len(annot))+\
                              '\n  annotation = '+str(annot)+'\n'+check_history                 
            else:
                for annot_v in annot:
                    pass
                for v in value:
                    self.check(param,annot_v,v,check_history+kind_text+' value check: '+str(annot_v)+'\n')

        # Check function/lambda: univariate, returns True, throws exception
        def check_predicate():
            assert len(annot.__code__.co_varnames) == 1, repr(param)+' annotation inconsistency: predicate should have 1 parameter but had '+str(len(annot.__code__.co_varnames))+\
                                                        '\n  annotation = '+str(annot)+'\n'+check_history                 
            try:
                worked = annot(value)
            except Exception as message:
                assert False, repr(param)+' annotation predicate('+str(annot)+') raised exception'+\
                              '\n  exception = '+str(message.__class__)[8:-2]+': '+str(message)+'\n'+check_history                 
            else:
                assert worked, repr(param)+' failed annotation check: value = '+repr(value)+\
                               '\n  predicate = '+str(annot)+'\n'+check_history 

        # Check string (as evaluated expression): returns True, throws exception
        def check_str():
            try:
                worked = eval(annot,self._args) 
            except Exception as message:
                    assert False, repr(param)+' annotation str('+str(annot)+') raised exception'+\
                                  '\n  exception = '+str(message.__class__)[8:-2]+': '+str(message)+'\n'+check_history                 
            else:
                assert worked, repr(param)+' failed annotation check(str predicate: '+repr(annot)+')'\
                               '\n  args for evaluation: '+', '.join([str(k)+'->'+str(v) for k,v in self._args.items()])+'\n'+check_history

        
        # Decode annotation and check it #print('checking',p+':'+str(value),annot)
        if annot == None:
            pass
        elif type(annot) is type:
            assert isinstance(value,annot), repr(param)+' failed annotation check(wrong type): value = '+repr(value)+\
                                        '\n  was type '+type_as_str(value)+' ...should be type '+str(annot)[8:-2]+'\n'+check_history                 
        elif isinstance(annot,list):      check_sequence(list,'list')
        elif isinstance(annot,tuple):     check_sequence(tuple,'tuple')
        elif isinstance(annot,dict):      check_dict()
        elif isinstance(annot,set):       check_set(set,'set')
        elif isinstance(annot,frozenset): check_set(frozenset,'frozenset')
        elif inspect.isfunction(annot):   check_predicate()
        elif isinstance(annot,str):       check_str()
        else:
            try:
                annot.__check_annotation__(self.check,param,value,check_history)
            except AttributeError: 
                assert False, repr(param)+' annotation undecipherable: '+str(annot)+'\n'+check_history                 
            except Exception as message:
                if message.__class__ is AssertionError:
                    raise
                else:
                    assert False, repr(param)+' annotation protocol('+str(annot)+') raised exception'+\
                                 '\n  exception = '+str(message.__class__)[8:-2]+': '+str(message)+'\n'+check_history                 
           
        
    # Return decorated function call, checking present parameter/return
    #   annotations if required
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
        if not (Check_Annotation.checking_on and self.checking_on):
            return self._f(*args,**kargs)
        
        # On first AssertionError, print the source lines of the function and reraise 
        self._args = param_arg_bindings()
        annot = self._f.__annotations__
        try:
            # Check the annotation for every parameter (if there is one)
            for p in self._args.keys():
                if p in annot:
                    self.check(p,annot[p],self._args[p])
                    
            # Compute/remember the value of the decorated function
            answer = self._f(*args,**kargs)
            
            # If the return has an annotation, check it
            if 'return' in annot:
                self._args['_return'] = answer
                self.check('return',annot['return'],answer)
            
            # Return the decorated answer
            return answer
        except AssertionError:
            print(80*'-')
            for l in inspect.getsourcelines(self._f)[0]: # ignore starting line #
                print(l.rstrip())
            print(80*'-')
            raise
        
  
  
if __name__ == '__main__':     
        
    @Check_Annotation
    def f(x:'x>0'): pass
    
    f('a')
