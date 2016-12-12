#include "TDRend.h"

enum {UP = 1, DOWN, };

int flag = 1;
int mState = UP;
int h = 0, w = 0;
int oldX = -13, oldY = -13;

float angle = 0, scle = 1;
float rot[3]={0.0,0.0,0.0};
float transx = 0, transy = 0, transz = 0;
float centx = 0.0, centy = 0.0, centz = 0.0;

std::vector<Point*> points; // List of input points
std::vector<Point*> bezier_c; // List of points which defines  bezier curve
std::vector<Point*> lagrange_c; // List of points which defines a curve obtained using Lagrange Interpolation

/*
 * Point class - represents each point
 */
// Point constructor
Point::Point(){
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
// Point constructor
Point::Point(float x_coor, float y_coor, float z_coor){
	x = x_coor;
	y = y_coor;
	z = z_coor;
}

float Point::getX(){
	return x;
}

void Point::setX(float x_coor){
	x = x_coor;
}

float Point::getY(){
	return y;
}
void Point::setY(float y_coor){
	y = y_coor;
}
float Point::getZ(){
	return z;
}
void Point::setZ(float z_coor){
	z = z_coor;
}

/*
 * bezier_curve - compute bezier curve for a set of input points
 */
void bezier_curve(){
	bezier_c.clear();
	int i=0, n = points.size();
	float t = 0, b = 0, px=0, py=0;
	int fact[n];
	
	fact[0] = 1;
	fact[n-1] = 1;
	
	// Computing nCr for 0 <= r <= n
	for (i=1; i <(n+1)/2; i++){
		fact[i] = fact[i-1]*(n-i)/i;
		fact[n-1-i] = fact[i];
	}
	
	// Computing points on the curve governed by the parameter t
	for (t=0; t<=1; t += 1.0/(10*n)){
		px = 0; py = 0;
		for (i=0; i<n;i++){
			b = fact[i]*pow(t, i)*pow(1-t, n-1-i);
			px += b*points[i]->getX();
			py += b*points[i]->getY();
		}
		// Adding intermediate points which are part of the curve to the list of points
		bezier_c.push_back(new Point(px, py, 0));
	}
	bezier_c.push_back(new Point(points[n-1]->getX(), points[n-1]->getY(), 0));
}

/*
 * compute_LagPol - compute Lagrangian polynomial 
 */
float compute_LagPol(float x){
	int i, j, n = points.size();
	float y = 0.0, tmp = 0.0;
	for (i = 0; i < n; i++){
		tmp = points[i]->getY();
		for (j = 0; j < n; j++){
			if (j == i)
				continue;
			tmp = tmp*((x - points[j]->getX())/(points[i]->getX() - points[j]->getX()));
		}
		y += tmp;
	}
	return y;
}

/*
 * lagrange_curve - compute Lagrange curve for a set of input points
 */
void lagrange_curve(){
	lagrange_c.clear();
	int i, j, n = points.size();
	float x, px, py;
	float min = minx(), max = maxx();
	for (x = min; x <= max; x += (max-min)/(10.0*points.size())){
			px = x;
		py = compute_LagPol(px);
		lagrange_c.push_back(new Point(px, py, 0));
	}
	lagrange_c.push_back(new Point(max, compute_LagPol(max), 0));
}

/*
 * minx - find minimum x component of a set of points
 */
float minx(){
	float min = 1000;
	for (int i=0; i < points.size(); i++)
		if (min > points[i]->getX())
			min = points[i]->getX();
	
	return min;
}

/*
 * manx - find maximum x component of a set of points
 */
float maxx(){
	float max = -100;
	for (int i=0; i < points.size(); i++)
		if (max < points[i]->getX())
			max = points[i]->getX();
	
	return max;
}

/*
 * initRendering: Setting up glut window and view
 * 
 */
void initRendering() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	::w = glutGet( GLUT_WINDOW_WIDTH );
	::h = glutGet( GLUT_WINDOW_HEIGHT );
	glViewport(0, 0, w, h);
	glOrtho(-100,100, -100, 100, 1, -1);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
}

