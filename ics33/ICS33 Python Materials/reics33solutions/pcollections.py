import re, traceback, keyword

def pnamedtuple(type_name, field_names, mutable=False):
    def show_listing(s):
        i = 1
        for l in s.split('\n'):
            print('{num: >3} {text}'.format(num=i, text = l.rstrip()))
            i += 1
            
    def legal_name(name): 
        return type(name) is str and name not in keyword.kwlist and re.match('[a-zA-Z]\w*',name)

    def unique(iterable,max_times=1):
        iterated = set()
        for i in iterable:
            if i not in iterated:
                iterated.add(i)
                yield i
    
    # Validate arguments
    if not legal_name(type_name):
        raise SyntaxError('type_name is not legal: '+str(type_name))
    
    if type(field_names) is str:
        field_names = field_names.replace(',', ' ').split()
        
    if type(field_names) is not list:
        raise SyntaxError('field_names is not legal: '+str(field_names))
    
    field_names = [f for f in unique(field_names)]
    for n in field_names:
        if not legal_name(n):
            raise SyntaxError('field name is not legal: '+str(n))
        
    # Define templates to be filled in, becoming definitions
    class_template = '''\
class {type_name}:
    def __init__(self, {fields}):
        {inits}
        self._fields = [{fields_as_string}]
        self._mutable = {mutable}            # must be set last: see __setattr__

    def __repr__(self):
        return '{type_name}({repr_fmt})'.format({self_fmt})
        
    def __getitem__(self,index):
        if 0 <= index < len(self._fields):
            return eval('self._get_'+self._fields[index]+'()')
        else:
            raise IndexError('{type_name} index('+str(index)+') >= '+str(len(self._fields)))

    def __eq__(self,right):
        if type(right) is not {type_name}:
            return False
            
        for i in range(len(self._fields)):
            if self[i] != right[i]:
                return False
        return True
        
    def _replace(self,**kargs):
        if self._mutable:
            for f,v in kargs.items():
                self.__dict__[f] = v
            return None
        else:
            for f in self._fields:
                if f not in kargs:
                    kargs[f] = self.__dict__[f]
            return {type_name}(**kargs)
     
'''

    mutable_template = '''\
    def __setattr__(self,name,value):
        if ('_mutable' not in self.__dict__ or self._mutable) and name in [{fields},'_fields','_mutable']:
            object.__setattr__(self,name,value)
        else:
            raise AttributeError("Attempted to mutate immutable {type_name} object for field named '"+name+"'")

'''

    get_template = '''\
    def _get_{name}(self):
        return self.{name}
  
'''
  
    init_template = 'self.{name} = {name}'
    repr_template = '{name}'
    self_template = '{name}=self.{name}'

    
    # Fill-in the class template
    class_definition = class_template.format(
        type_name        = type_name,
        fields           = ','.join(name for name in field_names),
        fields_as_string = ','.join("'"+name+"'" for name in field_names),
        inits            = ('\n'+8*' ').join(init_template.format(name=name) for name in field_names),
        mutable          = str(mutable),
        repr_fmt         = ','.join(name+'='+'{'+repr_template.format(name=name)+'}' for name in field_names),
        self_fmt         = ','.join(self_template.format(name=name) for name in field_names)
    )

    get_definition    =  ''.join([get_template.format(name=n) for n in field_names])
    mutate_definition =  mutable_template.format(fields=','.join("'"+name+"'" for name in field_names),type_name=type_name)

    class_definition  += get_definition + mutate_definition
    
    # Good for debugging; don't print when debugged
    #show_listing(class_definition)
    
    # Execute the class_definition string in a temporary name_space and
    name_space = dict(__name__='pnamedtuple_{type_name}'.format(type_name=type_name))
    try:
        exec(class_definition, name_space)
    except SyntaxError:
        show_listing(class_definition)
        traceback.print_exc()
    return name_space[type_name]
