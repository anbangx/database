def apply_filter(image,f):
    for x in range(image.size[0]):
        for y in range(image.size[1]):
            image.putpixel((x,y), f(image.getpixel((x,y))))


def gray_scale(rgb):
    #pixel -> pixel
    avg = (rgb[0]+rgb[1]+rgb[2])//3
    return (avg,avg,avg) 


def intensity(rgb_delta):
    def mm(v): return min(max(v,0),255)
    def inner(rgb):
        return mm(rgb[0]+rgb_delta[0]), mm(rgb[1]+rgb_delta[1]), mm(rgb[2]+rgb_delta[2])
    return inner