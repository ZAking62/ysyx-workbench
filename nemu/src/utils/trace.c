#include <common.h>

#define MAX_IRINGBUF 16
char* iringbuf[MAX_IRINGBUF];
int iringbuf_cur = 0;

void trace_inst(char* p){
	strcpy(iringbuf[iringbuf_cur], p);
	iringbuf_cur = (iringbuf_cur + 1) % MAX_IRINGBUF;
}

