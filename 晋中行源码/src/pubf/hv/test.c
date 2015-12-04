#include <stdio.h>
#include <stdlib.h>
#include "hvpack.h"

void main()
{
	int i;
	char *body="{1:000156  000000000003111111111111000000032001071009350431120010710}{3::30A:20010710:C47:001:051:20010710:CC1:111111111111:CE2:0:046:00000001:72A:³·Ïú±¨ÎÄ}";
	THVPACKAGE hv;
	MEMSET_DEBUG(hv,'\0',sizeof(hv));
	MEMCPY_DEBUG(hv.body,body,strlen(body));
	for(i=0;i<sizeof(hv),i++)
	{
		printf("%c",(char)hv[i]);
	}
	printf("\0");
	return;
}

