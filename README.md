ofxTriangleMesh
===============

This extends the original `ofxTriangleMesh` addons to use more features from the `Triangle` library, namely triangulating simple polygon with holes and generating Voronoï Diagram, Conforming Delaunay Triangulation, and Convex Hull.

An new example demonstrates the different features available.

![](https://tlgur.com/d/4RaXEMyg)

(original README below)

***

triangle mesh addon for openframeworks

this is based on ofxTriangle, but uses triangle rather than the c++ wrapper triangle++, which allows for more flexability and options in the triangulation. 

for example, you can set angle or size constraints on the triangles generated, and have the insertion of steiner points, etc. 

there's info here: 

https://www.cs.cmu.edu/~quake/triangle.switch.html

the triangle++ wrapper has a heavily modified version of triangle, this uses the straight thing: 

https://www.cs.cmu.edu/~quake/triangle.html

note: this addon is pretty rough still, so please test this before using it in production. 

examples of smallest minimum angle constraints: 

![](http://i.imgur.com/wzZWW.png)
![](http://i.imgur.com/sjvPM.png)

examples of size constraints: 

![](http://i.imgur.com/QoBPb.png)
![](http://i.imgur.com/rP2ol.png)

