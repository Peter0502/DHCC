/***** ������֧��ҵ���շ��嵥��ϸ��ѯ(sp5348)��������**********/
/*****************************************************
* �� �� ��:  sp5893.c
* ���������� ֧��ҵ���շ��嵥��ϸ��ѯ
* ��    ��:  wangyh
* ������ڣ� 2006��9��1�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_zf_c.h"
#include "hv_chargemx_c.h"
int sp5893()
{
	int ret=0,i;
	char in_date[9];
	char tx_date[9];
	char wt_date[9];
	char br_no[8];
	char dtlid[4];
	char orderno[4];
	char cTdtlid[4];
	char cPrtname[5];
	char cPrtname1[5];
	char cFilename[100];
	struct  hv_zf_c       f_hv_zf;
	struct  hv_chargemx_c       f_hv_chargemx;
	HV_FD123_AREA fd123;
	FILE *fp;      /*�ļ���*/
/*	memset(tx_date,0,sizeof(tx_date)); */
	memset(in_date,0,sizeof(in_date));
	memset(in_date,0,sizeof(tx_date));
 	memset(wt_date,0,sizeof(wt_date)); 
	memset(br_no,0,sizeof(br_no)); 
	memset(dtlid,0,sizeof(dtlid)); 
	memset(orderno,0,sizeof(orderno));
	memset(cTdtlid,0,sizeof(cTdtlid)); 
	memset(&f_hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(&f_hv_chargemx,0x00,sizeof(struct hv_chargemx_c));
	memset(&fd123, ' ',sizeof(fd123));
	memset(cPrtname,0,sizeof(cPrtname));
	memset(cPrtname1,0,sizeof(cPrtname1));
	vtcp_log("sp5893 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_data("0240",cTdtlid);
	memcpy(orderno,cTdtlid,sizeof(orderno)-1);	
	vtcp_log("[%s][%d]orderno=[%.3s]",__FILE__,__LINE__,orderno);		
	vtcp_log("[%s][%d]cTdtlid=[%.3s]",__FILE__,__LINE__,cTdtlid);		
	get_zd_data("0440",tx_date);
	get_zd_data("0450",wt_date);
	get_zd_data("0880",br_no);
	
	vtcp_log("[%s][%d]tx_date=[%.8s]",__FILE__,__LINE__,tx_date);
	vtcp_log("[%s][%d]wt_date=[%.8s]",__FILE__,__LINE__,wt_date);
	vtcp_log("[%s][%d]br_no=[%.7s]",__FILE__,__LINE__,br_no);
	vtcp_log("[%s][%d]cTdtlid=[%.3s]",__FILE__,__LINE__,cTdtlid);		

	vtcp_log("sp5893 ��ʼ��[%s][%d]",__FILE__,__LINE__);

	ret=Hv_zf_Sel(g_pub_tx.reply,&f_hv_zf,"tx_date='%s'and wt_date='%s' and br_no='%s'",tx_date,wt_date,br_no);
	vtcp_log("sp5893 ��ʼ��[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴�֧��ҵ���շ��嵥��ϸ");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ֧��ҵ���շ��嵥��ϸ������Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);

	strcpy(f_hv_chargemx.br_no, f_hv_zf.br_no);
	f_hv_chargemx.in_date = f_hv_zf.tx_date;
	vtcp_log("��ѯ�ɹ�[%d] [%s][%d]",f_hv_zf.tx_date,__FILE__,__LINE__);
	vtcp_log("��ѯ�ɹ�[%d] [%s][%d]",f_hv_chargemx.in_date,__FILE__,__LINE__);
	vtcp_log("��ѯ�ɹ�[%d] [%s][%d]",f_hv_chargemx.dtlid,__FILE__,__LINE__);
	ret=Hv_chargemx_Dec_Sel(g_pub_tx.reply,"in_date=%d",f_hv_chargemx.in_date);
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
	
	ERR_DEAL
	i=0;
	while(1)
	{ 
	  	memset(f_hv_chargemx.chgtype,0,sizeof(f_hv_chargemx.chgtype));
	  	ret=Hv_chargemx_Fet_Sel (&f_hv_chargemx,g_pub_tx.reply);
	  	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
	 	if( !i )
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
			fprintf( fp,"~CMT���|�Ʒѽ��|�Ʒѱ���|��������|�Ʒѵ���|�շ���־|��ӡ��־|\n" );
		  }
		  if(f_hv_chargemx.chgtype[0]='0')
		      {
			        memcpy(cPrtname,"����", sizeof(cPrtname)-1);
	        }
	    else
	        {
	            memcpy(cPrtname,"����", sizeof(cPrtname)-1);
	        }
		/*			if(f_hv_chargemx.aorm[0]='0')
					{
						memcpy(cPrtname1,"�Զ�", sizeof(cPrtname1)-1);
	        }
	        else
	        {
	                memcpy(cPrtname1,"�ֹ�", sizeof(cPrtname1)-1);
	        } 
	        vtcp_log("��ѯ�ɹ���[%s]  [%s][%d]",f_hv_chargemx.cmtno,__FILE__,__LINE__);
    */
		fprintf( fp,"%s|%s|%s|%s|%s|%s|%s|\n",f_hv_chargemx.cmtno,f_hv_chargemx.txamt,f_hv_chargemx.mnum,f_hv_chargemx.brate,f_hv_chargemx.price,cPrtname,f_hv_chargemx.prtflag);
		i++;
        }
	ret=Hv_zf_Clo_Sel();
	ERR_DEAL
	if( !i )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


