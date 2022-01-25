/*
 * project_final
 *
 * Author: Akarsh Chandrashekar
 */

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>



#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define BROWN 0x993300
#define DARKGREEN 0x1A9900
#define GOLD 0xFFD700
#define SILVER 0xD3D3D3
#define ORANGE 0xD3D3D3
#define ORANGERED 0xFF4500
#define SEAGREEN 0x2E8B57
#define YELLOW 0xFFFF00
#define FOREST 0x228B22
#define SKYBLUE 0x87CEEB
#define MAROON 0x800000
#define GREEN1 0xDFFF00
#define GREEN2 0x7CFC00
#define GREEN3 0x4CBB17



/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM            0


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29

#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

#define pi 3.14
#define alpha pi/6

#define UpperBD 51
#define 	Kdr 	0.8
#define 	Kdg 	0.0
#define 	Kdb 	0.0

#define 	display_scaling		150000.0
#define 	display_shifting	0.45


#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define BROWN 0x993300
#define DARKGREEN 0x1A9900
#define GOLD 0xFFD700
#define SILVER 0xD3D3D3
#define ORANGE 0xD3D3D3
#define ORANGERED 0xFF4500
#define SEAGREEN 0x2E8B57
#define YELLOW 0xFFFF00
#define FOREST 0x228B22
#define SKYBLUE 0x87CEEB
#define MAROON 0x800000
#define GREEN1 0xDFFF00
#define GREEN2 0x7CFC00
#define GREEN3 0x4CBB17



int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

void spiwrite(uint8_t c)
{
	 int pnum = 0;
	 src_addr[0] = c;
	 SSP_SSELToggle( pnum, 0 );
	 SSPSend( pnum, (uint8_t *)src_addr, 1 );
	 SSP_SSELToggle( pnum, 1 );
}

void writecommand(uint8_t c)
{
	 LPC_GPIO0->FIOCLR |= (0x1<<21);
	 spiwrite(c);
}

void writedata(uint8_t c)
{
	 LPC_GPIO0->FIOSET |= (0x1<<21);
	 spiwrite(c);
}

void writeword(uint16_t c)
{
	 uint8_t d;
	 d = c >> 8;
	 writedata(d);
	 d = c & 0xFF;
	 writedata(d);
}

void write888(uint32_t color, uint32_t repeat)
{
	 uint8_t red, green, blue;
	 int i;
	 red = (color >> 16);
	 green = (color >> 8) & 0xFF;
	 blue = color & 0xFF;
	 for (i = 0; i< repeat; i++) {
		  writedata(red);
		  writedata(green);
		  writedata(blue);
	 }
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	 writecommand(ST7735_CASET);
	 writeword(x0);
	 writeword(x1);
	 writecommand(ST7735_RASET);
	 writeword(y0);
	 writeword(y1);
}

void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	 int16_t width, height;
	 width = x1-x0+1;
	 height = y1-y0+1;
	 setAddrWindow(x0,y0,x1,y1);
	 writecommand(ST7735_RAMWR);
	 write888(color,width*height);
}

void lcddelay(int ms)
{
	 int count = 24000;
	 int i;
	 for ( i = count*ms; i--; i > 0);
}

void lcd_init()
{
	 int i;
	 printf("LCD Demo Begins!!!\n");
	 // Set pins P0.16, P0.21, P0.22 as output
	 LPC_GPIO0->FIODIR |= (0x1<<16);

	 LPC_GPIO0->FIODIR |= (0x1<<21);

	 LPC_GPIO0->FIODIR |= (0x1<<22);

	 // Hardware Reset Sequence
	 LPC_GPIO0->FIOSET |= (0x1<<22);
	 lcddelay(500);

	 LPC_GPIO0->FIOCLR |= (0x1<<22);
	 lcddelay(500);

	 LPC_GPIO0->FIOSET |= (0x1<<22);
	 lcddelay(500);

	 // initialize buffers
	 for ( i = 0; i < SSP_BUFSIZE; i++ )
	 {

	   src_addr[i] = 0;
	   dest_addr[i] = 0;
	 }

	 // Take LCD display out of sleep mode
	 writecommand(ST7735_SLPOUT);
	 lcddelay(200);

	 // Turn LCD display on
	 writecommand(ST7735_DISPON);
	 lcddelay(200);

}

