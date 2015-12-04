/*************************************************************
* �� �� ��: rpt005.c
* ������������Լ�̻����ʻص�
*
* ��    ��: rob
* �������: 2003��6��27��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "march_mst_c.h"

struct march_mst_c g_sMarch_mst;

int get_ratio_posmhd(int bli,int bll,char str[100]);
rpt005()
{
	int ret = 0;
	int flag=0;
	int opncnt;
	char vpfm[21];
	int     line; 
	char fname[100];
	char filename[100];
	FILE    *fp; 
	
    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
    sprintf( filename,"%s%ld","posmhd",g_pub_tx.tx_date);
    
    /***
    sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"),
			"POSM","posmhd",g_pub_tx.tx_date);
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret); 
		WRITEMSG 
		return ret;
	} 
	***/
	
	strcpy(vpfm, "posmhd");

	ret = March_mst_Dec_Sel(g_pub_tx.reply, "deal_sign='Y' and tx_date=%ld", 
			g_pub_tx.tx_date);
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret = March_mst_Fet_Sel(&g_sMarch_mst, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			break; 
		}

		
		pub_rpt_openfile( &fp,"POSM", filename); 
		
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
				get_ratio_posmhd) ) 
			goto ErrExit; 
		
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
				get_ratio_posmhd) ) 
			goto ErrExit; 
			
		fprintf(fp, "\n\n\n"); 
	} 
	
	March_mst_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɿ�Ŀ�սᵥ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"���ɿ�Ŀ�սᵥʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_posmhd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 
	
	switch( bli ) 
	{ 
		case 'A': 
			strcpy(str, g_sMarch_mst.ac_no); 
			break; 
		case 'B': 
			strcpy(str, "��Լ�̻��ʽ�����ת��"); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, g_sMarch_mst.tx_amt);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, g_sMarch_mst.march_name); 
			break; 
		case 'E': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, pub_rpt_pcask(g_sMarch_mst.tx_amt)); 
			break; 
		case 'X': 
			sprintf( str, "%04ld", g_pub_tx.tx_date/10000 ); 
			break; 
		case 'W': 
			sprintf( str, "%02ld", g_pub_tx.tx_date/100%100 ); 
			break; 
		case 'V': 
			sprintf( str, "%02ld", g_pub_tx.tx_date%100 ); 
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
