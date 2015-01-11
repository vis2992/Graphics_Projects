#include<stdlib.h>
#include<math.h>
#include<Gl/glut.h>
#include<stdio.h>
#include<string.h>
#include<windows.h>
#define h 600
#define w 800
//Global Variables
int funcsel = 0, needed = 0, cur_x, cur_y;
double collected[100][2];
int c = 0, flag = 0, g1 = 0;
float thickness = 1.0;
float R = 0, G = 0, B = 0; //Boundary Colour Default Black
float RB = 1, GB = 1, BB = 1; //Background Colour Default White
float RF = 0, GF = 0, BF = 1; //Fill Colour Default Blue
float d = .1;
int last_angle = 0;
//Scaling Related
GLfloat factor = 1, last_factor = 1;
int fillflag = 0;
int  ht = 599, wd = 799;

//So rotated function doesnt display rotated figure after clrscrn
int clrflag = 0;
//For Undoing Translation.
double utx = 0, uty = 0;
//For Scan Filling
int n = 0;
int pencilon;
int undofunc = 0, undo_c = 0; //undofunc to store previous funcsel for use in undo function 
//and undo_c to store count for use in undo function
int translateflag = 0;
//Flags For Mixed Operations..
int ang_scale = 0;
float rotate_scale_factor = 1;
void myline()
{
	if (c == 2)
	{

		glBegin(GL_LINES);
		glVertex2d((collected[0][0]), (ht - collected[0][1]));
		glVertex2d((collected[1][0]), (ht - collected[1][1]));
		glEnd();
		glFlush();
		c = 0;
	}
}

void myfreepoly()
{
	if (c == 4)
	{
		n = 0;
		glBegin(GL_LINE_LOOP);
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 1][1]));
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 3][0]), (ht - collected[c - 3][1]));
		glVertex2d((collected[c - 4][0]), (ht - collected[c - 4][1]));
		glEnd();
		glFlush();
		c = 0;
	}
}

void myrhombus()
{

	if (c % 2 == 0)
	{
		float dist;
		dist = sqrt(pow((collected[c - 1][0] - collected[c - 2][0]), 2) + pow((collected[c - 1][1] - collected[c - 2][1]), 2));
		n = 0;
		glBegin(GL_LINE_LOOP);
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 1][1]));
		glVertex2d((collected[c - 1][0] + dist), (ht - collected[c - 1][1]));
		glVertex2d((collected[c - 2][0] + dist), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 2][1]));
		glEnd();
		glFlush();
	}
}

void mysqr()
{
	if (c % 2 == 0)
	{
		float dist;
		dist = collected[c - 1][0] - collected[c - 2][0];
		n = 0;
		glBegin(GL_LINE_LOOP);
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 2][1] - dist));
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 2][1] - dist));
		glEnd();
		glFlush();
	}
}

void clrscrn()
{
	c = 0;
	clrflag = 1;
	glClearColor(RB, GB, BB, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void myrect()
{
	if (c % 2 == 0)
	{
		n = 0;
		glBegin(GL_LINE_LOOP);
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 1][1]));
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 1][1]));
		glEnd();
		glFlush();
	}

}

void mycircle()
{
	if (c % 2 == 0)
	{
		float r, x1, y1;
		float i;
		r = sqrt(pow((collected[c - 1][0] - collected[c - 2][0]), 2) + pow((collected[c - 1][1] - collected[c - 2][1]), 2));
		for (i = 0; i<360; i += d)
		{
			glPointSize(thickness);
			glBegin(GL_POINTS);
			x1 = collected[c - 2][0] + r*cos(i);//x+rcostheta
			y1 = ht - collected[c - 2][1] + r*sin(i);//y+rsintheta
			glVertex2f(x1, y1);
			glEnd();
		}
		glFlush();
	}
}

void myfilledcircle()
{
	if (c == 2)
	{
		float r, x1, y1;
		float i;
		glColor3f(RF, GF, BF); //Setting Fill Color
		r = sqrt(pow((collected[1][0] - collected[0][0]), 2) + pow((collected[1][1] - collected[0][1]), 2));
		for (i = 0; i<360; i += .01)
		{

			glPointSize(thickness);
			glBegin(GL_LINES);
			x1 = collected[c - 2][0] + r*cos(i);
			y1 = ht - collected[c - 2][1] + r*sin(i);
			glVertex2f(collected[c - 2][0], ht - collected[c - 2][1]);
			glVertex2f(x1, y1);
			glEnd();
		}
		glFlush();
		glColor3f(R, G, B); //Restoring Color To Boundary Color
		mycircle(); //To Color Boundary
	}
	c = 0;

}