void drawPixel(int16_t x, int16_t y, uint32_t color)
{
	 if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
	 return;
	 setAddrWindow(x, y, x + 1, y + 1);
	 writecommand(ST7735_RAMWR);
	 write888(color, 1);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	 int16_t slope = abs(y1 - y0) > abs(x1 - x0);
	 if (slope) {
		  swap(x0, y0);
		  swap(x1, y1);
	 }
	 if (x0 > x1) {
		  swap(x0, x1);
		  swap(y0, y1);
	 }
	 int16_t dx, dy;
	 dx = x1 - x0;
	 dy = abs(y1 - y0);
	 int16_t err = dx / 2;
	 int16_t ystep;
	 if (y0 < y1) {
		 ystep = 1;
	 }
	 else {
		 ystep = -1;
	 }
	 for (; x0 <= x1; x0++) {
		  if (slope) {
			  drawPixel(y0, x0, color);
		  }
		  else {
			  drawPixel(x0, y0, color);
		  }
		  err -= dy;
		  if (err < 0) {
			   y0 += ystep;
			   err += dx;
		  }
	 }
}


//////////////////////////////////////////////////////////////////////////////////////////

//Define 2d Point struct
struct Point
{
	int16_t x;
	int16_t y;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Define 3d Point structure
struct Point_3D
 {
	 float x;
	 float y;
	 float z;
 };

int v2p_x1(int16_t x)
{
	return x+_width/2;

}
int v2p_y1(int16_t y)
{
	return -y+_height/2;

}
//define e location
 float_t xe = 100;
 float_t ye = 100;
 float_t ze = 100;


 // To convert world to viewer coordinates
 struct Point Transformation_pipeline (struct Point_3D world)
 {
	 struct Point perspective;
	 struct Point_3D viewer;

	 //define distance
	 float_t Rho =sqrt(pow(xe,2)+pow(ye,2)+pow(ze,2));
	 float_t D_focal = 100;       //D value is supposed to between (10-20)

	 //sinThetha and cosTheta
	 double sintheta = ye / sqrt(pow(xe,2)+pow(ye,2));
	 double costheta = xe / sqrt(pow(xe,2)+pow(ye,2));

	 //second angle
	 double sinphi = sqrt(pow(xe,2)+pow(ye,2))/Rho;
	 double cosphi = ze / Rho;

	 viewer.x = (-sintheta*world.x)+(costheta*world.y);
	 viewer.y = (-costheta*cosphi*world.x)-(cosphi*sintheta*world.y)+(sinphi*world.z);
	 viewer.z = (-sinphi*costheta*world.x)-(sinphi*costheta*world.y)-(costheta*world.z)+Rho;
	 perspective.x = D_focal*viewer.x/viewer.z;
	 perspective.y = D_focal*viewer.y/viewer.z;

	 perspective.x = v2p_x1(perspective.x);
	 perspective.y = v2p_y1(perspective.y);

	 return perspective;
 }

