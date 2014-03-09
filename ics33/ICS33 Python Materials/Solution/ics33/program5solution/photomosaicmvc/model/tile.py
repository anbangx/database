from PIL import Image


class Tile():
    def __init__(self,file_name,size,metric):
        self.file_name = file_name
        self.image     = Image.open(file_name).resize(size)
        self.stat      = metric.compute_stat(self.image)

    def change_metric(self,metric):
        self.stat = metric.compute_stat(self.image)

    def reset(self):
        pass
    
    def used(self):
        pass

    def match_info(self):
        return (self.file_name, self.stat)

  
class Tile_Used(Tile):
    def __init__(self,file_name,size,metric):
        Tile.__init__(self,file_name,size,metric)
        self.used_count = 0

    def reset(self):
        self.used_count = 0
    
    def used(self):
        self.used_count += 1

    def match_info(self):
        return (self.file_name, (self.stat,self.used_count))