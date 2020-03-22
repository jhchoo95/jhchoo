//----------------------------------------------------------
//
// Project #1 : NAND Simulator
// 	- Embedded Systems Design, ICE3028 (Fall, 2019)
//
// Sep. 19, 2019.
//
// TA: Junho Lee, Somm Kim
// Prof: Dongkun Shin
// Embedded Software Laboratory
// Sungkyunkwan University
// http://nyx.skku.ac.kr
//
//---------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "nand.h"

u32 data[SECTORS_PER_PAGE];
u32 spare;
u32 read_data[SECTORS_PER_PAGE];
u32 read_spare;

#define N_BLOCKS		(BLKS_PER_BANK)
#define N_PAGES			(PAGES_PER_BLK)

#define SPARE(i,j,k)	(i * N_BANKS + j * N_BLOCKS + k * N_PAGES)
u32 *DATA(u32 i, u32 j, u32 k) {
	u32 idx = i * N_BANKS + j * N_BLOCKS + k * N_PAGES;	
	if (idx % 0xF == 0x0) memset(data, 0x00, DATA_SIZE); 
	else if (idx % 0xF == 0x1) memset(data, 0x11, DATA_SIZE); 
	else if (idx % 0xF == 0x2) memset(data, 0x22, DATA_SIZE); 
	else if (idx % 0xF == 0x3) memset(data, 0x33, DATA_SIZE); 
	else if (idx % 0xF == 0x4) memset(data, 0x44, DATA_SIZE); 
 	else if (idx % 0xF == 0x5) memset(data, 0x55, DATA_SIZE); 
	else if (idx % 0xF == 0x6) memset(data, 0x66, DATA_SIZE); 
	else if (idx % 0xF == 0x7) memset(data, 0x77, DATA_SIZE); 
	else if (idx % 0xF == 0x8) memset(data, 0x88, DATA_SIZE); 
	else if (idx % 0xF == 0x9) memset(data, 0x99, DATA_SIZE); 
	else if (idx % 0xF == 0xa) memset(data, 0xaa, DATA_SIZE); 
	else if (idx % 0xF == 0xb) memset(data, 0xbb, DATA_SIZE); 
	else if (idx % 0xF == 0xc) memset(data, 0xcc, DATA_SIZE); 
	else if (idx % 0xF == 0xd) memset(data, 0xdd, DATA_SIZE); 
	else if (idx % 0xF == 0xe) memset(data, 0xee, DATA_SIZE); 
	else memset(data, 0xff, DATA_SIZE); 

	return data;
}

u32 arr[SPARE(N_BANKS, N_BLOCKS, N_PAGES)][9]; //데이터를 저장할 곳
u32 arr_data1[SECTORS_PER_PAGE]; //arr에서 데이터를 읽는 행렬
u32 arr_data2[SECTORS_PER_PAGE]; //arr에서 데이터를 읽는 행렬

int nand_init(int nbanks, int nblks, int npages)
{
	int i, j, k;

	u32 idx = i * N_BANKS + j * N_BLOCKS + k * N_PAGES;
	for (i = 0; i < nbanks; i++)
	{
		for (j = 0; j < nblks; j++)
		{
			for (k = 0; k < npages; k++)
			{
				memset(arr[idx], 1, 9); //arr[idx]의 값을 1로 초기화
			}
		}
	}
	printf("init : %d banks, %d blocks, %d pages per block\n", nbanks, nblks, npages);
	return 0;
}


int nand_write(int bank, int blk, int page, u32 *data, u32 spare)
{
	int k;
	for(k=0; k<8; k++)
	{
		arr_data1[k]=arr[SPARE(bank, blk, page)][k]; //arr[SPARE(bank, blk, page)]값 복사
	}

	for(k=0; k<8; k++)
	{
		arr_data2[k]=arr[SPARE(bank, blk, page-1)][k]; //arr[SPARE(bank, blk, page-1)]값 복사
	}


	if (arr_data1!=data && arr_data2 == DATA(bank, blk, page-1))//arr_data1이 초기화 되어있고 그 전 페이지가 채워져있을때
	{
		printf("write(%d,%d,%d): data = 0x%x, spare = 0x%x\n", bank, blk, page, data[0], spare);
		arr[SPARE(bank, blk, page)][8] = spare;//spare값 입력
		int i;
		for (i = 0; i < 8; i++)
		{
			arr[SPARE(bank, blk, page)][i] = data[i];//arr에 data값 입력
		}
		return 0;
	}

	else
	{
		if (arr_data1 == data)//data값이 이미 입력되어있을때
		{
			printf("failed. the page was already written\n");
		}

		else//이전 페이지가 비어있을때
		{
			printf("failed. the page is not being sequentially written\n");
		}
		return -1;
	}
}

int nand_read(int bank, int blk, int page, u32 *data, u32 *spare)
{
	if (bank < 0) //bank가 음수일때
	{
		printf("failed. invalid bank number\n");
		return -1;
	}
	else if (blk < 0) //blk가 음수일때
	{
		printf("failed. invalid blk number\n");
		return -1;
	}
	else if(page<0) //page가 음수일때
	{
		printf("failed. invalid page number\n");
		return -1;
	}
	else
	{
		*spare = arr[SPARE(bank, blk, page)][8];
		int i;
		for (i = 0; i < 8; i++)
		{
			data[i] = arr[SPARE(bank, blk, page)][i];//arr[SPARE(bank, blk, page)]값 읽기
		}
		
		if (data[0] == 1)//입력값이 없을때
		{
			printf("failed. trying to read an empty page\n");
			return -1;
		}
		else//입력값이 있을 때 입력값 출력
		{
			printf("read(%d,%d,%d): data = ox%x, spare = 0x%x\n", bank, blk, page, data[0], *spare);
			return 0;
		}
		
	}
}

