
#include <cstdio> 
#include <iostream>
#include <vector>
#include <algorithm>
#include <SDL.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;


void clearScene(uint32_t(*pixels)[SCREEN_WIDTH]) {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			pixels[j][i] = 0xFFFFFFFF;
		}
	}
}

//Perform the operations needed to update the screens with the current pixel information
void updateScene(SDL_Texture *texture, SDL_Surface *canvas, SDL_Renderer *renderer) {
	SDL_UpdateTexture(texture, NULL, canvas->pixels, canvas->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

//Struct for x and y coordinates
struct Point {
	int x, y;
	Point() {}
	Point(int x, int y) { this->x = x; this->y = y; }
};

//Global variable to determine screen bounds;
vector<Point> boundsClipping;

void translatePolygon(vector<Point> &polygon, Point point) {

	for (Point &p : polygon) {
		p.x += point.x;
		p.y += point.y;
	}
}

/*
	Bresenham Line algorithm
	Reference https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B

*/
void drawLine(uint32_t(*pixels)[SCREEN_WIDTH], int x1, int y1, int x2, int y2) {

	bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep) {
		swap(x1, y1);
		swap(x2, y2);
	}
	//swap the initial x1 and final x2 if the inital point is beyond the final
	if (x1 > x2) {
		swap(x1, x2);
		swap(y1, y2);
	}

	float dx = x2 - x1;
	float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	int maxX = (int)x2;

	for (int x = (int)x1; x < maxX; x++) {

		if (steep) {
			pixels[x][y] = 0x000000FF;
		}
		else {
			pixels[y][x] = 0x000000FF;
		}

		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}
}

/*
	Method used to draw the polygons by using the drawLine Helper
*/
void drawPolygon(uint32_t(*pixels)[SCREEN_WIDTH], vector<Point> polygon) {
	for (int i = 0; i < polygon.size() - 1; i++) {
		drawLine(pixels, polygon[i].x, polygon[i].y, polygon[i + 1].x, polygon[i + 1].y);
	}

	drawLine(pixels, polygon[0].x, polygon[0].y, polygon[polygon.size() - 1].x, polygon[polygon.size() - 1].y);
}

/*
	**************************************************
	Clipping methods for SUTHERLAND HODGMAN CLIPPING
	ref: https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm-please-change-bmp-images-jpeg-png/
	*************************************************
*/

// Returns the x-value of point of interection of two lines
int x_intersect(Point p1, Point p2, Point p3, Point p4) {
	int num = (p1.x*p2.y - p1.y*p2.x) * (p3.x - p4.x) -
		(p1.x - p2.x) * (p3.x*p4.y - p3.y*p4.x);
	int den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	return num / den;
}

// Returns y-value of point of intersectipn of two lines 
int y_intersect(Point p1, Point p2, Point p3, Point p4) {
	int num = (p1.x*p2.y - p1.y*p2.x) * (p3.y - p4.y) -
		(p1.y - p2.y) * (p3.x*p4.y - p3.y*p4.x);
	int den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	return num / den;
}

// Will determine new points for the polygon
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


/*
	******************************************
		END OF SUTHERLAND HODGMAN METHODS
	******************************************
*/


/*
	**************************************
		LiangBarsky Methods	
		Reference: https://en.wikipedia.org/wiki/Liang%E2%80%93Barsky_algorithm
	**************************************
*/

// Returns max of an array
float maxi(float arr[], int n) {
	float m = 0;
	for (int i = 0; i < n; ++i) {
		if (m < arr[i]) {
			m = arr[i];
		}
	}

	return m;
}

// Returns min of an array
float mini(float arr[], int n) {
	float m = 1;
	for (int i = 0; i < n; ++i) {
		if (m > arr[i]) {
			m = arr[i];
		}
	}

	return m;
}

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

void liangBarskyHelper(vector<Point> poly, uint32_t(*pixels)[SCREEN_WIDTH]) {

	for (int i = 0; i < poly.size(); i++) {
		int k = (i + 1) % poly.size();

		liangBarsky(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, poly[i].x, poly[i].y, poly[k].x, poly[k].y, pixels);
	}
}

/*
	**********************************
		END OF LIANG BARSKY METHODS
	**********************************
*/

void clippingPolygonProcedure(uint32_t(*pixels)[SCREEN_WIDTH]) {

	vector<Point> polyPoints , copyPolyPoints;
	Point startPoint;
	int numOfPoints = 0, x, y;

	cout << "Specify the number of points: ";
	while (!(cin >> numOfPoints) || numOfPoints < 2) {
		cin.clear();
		cout << "Invalid input!\n";
		cout << "Specify the number of points: ";
	}

	cout << "Please specify the points in clockwise order: \n";
	for (int i = 0; i < numOfPoints; i++) {
		cout << "Point " << i + 1 << ": ";
		cin >> x >> y;
		polyPoints.push_back(Point(x, y));
	}
	
	for (int i = 0; i < 2; i++) {
		//COPY THE POLYGON POINTS 
		copyPolyPoints = polyPoints;

		cout << "Specify the start point of the polygon"<< i + 1<< ": ";
		while (!(cin >> startPoint.x >> startPoint.y)) {
			cin.clear();
			cout << "Invalid input!\n";
			cout << "Specify the start point of the polygon: ";
		}
		translatePolygon(copyPolyPoints, startPoint);

		//Determine clipping method
		int choice;
		cout << "Please select a clipping algorithm: \n";
		cout << "\t1. Sutherland-Hodgman\n"
			<< "\t2. Liang-Barsky\n"
			<< ">> ";
		while (!(cin >> choice) || choice < 1 || choice > 2) {
			cin.clear();
			cout << "Invalid input!\n";
			cout << "Please select a clipping algorithm: ";
		}

		if (choice == 1) {
			sutherlandHodgmanClip(copyPolyPoints, boundsClipping);
			drawPolygon(pixels, copyPolyPoints);
		}
		else {
			liangBarskyHelper(copyPolyPoints, pixels);
		}
	}

	return;
}


/*
	*****************************
	Recursive flood fill method
	Reference https://www.tutorialspoint.com/computer_graphics/polygon_filling_algorithm.htm
	*****************************
*/

void floodFill(uint32_t(*pixels)[SCREEN_WIDTH], int x, int y) {

	// Base Case
	if (x <= 0 || x >= SCREEN_WIDTH || y <= 0 || y >= SCREEN_HEIGHT)
		return;

	if (pixels[y][x] != 0x000000FF) {
		pixels[y][x] = 0x000000FF;

		//8 Neighbours
		floodFill(pixels,x-1, y);
		floodFill(pixels,x + 1, y);
		floodFill(pixels, x, y - 1);
		floodFill(pixels, x, y + 1);
		floodFill(pixels, x-1, y + 1);
		floodFill(pixels, x + 1, y + 1);
		floodFill(pixels, x + 1, y - 1);
		floodFill(pixels, x-1, y-1);

	}
}

/*
	***********************************
		Scanline filling methods
	************************************
*/
// Returns minimum Y value of polygon
int getMinY(vector<Point>const& poly) {
	int min = poly[0].y;
	for (Point p : poly) {
		if (p.y < min) {
			min = p.y;
		}
	}
	return min;
}

// Returns maximum Y value of polygon
int getMaxY(vector<Point>const& poly) {
	int max = 0;

	for (Point p : poly) {
		if (p.y > max) {
			max = p.y;
		}
	}

	return max;
}
struct Line {
	Point p1;
	Point p2;
	double m;
	double b;
	Line() {};
	Line(double m, double b, Point p1, Point p2) { this->m = m; this->b = b; this->p1=p1; this->p2=p2; }
};


/*
	IDEA: Starting from the polygon max y, find the x's
*/
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

/*
	**************************************
		FILLING ALGORITHM PROCEDURE
	**************************************
*/
void fillingPolygonProcedure(uint32_t(*pixels)[SCREEN_WIDTH], SDL_Texture *texture, SDL_Surface *canvas, SDL_Renderer *renderer, SDL_Event Event) {
	char blank;
	cout << "Demonstrating fill algorithms...\n";
	vector<Point> polyPoints;
	Point startPoint;
	int numOfPoints = 0, x, y;

	cout << "Specify the number of points: ";
	while (!(cin >> numOfPoints) || numOfPoints < 2) {
		cin.clear();
		cout << "Invalid input!\n";
		cout << "Specify the number of points: ";
	}

	cout << "Please specify the points in clockwise order: \n";
	for (int i = 0; i < numOfPoints; i++) {
		cout << "Point " << i + 1 << ": ";
		cin >> x >> y;
		polyPoints.push_back(Point(x, y));
	}

	
	while (SDL_PollEvent(&Event));
	sutherlandHodgmanClip(polyPoints, boundsClipping); //Using Sutherland-Hodgman clipping to update the polygon points
	drawPolygon(pixels, polyPoints);
	updateScene(texture, canvas, renderer);


	cout << "Demonstrating Recursive flood fill algorithm...\n";
	cout << "Give a point inside the polygon: ";
	Point inside;

	while (!(cin >> inside.x >> inside.y)) {
		cin.clear();
		cout << "Invalid input!\n";
		cout << "Give a point inside the polygon: ";
	}

	//floodFill(pixels, inside.x, inside.y);
	cout << "Flood fill completed!\n";
	while (SDL_PollEvent(&Event));
	updateScene(texture, canvas, renderer);
	cout << "Enter any character to continue...\n";
	cin >>blank;

	while (SDL_PollEvent(&Event));
	clearScene(pixels);
	updateScene(texture, canvas, renderer);

	while (SDL_PollEvent(&Event));
	drawPolygon(pixels, polyPoints);
	updateScene(texture, canvas, renderer);

	cout << "Demonstrating Scanline fill algorithm...\n";
	scanLine(pixels, getMinY(polyPoints), getMaxY(polyPoints), polyPoints);
	while (SDL_PollEvent(&Event));
	updateScene(texture, canvas, renderer);
}





int main(int argc, char* args[]) {

	//VERY IMPORTANT: Ensure SDL2 is initialized
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}


	//This creates the actual window in which graphics are displayed
	SDL_Window* window = SDL_CreateWindow(
		"Assignment 3 Steven Bodnar",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	//Check for errors: window will be NULL if there was a problem
	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0); //the "0" lets the SDL2 choose what's best.

	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create renderer: %s\n", SDL_GetError());
		return 1;
	}


	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
	if (canvas == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create surface: %s\n", SDL_GetError());
		return 1;
	}


	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create texture: %s\n", SDL_GetError());
		return 1;
	}



	SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0xFF, 0xFF, 0xFF));

	//Define the clipping bounds of our canvas
	boundsClipping.push_back(Point(0, 0));									//Top Right Corner
	boundsClipping.push_back(Point(0, SCREEN_HEIGHT - 1));					//Top Left Corner
	boundsClipping.push_back(Point(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));	//Bottom Right Corner
	boundsClipping.push_back(Point(SCREEN_WIDTH - 1, 0));					//Bottom Left Corner


	uint32_t(*pixels)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH]) canvas->pixels;

	/*
		************************************************
			ASSIGNMENT 3 LOGIC BEGINS HERE
		************************************************
	*/


	int choice;
	//Prompt the menu until the user decides to quit
	while (true) {
		SDL_Event Event;

		cout << "*********\nWelcome to Assignment 3:\n Please select one of the following:\n";
		printf(" 1. Clip Polygon\n 2. Fill Polygon\n 3. Exit Program\n>>");
		cin >> choice;

		/*
			This method will be used often,
			In Windows while waiting for user input the screen will stop working and a backlog of events
			will occur.
			This method is used to clear the backlog so changes can be displayed
		*/
		while (SDL_PollEvent(&Event));

		//Erase the values of the pixels for next procedure and show the blank canvas
		clearScene(pixels);
		updateScene(texture, canvas, renderer);

		//Clipping Polygon proc
		if (choice == 1) {

			while (SDL_PollEvent(&Event));
			clippingPolygonProcedure(pixels);
			while (SDL_PollEvent(&Event));
			//Update the canvas;
			updateScene(texture, canvas, renderer);

		}
		//Filling Procedure
		else if (choice == 2) {
			while (SDL_PollEvent(&Event));
			fillingPolygonProcedure(pixels, texture, canvas, renderer,Event);
			while (SDL_PollEvent(&Event));
			//Update the canvas;
			updateScene(texture, canvas, renderer);
		}
		//Exit the program
		else if (choice == 3) {
			break;
		}
		else {
			cout << "Sorry, your input is invalid.\n";
			choice = 0;
		}

		//Clear the backlogged events from input wait
		while (SDL_PollEvent(&Event));

	}

	cout << "Goodbye.\n\n";

	//Free the resources and quit.
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(canvas);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;

}
