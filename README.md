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
    - *Accepted Input*: The initial x and y coordinate within the bounds of the screen, seperated by a whitespace.
    - **Example**: `Please specify endpoint (xi, yi): 100 253` Sets the initial point at (100,253)
  - `Please specify endpoint (xf, yf): `
      - *Accepted Input*: The final x and y coordinate within the bounds of the screen, seperated by a whitespace.
    - **Example**: `Please specify endpoint (xf, yf): 500 372` Sets the initial point at (500 ,372)
  - `Please enter the Hex RGB values (0x00000000 - 0xFFFFFFFF):`
    - *Accepted Input*: The colour value expressed in hex.
    - **Example**: `0x0000FFFF` will make the line Blue.    
  - `Specify the x and y translation :`
    - *Accepted Input*: The positive or negative x and y tanslation you would like to apply, seperated by a white space. **NOTE** Since     the pixels are setup as an array and [0][0] is the top right, the y translation is inverted. So a y translation of 10 will actually     shift the graph down 10 pixels.
    - **Example**: `Specify the x and y translation : 10 25` will shift the line 10 to the right and 25 *DOWN*.  
  - `Specify the degree of rotation (clockwise): `
    - *Accepted Input*: An integer.
    - **Example**: `Specify the degree of rotation (clockwise): 90` Rotate the line 90 degrees clockwise.
    
### Draw Circle
  -`Please specify center of circle (x, y):`
    - *Accepted Input*: two integers seperated by a whitespace
    - **Example**: `Please specify center of circle (x, y): 100 100` will mark the center of the circle at (100,100)
  -`Please specify radius of circle (r):`
    - *Accepted Input*: An integer to indicate the radius of the circle
    - **Example**: `Please specify radius of circle (r): 10` the circle will have a radius 10
  - `Please enter the Hex RGB values (0x00000000 - 0xFFFFFFFF):`
    - *Accepted Input*: The colour value expressed in hex.
    - **Example**: `0xFF0000FF` will make the circle Red.  
   - `Specify the x and y translation :`
    - *Accepted Input*: The positive or negative x and y integers, seperated by a white space. 
   - `Specify the x and y scaling factors :`
    -*Accepted Input*: The scaling x and y factors (as integers), seperated by a white space.
    - **Example** `Specify the x and y scaling factors :2 4` will scale the x radius by 2 and the y radius by 4

  
 

## Implementation
I have written different methods to perform the heavy operations, this section will highlight them.
