 /*!
 
 ofxTriangleMesh by zach lieberman
 
 based off of 

 ofxTriangle by kikko.fr
 Point inside polygon by Theo / paul bourke
 

 this code uses the original triangle (not triangle++) to allow for switches like 
 q33 a100, etc 
 
 https://www.cs.cmu.edu/~quake/triangle.switch.html
 
 there are options that allow for the addition of new points to make a much nicer mesh, 
 triangle++ had that turned off and a heavily modified version of triangle, 
 so this is using the original triangle library and allows for some control of parameters
 
*/

#pragma once



#include "ofMain.h"




typedef struct{
    
    ofPoint pts[3];
    int index[3];           // for the mesh, what points does this triangle relate to.
    ofColor randomColor;    // useful for debugging / drawing 

} meshTriangle;





class ofxTriangleMesh {
    
    public :

    
    
        ofxTriangleMesh();
        
    
        // usage notes: 
    
        // -1 = don't use constraint, other values = use constraint 
        // 
        // for angle, 20-30 is pretty good
        // https://www.cs.cmu.edu/~quake/triangle.q.html
        // be careful!  "It usually doesn't terminate for angles above 34o"
        //
        // for size, this depends on the size of your shape, 
        // 100 to 200 is a good first guess for screen resolution based points
    
        void triangulate(ofPolyline contour, float angleConstraint = -1, float sizeConstraint = -1);

    
        
        ofPoint getTriangleCenter(ofPoint *tr);
        bool isPointInsidePolygon(ofPoint *polygon,int N, ofPoint p);

        void draw();
        void clear();

        int nTriangles;
        vector <ofPoint> outputPts;
        vector <meshTriangle> triangles;
        ofMesh triangulatedMesh;
    
      
    


};