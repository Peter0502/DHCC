/*************************************************************
* �� �� ��: rptHP7.c
* �������������л�Ʊ����Ҹ�����δ�Ҹ�����
* ��    ��: gujy
* �������: 2007��08��18��
*
* ע����ʽ�ļ�ΪrptHP7.tbl
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
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "hv_poinfo_c.h"

#define	 LINE_CNT 20
static int page = 1,total;
static struct hv_poinfo_c	hv_poinfo;
static struct com_sys_parm_c sComSysParm;
static struct tx_def_c sTxDef;
static	int	rownum = 0;
static	double	sum_amt = 0.0;

void cDeal_str(char *str,int lenth);
int get_ratio_mbfesd(int bli,int bll,char str[100],int prt_code,char mygsm[3]);
rptHP7()
{
	int line, opncnt;
	int hy_line;
	char vpfm[21];
	char fname[10];
	char pre_br_no[6];
	FILE *fp; 
	int    iRecordNum = 0;/***���������ļ�¼������,�������¼����������Ҳʱ���������һҳ�ϴ�ӡ���������**/
	int    iCycNum    = 0;/***ѭ���ߵĴ���**/
	int    zz_ind     = 1;/***�����ظ���ӡ��β����**/
	memset( pre_br_no, 0x00, sizeof( pre_br_no ) );


    g_reply_int = sql_begin(); /*������*/
    if( g_reply_int != 0 )
    {
        sprintf( acErrMsg, "BEGIN WORK error!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    
	strcpy( fname, "rptHP7" );
	g_reply_int = pub_rpt_rmfile_all( fname );
	if ( g_reply_int != 0)
	{
		sprintf(acErrMsg,"ɾ���������ҵ���嵥����!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1" );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
	

	strcpy(vpfm, "rptHP7");

	g_reply_int = Hv_poinfo_Dec_Sel(g_pub_tx.reply, \
		"l_tx_date=%ld  and po_sts='3' ORDER BY br_no  ",sComSysParm.lst_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "DECLARE hv_poinfo  CUR error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	} 

	while(1) 
	{ 
		g_reply_int = Hv_poinfo_Fet_Sel( &hv_poinfo, g_pub_tx.reply );
		if( g_reply_int && g_reply_int != 100 )
		{
			sprintf( acErrMsg,"FETCH hv_poinfo  CUR error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int == 100  )
		{
			vtcp_log("[%s][%d] fetch record finished!ret=[%d]\n",__FILE__,__LINE__,g_reply_int);
			if(iCycNum!=0)   /**ֻ�����м�¼��ʱ��Ż�������Ĵ����д��ļ���****/
			{
					g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_mbfesd );
					if( g_reply_int )
					{
						sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
						WRITEMSG
						goto ErrExit;
					}
			}
			else if(iCycNum==0)
			{
				vtcp_log("[%s][%d]û���ҵ���¼rownum!\n",__FILE__,__LINE__,rownum);
				if(rownum)fclose(fp);
				goto OkExit;
			}
			fclose(fp);
			break;
		}
		iCycNum++;

		/*�����ű�,��һ���ļ�*/
		if( strcmp( hv_poinfo.br_no, pre_br_no ) )
		{
			/*���ǵ�һ������,���ϸ������Ľ�β*/
			if( strlen( pre_br_no ) )
			{
				if(zz_ind!=0)
					{
					  g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "ZZ", &opncnt, get_ratio_mbfesd );
					  if( g_reply_int )
					  {
					  	sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
					  	WRITEMSG
					  	goto ErrExit;
					  }
					}
				fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
				fclose( fp );
			}

			g_reply_int = pub_rpt_openfile( &fp, hv_poinfo.br_no , fname );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT OPEN FILE [%s]error![%d]", fname, g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			
			 fprintf(fp,"%c%c",0x1b,0x40);
                         fprintf(fp,"%c%c%c%c%c\n",0x1b,0x40,0x1b,0x33,0x18);
                         fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
                         
                         
			rownum = 0;
			sum_amt=0.0;
			page = 1;
			zz_ind++;
			if(rownum) fprintf(fp, "\f");
			g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "HH", &opncnt, get_ratio_mbfesd );
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
		iCycNum	= 1;/*ԭֵΪ0������ֵ�һ����¼û�н�β  martin 20081021*/

			strcpy( pre_br_no, hv_poinfo.br_no );
		}else{
			if(0 == rownum %LINE_CNT ){
							if(rownum) fprintf(fp, "\f");
							zz_ind++;
							g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
							if( g_reply_int )
							{
								sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
								WRITEMSG
								goto ErrExit;
							}
			}
		}

		
		vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
		g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "BB", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/*******************delete by xyy 2007-7-28 12:16*******************/
		g_reply_int = pub_rpt_read_pfm_qd_xier( fp, &line, vpfm, "0001", "DD", &opncnt, get_ratio_mbfesd );
		if( g_reply_int )
		{
			sprintf( acErrMsg,"RPT READ PFM BB error![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		
		hy_line++;
			
						rownum++;
						sum_amt+=hv_poinfo.po_bal;

		if(  (rownum+1) % LINE_CNT == 0 )
		{
			vtcp_log("[%s][%d]liuyue==[%d]iRecordNum=[%d]\n",__FILE__,__LINE__,hy_line,iRecordNum);
			g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","ZZ",&opncnt,get_ratio_mbfesd);
			/*fprintf(fp,"\f");*/zz_ind=0;
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM CC error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}

			hy_line = 0;
			page++;

			/*g_reply_int = pub_rpt_read_pfm_qd_xier( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
			if( g_reply_int )
			{
				sprintf( acErrMsg,"RPT READ PFM HH error![%d]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}*/
			iCycNum	= 0;
		}
	} 
	
	vtcp_log("xier[%s][%d]\n",__FILE__,__LINE__);
	
	Hv_zf_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"%d@%s  ���ɴ�������嵥�ɹ�![%s]",__LINE__, __FILE__, g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"���ɴ��δ��������嵥ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_mbfesd( bli,bll,str,rpt_code,mygsm )
int bli,bll;
char str[100];
int  rpt_code;
char mygsm[3];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];

	memset(cTx_Desc,0x00,sizeof(cTx_Desc));
	memset(vstr,	0x00,sizeof(vstr));
	memset(cDate,	0x00,sizeof(cDate));
	memset(fmt,	0x00,sizeof(fmt));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	
	
	vtcp_log("[%s][%d]mygsm===[%s],bli=[%c],bll=[%d]\n",__FILE__,__LINE__,mygsm,(char)bli,bll);
	
	switch( bli ) 
	{
		case 'A':
			sprintf(vstr, "%s",  hv_poinfo.br_no);
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'C':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", hv_poinfo.br_no );
			if( g_reply_int )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", g_reply_int );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s��%c%s��", sComBranch.br_name, ' ',sComBranch.br_no );
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'L': 
			vtcp_log("[%s][%d]��ʼ��ӡ--����!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str,fmt,hv_poinfo.l_tx_date);
			/*strcpy( str, cDate ); */
			break;
		case 'U':
			vtcp_log("[%s][%d]��ʼ��ӡ--��������!\n",__FILE__,__LINE__);
			 break;
		case 'R':
			vtcp_log("[%s][%d]��ʼ��ӡ--�������!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%dd", bll );
			sprintf(str, fmt, rownum+1);
			 break;
		case 'I':
			sprintf( fmt, "%%%dd", bll );
			sprintf(str, fmt, rownum);
			 break;
			break;
		case 'S':
			vtcp_log("[%s][%d]��ʼ��ӡ--�����ʺ�!\n",__FILE__,__LINE__);
			
			memcpy(str,hv_poinfo.pay_ac_no,bll);
			
			break;
		case 'T':
			strncpy(str,hv_poinfo.op_tel,bll);
			vtcp_log("[%s][%d] str===[%s]\n",__FILE__,__LINE__,str);
			break;
		case 'Q': 
			vtcp_log("[%s][%d]��ʼ��ӡ--���׽��!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, hv_poinfo.po_bal );/*�ô����ŵĽ��*/
			strcpy( str, vstr ); 
			break; 
		case 'H': 
			vtcp_log("[%s][%d]��ʼ��ӡ--���׽��!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%%d.2lf", bll ); 
			sprintf( vstr, fmt, sum_amt );/*�ô����ŵĽ��*/
			strcpy( str, vstr ); 
			break; 
		case 'V':
			vtcp_log("[%s][%d]��ʼ��Ʊ����!\n",__FILE__,__LINE__);
			/*memcpy(str,hv_poinfo.ac_br_no,bll);*/
			                                                          
			if( '0' == hv_poinfo.po_ind[0] ){ strncpy(str, "��ת�û�Ʊ", bll);break;}
			if( '1' == hv_poinfo.po_ind[0] ){ strncpy(str, "����ת�û�Ʊ", bll);break;}
			if( '2' == hv_poinfo.po_ind[0] ){ strncpy(str, "�ֽ��Ʊ", bll);break;}
			break;
		case 'M':
		/*	po_sts        varchar2(1)   ��Ʊ��ǰ״̬��־ 0-�Ǽ� 1-ǩ�� 9-ǩ��ȡ�� 2-��Ʊ�ƴ�                                                    
    3-����Ҹ� 4-��Ʊ�Ҹ� 5-ȫ����� 6-�����˻� 7-δ���˻�                                                                          
    8-��Ʊ�⸶ A-�⸶ȡ�� B-���� C-����Ҹ�ȡ�� D-��Ʊ�Ǽ�ȡ��                                                                      
    E-��ʧ F-���ʧ G-����δ���˻� H-δ�ý� I-δ�ó��ʳ���             	*/
			if( '0' == hv_poinfo.po_sts[0] ){ strncpy(str, "�Ǽ�", bll);break;}
			if( '1' == hv_poinfo.po_sts[0] ){ strncpy(str, "ǩ��", bll);break;}
			if( '9' == hv_poinfo.po_sts[0] ){ strncpy(str, "ǩ��ȡ��", bll);break;}
			if( '2' == hv_poinfo.po_sts[0] ){ strncpy(str, "��Ʊ�ƴ�", bll);break;}
			if( '3' == hv_poinfo.po_sts[0] ){ strncpy(str, "����Ҹ�", bll);break;}
			if( '4' == hv_poinfo.po_sts[0] ){ strncpy(str, "��Ʊ�Ҹ�", bll);break;}
			if( '5' == hv_poinfo.po_sts[0] ){ strncpy(str, "ȫ������", bll);break;}
			if( '6' == hv_poinfo.po_sts[0] ){ strncpy(str, "�����˻�", bll);break;}
			if( '7' == hv_poinfo.po_sts[0] ){ strncpy(str, "δ���˻�", bll);break;}
			if( '8' == hv_poinfo.po_sts[0] ){ strncpy(str, "��Ʊ�⸶", bll);break;}
			if( 'A' == hv_poinfo.po_sts[0] ){ strncpy(str, "�⸶ȡ��", bll);break;}
			if( 'B' == hv_poinfo.po_sts[0] ){ strncpy(str, "����", bll);break;}
			if( 'C' == hv_poinfo.po_sts[0] ){ strncpy(str, "���ȡ��", bll);break;}
			if( 'D' == hv_poinfo.po_sts[0] ){ strncpy(str, "�Ǽ�ȡ��", bll);break;}
			if( 'E' == hv_poinfo.po_sts[0] ){ strncpy(str, "��ʧ", bll);break;}
			if( 'F' == hv_poinfo.po_sts[0] ){ strncpy(str, "���ʧ", bll);break;}
			if( 'G' == hv_poinfo.po_sts[0] ){ strncpy(str, "����δ���˻�", bll);break;}
			if( 'H' == hv_poinfo.po_sts[0] ){ strncpy(str, "δ�ý⸶", bll);break;}
			if( 'I' == hv_poinfo.po_sts[0] ){ strncpy(str, "δ�ó���", bll);break;}
			vtcp_log("[%s][%d]��ʼ��ӡ--����״̬!\n",__FILE__,__LINE__);
			/*memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetHvzfStat(hv_poinfo.hv_sts[0],cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);*/
			/*memcpy(str,hv_poinfo.cash_ac_no,bll);
			cDeal_str(str,bll);*/
			break;
		case 'W':
			vtcp_log("[%s][%d]cash_name=[%s]\n",__FILE__,__LINE__,hv_poinfo.cash_name);
			
			vtcp_log("[%s][%d]��ʼ��ӡ--�տ�������!\n",__FILE__,__LINE__);
			/*deal_half_charcter_xier(hv_poinfo.cash_name,vstr,sizeof(hv_poinfo.cash_name)-1);*/
			memcpy(vstr,hv_poinfo.cash_name,sizeof(hv_poinfo.cash_name)-1);
			deal_half_charcter(vstr);
			cDeal_str(vstr, (sizeof(hv_poinfo.cash_name)-1));
			/**ע������������������ĳ��ȴ�����***delete by xyy 2007-7-28 12:14
			if(!strncmp(mygsm,"DD",2)) 
			{
				memcpy(str,vstr+bll,bll);
				
			}
			else
			{
				memcpy(str,vstr,bll);
			}
			**************/
			memcpy(str,vstr,bll);
			break;
		case 'X':
			strncpy(str, hv_poinfo.po_no, bll);	
			break;
		case 'E': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'G': 
			vtcp_log("[%s][%d]��ʼ��ӡ--��!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'D': 
			vtcp_log("[%s][%d]��ʼ��ӡ--ҳ��!\n",__FILE__,__LINE__);
			sprintf( str, "%04ld", page ); 
			break; 
		case 'Z': 
			vtcp_log("[%s][%d]��ʼ��ӡ--�ܼ�ҳ��!\n",__FILE__,__LINE__);
			sprintf( str, "%04ld", page ); 
			break; 
			/**ע������������������ĳ��ȴ�����***delete by xyy 2007-7-28 12:15
		case 'Y': 
			vtcp_log("[%s][%d]��ʼ��ӡ--����������!\n",__FILE__,__LINE__);
			cGet_br_name(hv_poinfo.ac_br_no,vstr);
			deal_half_charcter(vstr);
			vtcp_log("[%s][%d]vstr==[%s]\n",__FILE__,__LINE__,vstr);
			*
			deal_half_charcter_xier(vstr,vstr,sizeof(vstr));*
			cDeal_str(vstr, bll*2);
			
			if(!strncmp(mygsm,"DD",2)) 
			{
				memcpy(str,vstr+bll,bll);
				
			}
			else
			{
				memcpy(str,vstr,bll);
			}
			
			memcpy(str,vstr,bll);
			break; 
			********************/
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]501501501-------[%s][%d],mygsm===[%s]\n",__FILE__,__LINE__,str,bll,mygsm);
	
	
	cDeal_str(str, bll);
	vtcp_log("[%s][%d]ֵ[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}



