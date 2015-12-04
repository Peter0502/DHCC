#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int spT003()
{
	int	ret;
	char filename[100];
	FILE *fp;
	int i=0;

	pub_base_sysinit();
/*
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	i=10;
	fprintf(fp,"%d|20030201|ժҪaaa|1000.00|2100.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030202|ժҪaab|2000.00|2200.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030203|ժҪaac|3000.00|2300.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030204|ժҪaad|4000.00|2400.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030205|ժҪaae|5000.00|2500.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030206|ժҪaaf|6000.00|2600.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030207|ժҪaag|7000.00|2700.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030208|ժҪaah|8000.00|2800.00|9999|8888|\n",i);
	i++;
	fprintf(fp,"%d|20030209|ժҪaai|9000.00|2900.00|9999|8888|\n",i);
	
	fclose( fp);

	set_zd_data(DC_FILE_SND,"1");
*/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}


