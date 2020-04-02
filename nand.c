//----------------------------------------------------------
//
// Lab #4 : Page Mapping FTL Simulator
// 	- Embedded System Design, ICE3028 (Fall, 2019)
//
// Oct. 10, 2019.
//
// TA: Junho Lee, Somm Kim
// Prof: Dongkun Shin
// Embedded Software Laboratory
// Sungkyunkwan University
// http://nyx.ssku.ac.kr
//
//---------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "nand.h"
u32** p;
int nand_init(int nbanks, int nblks, int npages)
{
	p=(u32**)calloc((nbanks*nblks*npages),sizeof(u32));
	int i;
	for(i=0; i<nbanks*nblks*npages; i++)
	{
		p[i]=(u32*)calloc(9, sizeof(u32));
	}
	printf("initialize");
	// initialize the NAND flash memory 
	// "nblks": the total number of flash blocks per bank
	// "npages": the number of pages per block
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	return 0;
}

int nand_write(int bank, int blk, int page, u32 *data, u32 spare)
{

	if(bank<0)
	{
		
		return -1;
	}
	else if(blk<0)
	{
		
		return -1;
	}
	else if(page<0)
	{
		
		return -1;
	}

	else if(p[bank*32+blk*32+page][0]!=0)
	{
		
		return -1;
	}
	else if(p[bank*32+blk*32+(page-1)][0]==0 && page>=1)
	{
		
		return -1;
	}
	else
	{	int i;
		for(i=0; i<8; i++)
		{
			p[bank*32+blk*32+page][i]=data[0];
		}
		p[bank*32+blk*32+page][8]=spare;
		
	}
	// write "data" and "spare" into the NAND flash memory pointed to by "blk" and "page"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	return 0;
}


int nand_read(int bank, int blk, int page, u32 *data, u32 *spare)
{
	if(bank<0)
	{
		printf("read(%d, %d, %d) : failed, invalid bank number\n", bank, blk, page);
		return -1;
	}

	else if(blk<0)
	{
		printf("read(%d, %d, %d) : failed, invalid blk number\n", bank, blk, page);
		return -1;
	}

	else if(page<0)
	{
		printf("read(%d, %d, %d) : failed, invalid page number\n", bank, blk, page);
		return -1;
	}

	else if(p[bank*32+blk*32+page][0]!=0)
	{
		printf("read(%d, %d, %d) : data = ox%x, spare = ox%x\n", bank, blk, page, p[bank*32+blk*32+page][0], p[bank*32+blk*32+page][8]);
		return 0;
	}
	else if(p[bank*32+blk*32+page][0]==0)
	{
		printf("read(%d, %d, %d) : failed, trying to read an empty page\n", bank, blk, page);
		return -1;
	}
	// read "data" and "spare" from the NAND flash memory pointed to by "blk" and "page"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	return 0;
}

int nand_erase(int bank, int blk)
{
	if(blk<0)
	{
		printf("erase(%d, %d) : failed. invalid block number\n", bank, blk);
		return -1;
	}
	else if(blk>=8)
	{
		printf("erase(%d, %d) : failed. invalid block number\n", bank, blk);
		return -1;
	}
	int i;
	int count=0;
	for(i=0; i<32; i++)
	{
		
		if(p[bank*32+blk*32+i][0]!=0)
			count++;
	}
	if(count!=0)
	{
		int j;
		int k;
		for(j=0; j<32; j++)
		{
			for(k=0; k<9; k++)
			{
				p[bank*32+blk*32+j][k]=0;
			}
		}
		printf("erase(%d, %d) : block erased\n", bank, blk);
		return 0;
	}
	else
	{
		printf("erase(%d, %d) : failed, trying to erase a free block\n", bank, blk);
		return -1;
	}
	// erase the NAND flash memory block "blk"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	return 0;
}


int nand_blkdump(int bank, int blk)
{
	// dump the contents of the NAND flash memory block [blk] (for debugging purpose)
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message
	int i;
	int count=0;
	for(i=0; i<32; i++)
	{
		if(p[bank*32+blk*32+i][0]!=0)
		{
			count++;
		}
	}
	
	if(count!=0)
	{
		printf("dlkdump(%d, %d) : Total %d page(s) written\n", bank, blk, count);
		
		for(i=0; i<32; i++)
		{
			if(p[bank*32+blk*32+i][0]!=0)
			{
				printf("dlkdump(%d, %d, %d) : data = 0x%x, spare = 0x%x\n",bank, blk, i, p[bank*32+blk*32+i][0],p[bank*32+blk*32+i][8]); 
			}
		}
		return 0;
	}
	else
	{
		printf("dlkdump(%d, %d) : FREE\n", bank, blk);
		return -1;
	}

	return 0;
}

