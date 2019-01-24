#include <cstdio> //when you want to include a C standard library header, like stdio.h, use cstdio instead
#include <iostream>
#include <cmath>

using namespace std;

//SDL2 includes
#include <SDL.h>
#include <SDL_ttf.h>

//Define the window size
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

//Define the PI value to find the radians
#define PI 3.14159265358979323846

/**
	Returns the hex (32bit) number the user has inputted to 
	define as the colour.

	@return The colour value.
*/
uint32_t pickColor() {
	uint32_t colour;
	cout << "Please enter the Hex RGB values (0x00000000 - 0xFFFFFFFF): ";
	while (!(cin >> std::hex >> colour) || colour < 0x00000000 || colour > 0xFFFFFFFF) {
		cin.clear();
		cin.ignore(10, '\n');
		cout << "Invalid input. Try Again.\n";
		cout << "Please enter the Hex RGB values: ";
	}
	return colour;
}

/**
	Update the pixels array to hold the
	user defined points to be displayed on the screen

	@param pixels The pixel values of the screen.
	@return void
*/
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

/**
	Compute the equation of the line given points
	Set pixels and returns an array showing the x,y 
	coordinates of the specified line
	
	@param pixels	array reference of screen pixel values
	@param xi		initial x coordinate.
	@param yi		initial y coordinate.
	@param xf		final x coordinate.
	@param yf		final y coordinate.
	@param colour	colour of the line
	@return Array containing the x,y coordinates of the line.
*/
size_t* drawLine(uint32_t(*pixels)[SCREEN_WIDTH], int xi, int yi, int xf, int yf, uint32_t colour) {
	

	//If your xf < xi swap the values
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
	//Initialize the line array to hold coordinates
	//Set all indexes to null
	size_t *linePos = new size_t[SCREEN_WIDTH];
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		linePos[i] = NULL;
	}

	//CASE: VERTICAL LINE
	if (xi == xf) {
		m = 0;
		c = yf;
		for (size_t yPos = yi; yPos <= yf; yPos++) {
			linePos[xi] = yPos;
			pixels[yPos][xi] = colour;
		}

	}
	//Other Cases
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

/**
	Given the pixels and an array of the object x,y coordinates
	translate the coordinates and return the updated array

	@param pixels reference of pixel information on the screen
	@param object an array containing the x and y coordinates of the line
	@param transX the specified xShift
	@param transY the specified yShift

	@return Array containing the new x,y coordinates of the line.
*/
size_t* translateObject(uint32_t(*pixels)[SCREEN_WIDTH], size_t object[SCREEN_WIDTH], int transX, int transY){
	cout << "Translating object\n";
	//Initialize an array to hold the new x,y coordinates of the line
	size_t *linePos = new size_t[SCREEN_WIDTH];

	for (size_t xPos = 0; xPos < SCREEN_WIDTH; xPos++) {
		if (object[xPos] != NULL) {
			if (object[xPos] - transY < SCREEN_HEIGHT && xPos + transX < SCREEN_WIDTH
				&& object[xPos] - transY >= 0 && xPos + transX >= 0) {
				pixels[object[xPos] - transY][xPos + transX] = 0x0000FFFF;
				linePos[xPos+transX] = object[xPos] + transY;
			}
			//If the point does not lie within the bounds do not add it to the array
			else {
				linePos[xPos] = NULL;
			}
		}
	}

	return linePos;
}

/**
	Given the line object rotate it some degree and update the pixels

	@param pixels reference of pixel information on the screen
	@param object an array containing the x and y coordinates of the line
	@param degree the degree of rotation
	@return void
*/
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
/**
	White out the pixel array to erase
	
	@pixels reference to the pixel information of the screen
	@return void
*/
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

/*
	Given the y coordinate check to see if it is within the bounds
*/
int checkBoundsY(int y) {
	if (y < 0)
		return 0;
	else if (y > SCREEN_HEIGHT)
		return SCREEN_HEIGHT - 1;
	else
		return y;
}


