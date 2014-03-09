import re, traceback, keyword

class Point:
    def __init__(self, field1, field2):
        self._fields = [field1, field2]
        self._mutable = True
    
    def __repr__(self):
        return 'Point(' + ",".join(str(i) for i in self._fields) + ')';
    
    def __getitem__(self, index):
        if type(index) is int:
            return eval('self._fields[' + str(index) + ']')
        elif index in self._fields:
            return self[index]
        else:
            raise IndexError('Point.__getitem__:' + str(index) + ' is illegal!')
    
    def __eq__(self, right):
        if type(right) is not Point:
            return False
        
        for i in self._fields:
            print('self[i]: ' + str(self[i]))
            print('right[i]: ' + str(right[i]))
            if self[i] != right[i]:
                return False
        return True
        
    def _replace(self, **kargs):
        if self._mutable:
            for f,v in kargs:
                self._fields[f] = v
            return self
        else:
            for f in self._fields:
                if f not in kargs:
                    kargs[f] = self._fields[f]
            return Point(kargs)
                
def pnamedtuple(type_name, field_names, mutable=False):
    def show_listing(s):
        for i,l in enumerate(s.split('\n'),1):
            print('{num: >3} {text}'.format(num=i, text = l.rstrip()))

            
    class_template = '''
        class {type_name}:
            def __init__(self, {fields}):
                {inits}
                self._mutable = {mutable}
            
            def __repr__(self):
                return '{type_name}(' + {repr_fmt} + ')';
            
            def __getitem__(self, index):
                if type(index) is int:
                    return eval('self._fields[' + str(index) + ']')
                elif index in self._fields:
                    return self[index]
                else:
                    raise IndexError('{type_name}.__getitem__:' + str(index) + ' is illegal!')
            
            def __eq__(self, right):
                if type(right) is not {type_name}:
                    return False
                
                for i in self._fields:
                    print('self[i]: ' + str(self[i]))
                    print('right[i]: ' + str(right[i]))
                    if self[i] != right[i]:
                        return False
                return True
                
            def _replace(self, **kargs):
                if self._mutable:
                    for f,v in kargs:
                        self._fields[f] = v
                    return self
                else:
                    for f in self._fields:
                        if f not in kargs:
                            kargs[f] = self._fields[f]
                    return {type_name}(kargs)
    '''
    # put your code here
    # bind class_definition (used below) to the string constructed for the class
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

    # For initial debugging, always show the source code of the class
    #show_listing(class_definition)
    
    # Execute the class_definition string in a local name_space and bind the
    #   name source_code in its dictionary to the class_defintion; return the
    #   class object created; if there is a syntax error, list the class and
    #   show the error
    name_space = dict(__name__='pnamedtuple_{type_name}'.format(type_name=type_name))
    try:
        exec(class_definition, name_space)
        name_space[type_name].source_code = class_definition
    except SyntaxError:
        show_listing(class_definition)
        traceback.print_exc()
    return name_space[type_name]


    
if __name__ == '__main__':
#     import driver
#     driver.driver()
    
#     x = Point(1,'hello')
#     print(x)
#     
#     print('index0: ' + str(x[0]))
#     print('index1: ' + str(x[1]))
#     
#     a = Point(1, 2)
#     b = Point(1, 2)
#     print('a == b ? ' + str(a == b))
    
    # Test pnamedtuple (as pnt)
    Triple1 = pnamedtuple('Triple1', 'a b c')
    
    