void mydashline()
{
	glLineStipple(2, 0xCCCC);
	glEnable(GL_LINE_STIPPLE);
}

void mythickline()
{
	glLineWidth(thickness);
	glPointSize(thickness);
}


void mydotdashline()
{
	glLineStipple(4, 0xD6B5);
	glEnable(GL_LINE_STIPPLE);
}
void myautojoinlines()
{
	if (c>1)
	{
		n = 0;
		glBegin(GL_LINE_LOOP);
		glVertex2d((collected[c - 2][0]), (ht - collected[c - 2][1]));
		glVertex2d((collected[c - 1][0]), (ht - collected[c - 1][1]));
		glEnd();
		glFlush();

	}
}

void myellipse()
{
	if (c % 3 == 0 && c != 1)
	{
		float r1, r2, x2, y2;
		float j;
		r1 = sqrt(pow((collected[c - 2][0] - collected[c - 3][0]), 2) + pow((collected[c - 2][1] - collected[c - 3][1]), 2));
		r2 = sqrt(pow((collected[c - 1][0] - collected[c - 3][0]), 2) + pow((collected[c - 1][1] - collected[c - 3][1]), 2));
		if (collected[c - 2][0]>collected[c - 1][0])
		{
			for (j = 0; j<360; j += d)
			{
				glBegin(GL_POINTS);
				x2 = collected[c - 3][0] + r1*cos(j);
				y2 = ht - collected[c - 3][1] + r2*sin(j);
				glVertex2f(x2, y2);
				glEnd();
				glFlush();
			}
		}
		else
		{
			for (j = 0; j<360; j += d)
			{
				glBegin(GL_POINTS);
				x2 = collected[c - 3][0] + r2*cos(j);
				y2 = ht - collected[c - 3][1] + r1*sin(j);
				glVertex2f(x2, y2);
				glEnd();
				glFlush();
			}
		}

	}
}

void myfilledellipse()
{

	if (c == 3)
	{
		float r1, r2, x2, y2;
		float j;
		r1 = sqrt(pow((collected[c - 2][0] - collected[c - 3][0]), 2) + pow((collected[c - 2][1] - collected[c - 3][1]), 2));//sqrt((x2-x1)^2+(y2-y1)^2)
		r2 = sqrt(pow((collected[c - 1][0] - collected[c - 3][0]), 2) + pow((collected[c - 1][1] - collected[c - 3][1]), 2));//sqrt((x3-x1)^2+(y3-y1)^2)
		glColor3f(RF, GF, BF);
		if (collected[c - 2][0]>collected[c - 1][0])
		{
			for (j = 0; j<360; j += .1)
			{
				glBegin(GL_LINES);
				x2 = collected[c - 3][0] + r1*cos(j);
				y2 = ht - collected[c - 3][1] + r2*sin(j);
				glVertex2f(collected[c - 3][0], ht - collected[c - 3][1]);
				glVertex2f(x2, y2);
				glEnd();
			}
		}
		else
		{
			for (j = 0; j<360; j += .1)
			{
				glBegin(GL_LINES);
				x2 = collected[c - 3][0] + r2*cos(j);
				y2 = ht - collected[c - 3][1] + r1*sin(j);
				glVertex2f(collected[c - 3][0], ht - collected[c - 3][1]);
				glVertex2f(x2, y2);
				glEnd();
			}
		}
		c = 0;
		glFlush();
		glColor3f(R, G, B);
		myellipse();
	}
}


void mypencil(int xp, int yp)
{
	if (pencilon)
	{
		glBegin(GL_POINTS);
		glVertex2i(xp, (ht - yp));
		glEnd();
		glFlush();
	}
}

