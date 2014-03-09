from PIL import Image

from tkinter.simpledialog import askinteger

import photomosaicmvc.model.filters as filters
import photomosaicmvc.controller

import prompt

class Photomosaic():
    def __init__(self,file_name,size=None):
        if size == None:
            self.image = Image.open(file_name)
        else:
            self.image = Image.open(file_name).resize(size)

    def filter(self,afilter):
        photomosaicmvc.controller.the_message_textbox.add("Updating picture with new filter")
        filters.apply_filter(self.image,afilter)
        photomosaicmvc.controller.the_message_textbox.add("Done updating picture with new filter")

    def create_lattice(self,dbsize):
        return [(x,y) for x in range(0,self.image.size[0]-dbsize[0],dbsize[0])
                        for y in range(0,self.image.size[1]-dbsize[1],dbsize[1])
               ] 
   
    def make(self,db):
        photomosaicmvc.controller.the_message_textbox.add("Making photomosaic...")
        computed = Image.new("RGB",self.image.size)
        lattice = self.create_lattice(db.size)
        db.reset() ###
        percent = 10
        i = 0
        for ul in lattice:
            match = db.closest(db.metric.compute_stat(self.image,ul,db.size))
            if match == None:
                photomosaicmvc.controller.the_message_textbox.add("Unable to find match...quiting with partially made photomosaic")
                return computed
            match.used() ###
            computed.paste(match.image,ul)
            i += 1
            if 100*i/len(lattice) > percent:
                photomosaicmvc.controller.the_message_textbox.add(str(percent)+"%...")
                percent += 10
        else:
            photomosaicmvc.controller.the_message_textbox.add("100% completed")

        return computed


class Photomosaic_Limited_Repeat(Photomosaic):
    def __init__(self,file_name,size=None):
        Photomosaic.__init__(self,file_name,size)
    
    def create_lattice(self,db):
        lattice = Photomosaic.create_lattice(self,db)
        def how_close_to_center(xy):
            return (xy[0]-self.image.size[0]/2)**2 + (xy[1]-self.image.size[1]/2)**2
        lattice.sort(key=how_close_to_center)
        return lattice

    def make(self,db):
        db.max_use = prompt.for_int("Maximum times to (re)use pixel")
        #db.max_use = askinteger("Photomosaic_Limited_Repeat","Maximum times to (re)use pixel")
        return Photomosaic.make(self,db)