 // To draw the world 3D coordinate system - xw,yw,zw
 void draw3Dcoordinate()
 {
	 struct Point axis;
	 int x1,y1,x2,y2,x3,y3,x4,y4;
	 struct Point_3D org ={0.0,0.0,0.0};
	 struct Point_3D x_ax ={0.0,0.0,0.0};
	 struct Point_3D y_ax ={0.0,0.0,0.0};
	 struct Point_3D z_ax ={0.0,0.0,0.0};
	 axis = Transformation_pipeline(org);

	 //x axis Red
	 x1=axis.x;
	 y1=axis.y;
	 x_ax.x = 180.0;
	 axis = Transformation_pipeline(x_ax);
	 x2=axis.x;
	 y2=axis.y;
	 drawLine(x1,y1,x2,y2,RED);

	 //y axis Green
	 y_ax.y = 180.0;
	 axis = Transformation_pipeline(y_ax);
	 x3=axis.x;
	 y3=axis.y;
	 drawLine(x1,y1,x3,y3,GREEN);

	  //z axis Blue
	 z_ax.z =180.0;
	 axis = Transformation_pipeline(z_ax);
	 x4=axis.x;
	 y4=axis.y;
	 drawLine(x1,y1,x4,y4,BLUE);

 }
 ////////////////////////////////////////////////////////////////////////
 void draw_cube(int start_pnt, int size)
 {
 	struct Point lcd;
 	int x_p[8], y_p[8];
  	double x[8] = {start_pnt,(start_pnt+size),(start_pnt+size),start_pnt,start_pnt,(start_pnt+size),(start_pnt+size),start_pnt};
  	double y[8] = {start_pnt, start_pnt, start_pnt+size, start_pnt+size, start_pnt, start_pnt, (start_pnt+size), (start_pnt+size) };
  	double z[8] = {start_pnt, start_pnt, start_pnt, start_pnt, (start_pnt+size), (start_pnt+size), (start_pnt+size), (start_pnt+size)};
  	for(int i=0; i<8; i++){
  		struct Point_3D a={x[i],y[i],z[i]};
  		lcd = Transformation_pipeline(a);
  		x_p[i] = lcd.x;
  		y_p[i] = lcd.y;
  	}

  	drawLine(x_p[0], y_p[0], x_p[1], y_p[1],BLACK);
  	drawLine(x_p[1], y_p[1], x_p[2], y_p[2],BLACK);
  	drawLine(x_p[2], y_p[2], x_p[3], y_p[3],BLACK);
  	drawLine(x_p[3], y_p[3], x_p[0], y_p[0],BLACK);

  	drawLine(x_p[4], y_p[4], x_p[5], y_p[5],BLACK);
  	drawLine(x_p[5], y_p[5], x_p[6], y_p[6],BLACK);
  	drawLine(x_p[6], y_p[6], x_p[7], y_p[7],BLACK);
  	drawLine(x_p[7], y_p[7], x_p[4], y_p[4],BLACK);

  	drawLine(x_p[0], y_p[0], x_p[4], y_p[4],BLACK);
  	drawLine(x_p[5], y_p[5], x_p[1], y_p[1],BLACK);
  	drawLine(x_p[7], y_p[7], x_p[3], y_p[3],BLACK);
  	drawLine(x_p[6], y_p[6], x_p[2], y_p[2],BLACK);

 }

////////////////////////////////////////////////////////////////////////////////////////////
 void fill_cube(int start_pnt,int size)
 {
 	struct Point s1;

 	int i,j;
 	size=size+start_pnt;

 	for(i=0;i<size;i++)
 	{
 		for(j=0;j<size;j++)
 		{
			struct Point_3D a= {j,i,size};
			struct Point_3D b= {i,size,j};
			struct Point_3D c= {size,j,i};
			s1=Transformation_pipeline(a);	//top fill green
			drawPixel(s1.x,s1.y,RED);
			s1=Transformation_pipeline(b);	// right fill yellow
			drawPixel(s1.x,s1.y,BLUE);
			s1=Transformation_pipeline(c);	// left fill pink
			drawPixel(s1.x,s1.y,GREEN);
 		}
 	}
 }

 void draw_shadow(double start_pnt, double size, struct Point_3D ps)
  {
  	int i,j;
  	int xShad[4]={0}, yShad[4]={0}, zShad[4]={0};
  	struct Point s[4];
	double x[4] = {start_pnt, start_pnt+size, start_pnt+size, start_pnt};
  	double y[4] = {start_pnt, start_pnt, start_pnt+size, start_pnt+size};
  	double z[4]= {start_pnt+size,start_pnt+size,start_pnt+size,start_pnt+size};
  	for(i=0; i<4; i++)
  	{

  		xShad[i] = ps.x + (-ps.z/(z[i]-ps.z))*(x[i]-ps.x);
  		yShad[i] = ps.y + (-ps.z/(z[i]-ps.z))*(y[i]-ps.y);
  		zShad[i] = ps.z + (-ps.z/(z[i]-ps.z))*(z[i]-ps.z);
  	}

  	struct Point_3D shades[4];
  	for(int i=0; i<4; i++){
  		shades[i].x = xShad[i];
  		shades[i].y = yShad[i];
  		shades[i].z = zShad[i];
  		s[i] = Transformation_pipeline (shades[i]);
  	}
	for(int i=size;i<shades[2].x;i++)
	{

		for(int j=0;j<shades[2].y;j++)
		{
				struct Point_3D a= {i,j,0};

				struct Point s1=Transformation_pipeline(a);	//top fill green
				drawPixel(s1.x,s1.y,BLUE);
		}
	}
  }


 ///////////////////////////////////////////////////////////////

