GRASS MODULE for pygame originally created by Dafluffypotato, a pygame MERCHANT. A modified version of it with an added functionality, burning grass. 🔥 

UPDATE (24.05.11) 

BURN SPREAD TESTING 

![burn_spread_test](https://github.com/papercd/pygame-grass-modified-for-Noel/assets/71172544/32ad1e75-252f-40bf-9a7e-2c22fc55a268)

UPDATE (24.05.15) 

grass now changes color if it is burning, and deleted if burnt completely. There is a  slight rendering problem where burn doesn't proceed when grass goes out of screen. 

![gif2](https://github.com/papercd/pygame-grass-modified-for-Noel/assets/71172544/0ff54162-b7be-4be1-9990-eab679dddf31)

added method for GRASSMANAGER class : 

```
def burn_tile(self,location):
        if tuple(location) in self.grass_tiles: 
            #if the location does indeed have grass, burn the grass.
            self.grass_tiles[tuple(location)].burning = 0

```
This method is used to burn the grass at a certain location. Burning grass in this code just means shrinking down the image and 
shading it with a certain color according to how long it has burnt. Burn spread is done by checking nearby grasstiles. 
