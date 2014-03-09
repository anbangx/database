from PIL import Image

class Average_BW:
    def compute_stat(self,image,ul=(0,0),size=None):
        if size == None:
            size = (image.size[0]-ul[0],image.size[1]-ul[1])
        i = 0
        pixels = size[0]*size[1]
        for x in range(ul[0],ul[0]+size[0]):
            for y in range(ul[1],ul[1]+size[1]):
                rgb = image.getpixel((x,y))
                i += rgb[0] + rgb[1] + rgb[2]
        return i //(3*pixels)

    def distance(self,stat1,stat2):
        return abs(stat1-stat2)


class Average_RGB:
    def compute_stat(self,image,ul=(0,0),size=None):
        if size == None:
            size = image.size
        r,g,b = 0,0,0
        pixels = size[0]*size[1]
        for x in range(ul[0],ul[0]+size[0]):
            for y in range(ul[1],ul[1]+size[1]):
                rgb = image.getpixel((x,y))
                r += rgb[0]
                g += rgb[1]
                b += rgb[2]
        return (r//pixels,g//pixels,b//pixels)

    def distance(self,stat1,stat2):
        return abs(stat1[0]-stat2[0]) + abs(stat1[1]-stat2[1]) + abs(stat1[2]-stat2[2])

    def reduce_distance(self,tile,tile_stat,picture_stat):
        def constrain(p): return 255 if p>255 else (0 if p<0 else p)
        shifted = Image.new("RGB",tile.size)
        delta = tuple(picture_stat[i] - tile_stat[i] for i in range(3))
        for x in range(tile.size[0]):
            for y in range(tile.size[1]):
                rgb = list(tile.getpixel((x,y)))
                for c in range(3):
                    rgb[c] = constrain(rgb[c]+delta[c])
                shifted.putpixel((x,y),tuple(rgb))
        return shifted
        

class Quad_Metric: 
    def __init__(self,metric):
        self.metric = metric

    def compute_stat(self,image,ul=(0,0),size=None):
        if size == None:
            size = image.size
        size2 = (size[0]//2,size[1]//2)
        return (
          self.metric.compute_stat(image, (ul[0],        ul[1]),           size2),
          self.metric.compute_stat(image, (ul[0]+size2[0],ul[1]),          size2),
          self.metric.compute_stat(image, (ul[0],        ul[1]+size2[1]),  size2),
          self.metric.compute_stat(image, (ul[0]+size2[0],ul[1]+size2[1]), size2)
        )


    def distance(self,stat1,stat2):
        return sum( [self.metric.distance(stat1[i],stat2[i]) for i in range(4)] )
    
    def reduce_distance(self,tile,tile_stat,picture_stat):
        size = tile.size
        shifted = Image.new("RGB",size)
        size2 = (size[0]//2,size[1]//2)
        tile0 = Image.new("RGB",(size2))
        for x in range(size2[0]):
            for y in range(size2[1]):
                tile0.putpixel((x,y),tile.getpixel((x,y)))
        shifted.paste(self.metric.reduce_distance(tile0,tile_stat[0],picture_stat[0]),(0,0))
        
        tile1 = Image.new("RGB",(size[0]-size2[0],size2[1]))
        for x in range(size[0]-size2[0]):
            for y in range(size2[1]):
                tile1.putpixel((x,y),tile.getpixel((x+size2[0],y)))
        shifted.paste(self.metric.reduce_distance(tile1,tile_stat[1],picture_stat[1]),(size2[0],0))
         
        tile2 = Image.new("RGB",(size2[0],size[1]-size2[1]))
        for x in range(size2[0]):
            for y in range(size[1]-size2[1]):
                tile2.putpixel((x,y),tile.getpixel((x,y+size2[1])))
        shifted.paste(self.metric.reduce_distance(tile2,tile_stat[2],picture_stat[2]),(0,size2[1]))
         
        tile3 = Image.new("RGB",(size[0]-size2[0],size[1]-size2[1]))
        for x in range(size[0]-size2[0]):
            for y in range(size[1]-size2[1]):
                tile3.putpixel((x,y),tile.getpixel((x+size2[0],y+size2[1])))
        shifted.paste(self.metric.reduce_distance(tile3,tile_stat[3],picture_stat[3]),(size2[0],size2[1]))
        
        
        
        
        return shifted