 void draw_X(int start_pnt, int size)
   {
   	struct Point p1;
   	int i,j;
   	size=size+start_pnt;
  	int map[size][size];

 	for(i = start_pnt; i < size/2;i++)
  		{
  			for(j = start_pnt; j < size;j++)
  			{
  			map[i][j]=0;
  			}
  		}

  	for(i = start_pnt; i < size;i++)
  		{
  			for(j = start_pnt; j < size;j++)
  			{
  					if(i==j || i==size-j-1)
  						map[i][j]=1;
  					else if((j+1<size && i==j+1) || (j+1<size && i==size-j-2))
  						map[i][j]=1;
  					else if((j+2<size && i==j+2) || (j+2<size && i==size-j-3))
  						map[i][j]=1;
  					else if((j+3<size && i==j+3) || (j+3<size && i==size-j-4))
  						map[i][j]=1;
  					else if((j+4<size && i==j+4) || (j+4<size && i==size-j-5))
  						map[i][j]=1;
  					else if((j+5<size && i==j+5) || (j+5<size && i==size-j-6))
  						map[i][j]=1;
  					else if((j+6<size && i==j+6) || (j+6<size && i==size-j-7))
  						map[i][j]=1;
  					else if((j+7<size && i==j+7) || (j+7<size && i==size-j-8))
  						map[i][j]=1;
  					else if((j+8<size && i==j+8) || (j+8<size && i==size-j-9))
  						map[i][j]=1;
  					else if((j+9<size && i==j+9) || (j+9<size && i==size-j-10))
  						map[i][j]=1;
  					else
  						map[i][j]=0;

  			}
  		}

  	for(i=0;i<size;i++)
  	{
  		for(j=0;j<size;j++)
  		{
  			if(map[i][j]==1)
  			{
  				struct Point_3D a= {j,i,size};
  				p1 = Transformation_pipeline(a);
  				drawPixel(p1.x,p1.y,WHITE);
  			}
  			else if(map[i][j]==0)
  			{
  				struct Point_3D a= {j,i,size};
  				p1 = Transformation_pipeline(a);
  			}
  		}
  	}
   }


 /////////////////////////////////////////////////////////////////

struct Point drawbranch(int x1, int y1, int size, char plane){
	if(plane == 'y') {
	struct Point_3D d={y1, size, x1};
	return Transformation_pipeline (d);
	}
	struct Point_3D d={size, y1, x1};
    return Transformation_pipeline (d);
 }

 void drawLine2DPoints(struct Point p1, struct Point p2, uint32_t color){
	 drawLine(p1.x, p1.y, p2.x, p2.y, color);
 }

