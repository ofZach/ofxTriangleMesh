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