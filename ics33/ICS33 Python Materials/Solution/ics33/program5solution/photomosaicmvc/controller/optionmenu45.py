from tkinter import OptionMenu, StringVar



#OptionMenu: with title and option_list
#get is a pull function; supply command= for push

class OptionMenu45(OptionMenu):
    def __init__(self,parent,title,option_list,**config):
        self.result = StringVar()
        self.result.set(title)
        OptionMenu.__init__(self,parent,self.result,*option_list,**config)
  
    def get(self):
        return self.result.get()


class OptionMenu45Whiten(OptionMenu45):
    def __init__(self,parent,title,option_list,whiten_entry=None,**config):
        self.whiten_entry = whiten_entry
        self.result = StringVar()
        self.result.set(title)
        OptionMenu45.__init__(self,parent,title,option_list,command=self.whiten,**config)

  
    def whiten(self,option):
        self.whiten_entry.config(bg="white")
        



#OptionMenu: with title and option_list
#get is an inherited pull function; put is a push function, pushing
#  the selected option into the linked_entry (replacing what is there)

class OptionMenu45ToEntry(OptionMenu45):
    def __init__(self,parent,title,option_list,linked_entry,**config):
        self.entry = linked_entry
        OptionMenu45.__init__(self,parent,title,option_list,command=self.put,**config)
  
    def put(self,option):
        self.entry.delete(0,len(self.entry.get()))
        self.entry.insert(0,option)
        self.entry.config(bg="white")