 //void draw_tree_for_cube(uint32_t color,int start_pnt, int size, char plane)
 void drawTrees_on_cube_surface(uint32_t color,int start_pnt, int size, char plane)
 {
 	int i=0;
 	float sin30=0.5;
 	float cos30=0.866;
 	float d=0.134;
 	int tree_branch[3][3]={{start_pnt,start_pnt+10,0.5*size},
 							{start_pnt,start_pnt+20,0.3*size},
							{start_pnt,start_pnt+20,0.8*size}};
 	while(i<3)
 	{
 		int x0, y0, y1, x1;
 		x0=tree_branch[i][0];
 		x1=tree_branch[i][1];
 		y0=tree_branch[i][2];
 		y1=y0;
 		i++;

 		struct Point arr[2];
 		arr[0] = drawbranch(x0, y0, size, plane);
 		arr[1] = drawbranch(x1, y1, size, plane);
	 	for (int j = -1; j < 2; j++)
	 	{
	 		drawLine(arr[0].x+j, arr[0].y+j, arr[1].x+j, arr[1].y+j, 0x00FF8000);
	 	}

 		for(int it=0;it<7;it++)
		{
 			struct Point left[4];
 			struct Point middle[4];
 			struct Point right[4];
 			middle[0] = arr[1];
			int16_t x2=(0.6*(x1-x0))+x1;
			int16_t y2=y1;
			middle[1] = drawbranch(x2, y2, size, plane);
			int16_t xr= ((d*x1)+(cos30*x2)-(sin30*y2)+(sin30*y1));
			int16_t yr=((sin30*x2)-(sin30*x1)+(cos30*y2)-(cos30*y1)+y1);
			middle[2] = drawbranch(xr, yr, size, plane);
			int16_t xl=((d*x1)+(cos30*x2)+(sin30*y2)-(sin30*y1));
			int16_t yl=((sin30*x1)-(sin30*x2)+(d*y2)+(cos30*y1));
			middle[3] = drawbranch(xl, yl, size, plane);
			// Right subtree building
			int16_t xrLen = sqrt(pow((xr-x1),2)+pow((yr-y1),2)) ;
			int16_t xrImag= (0.8*xrLen)+xr;
			int16_t xr1 = ((d*xr)+(cos30*xrImag)-(sin30*yr)+(sin30*yr));
			int16_t yr1 = ((sin30*xrImag)-(sin30*xr)+(cos30*yr)-(cos30*yr)+yr);
			right[0] = middle[2];
			right[1] = drawbranch(xr1, yr1,size, plane);
			int16_t xrr,xrl,yrr,yrl;
			xrr = ((d*xr)+(cos30*xr1)-(sin30*yr1)+(sin30*yr));
			yrr = ((sin30*xr1)-(sin30*xr)+(cos30*yr1)-(cos30*yr)+yr);
			right[2] = drawbranch(xrr, yrr,size, plane);
			xrl = ((d*xr)+(cos30*xr1)+(sin30*yr1)-(sin30*yr));
			yrl = ((sin30*xr)-(sin30*xr1)+(d*yr)+(cos30*yr1));
			right[3] = drawbranch(xrl, yrl,size, plane);
			// Left Subtree building
			left[0] = middle[3];
			int16_t xlImag= (0.8*xrLen)+xl;
			int16_t xl1 = ((d*xl)+(cos30*xlImag)+(sin30*yl)-(sin30*yl));
			int16_t yl1 = ((sin30*xl)-(sin30*xlImag)+(d*yl)+(cos30*yl));
			left[1] = drawbranch(xl1, yl1,size, plane);
			int16_t xlr,xll,ylr,yll;
			xlr = ((d*xl)+(cos30*xl1)-(sin30*yl1)+(sin30*yl));
			ylr = ((sin30*xl1)-(sin30*xl)+(cos30*yl1)-(cos30*yl)+yl);
			left[2] = drawbranch(xlr, ylr,size, plane);
			xll = ((d*xl)+(cos30*xl1)+(sin30*yl1)-(sin30*yl));
			yll = ((sin30*xl)-(sin30*xl1)+(d*yl)+(cos30*yl1));
			left[3] = drawbranch(xll, yll,size, plane);
			// Drawing all the branches.
			for(int i1=0; i1<4; i1++){
				drawLine2DPoints(middle[0], middle[i1], color);
				drawLine2DPoints(right[0], right[i1], color);
				drawLine2DPoints(left[0], left[i1], color);
			}
			x0=x1;
			x1=x2;
 		}
 	}
 }
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	float r,g,b;
} diffuse_rgb;

uint32_t getColor(int Kr){
	uint32_t color = 0x000000;
	Kr |= 0x000000;
	color |= Kr << 16;
	color |= (0x000000) << 8;
	color |= (0x000000);
	return color;
}

uint32_t getColor2(int Kb){
	uint32_t color = 0x000000;
	Kb |= 0x000000;
	color |= (0x000000) << 16;
	color |= (0x000000) << 8;
	color |= Kb;
	return color;
}

uint32_t getColor3(int Kg){
	uint32_t color = 0x000000;
	Kg |= 0x000000;
	color |= (0x000000) << 16;
	color |= Kg << 8;
	color |= (0x000000);
	return color;
}

int getScaledKr(float rmax, float rmin, float kr) {

float k = (255.0-20.0)/(rmax-rmin);
float c = 20.0-(k*rmin);
return (int)(k*kr+c);
}

