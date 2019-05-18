# Water Ripple

## Animation of a water ripple effect over a selectable image using Qt Widgets

The idea and some of the code were copied from: http://agilerepose.weebly.com/water-ripple.html

Example of the application:

![](https://github.com/sfadiga/WaterRipple/blob/master/anim.gif)

the idea behind this code is to update a pixel table from the original image making the origin of the efect in (x,y) to propagate changes on the pixels in 4 directions, basically a loop updates the frame buffer using the following pseudo-code:

```

damping = some non-integer between 0 and 1

  begin loop 
      for every non-edge element:
      loop
          Buffer2(x, y) = (Buffer1(x-1,y)
                           Buffer1(x+1,y)
                           Buffer1(x,y+1)
                           Buffer1(x,y-1)) / 2 - Buffer2(x,y)
 
          Buffer2(x,y) = Buffer2(x,y) * damping
      end loop

      Display Buffer2
      Swap the buffers 

  end loop

```
the original idea is well explained here:
https://web.archive.org/web/20160418004149/http://freespace.virgin.net/hugo.elias/graphics/x_water.htm