void filter()
{
	switch (funcsel)
	{
	case 1:
		myline();
		break;
	case 2:
		myfreepoly();
		break;
	case 3:
		mydotdashline();
		break;
	case 4:
		myrhombus();
		break;
	case 5:
		mysqr();
		break;
	case 6:
		clrscrn();
		break;
	case 7:
		myrect();
		break;
	case 8:
		mydashline();
		break;
	case 9:
		mythickline();
		break;
	case 10:
		exit(0);
		break;
	case 11:
		myautojoinlines();
		break;
	case 12:
		mycircle();
		break;
	case 13:
		myfilledcircle();
		break;
	case 14:
		myellipse();
		break;
	case 15:
		myfilledellipse();
		break;
	}
}


void collect(int btn, int state, int x, int y)
{
	cur_x = x;
	cur_y = ht - y;
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (funcsel != 0)
		{
			collected[c][0] = x;
			collected[c][1] = y;
			c++;
		}
		if (c >= needed)
		{
			clrflag = 0;
			fillflag = 0;
			utx = 0, uty = 0;
			filter();
		}
	}
	if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		c = 0;
	}
}

void character(unsigned char key, int x, int y)
{
	glRasterPos2i(cur_x += 10, cur_y);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, key);
	glFlush();
	if (cur_x >= wd){
		cur_y -= 20;
		cur_x = 0;
	}
	if (key == 13)
	{
		cur_y -= 20;
		cur_x = 0;
	}
}

void main_menu(int id)
{
	undofunc = funcsel;
	funcsel = 0;
	undo_c = c;
	c = 0;
	translateflag = 0;

	//to switch off pencil drawing function	
	if (pencilon)
	{
		pencilon = 0;
		glColor3f(R, G, B);
	}
	switch (id)
	{
	case 1:
		fillflag = 0;
		funcsel = 1;
		needed = 2;
		break;
	case 2:
		fillflag = 0;
		funcsel = 2;
		needed = 4;
		break;
	case 3:
		pencilon = 1;
		glutMotionFunc(mypencil);
		break;
	case 4:
		fillflag = 0;
		funcsel = 4;
		needed = 2;
		break;
	case 5:
		fillflag = 0;
		funcsel = 5;
		needed = 2;
		break;
	case 6:
		clrscrn();
		break;
	case 7:
		fillflag = 0;
		funcsel = 7;
		needed = 2;
		break;
	case 8:
		pencilon = 1;
		glPointSize(10);
		glColor3f(RB, GB, BB);
		glutMotionFunc(mypencil);
		break;
	case 10:
		exit(1);
		break;
	case 11:
		funcsel = 11;
		needed = 2;
		break;
	case 12: glutKeyboardFunc(character);
		break;
	}

}
void circle_func(int id)
{
	funcsel = undofunc;
	c = undo_c;
	c = 0; //Becoz on click next point ellipse act as if third point is taken
	translateflag = 0;
	utx = 0, uty = 0;
	switch (id)
	{
	case 1:
		fillflag = 0;
		needed = 2;
		funcsel = 12;
		break;
	case 2:
		needed = 2;
		funcsel = 13;
		break;
	}
}

void ellipse_func(int id)
{
	funcsel = undofunc;
	c = undo_c;
	c = 0; //Becoz on click next point ellipse acts as if third point is taken
	translateflag = 0;
	utx = 0, uty = 0;
	switch (id)
	{
	case 1:
		fillflag = 0;
		needed = 3;
		funcsel = 14;
		break;
	case 2:
		needed = 3;
		funcsel = 15;
		break;
	}

}

void thickness_menu(int thick_id)
{
	switch (thick_id)
	{
	case 1:
		thickness = 1.0;
		break;
	case 2:
		thickness = 2.0;
		break;
	case 3:
		thickness = 3.0;
		break;
	case 4:
		thickness = 4.0;
		break;
	case 5:
		thickness = 5.0;
		break;
	case 6:
		thickness = 6.0;
		break;
	case 7:
		thickness = 7.0;
		break;
	case 8:
		thickness = 8.0;
		break;
	}
	mythickline();
}

void drawbackground()
{

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(RB, GB, BB);

	glBegin(GL_POLYGON);
	{
		glVertex2f(0, h);
		glVertex2f(0, 0);
		glVertex2f(w, 0);
		glVertex2f(w, h);
	}
	glEnd();
	glFlush();
}