/*
	Given the x coordinate check to see if it is within the bounds
*/
int checkBoundsX(int x) {
	if (x < 0)
		return 0;
	else if (x > SCREEN_WIDTH)
		return SCREEN_WIDTH - 1;
	else
		return x;
}

/*
	Given the pixel information the desired center x,y coordinate, specified colour and scale
	draw an ellipse
*/
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


int main(int argc, char* args[]) {

	//Ensure SDL2 is initialized
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	//Create the window in which graphics are displayed
	SDL_Window* window = SDL_CreateWindow(
		"Steven Bodnar - Assignment 1",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	//Check for errors
	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create window: %s\n", SDL_GetError());
		return 1;
	}

	//Create a SDL_renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	//Check for errors in initializing the renderer
	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	//Instantiate the canvas
	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
	if (canvas == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create surface: %s\n", SDL_GetError());
		return 1;
	}

	//Instantiate the texture to draw on
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create texture: %s\n", SDL_GetError());
		return 1;
	}

	
	SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0xFF, 0xFF, 0xFF));

	//Pixels will hold the data of the pixel values on the canvas
	uint32_t(*pixels)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH]) canvas->pixels;

	/*
	************************************************
		ASSIGNMENT 1 LOGIC BEGINS HERE
	************************************************
	*/

	int choice;
	//Prompt the menu until the user decides to quit
	while (true) {
		SDL_Event Event;

		printf("*********\nWelcome to Assignment 1:\n Please select one of the following:\n");
		printf(" 1. Draw Points\n 2. Draw Line\n 3. Draw Circle\n 4. Exit Program\n>>");
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

		//Draw Points
		if (choice == 1) {
			//Call method to record user input and update the pixel values
			drawPoints(pixels);

			//Update screen with changes
			updateScene(texture, canvas, renderer);
		}
		//Draw Line
		else if (choice == 2) {
			int xi=0, yi=0;

			//Receive the inital endpoints 
			cout << "Please specify endpoint (xi, yi): ";
			while (!(cin >> std::dec >> xi >> std::dec >> yi)
				|| xi > SCREEN_WIDTH
				|| xi < 0
				|| yi > SCREEN_HEIGHT
				|| yi < 0) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: Please make sure the x <= " << SCREEN_WIDTH << " and y <= " << SCREEN_HEIGHT << "\n";
				cout << "Please specify endpoint (xi, yi): ";
			}

			int xf=0, yf=0;
			//Receive the final endpoints
			cout << "Please specify endpoint (xf, yf): ";
			while (!(cin >> std::dec >> xf>> std::dec >> yf)
				|| xf > SCREEN_WIDTH
				|| xf < 0
				|| yf > SCREEN_HEIGHT
				|| yf < 0
				|| xf == xi && yf == yi) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: x = " << xf << " and y = " << yf << " is invalid.\n";
				cout << "Please specify endpoint (xf, yf): ";
			}

			//Pick a colour for the line
			uint32_t colour = pickColor();

			cout << "Drawing line from (" << xi << "," << yi << ") to (" << xf << "," << yf << ")..\n";

			//Call the drawLine method and store the line values for later manipulation
			size_t *line = drawLine(pixels,xi, yi, xf, yf, colour);
			
			//Update the scene
			while (SDL_PollEvent(&Event));
			updateScene(texture, canvas, renderer);

			
			cout << "The line has been rendered.\n";
			cout << "Specify the x and y translation : ";
			int transX, transY;
			//Input translation values
			while (!(cin >> std::dec >> transX >> std::dec >> transY)
				|| transX > SCREEN_WIDTH
				|| transY > SCREEN_HEIGHT)
			{
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: Please make sure the x <= " << SCREEN_WIDTH << " and y <= " << SCREEN_HEIGHT << "\n";
				cout << "Please specify x and y translation: ";
			}
			cout << "Translating line x=" << transX << ", y=" << transY <<" ..\n";

			//Call the translate object method and store the newline value
			size_t *newline = translateObject(pixels, line, transX, transY);
			cout << "The translation has been applied.\n";
		
			//Update the scene
			while (SDL_PollEvent(&Event));
			updateScene(texture, canvas, renderer);

			cout << "Specify the degree of rotation (clockwise): ";
			int theta;
			//Input the degree of rotation
			while (!(cin >> std::dec >> theta))
			{
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: Invalid character\n";
				cout << "Please specify the degree of rotation: ";
			}

			cout << "Rotating the line " << theta << " degrees ..\n";
			//Rotate the line by passing the newLine values for the method to calculate 
			rotatePoint(pixels, theta, newline);
	
			//Draw a center dot for reference
			pixels[(SCREEN_HEIGHT / 2) - 1][(SCREEN_WIDTH / 2) - 1] = 0x000000FF;
			
			//Update the scene
			while (SDL_PollEvent(&Event));
			updateScene(texture, canvas, renderer);
		}
		else if (choice == 3) {
			int centerX = 0, centerY = 0;
			cout << "Please specify center of circle (x, y): ";
			//Input the center points of the circle
			while (!(cin >> std::dec >> centerX >> std::dec >> centerY)
				|| centerX > SCREEN_WIDTH
				|| centerX < 0
				|| centerY > SCREEN_HEIGHT
				|| centerY < 0) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: x = " << centerX << " and y = " << centerY << " is invalid.\n";
				cout << "Please specify center of circle (x, y): ";
			}

			int radius;
			//Get the radius of the circle
			cout << "Please specify radius of circle (r): ";
			while (!(cin >> std::dec >> radius)
				|| radius < 0) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: radius = " << radius <<" is invalid.\n";
				cout << "Please specify radius of circle (r): ";
			}

			//Obtain the colour of the circle from the user
			uint32_t colour = pickColor();

			//Call the draw circle with a scaling value of 1, 1
			drawCircle(pixels, centerX, centerY, radius, colour, 1,1);
			while (SDL_PollEvent(&Event));
			updateScene(texture, canvas, renderer);

			//Perform translaton operation
			cout << "The circle has been rendered.\n";
			cout << "Specify the x and y translation : ";
			int transX, transY;
			while (!(cin >> std::dec >> transX >> std::dec >> transY)
				|| transX > SCREEN_WIDTH
				|| transY > SCREEN_HEIGHT)
			{
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: Please make sure the x <= " << SCREEN_WIDTH << " and y <= " << SCREEN_HEIGHT << "\n";
				cout << "Please specify x and y translation: ";
			}
			cout << "Translating circle x=" << transX << ", y=" << transY << " ..\n";

			//Draw the circle with translated centerX and centerY values
			drawCircle(pixels, centerX + transX, centerY + transY, radius, 0x0000FFFF, 1, 1);

			//Update the scene
			while (SDL_PollEvent(&Event));
			updateScene(texture, canvas, renderer);
			cout << "The translation has been applied.\n";


			//Scaling operation
			cout << "Specify the x and y scaling factors : ";
			int scaleX, scaleY;
			while (!(cin >> std::dec >> scaleX >> std::dec >> scaleY)
				|| scaleX > SCREEN_WIDTH
				|| scaleY > SCREEN_HEIGHT)
			{
				cin.clear();
				cin.ignore(100, '\n');
				cout << "ERROR: Please make sure the x <= " << SCREEN_WIDTH << " and y <= " << SCREEN_HEIGHT << "\n";
				cout << "Please specify x and y scaling: ";
			}
			cout << "Scaling the circle by x=" << scaleX << ", y=" << scaleY << " ..\n";

			//draw the latest circle with scaled values
			drawCircle(pixels, centerX + transX, centerY + transY, radius, 0xFF0000FF, scaleX, scaleY);
			
			//Update scene
			while (SDL_PollEvent(&Event));
			updateScene(texture, canvas, renderer);
		}
		//Exit the program
		else if (choice == 4) {
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

