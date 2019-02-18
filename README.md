# Steven Bodnar Assignment 3
Welcome to my solution to the problems shown in Assignment 3. This document will discuss my approach, walk-through, implementation and process to soliving each problem

## Walkthrough
In this section I will walk you through the user interface and the accepted inputs for the optimal run.
### Main menu
```` *********
Welcome to Assignment 3:
 Please select one of the following:
 1. Clip Polygon
 2. Fill Polygon
 3. Exit Program
>>
 ````
 **Accepted Input**: Integer 1-3.
 
 ### Clip Polygon
 - `Specify the number of points:`
    - *Accepted Input*: Any integer > 2.
    - **Example**: `Specify the number of points: 3`
 - `Point 1:`
    - *Accepted Input*: Desired x integer value, a whitespace, desired y. <br/>
    - **Example**: `Point 1: 1 1`
    - **NOTE**: You must input your vertices in clockwise manner to display the desired shape<br/>
Sample: Below will draw a triangle
````
Point 1: 150 100
Point 2: 200 200
Point 3: 100 200
````
 - `Specify the start point of the polygon1:`
    - *Accepted Input*: Desired x integer value, a whitespace, desired y . <br/>
    - **Example**: `Specify the start point of the polygon1: -100 -100` Will start drawing your polygon shape from (-100,-100)
    - **NOTE**: You must input your vertices in clockwise manner to display the desired shape
````
Please select a clipping algorithm:
        1. Sutherland-Hodgman
        2. Liang-Barsky
>>
````
  -
    - *Accepted Input*: 1 or 2, Selection of the clipping algorithim you wish to test<br/>
    - **Example**: `>>1` will clip your polygon using the Sutherland-Hodgman Algorithm
 



###  Fill Polygon
 - `Specify the number of points:`
    - *Accepted Input*: Any integer > 2.
    - **Example**: `Specify the number of points: 3`
  - `Point 1:`
    - *Accepted Input*: Desired x integer value, a whitespace, desired y. <br/>
    - **Example**: `Point 1: 1 1`
    - **NOTE**: You must input your vertices in clockwise manner to display the desired shape<br/>
Sample: Below will draw a triangle
````
Point 1: 150 100
Point 2: 200 200
Point 3: 100 200
````
  - `Give a point inside the polygon:`
    - *Accepted Input*:  x integer value, a whitespace, y value. <br/>
    - **Example**: `Give a point inside the polygon: 151 101`
    - **NOTE**: The x and y must be inside the polygon to work<br/>
  - `Enter any character to continue...`
    - *Accepted Input*:  Any character and enter <br/>
    
## Implementation
This section is intended to better explain the process of my application. You'll probably notice that 
my implementation of these problems have changed as you go down the list.

 1. The program begins by initalizing the required SDL components.
 2. An infinite loop begins to accept users input and selecting the operations desired.
 3. After the choice is made on the **Main Menu** the canvas is cleared
 4. Depending on your selection a different procedure will occur.
 
 #### Clip Polygon
 1. Calls out to the clippingPolygonProcedure() function
 2. Here the users will enter their choices and points
 3. Once the input has been validated the user is then prompted to enter the x ,y to translate the point
 4. Then the user is to select a clipping algorithm
 5. If the user chooses OPTION(1) then the sutherlandHodgmanClip() is called
 6. If the suer chooses OPTION(2) then liangBarskyHelper() is called
 7. Steps 4-6 are repeated one more time for the user to select another clipping alogrithm
 
 

 #### Fill Polygon Line
 1. Calls out to the fillingPolygonProcedure() function
 2. Here the users will enter their choices and points
 3. Once the input has been validated the user inputs a point that exists in the polygon
 4. Then floodFill() method is invoked
 5. Once floodFill() completes the canvas is updated and the user must input any character to execute the scanline algorithm
 6. After the key is inputted scanLine() is executed and the canvas is updated
 7. End Procedure.
 
 
 ## Code
 **References are mentioned in the source code**
 ### scanLine()
 ```cpp
void scanLine(uint32_t(*pixels)[SCREEN_WIDTH], int yMin, int yMax, vector<Point> poly) {

	//Going top down lets create a table of edges we can scan
	vector<Line>edges;
	for (int i = 0; i < poly.size(); i++) {
		int k = (i + 1) % poly.size();

		Point p1 = poly[i];
		Point p2 = poly[k];
		
		if (p1.x > p2.x) {
			swap(p1.x, p2.x);
			swap(p1.y, p2.y);
		}
		double m;
		double b;
		if (p1.x != p2.x) {
			m = (double)(p2.y - p1.y) / (double)(p2.x - p1.x);
			b = p2.y - (double)m*(double)p2.x;
		}
		else {
			m = 0;
			b = p2.y;
		}
		if ((p1.y != p2.y)) {
			edges.push_back(Line(m, b, p1,p2));
			cout << "Line " << i << ": (" << poly[i].x << ", " << poly[i].y << ") to (" << poly[k].x << ", " << poly[k].y << ")\n";
			cout << "y = " << m << "x +" << b << "\n";
		}
	}

	for (int y = yMin + 1; y < yMax; y++) {
		vector<int>xPoints;
		int xPos;
		for (Line line : edges) {
			if (line.p1.x != line.p2.x) {
				xPos = round((y - line.b) / line.m);
				xPoints.push_back(xPos);
			}
			else {
				xPos = line.p1.x;
				xPoints.push_back(xPos);
			}
		}

		//Remove duplicates
		sort(xPoints.begin(), xPoints.end());
		xPoints.erase(unique(xPoints.begin(), xPoints.end()), xPoints.end());


		for (int x : xPoints) {
			cout << x << " ";
		}
		cout << "\n";

		int pos = xPoints[0]; // Current intersection point we are looking for
		int x = 0;
		bool isFilling = false;
		int k = 1;
		while (x < SCREEN_WIDTH) {
			if (x == pos) {
				isFilling = !isFilling;
				if(k < xPoints.size())
				pos = xPoints[k++]; // Update the intersection
			}

			if (isFilling) {
				pixels[y][x] = 0x000000FF;
			}
			else {
				pixels[y][x] = 0xFFFFFFFF;
			}
			x++;
		}
	}
}
```

