ofxTriangleMesh
===============

triangle mesh addon for openframeworks

this is based on ofxTriangle, but uses triangle rather than the c++ wrapper triangle++, which allows for more flexability and options in the triangulation. 

for example, you can set angle or size constraints on the triangles generated, and have the insertion of steiner points, etc. 

there's info here: 

https://www.cs.cmu.edu/~quake/triangle.switch.html

the triangle++ wrapper has a heavily modified version of triangle, this uses the straight thing: 

https://www.cs.cmu.edu/~quake/triangle.html

note: this addon is pretty rough still, so please test this before using it in production. 

examples of size constraints: 

http://s10.postimage.org/ecq035xpz/Screen_Shot_2012_11_28_at_4_17_56_PM.png
http://s10.postimage.org/up01swc1j/Screen_Shot_2012_11_28_at_4_26_00_PM.png

smallest minimum angle constraints: 

http://s10.postimage.org/bl6qcjz7b/Screen_Shot_2012_11_28_at_4_26_40_PM.png
http://s10.postimage.org/pswf17bw7/Screen_Shot_2012_11_28_at_4_26_45_PM.png


