/*********************************************************************
* �� �� �� :  
* �������� :  �⺣��
* ��    �� :  2015/11/19
* ����ģ�� :  �ֻ���Ʋ�Ʒ�б��ѯ
* �������� :  spMK07.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('5052','�ֻ���Ʋ�Ʒ�б��ѯ','10000000000000000000110011111111100000111111111111001000000001000111111100000000000110000000100000011000000000000000000000000000', '0', '4', '0');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('5052', 0, 'MK07', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('MK07', '�ֻ�����б��ѯ', 'spMK07', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "fn_parm_c.h"			/*��Ʋ�Ʒ������*/
#include "fn_reg_c.h"			/*��Ʋ�Ʒ�Ǽǲ�*/


int spMK07()
{
    
	struct fn_parm_c sFn_parm;   /*��Ʋ�Ʒ������*/
	
	char proName[62];	/*��Ʒ�����ֶ�*/
	char purchaMark[6];	/*�ܷ����־*/
	char turnPageBeginPos[10];	/*��ѯ��ʼ��¼��*/
	char turnPageShowNum[10];	/*�����¼��*/
	char proCode[26];		/*��Ʒ����*/
	char sortAttr[4];		/*�����ֶ�0:����1:������2:���յȼ�*/
	char flag[6];		/*�����־0:����1:����*/
	char cstType[6];	/*�ͻ�����*/

	char tx_date[8+1];		/*ƽ̨��������*/
	char sw_traceno[10+1];		/*ƽ̨��ˮ*/
	char filename[60];
	char pcfilename[256];
	char cFtpfile[256];
	char wherelist[1024];
	char wherebuf[256];
	FILE *fp = NULL;
	int iRt;
	int cnt=1,proSum=0;
	double dTx_amt=0.00;	/*����ʣ���*/
	double sale_amt=0.00;	/*���۽��*/
	char d_date[10];
	long l_date=0;
	int count=0;			/*��Ʒ����*/
	double proProgress=0.00; /*��Ʒ����*/
	
	memset(proName,0x00,sizeof(proName));
	memset(purchaMark,0x00,sizeof(purchaMark));
	memset(proCode,0x00,sizeof(proCode));
	memset(turnPageBeginPos,0x00,sizeof(turnPageBeginPos));
	memset(turnPageShowNum,0x00,sizeof(turnPageShowNum));
	memset(sortAttr,0x00,sizeof(sortAttr));
	memset(flag,0x00,sizeof(flag));
	
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(tx_date, 0x00, sizeof(tx_date));
	memset(sw_traceno, 0x00, sizeof(sw_traceno));
	memset(d_date, 0x00, sizeof(d_date));
	memset(wherelist, 0x00, sizeof(wherelist));
	memset(wherebuf, 0x00, sizeof(wherebuf));
	
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	
	get_zd_data("0700",sortAttr);
	zip_space(sortAttr);
	get_zd_data("0230",purchaMark);
	zip_space(purchaMark);
	get_zd_data("0240",flag);
	zip_space(flag);
	get_zd_data("0260",proName);
	zip_space(proName);
	get_zd_data("0280",turnPageBeginPos);
	zip_space(turnPageBeginPos);
	get_zd_data("0290",turnPageShowNum);
	zip_space(turnPageShowNum);
	get_zd_data("0320",proCode);
	zip_space(proCode);
	get_zd_data("0660",cstType);
	zip_space(cstType);
	
	vtcp_log("%s %d ::cstType[%s]sortAttr[%s]proName[%s]purchaMark[%s]flag[%s]proCode[%s]turnPageBeginPos[%s]turnPageShowNum[%s]",\
		__FILE__,__LINE__,cstType,sortAttr,proName,purchaMark,flag,proCode,turnPageBeginPos,turnPageShowNum);
	
	if(!strlen(cstType))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","�ͻ����Ͳ���Ϊ��");
		vtcp_log("%s %d ::�ͻ����Ͳ���Ϊ��[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		goto ErrExit;
	}
	
	if(0 == strcmp(cstType,"0"))
	{
		strcpy(cstType,"1");	
	}
	else
	{
		strcpy(cstType,"2");
	}
	
	/*��ѯ��ʼ��¼��*/
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","��ѯ��ʼֵ����");
		vtcp_log("%s %d ::[%s]��ѯ��ʼֵ����turnPageBeginPos=%ld",__FILE__,__LINE__,g_pub_tx.reply,turnPageBeginPos);
		goto ErrExit;
	}
	vtcp_log("%s %d ::��ѯ��ʼ��¼��turnPageBeginPos=%s",__FILE__,__LINE__,turnPageBeginPos);

	/*�����¼��*/	
	if(!strlen(turnPageShowNum))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","�����¼������");
		vtcp_log("%s %d ::[%s]�����¼������turnPageShowNum=%s",__FILE__,__LINE__,g_pub_tx.reply,turnPageShowNum);
		goto ErrExit;
	}
	
	vtcp_log("%s %d ::�����¼��turnPageShowNum=%s",__FILE__,__LINE__,turnPageShowNum);
	
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	
	vtcp_log("%s %d ::ƽ̨����tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::ƽ̨��ˮsw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	
	/* �����ļ� */
	sprintf( filename,"5052mobbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
	fp=fopen(pcfilename,"w");
	if( fp==NULL )
	{
	    vtcp_log("%s %d::open file [%s] error ",__FILE__,__LINE__,pcfilename);
	    strcpy(g_pub_tx.reply,"P157");
	    set_zd_data("0130","���ļ���");
	    goto ErrExit;
	}
	/*�ļ�ͷ*/
	fprintf(fp, "|@��Ʒ����|@��Ʒ����|@����|@���յȼ�|@Ԥ��������|@��Ʒ��Ϣ��|@��Ʒ������|@�𹺽��|@��Ʒ����|@״̬|@ļ����ʼ��|@ļ��������|\n");
	
	/*��ѯ����*/
	if(strlen(proCode) >0)
	{
		/*��Ʒ����*/
		sprintf(wherelist,"prdt_code='%s'",proCode);
		vtcp_log("%s %d ::prdt_code=%s",__FILE__,__LINE__,proCode);
	}
	else
	{
		if(strlen(proName) > 0)
		{
			/*��Ʒ�ֶ���*/
			sprintf(wherelist,"title like '%%%%%s%%%%'",proName);	
		}
		else
		{
			if(strlen(cstType) > 0)
			{
				/*�ͻ�����*/
				sprintf(wherelist,"(cif_type='0' or cif_type='%s')",cstType);	
			}
		}
		vtcp_log("%s %d ::proName=[%s]cstType=[%s]",__FILE__,__LINE__,proName,cstType);
		
		if(strlen(purchaMark) > 0)
		{
			/*�ܷ����־*/
			memset(wherebuf, 0x00, sizeof(wherebuf));
			vtcp_log("%s %d ::purchaMark=[%s]",__FILE__,__LINE__,purchaMark);
			
			/*�����־: 0 �ɹ���  1 ���ɹ���*/
			if(!memcmp(purchaMark,"0",1))
			{
				sprintf(wherebuf," and stat='0' and beg_date<='%s' and end_date>='%s'",tx_date,tx_date);
					
			}	
			else
			{
				sprintf(wherebuf," and stat <> '0'");	
			}
			strcat(wherelist,wherebuf);
		}
		
		/*ȡ��ǰ����ǰ90������*/
		l_date=pub_base_daynumSUB(g_pub_tx.tx_date,90);
		memset(wherebuf, 0x00, sizeof(wherebuf));
		sprintf(wherebuf," and beg_date>='%d'",l_date);
		strcat(wherelist,wherebuf);
		
		if(strlen(flag) > 0)
		{
			vtcp_log("%s %d ::flag=[%s]",__FILE__,__LINE__,flag);
			vtcp_log("%s %d ::sortAttr=[%s]",__FILE__,__LINE__,sortAttr);
			
			/*�����־  0:����1:����*/
			if(memcmp(flag,"0",1) == 0)
			{
				if(strlen(sortAttr) > 0)
				{
					vtcp_log("%s %d ::sortAttr=[%s]",__FILE__,__LINE__,sortAttr);
					
					/*�����ֶ�*/
					if(memcmp(sortAttr,"0",1) == 0)
					{
						/*����*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by term asc");
						strcat(wherelist,wherebuf);	
					}	
					else if(memcmp(sortAttr,"1",1) == 0)
					{
						/*������*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						/*sprintf(wherebuf," order by fct_rate asc"); by guanjianfeng 20151202*/
						sprintf(wherebuf," order by rate desc");
						strcat(wherelist,wherebuf);	
					}
					else
					{
						/*���յȼ�*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by risk_lvl asc");
						strcat(wherelist,wherebuf);	
					}
				}
			}
			else
			{
				if(strlen(sortAttr) > 0)
				{
					vtcp_log("%s %d ::sortAttr=[%s]",__FILE__,__LINE__,sortAttr);
					
					/*�����ֶ�*/
					if(memcmp(sortAttr,"0",1) == 0)
					{
						/*����*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by term desc");
						strcat(wherelist,wherebuf);	
					}	
					else if(memcmp(sortAttr,"1",1) == 0)
					{
						/*������*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						/*sprintf(wherebuf," order by fct_rate desc"); by guanjianfeng 20151202*/
						sprintf(wherebuf," order by rate asc");
						strcat(wherelist,wherebuf);	
					}
					else
					{
						/*���յȼ�*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by risk_lvl desc");
						strcat(wherelist,wherebuf);	
					}
				}
			}
		}
	}
	
	vtcp_log("%s %d::��ѯ����wherelist[%s]",__FILE__,__LINE__,wherelist);
	
	/*��Ʒ����*/
	count=sql_count("fn_parm",wherelist);
	if(count < 0)
	{
		vtcp_log("%s %d::sql_count [%d] error ",__FILE__,__LINE__,count);
	 	strcpy(g_pub_tx.reply,"D101");	
	 	fclose(fp);
	 	goto ErrExit;
	}
	set_zd_int("0490",count);
	vtcp_log("%s %d::��Ʒ����count=%d",__FILE__,__LINE__,count);
	
	iRt = -1;
	/*��ѯ��Ʋ�Ʒ������fn_parm*/
	iRt=Fn_parm_Dec_Sel(g_pub_tx.reply,wherelist);
	
	if(0 != iRt)
	{
		vtcp_log("%s %d::Fn_parm_Dec_Sel [%s] error ",__FILE__,__LINE__,iRt);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0130","���α����");
	 	fclose(fp);
	 	goto ErrExit;
	}
	else
	{
		while(1)
		{
			memset(&sFn_parm,0x00,sizeof(sFn_parm));
			iRt=Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
			if(iRt == 1403 || iRt == 100)
			{
				vtcp_log("%s %d::��Ҫ��ѯ����Ʋ�Ʒ��¼[%s]",__FILE__,__LINE__,iRt);
	 			break;	
			}
			else if(0 != iRt && 100 != iRt)
			{
				vtcp_log("%s %d::Fn_parm_Fet_Sel error[%s]",__FILE__,__LINE__,iRt);
				strcpy(g_pub_tx.reply,"D001");
	 			set_zd_data("#sys13","��ѯ����");
	 			Fn_parm_Clo_Sel();
	 			fclose(fp);
	 			goto ErrExit;
	 		}
	 		else
	 		{
	 			if(cnt < atoi(turnPageBeginPos))
	 			{
	 				cnt++;
	 				continue;	
	 			}
	 			else
	 			{
	 				double perLowest=0.00;
	 				
	 				/*����޶�*/
	 				if(0 == strcmp(cstType,"1"))
	 				{
	 					perLowest=sFn_parm.per_min_amt;	/*����*/
	 				}
	 				else
	 				{
	 					perLowest=sFn_parm.com_min_amt;	/*�Թ�*/
	 				}
	 				
	 				/*����ʣ���*/
	 				/*�Ϲ����ò�Ʒ�����Ϲ�����ܺ�,��Ʋ�Ʒ�Ǽǲ���ĳһ״̬��Ϊ*(����)�Ĳ�Ʒ*/
	 				iRt = -1;
	 				iRt=sql_sum_double("fn_reg","tx_amt",&dTx_amt,"prdt_code='%s' and sts <> '*'",\
	 					sFn_parm.prdt_code);
	 				vtcp_log("%s %d::�Ϲ����dTx_amt=[%f] ",__FILE__,__LINE__,dTx_amt);
	 				
    			if( iRt )
    			{
    			 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,iRt);
    			 	strcpy(g_pub_tx.reply,"9999");
    			 	set_zd_data("0130","�Ϲ�������");
    				break;
    			}
    			/*����ʣ���=���۽��-�Ϲ�����ܺ�*/
	 				dTx_amt=sFn_parm.sale_amt-dTx_amt;
	 				
	 				sale_amt=sFn_parm.sale_amt;
	 				vtcp_log("%s %d::dTx_amt[%.2f]sFn_parm.sale_amt[%.2f]sale_amt[%.2f] ",__FILE__,__LINE__,dTx_amt,sFn_parm.sale_amt,sale_amt);
	 				
	 				if(memcmp(sFn_parm.stat,"0",1) == 0)
	 				{
	 					/*��Ʒ����=1-����ʣ���/���۽�� ����������*/
	 					proProgress = 1-dTx_amt / sale_amt;
	 				}
	 				else
	 				{
	 					vtcp_log("%s %d::dTx_amt[%.2f]sFn_parm.sale_amt[%.2f] ",__FILE__,__LINE__,dTx_amt,sFn_parm.sale_amt);
	 					
	 					proProgress=1;
	 				}
	 				
	 				vtcp_log("%s %d::proProgress[%.2f] ",__FILE__,__LINE__,proProgress);
	 				
	 				if(!memcmp(purchaMark,"0",1))
	 				{
	 					if(dTx_amt > 0)
	 					{
	 						/*"|@��Ʒ����|@��Ʒ����|@����|@���յȼ�|@Ԥ��������|@��Ʒ��Ϣ��|@��Ʒ������|@�𹺽��|@��Ʒ����|@״̬|@ļ����ʼ��|@ļ��������|*/
	 						fprintf(fp,"|%s|%s|%d|%s|%9.2f|%d|%d|%16.2f|%9.2f|%s|%d|%d|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat,sFn_parm.beg_date,sFn_parm.end_date);
	 						/*vtcp_log("%s %d��ʼ��ӡ��|%s|%s|%s|%s|%9.2f|%s|%s|%16.2|%9.2|%s|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat);*/
	 					
	 						/*��Ʒ��¼��*/
	 						proSum++;
	 						vtcp_log("%s %d::��Ʒ��¼��proSum=%d",__FILE__,__LINE__,proSum);
	 						if(proSum >= atoi(turnPageShowNum))
	 						{
	 							vtcp_log("%s %d::��Ʒ��¼�����",__FILE__,__LINE__);
	 							break;	
	 						}
	 					}
	 					else
	 					{
	 						continue;	
	 					}
	 				}
	 				else
	 				{
	 					/*"|@��Ʒ����|@��Ʒ����|@����|@���յȼ�|@Ԥ��������|@��Ʒ��Ϣ��|@��Ʒ������|@�𹺽��|@��Ʒ����|@״̬|@ļ����ʼ��|@ļ��������|*/
	 						fprintf(fp,"|%s|%s|%d|%s|%9.2f|%d|%d|%16.2f|%9.2f|%s|%d|%d|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat,sFn_parm.beg_date,sFn_parm.end_date);
	 						/*vtcp_log("%s %d��ʼ��ӡ��|%s|%s|%s|%s|%9.2f|%s|%s|%16.2|%9.2|%s|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat);
	 					*/
	 					/*��Ʒ��¼��*/
	 					proSum++;
	 					vtcp_log("%s %d::��Ʒ��¼��proSum=%d",__FILE__,__LINE__,proSum);
	 					if(proSum >= atoi(turnPageShowNum))
	 					{
	 						vtcp_log("%s %d::��Ʒ��¼�����",__FILE__,__LINE__);
	 						break;	
	 					}	
	 				}	
	 			}
	 		}
		}	
	}
	Fn_parm_Clo_Sel();
	fclose(fp);
	
	/*ftp�����ļ�*/
	iRt = -1;
	vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,filename);
	set_zd_data("0260",filename);
	
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	iRt=system(cFtpfile);
	if (iRt)
	{
		vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨����[%d]", __FILE__, __LINE__,filename,iRt);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","�ļ��������");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨�ɹ�", __FILE__, __LINE__,filename);
	
   
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","��Ʋ�Ʒ�б��ѯ�ɹ���");
	vtcp_log("success !!: 5050 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("0120",g_pub_tx.reply);
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}