### sutherlandHodgmanClip
```cpp
void clip(vector<Point> &poly, Point p1, Point p2) {

	vector<Point> newPoly;

	int x1, x2, y1, y2;
	x1 = p1.x; y1 = p1.y; x2 = p2.x; y2 = p2.y;

	// (ix,iy),(kx,ky) are the values of the points 
	for (int i = 0; i < poly.size(); i++) {
		int k = (i + 1) % poly.size();

		// Two consecutive points
		int ix = poly[i].x, iy = poly[i].y;
		int kx = poly[k].x, ky = poly[k].y;

		// Calculating position of first point 
		// w.r.t. clipper line 
		int i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

		// Calculating position of second point 
		// w.r.t. clipper line 
		int k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

		// Case 1 : When both points are inside 
		if (i_pos < 0 && k_pos < 0) {
			newPoly.push_back(Point(kx, ky));
		}

		// Case 2: When only first point is outside 
		else if (i_pos >= 0 && k_pos < 0) {
			Point temp;
			temp.x = x_intersect(Point(x1, y1), Point(x2, y2), Point(ix, iy), Point(kx, ky));
			temp.y = y_intersect(Point(x1, y1), Point(x2, y2), Point(ix, iy), Point(kx, ky));
			newPoly.push_back(temp);
			newPoly.push_back(Point(kx, ky));
		}

		// Case 3: When only second point is outside 
		else if (i_pos < 0 && k_pos >= 0) {
			Point temp;
			temp.x = x_intersect(Point(x1, y1), Point(x2, y2), Point(ix, iy), Point(kx, ky));
			temp.y = y_intersect(Point(x1, y1), Point(x2, y2), Point(ix, iy), Point(kx, ky));
			newPoly.push_back(temp);
		}

		// Case 4: When both points are outside, do nothing
	}

	// The new polygon after being clipped
	poly = newPoly;
}

void sutherlandHodgmanClip(vector<Point> &poly, vector<Point> clipper) {
	//i and k are two consecutive indexes 
	for (int i = 0; i < clipper.size(); i++) {
		int k = (i + 1) % clipper.size();
		clip(poly, clipper[i], clipper[k]);
	}
}

```
### drawPolygon()
```cpp
void drawPolygon(uint32_t(*pixels)[SCREEN_WIDTH], vector<Point> polygon) {
	for (int i = 0; i < polygon.size() - 1; i++) {
		drawLine(pixels, polygon[i].x, polygon[i].y, polygon[i + 1].x, polygon[i + 1].y);
	}

	drawLine(pixels, polygon[0].x, polygon[0].y, polygon[polygon.size() - 1].x, polygon[polygon.size() - 1].y);
}
```

### liangBarsky()
```cpp

void liangBarsky(float xmin, float ymin, float xmax, float ymax, 
				int x1, int y1, int x2, int y2, uint32_t(*pixels)[SCREEN_WIDTH]) {


	//defining variables
	float p1 = -(x2 - x1);
	float p2 = -p1;
	float p3 = -(y2 - y1);
	float p4 = -p3;

	float q1 = x1 - xmin;
	float q2 = xmax - x1;
	float q3 = y1 - ymin;
	float q4 = ymax - y1;

	float posarr[5], negarr[5];
	int posind = 1, negind = 1;
	posarr[0] = 1;
	negarr[0] = 0;

	if ((p1 == 0 && q1 < 0) || (p3 == 0 && q3 < 0)) {
		//Line is parallel to clipping window!
		return;
	}
	if (p1 != 0) {
		float r1 = q1 / p1;
		float r2 = q2 / p2;

		if (p1 < 0) {
			negarr[negind++] = r1; // for negative p1, add it to negative array
			posarr[posind++] = r2; // and add p2 to positive array
		}
		else {
			negarr[negind++] = r2;
			posarr[posind++] = r1;
		}
	}

	if (p3 != 0) {
		float r3 = q3 / p3;
		float r4 = q4 / p4;

		if (p3 < 0) {
			negarr[negind++] = r3;
			posarr[posind++] = r4;
		}
		else {
			negarr[negind++] = r4;
			posarr[posind++] = r3;
		}
	}

	float xn1, yn1, xn2, yn2;
	float rn1, rn2;
	rn1 = maxi(negarr, negind); // maximum of negative array
	rn2 = mini(posarr, posind); // minimum of positive array

	if (rn1 > rn2) { // reject
		return;
	}

	// Computing the new points
	xn1 = x1 + p2 * rn1;
	yn1 = y1 + p4 * rn1;
	xn2 = x1 + p2 * rn2;
	yn2 = y1 + p4 * rn2;

	// Directly drawing the line made by the 2 (new) points
	drawLine(pixels, xn1, yn1, xn2, yn2);
}
```
