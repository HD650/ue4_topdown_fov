# ue4_topdown_fov
Implementation of the top down field of view system.   
Here is the [unity version tutorial](https://www.youtube.com/watch?v=xkcCWqifT9M&t=296s).   
Use ray casting to calculate the field of view, then generate a procedural mesh as a stencil, 
finally use the stencil to darken pixel during post processing.   
![In Orthographic camera](https://github.com/HD650/ue4_topdown_fov/raw/master/gif/orth.gif)   
![In Perspective camera](https://github.com/HD650/ue4_topdown_fov/raw/master/gif/persp.gif)
## TODO
* capsulate it as a component
* hide enemy out side the view
* find a better way when using perspective camera