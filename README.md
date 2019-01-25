# Steven Bodnar Assignment ~1~ 2
Welcome to my solution to the problems shown in Assignment 2. This document will discuss my approach, walk-through, implementation and process to soliving each problem

## Approach
My approach to this assignment was to learn how to make useful functions in computer graphics so different operations can use the same functions. You'll notice my approach to drawing the line and drawing the circle are much different even though the same operations are performed.

## Walkthrough
In this section I will walk you through the user interface and the accepted inputs for the optimal run.
### Main menu
```` *********
Welcome to Assignment 1:
 Please select one of the following:
 1. Draw Points
 2. Draw Line
 3. Draw Circle
 4. Exit Program
 ````
 **Accepted Input**: Integer 1-4.
 
 ### Draw Points
 - `Number of points (1-5)?`
    - *Accepted Input*: Integer 1-5.
 - `Point 1: `
    - *Accepted Input*: Desired x integer value, a whitespace, desired y space. <br/>
    - **Example**: `Point 1: 1 1`
  - `Please enter the Hex RGB values (0x00000000 - 0xFFFFFFFF):`
    - *Accepted Input*: The colour value expressed in hex.
    - **Example**: `0x00FF00FF` will make the point Green.
###  Draw Line
  - `Please specify endpoint (xi, yi): `
    - *Accepted Input*: The initial x and y coordinate within the bounds of the screen, separated by a whitespace.
    - **Example**: `Please specify endpoint (xi, yi): 100 253` Sets the initial point at (100,253)
  - `Please specify endpoint (xf, yf): `
      - *Accepted Input*: The final x and y coordinate within the bounds of the screen, separated by a whitespace.
    - **Example**: `Please specify endpoint (xf, yf): 500 372` Sets the initial point at (500 ,372)
  - `Please enter the Hex RGB values (0x00000000 - 0xFFFFFFFF):`
    - *Accepted Input*: The colour value expressed in hex.
    - **Example**: `0x0000FFFF` will make the line Blue.    
  - `Specify the x and y translation :`
    - *Accepted Input*: The positive or negative x and y translation you would like to apply, separated by a white space. **NOTE** Since     the pixels are setup as an array and [0][0] is the top right, the y translation is inverted. So a y translation of 10 will actually     shift the graph down 10 pixels.
    - **Example**: `Specify the x and y translation : 10 25` will shift the line 10 to the right and 25 *DOWN*.  
  - `Specify the degree of rotation (clockwise): `
    - *Accepted Input*: An integer.
    - **Example**: `Specify the degree of rotation (clockwise): 90` Rotate the line 90 degrees clockwise.
    
### Draw Circle
 - `Please specify center of circle (x, y):`
   - *Accepted Input*: two integers separated by a whitespace
   - **Example**: `Please specify center of circle (x, y): 100 100` will mark the center of the circle at (100,100)
 - `Please specify radius of circle (r):`
   - *Accepted Input*: An integer to indicate the radius of the circle
   - **Example**: `Please specify radius of circle (r): 10` the circle will have a radius 10
 - `Please enter the Hex RGB values (0x00000000 - 0xFFFFFFFF):`
   - *Accepted Input*: The colour value expressed in hex.
   - **Example**: `0xFF0000FF` will make the circle Red.  
 - `Specify the x and y translation :`
   - *Accepted Input*: The positive or negative x and y integers, separated by a white space. 
 - `Specify the x and y scaling factors :`
   - *Accepted Input*: The scaling x and y factors (as integers), separated by a white space.
   - **Example** `Specify the x and y scaling factors :2 4` will scale the x radius by 2 and the y radius by 4

  
 

