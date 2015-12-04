/*********************************************************************
* �� �� �� :  
* �������� :  �⺣��
* ��    �� :  2014/11/01
* ����ģ�� :  �����˺�������˺Ź�����ѯ
* �������� :  spW220.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5060', '�����˺�������˺Ź�����ѯ', '10000000000000000000000111011110000000000000001110000000000000000000000000000000000000000000000000000000000000000000000000000000', '0','4', '2');
insert into tx_flow_def values ('5060', 0, 'W220', '#$');
insert into tx_sub_def values ('W220', '�����˺�������˺Ź�����ѯ', 'spW220', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"

int spW220()
{
	struct mdm_ac_rel_c d_mdm_ac_rel;	/*�������˻���ϵ*/
	struct ln_mst_c	d_ln_mst;		/*�������ļ�*/
	
	char payAccount[32+1];		/*�����˺�*/
	char clearType[1+1];		/*�����־*/
	char tx_date[8+1];		/*ƽ̨����*/
	char sw_traceno[10+1];		/*ƽ̨��ˮ*/
	
	char turnPageBeginPos[10];		/*��ѯ��ʼ��¼��*/
	char turnPageShowNum[10];		/*�����¼��*/
	
	char filename[60];
	char pcfilename[256];
	char cFtpfile[256];
	FILE *fp=NULL;
	
	double debitInterest=0.00;	/*��Ƿ��Ϣ*/
	double OpenInterest=0.00;	/*δ����Ϣ*/
	double vbjlx=0.00,vbnlx=0.00,vbylx=0.00,vfllx=0.00;
		/*���������Ϣ������ǷϢ����������ǷϢ����������ǷϢ����*/
	
	int ret;
	int recordNo=0;		/*��¼��*/
	int count=0;		/*�������ļ��ܼ�¼��*/
	int cnt=1;		/*��ѯ����*/
	
	memset(&d_mdm_ac_rel,0x00,sizeof(d_mdm_ac_rel));
	memset(&d_ln_mst,0x00,sizeof(d_ln_mst));
	memset(payAccount,0x00,sizeof(payAccount));
	memset(clearType,0x00,sizeof(clearType));
	memset(tx_date, 0x00, sizeof(tx_date));
	memset(sw_traceno, 0x00, sizeof(sw_traceno));
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(turnPageBeginPos,0x00,sizeof(turnPageBeginPos));
	memset(turnPageShowNum,0x00,sizeof(turnPageShowNum));
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0300",payAccount);
	if(!strlen(payAccount))
	{
		strcpy(g_pub_tx.reply, "9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","�ؼ���'�����˺�'Ϊ��");
		return 1;
	}
	vtcp_log("%s,%d payAccount=[%s]\n",__FILE__,__LINE__,payAccount);
	get_zd_data("0660",clearType);
	
	get_zd_data("0280",turnPageBeginPos);
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply, "9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","�ؼ���'��ѯ��ʼ��¼��'Ϊ��");
		return 1;
	}
	
	get_zd_data("0290",turnPageShowNum);
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply, "9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","�ؼ���'�����¼��'Ϊ��");
		return 1;
	}
	
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	vtcp_log("%s %d ::ƽ̨����tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::ƽ̨��ˮsw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	
	/*ͨ�������˺�ȡ�����˻�ID---�������˻���ϵ*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&d_mdm_ac_rel,"ac_no='%s'",payAccount);
	if(ret == 100 || ret == 1403)
	{
		strcpy(g_pub_tx.reply,"D137");
		set_zd_data("0120","D137");
		set_zd_data("0130","�����˺Ų�����");
		goto ErrExit;	
	}
	else if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		set_zd_data("0120","W011");
		set_zd_data("0130","��ѯ�������˻���ϵ�����");
		goto ErrExit;	
	}
	
	/* �����ļ� */
	sprintf( filename,"5060netbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
	fp=fopen(pcfilename,"w");
	if( fp==NULL )
	{
	    vtcp_log("%s %d::open file [%s] error ",__FILE__,__LINE__,pcfilename);
	    strcpy(g_pub_tx.reply,"P157");
	    set_zd_data("0120","P157");
	    set_zd_data("0130","���ļ�����");
	    goto ErrExit;
	}
	/*�ļ�ͷ*/
	fprintf(fp, "�����˺�|�����|�������|��Ƿ��Ϣ|δ����Ϣ|\n");
	
	/* ���ݻ����˻�ID��ѯ�������ļ����� */
	count = sql_count("ln_mst", "repay_ac_id='%ld' and ac_sts in('1','2','3','4','5')" , d_mdm_ac_rel.ac_id);
	if(count < 0)
	{
		vtcp_log("%s %d::sql_count [%d] error ",__FILE__,__LINE__,count);
	 	strcpy(g_pub_tx.reply,"D101");
	 	set_zd_data("0120","D101");
	 	fclose(fp);	
	 	goto ErrExit;
	}
	set_zd_int("0490",count);
	vtcp_log("%s %d::��Ʒ����count=%d",__FILE__,__LINE__,count);
	
	/*ͨ�������˻�IDȡ�����˻���Ϣ---�������ļ�*/
	ret = -1;
	ret=Ln_mst_Dec_Sel(g_pub_tx.reply,"repay_ac_id='%ld' and ac_sts in('1','2','3','4','5')",d_mdm_ac_rel.ac_id);
	if(ret)
	{
		vtcp_log("%s %d::Ln_mst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0120","D102");
	 	set_zd_data("0130","���α����");
	 	fclose(fp);
	 	goto ErrExit;		
	}
	
	while(1)
	{		
		memset(&d_ln_mst,0x00,sizeof(d_ln_mst));
		memset(&d_mdm_ac_rel,0x00,sizeof(d_mdm_ac_rel));
		
		memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
		memset(&g_ln_parm,0x00,sizeof(g_ln_parm));
		memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
		
		ret = -1;
		ret = Ln_mst_Fet_Sel(&d_ln_mst,g_pub_tx.reply);	
		if(ret == 1403 || ret == 100)
		{
			vtcp_log("%s %d::������Ҫ��ѯ���˺�[%d]",__FILE__,__LINE__,ret);
			break;	
		}
		 else if(0 != ret && 100 != ret)
		{
			vtcp_log("%s %d::Ln_mst_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D001");
			set_zd_data("0120","D001");
			set_zd_data("0130","��ѯ����");
			Ln_mst_Clo_Sel();
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
				/*ͨ�������˺�IDȡ�����˺�---�������˻���ϵ*/
				ret = -1;
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&d_mdm_ac_rel,"ac_id='%ld'",d_ln_mst.ac_id);
				if(ret)
				{
					vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0120","W011");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit;	
				}
				
				/* ȡ���������Ϣ */
				ret = -1;
				ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' " ,d_ln_mst.prdt_no );
				if( ret==100 )
				{
					sprintf(acErrMsg,"��������ļ����޴˼�¼ [%s][%d]",d_ln_mst.prdt_no,ret);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L001" );
					set_zd_data("0120","L001");
					Ln_mst_Clo_Sel();
					fclose(fp);
					return 1;
				} 
				else if( ret )
				{
					sprintf( acErrMsg,"ȡ��������ļ��쳣 [%s][%d]",d_ln_mst.prdt_no,ret);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L001" );
					set_zd_data("0120","L001");
					Ln_mst_Clo_Sel();
					fclose(fp);
					return -1;
				}
				
				/* �����������Ƿ��ǰ��Ҵ��� */
				if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
				{
					vtcp_log("%s %d:: �˴���Ϊ���Ҵ���,�����ڴ˽���![%s]",__FILE__,__LINE__,g_ln_parm.ln_pay_type);
					continue;
					
				}
				vtcp_log("%s %d:: �����������Ƿ�����ͨ����",__FILE__,__LINE__);	
				
				vbjlx=vbnlx=vbylx=vfllx=0.00;
				
				vtcp_log("--------�������������Ϣ:bjlx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"0",&vbjlx,1 );
				if( ret ) 
				{
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","�������������Ϣ����");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }
					
				vtcp_log("--------����ǷϢ�������㸴��:bnlx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"1",&vbnlx,1 );
				if( ret ) 
				{ 
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","����ǷϢ�������㸴������");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }	
				
				vtcp_log("--------����ǷϢ�������㸴��:bylx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"2",&vbylx,1 );
				if( ret ) 
				{ 
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","����ǷϢ�������㸴������");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }		
				
				vtcp_log("--------����ǷϢ�������㸴��:fllx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"3",&vfllx,1 );
				if( ret ) 
				{ 
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","����ǷϢ�����������");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }
				 
				/*δ����Ϣ=δ�᱾����Ϣ+δ�Ḵ����Ϣ
				  δ�Ḵ����Ϣ=����ǷϢ����+����ǷϢ����+����ǷϢ����
				*/	
				OpenInterest=vbjlx+vbnlx+vbylx+vfllx;
				 
				/*��Ƿ��Ϣ=����ǷϢ+����ǷϢ+Ƿ������Ϣ*/
				debitInterest=d_ln_mst.in_lo_intst+d_ln_mst.out_lo_intst+d_ln_mst.cmpd_lo_intst;
				
				/*д�ļ�
					�����˺�|�����|�������|��Ƿ��Ϣ|δ����Ϣ|
				*/
				
				if(0 == memcmp(clearType,"0",1))
				{
					if(d_ln_mst.bal == 0 && debitInterest == 0 && OpenInterest == 0)
					{
						fprintf(fp, "%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
							d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
						vtcp_log("%s %d��ʼ��ӡ��%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
	 							__FILE__,__LINE__,d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);	
	 						
	 					++recordNo;
	 					vtcp_log("%s %d::��Ʒ��¼��recordNo=%d",__FILE__,__LINE__,recordNo);
	 					if(recordNo >= atoi(turnPageShowNum))
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
				else if(0 == memcmp(clearType,"1",1))
				{
					if(d_ln_mst.bal != 0 || debitInterest != 0 || OpenInterest != 0)
					{
						fprintf(fp, "%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
							d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
						vtcp_log("%s %d��ʼ��ӡ��%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
	 							__FILE__,__LINE__,d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
						
						++recordNo;
						vtcp_log("%s %d::��Ʒ��¼��recordNo=%d",__FILE__,__LINE__,recordNo);
	 					if(recordNo >= atoi(turnPageShowNum))
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
					fprintf(fp, "%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
						d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
					vtcp_log("%s %d��ʼ��ӡ��%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
	 						__FILE__,__LINE__,d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
					
					++recordNo;
					vtcp_log("%s %d::��Ʒ��¼��recordNo=%d",__FILE__,__LINE__,recordNo);
	 				if(recordNo >= atoi(turnPageShowNum))
	 				{
	 					vtcp_log("%s %d::��Ʒ��¼�����",__FILE__,__LINE__);
	 					break;	
	 				}
				}
			}
		}
		
	}
	Ln_mst_Clo_Sel();
	
	fclose(fp);
	
	/*ftp�����ļ�*/
	vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,filename);
	set_zd_data("0260",filename);
	
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret = -1;
	ret=system(cFtpfile);
	if (ret)
	{
		vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨����[%d]", __FILE__, __LINE__,filename,ret);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","�ļ����ݳ���");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨�ɹ�", __FILE__, __LINE__,filename);
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","�����˺�������˺Ź�����ѯ��");
	vtcp_log("success !!: 5050 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}