int nand_erase(int bank, int blk)
{
	if (blk < 0 || blk >= 8)//blk가 음수이거나 blk가 8이상일때
	{
		printf("failed, invalid block number\n");
		return -1;
	}
	int non_empty=0; //비어있지 않는 페이지 count하는 변수
	
	int k;
	for(k=0; k<N_PAGES; k++)
	{
		if(arr[SPARE(bank, blk, k)][0]!=1) //비어있지 않는 페이지 세기
			non_empty++;
	}

	if (non_empty==0)//만약 블록 전체가 비어있다면
	{
		printf("failed. trying to erase empty block\n");
		return -1;
	}

	else
	{
		int m;
		int i;
		for (m = 0; m < N_PAGES; m++)
		{
			for (i = 0; i < 9; i++)
			{
				arr[SPARE(bank, blk, m)][i] = 1; //블록전체 초기화
			}
		}
		
		printf("block erased\n");
		return 0;
	}
}

int nand_blkdump(int bank, int blk)
{
	int count = 0;//입력되어있는 페이지 갯수세는 변수
	int i;
	for (i = 0; i < N_PAGES; i++)
	{
		if (arr[SPARE(bank, blk, i)][0] != 1)
		{
			count++;//입력되어 있는 페이지 세기
		}
	}

	printf("blkdump(%d, %d) : total %d pages are written\n", bank, blk, count);
	int j;
	for (j = 0; j < N_PAGES; j++)
	{
		if (arr[SPARE(bank, blk, j)][0] != 1)
		{
			printf("blkdump(%d, %d, %d) : data = 0x%x, spare = 0x%x\n", bank, blk, j, arr[SPARE(bank, blk, j)][0], arr[SPARE(bank, blk, j)][8]);//입력되어있는 페이지 출력
		}
	}
	return 0;
}


int main(int argc, char *argv[])
{
	int i, j, k;

	nand_init(N_BANKS, N_BLOCKS, N_PAGES);

	for (i = 0; i < N_BANKS; i++)
	{
		// write a block in full
		for (j = 0; j < N_PAGES; j++)
			nand_write(i, 3, j, DATA(i, 3, j), SPARE(i, 3, j));

		// read the block in the reverse order
		for (j = N_PAGES - 1; j >= 0; j--)
		{
//			nand_read(i, 3, j, DATA(0, 0, 0), &spare);
			nand_read(i, 3, j, read_data, &read_spare);
			for (k = 0; k < 8; k++) {
				if (DATA(i, 3, j)[k] != read_data[k]) {
					printf("0x%x 0x%x", DATA(i, 3, j)[k], read_data[k]);
					assert(0);
				}
			}
			if (SPARE(i, 3, j) != read_spare) {
				printf("spare: 0x%x 0x%x", SPARE(i, 3, j), read_spare);
				assert(0);
			}
		}
	}

	nand_write(0, 4, 0, DATA(0, 4, 0), SPARE(0, 4, 0));

	// trying to overwrite
	nand_write(0, 4, 0, DATA(0, 4, 0), SPARE(0, 4, 0));

	// trying to write non-sequentially
	nand_write(0, 4, 2, DATA(0, 4, 2), SPARE(0, 4, 2));

	// trying to read an empty page
	nand_read(0, 4, 3, DATA(0, 0, 0), &spare);
	nand_read(0, 0, 0, DATA(0, 0, 0), &spare);
	nand_read(0, N_BLOCKS - 1, 0, DATA(0, 0, 0), &spare);

	// erase a block
	for (i = 0; i < N_BANKS; i++)
		nand_erase(i, 3);
	nand_erase(0, 4);

	// trying to erase an empty block
	for (i = 0; i < N_BANKS; i++)
		nand_erase(i, 0);

	// trying to write the erased block
	for (i = 0; i < N_BANKS; i++)
	{
		for (j = 0; j < N_PAGES; j++)
			nand_write(i, 3, j, DATA(i, 3, j), SPARE(i, 3, j));
	}

	nand_write(0, 4, 0, DATA(0, 4, 0), SPARE(0, 4, 0));
	nand_write(0, 4, 1, DATA(0, 4, 1), SPARE(0, 4, 1));
	nand_read(0, 3, 0, DATA(0, 0, 0), &spare);
	nand_read(0, 4, 0, DATA(0, 0, 0), &spare);

	// invalid page number
	nand_write(0, 4, -1, DATA(0, 4, -1), SPARE(0, 4, -1));
	nand_read(0, 4, -1, DATA(0, 0, 0), &spare);
	nand_write(0, 4, N_PAGES, DATA(0, 4, N_PAGES), SPARE(0, 4, N_PAGES));
	nand_read(0, 4, N_PAGES, DATA(0, 0, 0), &spare);

	// invalid block number
	nand_write(0, -1, 0, DATA(0, -1, 0), SPARE(0, -1, 0));
	nand_read(0, -1, 0, DATA(0, 0, 0), &spare);
	nand_write(0, N_BLOCKS, 0, DATA(0, N_BLOCKS, 0), SPARE(0, N_BLOCKS, 0));
	nand_read(0, N_BLOCKS, 0, DATA(0, 0, 0), &spare);
	nand_erase(0, -1);
	nand_erase(0, N_BLOCKS);
	nand_erase(0, N_BLOCKS + 1000000);

	for (i = 0; i < N_BANKS; i++)
	{
		for (j = 0; j < N_BLOCKS; j++)
			nand_blkdump(i, j);
	}

	return 0;
}

