/*************************************************
* �� �� ��:  spJ123.c
* ���������� ���ʽ��ײ�ѯ
* ��    ��:  jane
* ������ڣ� 2004��6��25��
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

spJ123()
{
        int ret=0,i;
	struct hv_zf_c  hv_zf;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*������*/
	char cTx_date[9];/*��������*/
	char cPrtflag[2];/*��ӡ��־*/
	char cTxstat[2];/*����״̬*/
	char cCmtno[4];/*��Ʊ����*/
	char cBreakpoint[9];/*�ϵ�*/
	char cFilename[100];/*�ļ���*/
	int ttlnum=0;	    /**��ȡ��������**/
	FILE *fp;
	char wherelist[1024];  /**��ѯ����**/
	memset(cBr_no     ,0, sizeof(cBr_no ));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cPrtflag   ,0, sizeof(cPrtflag));
	memset(cTxstat    ,0, sizeof(cTxstat ));
	memset(cCmtno     ,0, sizeof(cCmtno  ));
	memset(cBreakpoint,0, sizeof(cBreakpoint));
	memset(cFilename, 0 , sizeof(cFilename));
	
	memset(&fd123, ' ',sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	
	vtcp_log("spJ123 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	/**��ʼ����**/
	pub_base_sysinit();
	/*get_zd_data("0030",cBr_no);
	get_zd_data("0440",cTx_date);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0690",cTxstat);
	get_zd_data("0230",cCmtno);*/
	MEMCPY_DEBUG(cBr_no,"51801",sizeof(cBr_no)-1);
	MEMCPY_DEBUG(cTx_date,"20080621",sizeof(cTx_date)-1);
	MEMCPY_DEBUG(cPrtflag,"2",sizeof(cPrtflag)-1);
	MEMCPY_DEBUG(cTxstat,"W",sizeof(cTxstat)-1);
	MEMCPY_DEBUG(cCmtno,"100",sizeof(cCmtno)-1);
	/**��ɲ�ѯ����**/
	if(cTxstat[0]=='W')/*ȫ������״̬*/
	{
		if(cPrtflag[0]=='2')/*ȫ��*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date='%s'",cCmtno,cBr_no,cTx_date);
		}
		else/*δ��ӡ���ߴ�ӡ*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date='%s' and hv_prt_ind='%s'",cCmtno,cBr_no,cTx_date,cPrtflag);
		}		
	}
	else
	{
		if(cPrtflag[0]=='2')/*ȫ��*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s'",cCmtno,cBr_no,cTx_date,cTxstat);
		}
		else/*δ��ӡ���ߴ�ӡ*/
		{
			sprintf(wherelist,"cmtno='%s'and br_no='%s' and tx_date=%s and hv_sts='%s' and hv_prt_ind='%s'",cCmtno,cBr_no,cTx_date,cTxstat,cPrtflag);
		}
	
	}
	/*��ʼ��ѯ*/
        ret = Hv_zf_Dec_Sel(g_pub_tx.reply, wherelist);
        ERR_DEAL
        ttlnum=0;
        while(1)
        {
        	ret=Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
        	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			/*fprintf( fp,"~������|��������|˳���|���׽��|������(��)|������(��)|״   ̬|���ı��|��ӡ��־|\n" );*/
		}
		fprintf( fp,"%s|%s|%s|%f|%s|%s|%s|%s|%s|\n",hv_zf.br_no,hv_zf.tx_date,hv_zf.orderno,hv_zf.tx_amt,hv_zf.or_br_no,hv_zf.ac_br_no,hv_zf.hv_sts,hv_zf.cmtno,hv_zf.hv_prt_ind);
		ttlnum++;
        }
	ret=Hv_zf_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}