/*
 * handleResize: Setting up glut window and view after window resize
 * 
 */
void handleResize(int wt, int ht){
	if (ht  == 0)
		ht = 1;
	
	::w = wt;
	::h = ht;
	glViewport(0,0, wt, ht);
	glOrtho(-100,100, -100, 100, 1, -1);
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
}

/*
 * keyboard: add keys for interaction
 */
void keyboard(unsigned char key, int x, int y){
	if (key == char(27) || key == 'q'){
		std::cout<<"Quiting ..."<<std::endl;
		exit(-1);
	}else if (key == 'l'){
		angle += 0.5;
		calculateCentroid();
	}else if (key == 'r'){
		angle -= 0.5;
		calculateCentroid();
	}else if (key == '+'){
		scle += 0.2;
	}else if (key == '-'){
		scle -= 0.2;
	} 
	glutPostRedisplay();
}

/*
 * glutMotion: add mouse drag for interaction
 */
void glutMotion(int x, int y)
{
	if (mState == DOWN)
	{
		glutPostRedisplay ();
	}
	oldX = x;
	oldY = y;
}

/*
 * glutMouse: add mouse click for interaction
 */
void glutMouse(int button, int state, int x, int y)
{
	float dist = 0.0;
	float px = 0, py = 0;
	GLfloat pz = 0;
    GLdouble posX, posY, posZ;
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    if(state == GLUT_DOWN)
    {
        switch(button)
        {
            case GLUT_LEFT_BUTTON:
				mState = DOWN;
                oldX = x;
                oldY = y;
                break;
            case GLUT_RIGHT_BUTTON:
				mState = DOWN;
                oldX = x;
                oldY = y;
            break;
        }
    } else if (state == GLUT_UP){
        mState = UP;
        switch(button)
        {
            case GLUT_LEFT_BUTTON:
				flag = 0;
				glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
				glGetDoublev( GL_PROJECTION_MATRIX, projection );
				glGetIntegerv( GL_VIEWPORT, viewport );
 
				px = oldX; py = (float)viewport[3] - (float)oldY;
				gluUnProject( px, py, pz, modelview, projection, viewport, &posX, &posY, &posZ);
				px = posX; py = posY; pz = posZ;
				for (int i = 0; i < points.size(); i++){
					dist = (points[i]->getX() - px)*(points[i]->getX() - px);
					dist += (points[i]->getY() - py)*(points[i]->getY() - py);
					if (dist <= 4){
						points.erase(points.begin() + i);
						flag = 1;break;
					}
				}
				if (!flag)
					points.push_back(new Point(px, py, 0));
				flag = 1;
				if (points.size() > 1){
					lagrange_curve();
					bezier_curve();
				}
				oldX = x;
                oldY = y;
                break;
            case GLUT_RIGHT_BUTTON:
                transx += (float)(x - oldX)/400;
                transy -= (float)(y - oldY)/400;
				break;
        }        
        glutPostRedisplay();
	}
}

/*
 * SpecialKeys: add special keys for interaction
 */
void SpecialKeys(int key, int x, int y){
	if (key == GLUT_KEY_LEFT){
		transx+=0.1;
	}
	else if (key == GLUT_KEY_UP){
		transy-=0.1;
	}
	else if (key == GLUT_KEY_RIGHT){
		transx-=0.1;
	}
	else if (key == GLUT_KEY_DOWN){
		transy+=0.1;
	}
	glutPostRedisplay();
}

/*
 * legend: display legend
 */
