/*************************************************
* 文 件 名:    spTCDZ.c
* 功能描述：   同城清算调用该接口，查询chnl_single_acct表记录，写入文件，供同城平台与核心对账使用。
*
* 作    者:    zgf
* 完成日期：   2014年07月06日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
* insert into tx_def values('6230','同城银联对账','10000000000000000000000011100100000000010000010000000000000001000100000000000000110000000000000000000000000000000000000000000000','0','5','0');
* insert into tx_flow_def values('6230',0,'6230','#$');
* insert into tx_sub_def values('6230','同城银联对账','spTCDZ','0','1');
*************************************************/
#include "string.h"
#include <errno.h>
#define EXTERN
#include "public.h"
#include "chnl_single_acct_c.h"
spTCDZ()
{ 		
	int	ret = 0;
	FILE 	*fp = NULL;
	char 	filename[128];
	
	/*add 20140926*/
	char    string[128];
	/**/
	
	long txdate;
	struct chnl_single_acct_c	sDdMstHst;
	
	/*add 20140926*/
	struct chnl_single_acct_c	sDdMstHst1;
	
	memset(string, 0x0, sizeof(string));
	/*add 20140926*/
	
	
	memset(filename, 0x0, sizeof(filename));
	memset(&sDdMstHst, 0x0, sizeof(sDdMstHst));
	memset(&sDdMstHst1, 0x0, sizeof(sDdMstHst1));
	
	get_zd_long("0440",&txdate);	
		/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	ret = Chnl_single_acct_Dec_Sel( g_pub_tx.reply, "pt_date= %ld and brf = '招财晋宝' " , txdate);
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Chnl_single_acct_Fet_Sel( &sDdMstHst, g_pub_tx.reply );
		if( ret==100 ) 
			break;
		if( ret ) 
			goto ErrExit;

		fprintf(fp,"%8ld|%s|\n",sDdMstHst.pt_date,sDdMstHst.pt_trace_no);
	}

	Chnl_single_acct_Clo_Sel();
	fclose(fp);
	
	
	set_zd_data( DC_FILE_SND,"2" );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