void backgroundcolor_menu(int id)
{
	switch (id)
	{
	case 1:
		RB = 1, GB = 0, BB = 0;
		break;
	case 2:
		RB = 0, GB = 1, BB = 0;
		break;
	case 3:
		RB = 0, GB = 0, BB = 1;
		break;
	case 4:
		RB = 1, GB = 1, BB = 0;
		break;
	case 5:
		RB = 0.5, GB = 0.5, BB = 0.5;
		break;
	case 6:
		RB = 1, GB = 0, BB = 1;
		break;
	case 7:
		RB = 0, GB = 0, BB = 0;
		break;
	case 8:
		RB = 1, GB = 1, BB = 1;
		break;
	}
	drawbackground();
}

void colorobject(int id)
{
	switch (id)
	{
	case 1:
		R = 1, G = 0, B = 0;
		break;
	case 2:
		R = 0, B = 0, G = 1;
		break;
	case 3:
		R = 0, B = 1, G = 0;
		break;
	case 4:
		R = 1, G = 1, B = 0;
		break;
	case 5:
		R = 0, G = 1, B = 1;
		break;
	case 6:
		R = 1, G = 0, B = 1;
		break;
	case 7:
		R = 0, G = 0, B = 0;
		break;
	case 8:
		R = 1, G = 1, B = 1;
		break;
	}
	glColor3f(R, G, B);
}

void fillcolorobject(int id)
{
	switch (id)
	{
	case 1:
		RF = 1, GF = 0, BF = 0;
		break;
	case 2:
		RF = 0, BF = 0, GF = 1;
		break;
	case 3:
		RF = 0, BF = 1, GF = 0;
		break;
	case 4:
		RF = 1, GF = 1, BF = 0;
		break;
	case 5:
		RF = 0, GF = 1, BF = 1;
		break;
	case 6:
		RF = 1, GF = 0, BF = 1;
		break;
	case 7:
		RF = 0, GF = 0, BF = 0;
		break;
	case 8:
		RF = 1, GF = 1, BF = 1;
		break;
	}
	main_menu(13);
}
void mylinestyle_menu(int style_id)
{
	switch (style_id)
	{
	case 1:mydashline();
		break;
	case 2:mydotdashline();
		break;
	case 3:glDisable(GL_LINE_STIPPLE);
		break;
	}
}