void legend(){
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(0.5, 0, 1);
	glVertex3f(70, 70, 0);	
	glColor3f(1, 0.5, 0);
	glVertex3f(70, 65, 0);
	glEnd();
	
	char bez[] = {'B','e','z','i','e','r'};
	glColor3f(0.5, 0, 1);
	glRasterPos3f( 74, 69, 0); // location to start printing text
	for( int i=0; i < 6; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, bez[i] ); // Print a character on the screen
	
	char lag[] = {'L','a','g','r','a','n', 'g', 'e'};
	glColor3f(1, 0.5, 0);
	glRasterPos3f( 74, 64, 0); // location to start printing text
	for( int i=0; i < 8; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, lag[i] ); // Print a character on the screen
}

/*
 * calculateCentroid: Compute centroid for a set of input points 
 */
void calculateCentroid(){
	centx = 0.0; centy = 0.0; centz = 0.0;
	for (int i=0;i<points.size();i++){
		centx += points[i]->getX();
		centy += points[i]->getY();
	}
	if (points.size() > 0){
		centx /= points.size(); centy /= points.size();
		centx /= 100.0; centy /= 100.0;
	}
}

/*
 * drawAxis: draw x and y axis
 */
void drawAxis(){
	glBegin(GL_LINES);
	
	//drawing x -axis
	glColor3f(0,0,1);
	glVertex3f(60,0,0);
	glVertex3f(-60,0,0);
	
	//drawing x -axis
	glColor3f(0,1,0);
	glVertex3f(0,60,0);
	glVertex3f(0,-60,0);
	
	glEnd();
}

/*
 * drawLagrangeCurve: draw lagrangian curve
 */
void drawLagrangeCurve(){
	glBegin(GL_LINES);
	glColor3f(1, 0.5, 0);
	for(int i=0; i + 1 < lagrange_c.size(); i++){
		glVertex3f(lagrange_c[i]->getX(), lagrange_c[i]->getY(), 0);
		glVertex3f(lagrange_c[i+1]->getX(), lagrange_c[i+1]->getY(), 0);
	}
	glEnd();
}

/*
 * drawBezierCurve: draw bezier curve
 */
void drawBezierCurve(){
	glBegin(GL_LINES);
	glColor3f(0.5, 0, 1);
	for(int i=0; i + 1 < bezier_c.size(); i++){
		glVertex3f(bezier_c[i]->getX(),bezier_c[i]->getY(), 0);
		glVertex3f(bezier_c[i+1]->getX(),bezier_c[i+1]->getY(), 0);
	}
	glEnd();
}

/*
 * drawPoints: draw input points
 */
void drawPoints(){
	glPointSize(4);
	
	glBegin(GL_POINTS);
	glColor3f(0.2,0.6,0.7);
	
	for (int i=0;i < points.size();i++){
		glVertex3f(points[i]->getX(),points[i]->getY(), 0);
	}
	glEnd();	
}

/*
 * draw: draw the complete scene
 */
void draw(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode( GL_PROJECTION );
	
	int i=0;
	glLoadIdentity();
	drawAxis();
	legend();

	glPushMatrix();	
	glScalef(scle, scle, scle);
	glTranslatef(transx, transy, transz);
	glTranslatef(centx, centy, centz);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(-centx, -centy, -centz);
	
	if (points.size() > 1){
		// Draw Bezier curve
		drawBezierCurve();
		// Draw curve obtained using Lagrange Interpolation
		drawLagrangeCurve();
	}
	drawPoints();
	
	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char **argv){
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(400, 400); //Set the window size
	glutInitWindowPosition(50,50);
	//Create the window
	glutCreateWindow("Bezier and Legrange Curves");
	initRendering(); //Initialize rendering
	//Set handler function for drawing
	glutDisplayFunc(draw);
	//Set handler function for window resize
	glutReshapeFunc(handleResize);
	//Set handler function for keypress
	glutKeyboardFunc(keyboard);
	//Set handler function for Special keypress
	glutSpecialFunc(SpecialKeys);
	//glutMotionFunc(glutMotion);
	glutMouseFunc(glutMouse);
	glutMainLoop(); //Start the main loop.
	return 0; 
}