## Implementation
This section is intended to better explain the process of my application. You'll probably notice that 
my implementation of these problems have changed as you go down the list.

 1. The program begins by initalizing the required SDL components.
 2. An infinite loop begins to accept users input and selecting the operations desired.
 3. After the choice is made on the **Main Menu** the canvas is cleared
 4. Depending on your selection a different procedure will occur.
 
 #### Draw Points
 1. Calls out to the drawPoints() function
 2. Here the users will specify their number of points
 3. Once the input has been validated the user is then prompted to enter the x ,y and colour of each point
 4. Each input is validated to ensure the points exist on the canvas.
 5. For each point the pixels array updates the x,y coordinate to contain a pixel of the desired colour
 6. End procedure.
 

 #### Draw Line
 1. Once the user inputs are given the drawLine() function is called.
 2. The function returns an array of the x and y positions of the line.
 3. Now the user inputs how they wish to translate the line
 4. With the array given by the drawLine() call the translateObject() function to update the points of the object
 5. With the updated array we take the input for the rotation of the line.
 6. Using the data of the line and the degree we wish to rotate we call rotatePoint() to perform the final operation.
 7. End Procedure.
 
 #### Draw Circle
 1. The user begins by inputting information of the circle: center point, radius, colour
 2. The program then calls drawCircle() with a scaleX and scaleY of 1
 3. The user is then prompted to enter the translation in the x and y direction
 4. The drawCircle() function is called with the centerPoints and translation amounts being added and scale is still constant
 5. Lasty the user is prompted to indicate a scaling factor in the x and y direction
 6. The drawCircle() function is called this time same new points but scales indicated to their inputted amounts
 7. Update the pixels and display
 8. End procedure.
 
 ## Code
 
 ### Draw Points
 ```cpp
 void drawPoints(uint32_t(*pixels)[SCREEN_WIDTH]) {
	int numPoints;
	cout << "Number of points (1-5)? ";
	while (!(cin >> std::dec >>numPoints) || numPoints > 5 || numPoints < 1) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "Invalid input. Try Again.\n";
		cout << "Number of points (1-5)? ";
	}
	int x, y;
	for (int i = 1; i <= numPoints; i++) {
		cout << "Point " << i << ": ";
		while (!(cin >> std::dec >> x >> std::dec >> y)
			|| x > SCREEN_WIDTH 
			|| x < 0
			|| y > SCREEN_HEIGHT 
			|| y < 0) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "ERROR: Please make sure the x <= "<< SCREEN_WIDTH << " and y <= "<< SCREEN_HEIGHT<<"\n";
			cout << "Point " << i << ": ";
		}

		//show the point on the pixels array
		pixels[y][x] = pickColor();
	}
}
```

### Draw Line
```cpp
size_t* drawLine(uint32_t(*pixels)[SCREEN_WIDTH], int xi, int yi, int xf, int yf, uint32_t colour) {
	if (xf < xi) {
		int temp = xi;
		xi = xf;
		xf = temp;

		temp = yi;
		yi = yf;
		yf = temp;
	}
	double m;
	double c;

	size_t *linePos = new size_t[SCREEN_WIDTH];
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		linePos[i] = NULL;
	}
 
	if (xi == xf) {
		m = 0;
		c = yf;
		for (size_t yPos = yi; yPos <= yf; yPos++) {
			linePos[xi] = yPos;
			pixels[yPos][xi] = colour;
		}

	}
	else {
		m = ((double)yf - (double)yi) / ((double)xf - (double)xi);
		c = yf - (m*xf);
		for (size_t xPos = xi; xPos <= xf; xPos++) {
			unsigned int yPos = static_cast<unsigned int>(round((m*xPos) + c));
			//Once yPos found assign it to the array
			linePos[xPos] = yPos;
			pixels[yPos][xPos] = colour;
		}
	}
	return linePos;
}
```
### Rotate Points
```cpp
void rotatePoint( uint32_t(*pixels)[SCREEN_WIDTH], double degree, size_t object[SCREEN_WIDTH]) {

	int midX, midY;
	midX = (SCREEN_WIDTH / 2) - 1;
	midY = (SCREEN_HEIGHT / 2) - 1;
		
	double theta = (degree * PI) / 180.0;
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		if (object[x] != NULL) {
			int y = (int) object[x];
			double xPrime;
			double yPrime;
			xPrime = (cos(theta)*((double) x-midX)) - (sin(theta)*((double) y-midY));
			yPrime = (sin(theta)*((double) x-midX)) + (cos(theta)*((double) y-midY));

			int xFinal = (int) (xPrime + midX);
			int yFinal = (int) (yPrime + midY);
		
			//if the rotated points lie within the bounds of the pixelsArray then add them
			if (yFinal < SCREEN_HEIGHT && xFinal < SCREEN_WIDTH
				&& yFinal >= 0 && xFinal >= 0) {
				pixels[yFinal][xFinal] = 0xFF0000FF;
			}
		}
		

		
	}
	pixels[(SCREEN_HEIGHT / 2) - 1][(SCREEN_WIDTH / 2) - 1] = 0x000000FF;
}
```

### Draw Circle
```cpp
void drawCircle(uint32_t(*pixels)[SCREEN_WIDTH], int centerX, int centerY, int radius, uint32_t colour, int scaleX, int scaleY) {
	int xTop, xBottom, yTop, yBottom;
	int yRadius = radius * scaleY;
	int xRadius = radius * scaleX;

	//Go through the circle and find the x, y and opposite x coordinate
	for (double i = 0; i < 360; i+= 0.1) {
		xTop =  checkBoundsX((int) (xRadius * cos(i*(PI/180.0))) + centerX);
		yTop = checkBoundsY((int) (yRadius * sin(i*(PI / 180.0))) + centerY);

		xBottom = checkBoundsX((int) abs((xRadius * cos(0-(i*(PI / 180.0)))) - centerX));
		yBottom = yTop;

		//Draw a line connecting the points
		drawLine(pixels, xTop, yTop, xBottom, yBottom, colour);
		
	}
	//Mark the circle center incase
	pixels[centerY][centerX] = 0x000000FF;
}
```