void Draw_diffusion(int pt, int cube_size, int elev, struct Point_3D pl_source)
{
	 struct Point_3D pt1 = {pt,pt,(cube_size+pt+elev)};
	 struct Point_3D pt2 = {(cube_size+pt),(cube_size+pt),(cube_size+pt+elev)};
	 float diffuse_rmin, diffuse_rmax;
	 float temp_distance1 = pow((pt1.x- pl_source.x),2)+
					  pow((pt1.y- pl_source.y),2)+
					  pow((pt1.z- pl_source.z),2);
	 float temp_distance2 = pow((pt2.x- pl_source.x),2)+
					  pow((pt2.y- pl_source.y),2)+
					  pow((pt2.z- pl_source.z),2);
	 diffuse_rmax =  (Kdr * abs(pt1.z-pl_source.z)) / (temp_distance1*sqrt(temp_distance1));
	 diffuse_rmin =  (Kdr * abs(pt2.z-pl_source.z)) / (temp_distance2*sqrt(temp_distance2));
	 diffuse_rgb  diffuse;
	 for(int i=0; i<cube_size; i++) {
		 for(int j=0; j<cube_size; j++) {
			 struct Point_3D pt = {i, j, cube_size+elev};
			 float distance = pow((pt.x- pl_source.x),2)+
							  pow((pt.y- pl_source.y),2)+
							  pow((pt.z- pl_source.z),2);
			 diffuse.r = (Kdr * abs(pt.z-pl_source.z)) / (distance*sqrt(distance));
			 diffuse.g = (Kdg * abs(pt.z-pl_source.z)) / distance;
			 diffuse.b = (Kdb * abs(pt.z-pl_source.z)) / distance;
			 int scaled_kr = getScaledKr(diffuse_rmax, diffuse_rmin, diffuse.r);
			 struct Point lcd = Transformation_pipeline(pt);
			 drawPixel(lcd.x, lcd.y, getColor(scaled_kr));
		 }
	 }

}

void Draw_diffusion_XZ(int pt, int cube_size, int elev, struct Point_3D pl_source)
{
	 struct Point_3D pt1 = {pt,(cube_size+pt+elev),(cube_size+pt+elev)};
	 struct Point_3D pt2 = {(cube_size+pt),(cube_size+pt),pt+elev};
	 float diffuse_rmin, diffuse_rmax;
	 float temp_distance1 = pow((pt1.x- pl_source.x),2)+
					  pow((pt1.y- pl_source.y),2)+
					  pow((pt1.z- pl_source.z),2);
	 float temp_distance2 = pow((pt2.x- pl_source.x),2)+
					  pow((pt2.y- pl_source.y),2)+
					  pow((pt2.z- pl_source.z),2);
	 float Kdb2 = 0.8;
	 float Kdr2 = 0.0;
	 float Kdg2 = 0.0;
	 diffuse_rmax =  (Kdb2 * abs(pt1.y-pl_source.y)) / temp_distance1;
	 diffuse_rmin =  (Kdb2 * abs(pt2.y-pl_source.y)) / temp_distance2;
	 diffuse_rgb  diffuse;
	 for(int i=0; i<cube_size; i++) {
		 for(int j=0; j<cube_size; j++) {
			 struct Point_3D pt = {i, cube_size+elev, j};
			 float distance = pow((pt.x- pl_source.x),2)+
							  pow((pt.y- pl_source.y),2)+
							  pow((pt.z- pl_source.z),2);
			 diffuse.r = (Kdr2 * abs(pt.y-pl_source.y)) / distance;
			 diffuse.g = (Kdg2 * abs(pt.y-pl_source.y)) / distance;
			 diffuse.b = (Kdb2 * abs(pt.y-pl_source.y)) / distance;
			 int scaled_kr = getScaledKr(diffuse_rmax, diffuse_rmin, diffuse.b);
			 struct Point lcd = Transformation_pipeline(pt);
			 drawPixel(lcd.x, lcd.y, getColor2(scaled_kr));
		 }
	 }

}


/*Main Function main()*/
int main (void)

{
	uint32_t pnum = PORT_NUM;
	pnum = 1 ;
	if ( pnum == 1 )
	SSP1Init();
	else
	puts("Port number is not correct");
	lcd_init();
	fillrect(0, 0, ST7735_TFTWIDTH,ST7735_TFTHEIGHT, 0xBFEFFF);
	printf("Choose 1 for 3D Coordinate for R-G-B axis\n");
	int choice;
	scanf("%d",&choice);
	int size = 50, start_pnt = 0;
	if(choice==1)
	 {
		draw3Dcoordinate();
		draw_cube(start_pnt,size);
		fill_cube(start_pnt,size);
		struct Point_3D pl_source = {-400,20,400};
		int elev = 0;
		Draw_diffusion_XZ(start_pnt, size, elev, pl_source);
		drawTrees_on_cube_surface(GREEN,start_pnt,size, 'y');
		drawTrees_on_cube_surface(YELLOW,start_pnt,size, 'x');
		Draw_diffusion(start_pnt, size, elev, pl_source);
		draw_X(start_pnt, size);
		draw_shadow(start_pnt,size,pl_source);
	}
	else
	{
		printf("Invalid choice.!!!!");
	}

return 0;

}



