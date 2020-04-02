//----------------------------------------------------------
//
// Lab #4 : Page Mapping FTL Simulator
//    - Embedded System Design, ICE3028 (Fall, 2019)
//
// Oct. 10, 2019.
//
// Junho Lee, Somm Kim
// Dongkun Shin (dongkun@skku.edu)
// Embedded Systems Laboratory
// Sungkyunkwan University
// http://nyx.skku.ac.kr
//
//---------------------------------------------------------

#include "ftl.h"
#include "nand.h"
u32* q;
u32** r;
#ifdef COST_BENEFIT
static long now() {
	return s.host_write + s.gc_write;
}
#endif

void garbage_collection(u32 bank);

void ftl_open()
{
	q=(u32*)calloc(N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK, sizeof(u32));
	r=(u32**)calloc(N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK, sizeof(u32));
	
	int i;
	for(i=0; i<N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK; i++)
	{
		r[i]=(u32*)calloc(9, sizeof(u32));
	}
	
	
	return;
}

void ftl_read(u32 lpn, u32 *read_buffer)
{
	int i;
	for(i=0; i<8; i++)
	{
		read_buffer[i]=r[q[lpn]][i];
	}
		
	return;
}

void ftl_write(u32 lpn, u32 *write_buffer)
{	
	int i;
	int j;
	int k;


	if(q[lpn]!=0)
	{
		for(k=0; k<9; k++)
		{
			r[q[lpn]][k]=1;
		}

		for(i=0; i<N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK; i++)
		{
			
			if(r[i][0]==0)
			{
				for(j=0; j<8; j++)
				{
					r[i][j]=write_buffer[j];
				}
				r[i][8]=lpn;
				q[lpn]=i;
				break;
			}	 
		}
		if(s.host_write%200==0)
		{
			garbage_collection(0);
		}
	}
	else
	{
		
		
		for(i=0; i<N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK; i++)
		{
			
			if(r[i][0]==0)
			{
				for(j=0; j<8; j++)
				{
					r[i][j]=write_buffer[j];
				}
				r[i][8]=lpn;
				q[lpn]=i;
				break;
			}	 
		}

	}
	
	return;
}

void garbage_collection(u32 bank)
{
	s.gc++;

#ifndef COST_BENEFIT
	// Greedy policy


#else
	// Cost-Benefit policy

#endif

/***************************************
Add

s.gc_write++;

for every nand_write call (every valid page copy)
that you issue in this function
***************************************/
	int a;
	int b;
	int c;
	int m;
	int max=0;
	int pos=0;	
	for(a=bank*BLKS_PER_BANK*PAGES_PER_BLK; a<(bank+2)*BLKS_PER_BANK*PAGES_PER_BLK; a+=PAGES_PER_BLK)
	{
		c=0;
		for(b=a; b<a+PAGES_PER_BLK; b++)
		{
			if(r[b][0]==1)
			{
				c++;
				for(m=0; m<9; m++)
				{
					r[b][m]=0;
				}
			}
		}
		if(c>max)
		{
			max=c;
			pos=a;
		}				
	}
	for(a=pos; a<pos+PAGES_PER_BLK; a++)
	{
		if(r[a][0]!=0)
		{
			s.gc_write++;
		}
	}
				
	return;
}

