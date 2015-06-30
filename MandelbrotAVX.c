/* 
Modified from: http://rosettacode.org/wiki/Mandelbrot_set#PPM_non_interactive
c program:
--------------------------------
1. draws Mandelbrot set for Fc(z)=z*z +c
using Mandelbrot algorithm ( boolean escape time )
-------------------------------         
2. technique of creating ppm file is  based on the code of Claudio Rocchini
http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
create 24 bit color graphic file ,  portable pixmap file = PPM 
see http://en.wikipedia.org/wiki/Portable_pixmap
to see the file use external application ( graphic viewer)
*/
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

int main()
{
        /* screen ( integer) coordinate */
        int iX,iY;
        const int iXmax = 16384; 
        const int iYmax = 16384;
        /* world ( double) coordinate = parameter plane*/
        double Cx[4],Cy[4];
        const double CxMin=-2.5;
        const double CxMax=1.5;
		const double CyMin[4]={-2.0, -2.0, -2.0, -2.0};
        const double CyMax=2.0;
        /* */
		double PixelWidth[4] = {(CxMax-CxMin)/iXmax, (CxMax-CxMin)/iXmax, (CxMax-CxMin)/iXmax, (CxMax-CxMin)/iXmax};
		double PixelHeight[4] = {(CyMax-CyMin[0])/iYmax, (CyMax-CyMin[0])/iYmax, (CyMax-CyMin[0])/iYmax, (CyMax-CyMin[0])/iYmax};
        /* color component ( R or G or B) is coded from 0 to 255 */
        /* it is 24 bit color RGB file */
        const int MaxColorComponentValue=255; 
        FILE * fp;
        char *filename="mandelbrot.ppm";
        static unsigned char color[3];
        /* Z=Zx+Zy*i  ;   Z0 = 0 */
        double Zx, Zy;
        double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
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
		
		__m256d ymm0 = _mm256_broadcast_sd(Cy);   //all Cy
		__m256d ymm1 = _mm256_broadcast_sd(CyMin);//all CyMin
		__m256d ymm2 = _mm256_broadcast_sd(PixelHeight);//all PixelHeight
		
		double incr[4] = {0.0, 1.0, 2.0, 3.0};
		__m256d ymm3 = _mm256_load_pd(incr);
		
		double l1[4] = {4.0, 4.0, 4.0, 4.0};
		__m256d ymm4 = _mm256_load_pd(l1);

		for(iY=0;iY<iYmax/4;iY+=4)
		{
			 __m256d ymm0 = _mm256_mul_pd(ymm3, ymm2); //Cy = iY*PixelHeight
			 ymm0 = _mm256_add_pd(ymm0, ymm1);         //Cy = iY*PixelHeight + CyMin
			 __asm{
					vmovupd [Cy], ymm0
				  }
			 ymm3 = _mm256_add_pd(ymm3, ymm4);
		}



        for(iY=0;iY<iYmax;iY++)
        {
             //Cy=CyMin + iY*PixelHeight;
             if (fabs(Cy[iY])< PixelHeight[0]/2) Cy[iY]=0.0; /* Main antenna */
             for(iX=0;iX<iXmax;iX++)
             {         
                        Cx[0]=CxMin + iX*PixelWidth[0];
                        /* initial value of orbit = critical point Z= 0 */
                        Zx=0.0;
                        Zy=0.0;
                        Zx2=Zx*Zx;
                        Zy2=Zy*Zy;
                        /* */
                        for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
                        {
                            Zy=2*Zx*Zy + Cy[0];
                            Zx=Zx2-Zy2 +Cx[0];
                            Zx2=Zx*Zx;
                            Zy2=Zy*Zy;
                        };
                        /* compute  pixel color (24 bit = 3 bytes) */
                        if (Iteration==IterationMax)
                        { /*  interior of Mandelbrot set = black */
                           color[0]=0;
                           color[1]=0;
                           color[2]=0;                           
                        }
                        else 
                        { /* exterior of Mandelbrot set = white */
                             color[0]=((IterationMax-Iteration) % 8) *  63;  /* Red */
                             color[1]=((IterationMax-Iteration) % 4) * 127;  /* Green */ 
                             color[2]=((IterationMax-Iteration) % 2) * 255;  /* Blue */
                        };
                        /*write color to the file*/
                        fwrite(color,1,3,fp);
                }
        }
        fclose(fp);
        return 0;
}
