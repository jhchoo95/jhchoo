//----------------------------------------------------------
// Dongkun Shin (dongkun@skku.edu) embeded software lab

// Jonghoon Choo 
//---------------------------------------------------------

#include "ftl.h"
#include "nand.h"
u32* q;
u32** r;

#ifdef COST_BENEFIT
/*
static long now() {
	return s.host_write + s.gc_write;
}
*/
#endif

void garbage_collection(u32 bank);

int nand_in()
{
	r=(u32**)calloc(N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK, sizeof(u32));
	int i;
	for(i=0; i<N_BANKS*BLKS_PER_BANK*PAGES_PER_BLK; i++)
	{
		r[i]=(u32*)calloc(9, sizeof(u32));
	}
	return 0;
}

int nand_re(u32 lpn, int i)
{
	return r[q[lpn]][i];
}

int nand_wr(u32 lpn, u32 *write_buffer)
{
	int i;
	int j;
	int k;
	int or=0;
	if(q[lpn]!=2500)
	{
		for(k=0; k<9; k++)
		{
			r[q[lpn]][k]=1;
		}
	
		for(i=0; i<28*32; i++)
		{
			
			if(r[i][0]==0)
			{
				for(j=0; j<8; j++)
				{
					r[i][j]=write_buffer[j];
				}
				r[i][8]=lpn;
				q[lpn]=i;
				or=1;
				break;
			}	 
		}
		if(or==0)
		{
			for(i=32*32; i<1024+(28*32); i++)
			{
			
				if(r[i][0]==0)
				{
					for(j=0; j<8; j++)
					{
						r[i][j]=write_buffer[j];
					}
				
					r[i][8]=lpn;
					q[lpn]=i;
					or=1;
					break;
				}
			}	 
		}
		
	}
	else
	{	
		for(i=0; i<28*32; i++)
		{
			
			if(r[i][0]==0)
			{
				for(j=0; j<8; j++)
				{
					r[i][j]=write_buffer[j];
				}
				r[i][8]=lpn;
				q[lpn]=i;
				or=1;
				break;
			}	 
		}
		if(or==0)
		{
			for(i=32*32; i<1024+28*32; i++)
			{
			
				if(r[i][0]==0)
				{
					for(j=0; j<8; j++)
					{
						r[i][j]=write_buffer[j];
					}
				
					r[i][8]=lpn;
					q[lpn]=i;
					or=1;
					break;
				}
			}	 
		}
		

	}
	return 0;
}

int nand_er(int bank, int pos, int pages)
{
	int a;
	int b;
			
	for(a=pos; a<pos+pages; a++)
	{
		if(r[a][0]==1)
			memset(r[a],0,sizeof(u32)*9);
		if(r[a][0]!=0)
		{
			s.gc_write++;
			for(b=bank*32*32; b<pos; b++)
			{
				if(r[b][0]==0)
				{
					memset(r[b],r[a][0],sizeof(u32)*8);
					r[b][8]=r[a][8];
					q[r[a][8]]=b;
					memset(r[a],0,sizeof(u32)*9);
					break;
				}
			}	 
		}
			
	}
	return 0;
}

void ftl_open()
{
	q=(u32*)calloc(1792, sizeof(u32));
	int i;
	
	for(i=0; i<1792; i++)
	{
		q[i]=2500;
	}
	nand_in();
	return;
}

void ftl_read(u32 lpn, u32 *read_buffer)
{
	int i;
	for(i=0; i<8; i++)
	{
		read_buffer[i]=nand_re(lpn,i);
	}
		
	return;
}

void ftl_write(u32 lpn, u32 *write_buffer)
{	
	
	if(1792-s.host_write-s.gc_write+32*s.gc<100)
	{	
		garbage_collection(0);
		garbage_collection(1);
	}
	nand_wr(lpn,write_buffer);
	
	return;
}

