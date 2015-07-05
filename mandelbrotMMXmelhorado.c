#include <stdio.h>
#include <math.h>

int main()
{
        /* screen ( integer) coordinate */
        int iX,iY;
        const int iXmax = 16384; 
        const int iYmax = 16384;
        /* world ( double) coordinate = parameter plane*/
        double Cx,Cy;
        const double CxMin=-2.5;
        const double CxMax=1.5;
        const double CyMin=-2.0;
        const double CyMax=2.0;
        /* */
        double PixelWidth=(CxMax-CxMin)/iXmax;
        double PixelHeight=(CyMax-CyMin)/iYmax;
        /* color component ( R or G or B) is coded from 0 to 255 */
        /* it is 24 bit color RGB file */
        const int MaxColorComponentValue=255; 
        FILE * fp;
        char *filename="mandelbrot.ppm";
        static unsigned char color[3], colorBuffer[5]; //3bytes + 5bytes de buffer

		static unsigned char colorAux[]={63,0,127,0,255,0,0,0};//auxiliar pra fazer mult de word com byte
		static unsigned char colorAux2[8];
		
        /* Z=Zx+Zy*i  ;   Z0 = 0 */
        double Zx;
		double Zy;
        double Zx2;
		double Zy2;
        /*  */
        int Iteration;
        const int IterationMax=256;
        /* bail-out value , radius of circle ;  */
        const double EscapeRadius=2;
        double ER2=EscapeRadius*EscapeRadius;
        /*create new file,give it a name and open it in binary mode  */
        fp= fopen(filename,"wb"); /* b -  binary mode */
        /*write ASCII header to the file*/
        fprintf(fp,"P6\n %d\n %d\n %d\n",iXmax,iYmax,MaxColorComponentValue);
        /* compute and write image data bytes to the file*/
        for(iY=0;iY<iYmax;iY++)
        {
             Cy=CyMin + iY*PixelHeight;
             if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
             for(iX=0;iX<iXmax;iX++)
             {   

                        Cx=CxMin + iX*PixelWidth;
                        /* initial value of orbit = critical point Z= 0 */
						
                        Zx=0.0;
                        Zy=0.0;
                        Zx2=Zx*Zx;
                        Zy2=Zy*Zy;
						
                        for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
                        {
                            Zy=2*Zx*Zy + Cy;
                            Zx=Zx2-Zy2 +Cx;
                            Zx2=Zx*Zx;
                            Zy2=Zy*Zy;
                        };
                        /* compute  pixel color (24 bit = 3 bytes) */
                        if (Iteration==IterationMax)
                        { /*  interior of Mandelbrot set = black */
								colorAux2[0]=0;
								colorAux2[2]=0;
								colorAux2[4]=0;
                        }
                        else 
                        { /* exterior of Mandelbrot set = white */
						
                            colorAux2[0]=((IterationMax-Iteration) % 8);  /* Red */
                            colorAux2[2]=((IterationMax-Iteration) % 4);  /* Green */
                            colorAux2[4]=((IterationMax-Iteration) % 2);  /* Blue */
						
							__asm{
								movq mm2, qword ptr colorAux2[0]
								PMULLW mm2, qword ptr colorAux[0] //(%8, %4, %2)*(63, 127, 255)
								movq qword ptr colorAux2[0], mm2
								emms
							 }
							 
                        }
                        /*write color to the file*/
                        fwrite(&colorAux2[0],1,1,fp);
						fwrite(&colorAux2[2],1,1,fp);
						fwrite(&colorAux2[4],1,1,fp);
                }
        }
        fclose(fp);
        return 0;
}
