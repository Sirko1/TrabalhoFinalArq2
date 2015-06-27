#include <stdio.h>
#include <math.h>

int main()
{
		//variaveis auxiliares para uso de instrucoes simd
		static char multiplicador[]= {63,0,127,0,255,0,0,0};

		int cont=0, cont2=0;

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
        static unsigned char color[8]; //color[3] pra color[8]
		static unsigned char colorArquivo[3]; //pra escrever no arquivo
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

		 __asm{ //fora dos loops por questao de eficiencia
			mov edx, offset multiplicador[0]
			movq mm2, qword ptr [edx]   //mm1=     63   :  127     :   255
		 }



        for(iY=0;iY<iYmax;iY++)
        {
			 cont++;
			 cont2++;
			 if(cont==1000)
				 scanf("%d", &cont);

			 Cy=CyMin + iY*PixelHeight;
             if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
             for(iX=0;iX<iXmax;iX++)
             {         
                        Cx=CxMin + iX*PixelWidth;
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
						__asm{
								mov edx, offset color[0]
								pxor mm0, mm0 //mm0=0
								movq qword ptr [edx], mm0
							}
						}
                        else 
                        { /* exterior of Mandelbrot set = white */


							color[0]=((IterationMax-Iteration) % 8);  /* Red */
							color[2]=((IterationMax-Iteration) % 4);  /* Green */ 
							color[4]=((IterationMax-Iteration) % 2);  /* Blue */

							__asm{

								
								mov edx, offset color[0]
								movq mm1, qword ptr [edx]   //mm1= color[0] : color[1] : color[2]
								
								pmullw mm1, mm2 // mm1*=mm2 (multiplicacao com sinal, mas nao importa)
								
								movq [edx], mm1						
							 };
                        };						

						//colorArquivo serah escrito no arquivo                        
						__asm{
							mov al, color[0]
							mov colorArquivo[0], al
							mov al, color[2]
							mov colorArquivo[1], al
							mov al, color[4]
							mov colorArquivo[2], al
						};
						
                        fwrite(colorArquivo,1,3,fp);
                }
        }
        fclose(fp);
        return 0;
}
