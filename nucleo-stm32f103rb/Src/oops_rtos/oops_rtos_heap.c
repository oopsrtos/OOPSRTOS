#include "string.h"

#define HEAP_QLY 16

struct general_buf
{
	unsigned int block_qly;					//剩余缓存块
	unsigned char dirty[100];					//缓存空间脏标志
	unsigned char buffer[HEAP_QLY*100];		//缓存空间
}Heap_Buf;

/**
 * @brief
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_heap_buffer_init(void)
{
	Heap_Buf.block_qly = sizeof(Heap_Buf.dirty);
	memset(Heap_Buf.buffer,0,sizeof(Heap_Buf.buffer));
	memset(Heap_Buf.dirty,0,sizeof(Heap_Buf.dirty));
}

/**
 * @brief
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void* oops_rtos_malloc(unsigned int size)
{
	unsigned int size_need=size/HEAP_QLY+(size%16?1:0);/*获得块需求*/
	unsigned int i,j;
	unsigned int free_num=0;
	unsigned char* address=NULL;
	unsigned char enought_space=0;

	if(size_need>Heap_Buf.block_qly)
		return NULL;

	for(i=0;i<Heap_Buf.block_qly;i++){
		if(!Heap_Buf.dirty[i]){
			free_num++;
			if(free_num>=size_need){
				enought_space=1;
				break;
			}
		}else
			free_num=0;
	}

	if(enought_space){
		Heap_Buf.block_qly -= free_num;
		address = &Heap_Buf.buffer[(i-(free_num-1))*HEAP_QLY];
		for(j=0;j<free_num;j++)
			Heap_Buf.dirty[(i-(free_num-1))+j] = j+1;
	}

	return address;
}

/**
 * @brief
 * @param  none
 * @author ma57457@163.com
 * @date 2019-11-25
 */
void oops_rtos_free(void* address)
{
	unsigned int offset=((unsigned char*)address-Heap_Buf.buffer)/HEAP_QLY;
	unsigned char dirtyval=0;

	if(Heap_Buf.dirty[offset] != 1){
		return;
	}else{
		do{
			Heap_Buf.dirty[offset] = 0;
			Heap_Buf.block_qly++;
			offset ++;
			dirtyval = Heap_Buf.dirty[offset];
		}while( dirtyval > 1 && offset < sizeof(Heap_Buf.dirty) );	
	}
}

