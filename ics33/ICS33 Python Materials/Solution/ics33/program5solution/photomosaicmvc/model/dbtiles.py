from PIL import Image
from photomosaicmvc.view.displayimage import display_image

import photomosaicmvc.controller
import photomosaicmvc.view

from photomosaicmvc.model.tile import Tile,Tile_Used
import photomosaicmvc.model.filters as filters
import math



class DB_Tiles():
    def __init__(self,size,metric):
        self.size   = size
        self.metric = metric
        self.tiles  = []

  
    def load_helper(self,tile_class,file_list,reset_before_read=True):
        photomosaicmvc.controller.the_message_textbox.add("Loading database...")
        if reset_before_read:
            self.tiles = []
        percent = 10
        i = 0
        for f in file_list: 
            if self.size == None:
                t = Image.open(f)
                self.size = t.size
                self.tiles.append(tile_class(f,self.size,self.metric))
            else: 
                self.tiles.append(tile_class(f,self.size,self.metric))
            i += 1
            if 100*i/len(file_list) > percent:
                photomosaicmvc.controller.the_message_textbox.add(str(percent)+"%...")
                percent += 10
        photomosaicmvc.controller.the_message_textbox.add("100% completed")
        

    def load(self,file_list,reset_before_read=True):
        self.load_helper(Tile,file_list,reset_before_read)
     

    def change_metric(self,metric):
        photomosaicmvc.controller.the_message_textbox.add("Updating dbpictures with new metric")
        self.metric = metric
        for p in self.tiles:
            p.change_metric(metric)
        photomosaicmvc.controller.the_message_textbox.add("Done")


    def filter(self,afilter):
        photomosaicmvc.controller.the_message_textbox.add("Updating dbpictures with new filter/recompute metric")
        for p in self.tiles:
            filters.apply_filter(p.image,afilter)
        #recompute same metric (not really change) on each filtered picture
        for p in self.tiles:
            p.change_metric(self.metric)
        photomosaicmvc.controller.the_message_textbox.add("Done")
  

    
    def sort(self,key):
        self.tiles.sort(key=key)

    
    #Cannot show this one yet
    #xs*ys = len
    #xs = height/width * ys
    #ys**2 height/width = len
    #ys = sqrt(width/height * len)
    #xs = sqrt(height/width * len)
    #xs * ys = len
    def display(self):
        def compose_images(picture_list,xs,ys):
            result = Image.new("RGB",(self.size[0]*xs,self.size[1]*ys))
            i = 0
            for y in range(ys):
                for x in range(xs):
                    if i >= len(picture_list):
                        return result
                    result.paste(picture_list[i].image,(x*self.size[0],y*self.size[1]))
                    i += 1
            return result
    
        xs = round(math.sqrt(self.size[1]/self.size[0] * len(self.tiles)))
        ys = round(math.sqrt(self.size[0]/self.size[1] * len(self.tiles)))
        while xs*ys < len(self.tiles):
            ys += 1
        result = compose_images(self.tiles,xs,ys)
        display_image("Database",result)

  
    def closest(self,picture_stat):
        best      = None
        best_dist = None
        for i in self.tiles:
            dist = self.metric.distance(i.stat,picture_stat)
            if (best_dist == None or dist < best_dist):
                best = i
                best_dist = dist
        return best
    
  
    def reset(self):
        for p in self.tiles:
            p.reset()

  
    def match_info(self):
        return dict([i.match_info() for i in self.tiles])



  
class DB_Tiles_Used(DB_Tiles):
    def __init__(self,size,metric):
        DB_Tiles.__init__(self,size,metric)

    def load(self,file_list,reset_before_read=True):
        self.load_helper(Tile_Used,file_list,reset_before_read)

    def closest(self,picture_stat):
        best      = None
        best_dist = None
        for i in self.tiles:
            dist = self.metric.distance(i.stat,picture_stat)
            if (best_dist == None or dist < best_dist) and (self.max_use == None or i.used_count < self.max_use):
                best = i
                best_dist = dist
        return best
    

class DB_Tiles_Random_Shifted(DB_Tiles):
    def __init__(self,size,metric):
        DB_Tiles.__init__(self,size,metric)

    def closest(self,picture_stat):
        import random
        best = random.choice(self.tiles)
        
        class T(Tile):
            def __init__(self,image,file_name,stat):
                self.file_name = file_name
                self.image     = image
                self.stat      = stat
        best_image = self.metric.reduce_distance(best.image,best.stat,picture_stat)
        return T(best_image,best.file_name,self.metric.compute_stat(best_image))


class DB_Tiles_Best_Shifted(DB_Tiles):
    def __init__(self,size,metric):
        DB_Tiles.__init__(self,size,metric)

    def closest(self,picture_stat):
        best = DB_Tiles.closest(self,picture_stat)
        
        class T(Tile):
            def __init__(self,image,file_name,stat):
                self.file_name = file_name
                self.image     = image
                self.stat      = stat
        best_image = self.metric.reduce_distance(best.image,best.stat,picture_stat)
        return T(best_image,best.file_name,self.metric.compute_stat(best_image))