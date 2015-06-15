void swapInline (int entrada, int saida)
{
	__asm__
	(
	  "movl %1,%%eax\n\t"
	  "movl %%eax,%0\n\t"
		
		//  %0
		:"=r"(saida)
		//  %1
		:"=r"(entrada)
		:"%eax","%ebx","%ecx","%edx"
	);
}