void createmenu()
{
	//*************************Line Thickness*****************************//
	int thick_menu = glutCreateMenu(thickness_menu);
	glutAddMenuEntry("Normal Thickness", 1);
	glutAddMenuEntry("2.0", 2);
	glutAddMenuEntry("3.0", 3);
	glutAddMenuEntry("4.0", 4);
	glutAddMenuEntry("5.0", 5);
	glutAddMenuEntry("6.0", 6);
	glutAddMenuEntry("7.0", 7);
	glutAddMenuEntry("8.0", 8);

	//**************************Background Color***************************//
	int bc_menu = glutCreateMenu(backgroundcolor_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Yellow", 4);
	glutAddMenuEntry("Grey", 5);
	glutAddMenuEntry("Pink", 6);
	glutAddMenuEntry("Black", 7);
	glutAddMenuEntry("White", 8);
	//***************************Object Color*****************************//
	int objcolormenu = glutCreateMenu(colorobject);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Yellow", 4);
	glutAddMenuEntry("Grey", 5);
	glutAddMenuEntry("Pink", 6);
	glutAddMenuEntry("Black", 7);
	glutAddMenuEntry("White", 8);
	//****************************Fill Color*******************************//
	int fillcolormenu = glutCreateMenu(fillcolorobject);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Yellow", 4);
	glutAddMenuEntry("Grey", 5);
	glutAddMenuEntry("Pink", 6);
	glutAddMenuEntry("Black", 7);
	glutAddMenuEntry("White", 8);
	//*****************************Circle Sub Menu*************************//
	int circle_sel = glutCreateMenu(circle_func);
	glutAddMenuEntry("Simple", 1);
	glutAddMenuEntry("Filled", 2);
	//*****************************Ellipse Sub Menu************************//
	int ellipse_sel = glutCreateMenu(ellipse_func);
	glutAddMenuEntry("Simple", 1);
	glutAddMenuEntry("Filled", 2);
	//****************************Lines************************************//
	int line_style_menu = glutCreateMenu(mylinestyle_menu);
	glutAddMenuEntry("Dashed Line", 1);
	glutAddMenuEntry("Dot-Dash Line", 2);
	glutAddMenuEntry("Normal Line", 3);
	//***************************Main**********************************//
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutCreateMenu(main_menu);//call back fun()
	//glutAddSubMenu("Fill Colors",fillcolormenu);
	glutAddMenuEntry("Line", 1);
	glutAddMenuEntry("Square", 5);
	glutAddMenuEntry("Rectangle", 7);
	glutAddMenuEntry("Rhombus", 4);
	glutAddMenuEntry("General Polygon-4", 2);
	glutAddMenuEntry("Free Polygon", 11);
	glutAddMenuEntry("Pencil", 3);
	glutAddMenuEntry("Eraser", 8);
	glutAddMenuEntry("text", 12);
	glutAddSubMenu("Circle Seliect", circle_sel);
	glutAddSubMenu("Ellipse Select", ellipse_sel);
	glutAddSubMenu("Thickness", thick_menu);
	glutAddSubMenu("Object Colors", objcolormenu);
	glutAddSubMenu("Line Type", line_style_menu);
	glutAddSubMenu("Background Colors", bc_menu);
	glutAddMenuEntry("Clear Screen", 6);
	glutAddMenuEntry("Exit", 10);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int page()
{
	int O = 0, X = 268;
	float r = 0, g = 0, b = 0;
	char *a = { "Welcome to MS Paint-Please wait till the loading has finished" };
	char *c = { "Name-College" };
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertex2i(269, 238);
	glVertex2i(269, 264);
	glVertex2i(527, 264);
	glVertex2i(527, 238);
	glEnd();
	glFlush();
	glLineWidth(1);
	for (int i = 0; i<20; i++)
	{
		glColor3f(0, 0, 0);
		glRectf(290, 380, 510, 595);
		glTranslatef(400, 490, 0);
		glRotatef(O += 5, 1, 1, 1);
		r = rand() % 2; g = rand() % 2, b = rand() % 2;
		if (r == 0.0&&g == 0.0&&b == 0.0)
		{
			r = 0.896; g = 0.456; b = 0.458;
		}
		glColor3f(r, g, b);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(X += 13, 250, 0); //Point translate
		glColor3f(0, 0, 1);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex2i(0, 0);
		glEnd();
		glPointSize(1);
		glColor3f(0, 0, 0);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(8);
		glBegin(GL_POINTS);
		glVertex2i(0, 0);
		glEnd();
		glPointSize(1);
		glColor3f(0, 1, 1);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(4);
		glBegin(GL_POINTS);
		glVertex2i(0, 0);
		glEnd();
		glPointSize(1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Sleep(200);
		glFlush();
		glColor3f(1, 1, 1);
		glRasterPos2f(300, 320);
		void *f = GLUT_BITMAP_HELVETICA_18;
		if (g1 == 0)
		{
			for (int j = 0; j<(signed)strlen(a); j++)
			{
				if (a[j] == '-')
				{
					glRasterPos2f(240, 290);
					continue;
				}
				glutBitmapCharacter(f, a[j]);
				Sleep(10);
				g1 = 1;
			}
			glRasterPos2f(600, 100);
			for (int j = 0; j<(signed)strlen(c); j++)
			{
				if (c[j] == '-')
				{
					glRasterPos2f(600, 70);
					continue;
				}
				if (c[j] == '_')
				{
					glRasterPos2f(600, 40);
					continue;
				}
				glutBitmapCharacter(f, c[j]);
				Sleep(10);
				g1 = 1;
			}
		}
	}
	g1 = 0;
	return 0;
}

void display()
{
	page();
	clrscrn();
	glFlush();
}
void myreshape(int wi, int hi)
{
	wd = wi; ht = hi;

	glViewport(0, 0, wi, hi);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, wi, 0, hi);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}
void myinit()
{
	glClearColor(1, 1, 1, 1);
	glColor3f(RB, GB, BB);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(w, h);
	glutCreateWindow("PAINT");
	createmenu();
	glutMouseFunc(collect);
	myinit();
	glutDisplayFunc(display);
	glutReshapeFunc(myreshape);
	glutMainLoop();
	return 0;
}