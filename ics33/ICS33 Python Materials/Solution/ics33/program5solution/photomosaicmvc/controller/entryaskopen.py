from tkinter import Entry
from tkinter.filedialog import askopenfilename

import os.path


#FileEnableEntry: with parent, for entering file names
#We can either type the file name or right-click (<Button-3>)
#  to bring up a special file dialog box, for selecting a file
#If we type anything into this Entry or select using a file dialog
#  box, we change its background to white (meaning a new name is there,
# as of yet unprocessed)

class FileEnabledEntry45(Entry):
    def __init__(self,parent,**config):
        Entry.__init__(self,parent,**config)
        self.parent = parent
        self.displayed_picture = False
        self.bind("<Button-3>",   lambda event : self.ask_name())
        self.bind("<KeyRelease>", lambda event : self.config(bg="white"))
  
    def derive_name(self,file):
        return os.path.basename(file)
    
    def ask_name(self):
        file = askopenfilename()
        self.delete(0,len(self.get()))
        self.insert(0,self.derive_name(file))
        self.config(bg="white")
        self.focus_set()
    
    
    
#FolderEnableEntry: with parent, for entering list of file names
#We can either type the file name (for globbing) or right-click
#  (<Button-3>) to bring up a special file dialog box for selecting
#  a file; what comes back is text for selecting all the files that
#  are in the same folder as the selected one
#If we type anything into this Entry or select using a file dialog
#  box, we change its background to white (meaning a new name is there,
# as of yet unprocessed)

class FolderEnabledEntry45(FileEnabledEntry45):
    def __init__(self,parent,**config):
        FileEnabledEntry45.__init__(self,parent,**config)
    
    def derive_name(self,file):
        return os.path.basename(os.path.dirname(file))+os.sep+"*"