void garbage_collection(u32 bank)
{
	s.gc++;

#ifndef COST_BENEFIT
	// Greedy policy
	int a;
	int b;
	int c;
				
	int max=0;
	int pos=0;
		
	for(a=bank*32*32; a<bank*32*32+28*32; a+=PAGES_PER_BLK)
	{
		c=0;
		for(b=a; b<a+PAGES_PER_BLK; b++)
		{
			if(r[b][0]==1)
			{
				c++;
			}
				
		}
		if(c==32)
		{
			pos=a;
			break;
		}
		if(c>max)
		{
			max=c;
			pos=a;
		}				
	}
	nand_wr(bank, pos, PAGES_PER_BLK);


	
#else
	// Cost-Benefit policy

	int a;
	int b;
	int c;
	double d;

	double max=0;
	int pos=0;
		
/*	
	for(a=bank*32*32; a<bank*32*32+28*32; a+=PAGES_PER_BLK)
	{
		c=32;
		for(b=a; b<a+PAGES_PER_BLK; b++)
		{
			if(r[b][0]==1)
			{
				c--;	
			}
		}
		if(c==0)
		{
			pos=a;
			break;
		}
		d=now()*(320/c);
			
		if(d>max)
		{
			max=d;
			pos=a;
		}	
			
	}
	for(a=pos; a<pos+PAGES_PER_BLK; a++)
	{
		if(r[a][0]==1)
			memset(r[a],0,sizeof(u32)*9);
		if(r[a][0]!=0)
		{
			s.gc_write++;
			for(b=bank*32*32; b<pos; b++)
			{
				if(r[b][0]==0)
				{
					memset(r[b],r[a][0],sizeof(u32)*8);
					r[b][8]=r[a][8];
					q[r[b][8]]=b;
					memset(r[a],0,sizeof(u32)*9);
					break;
				}
			}
		}
	}
	//CAT POLICY
	
*/
/*
	for(a=bank*32*32; a<bank*32*32+28*32; a+=PAGES_PER_BLK)
	{
		c=32;
		
		for(b=a; b<a+PAGES_PER_BLK; b++)
		{
			if(r[b][0]==1)
			{
				c--;	
			}
		}
		if(c==0)
		{
			pos=a;
			break;
		}
		if(s.count[a/32]==0)
		{
			pos=a;
			break;
		}
		d=now()*(32/c)/(s.count[a/32]);
			
		if(d>max)
		{
			max=d;
			pos=a;
		}	
			
	}
	s.count[pos/32]++;	
	for(a=pos; a<pos+PAGES_PER_BLK; a++)
	{
		if(r[a][0]==1)
			memset(r[a],0,sizeof(u32)*9);
		if(r[a][0]!=0)
		{
			s.gc_write++;
			for(b=bank*32*32; b<pos; b++)
			{
				if(r[b][0]==0)
				{
					memset(r[b],r[a][0],sizeof(u32)*8);
					r[b][8]=r[a][8];
					q[r[b][8]]=b;
					memset(r[a],0,sizeof(u32)*9);
					break;
				}
			}
		}
	}
*/
	//utilization-count method

	for(a=bank*32*32; a<bank*32*32+28*32; a+=PAGES_PER_BLK)
	{
		c=32;
		
		for(b=a; b<a+PAGES_PER_BLK; b++)
		{
			if(r[b][0]==1)
			{
				c--;	
			}
		}
		if(c==0)
		{
			pos=a;
			break;
		}
		if(s.count[a/32]==0)
		{
			pos=a;
			break;
		}
		d=(32000/c)/(s.count[a/32]);
			
		if(d>max)
		{
			max=d;
			pos=a;
		}	
			
	}
	s.count[pos/32]++;	
	for(a=pos; a<pos+PAGES_PER_BLK; a++)
	{
		if(r[a][0]==1)
			memset(r[a],0,sizeof(u32)*9);
		if(r[a][0]!=0)
		{
			s.gc_write++;
			for(b=bank*32*32; b<pos; b++)
			{
				if(r[b][0]==0)
				{
					memset(r[b],r[a][0],sizeof(u32)*8);
					r[b][8]=r[a][8];
					q[r[b][8]]=b;
					memset(r[a],0,sizeof(u32)*9);
					break;
				}
			}
		}
	}

#endif

/***************************************
Add

s.gc_write++;

***************************************/
				
	return;
}

