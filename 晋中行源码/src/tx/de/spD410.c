/***************************************************************
* �� �� ��:     spD410.c
* ����������	  ��֤�ӽ���
* ��    ��:     jack
* ������ڣ�    2004��1��25��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_prdt_prot_c.h"
#include "mob_acct_type_c.h" /*add by zgf 2013-07-25*/

spD410()
{
	int	mode=0;
	int mob_flag = 0;/*add by zgf 2013-07-25*/
	int ret;/*add by zgf 2013-07-25*/
	char	tmpstr[2];
	char  name[51];
	char	oldacno[30]={0};
	char	newacno[30]={0};
	struct mo_prdt_prot_c mo_prdt_prot;
	struct mob_acct_type_c st_mob_acct_type;/*add by zgf 2013-07-25*/
	int temp=0;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	
	g_reply_int=0;
  get_zd_data("0250",name);
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	
	get_zd_data("0670",tmpstr);
	mode=atoi(tmpstr);

	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0300",oldacno);
	get_zd_data("1142",g_pub_tx.draw_pwd_yn);
	get_zd_data("1144",g_pub_tx.draw_pwd);
	get_zd_data("1145",g_pub_tx.draw_id_yn);
	get_zd_data("1148",g_pub_tx.id_type);
	get_zd_data("1146",g_pub_tx.id_no);
  
	get_zd_data("0310", g_pub_tx.crd_no); 
	get_zd_data("0310", newacno); 
	get_zd_data("0160",g_pub_tx.tx_code);
	sprintf(acErrMsg,"tx_code[%s]",g_pub_tx.tx_code);
	WRITEMSG
  pub_base_old_acno(g_pub_tx.ac_no);
  pub_base_old_acno(g_pub_tx.crd_no);
	get_zd_data("0590",g_pub_tx.beg_note_no);
	strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	/** modified by xxxxx 20041009 ***/
	if( mode==1 )
	{
		strcpy( g_pub_tx.brf,"������֤" );
	}else{
		strcpy( g_pub_tx.brf,"��ʧ��֤" );
	}
	sprintf(acErrMsg, "xxx******** ac_no = [%s]", g_pub_tx.ac_no);
	WRITEMSG
	
	/*���ӿ��ƶ��Ŵ���ǩԼ��Ϣʱ���Ȳ�ѯ���˻���ǩԼ��Ϣ�����ڽṹ��st_mob_acct_type��----add by zgf 2013-07-25*/
  memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
  ret=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"ac_no='%s' ",g_pub_tx.ac_no);
  if(ret==100){
          vtcp_log("[%s][%d]�˺�[%s]δ��ѯ������ǩԼ��¼,��������!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
  }else if(ret==0){
  				mob_flag = 1;
          vtcp_log("[%s][%d]�˺�[%s]���ڶ���ǩԼ��¼!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
          sprintf(acErrMsg, "�ÿ��Ŵ��ڶ���֪ͨҵ�񣬻���ʱ���ĳ���ǩԼ����!!");

  }else{
          vtcp_log("[%s][%d]��ѯ��������ǩԼ�����!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
          sprintf(acErrMsg, "��ѯ��������ǩԼ�����!!");
          strcpy(g_pub_tx.reply,"XXXX");
          set_zd_data(DC_GET_MSG,acErrMsg);
          goto ErrExit;
  }
  /*end add by zgf*/
  /*20150720  add  by  cfq */
	strcpy(g_pub_tx.name,name);
	g_reply_int=pub_acct_chg_mdm(mode);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��֤���س���!");
		WRITEMSG
		goto ErrExit;
	}

/**IC�������˻����ں��� modified by wzw 20120309***/	
	/*M_wenbin���IC����*/
/***	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",g_pub_tx.ac_no);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�ÿ�������!");
		WRITEMSG
		goto ErrExit;
	}
	if( memcmp(g_mdm_ac_rel.mdm_code,"0024",4)==0)
	{
		temp = g_pub_tx.ac_seqn;
		g_pub_tx.ac_seqn=1;
		g_reply_int=pubf_base_IC(1,1);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��IC�����˻�����!");
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.ac_seqn = temp;
	}***/
	/*M_wenbin��ӽ���*/
	
	g_reply_int=Mo_prdt_prot_Sel(g_pub_tx.reply,&mo_prdt_prot,"substr(prot_type,1,2)='TZ' and main_ac_id=%d and main_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn );
	if( g_reply_int==0 ) 
	{
		set_zd_data("0680","1" );
	}
	else if( g_reply_int==100 )
	{
		set_zd_data("0680","0" );
	}
	else
	{
		TRACE
		goto ErrExit;
	}

	if( mode==2 )
	{
		vtcp_log("�Զ�ȡ���ײ�");
	vtcp_log("update mo_prdt_prot set sts='*' where prot_type[1,2]='TZ' and main_ac_id=%d and main_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn ); 
/*
	sql_execute("update mo_prdt_prot set sts='*' where prot_type[1,2]='TZ' and main_ac_id=%d and main_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn ); 
	sql_execute("update mo_prot_ac_rel set sts='*' where prot_type[1,2]='TZ' and main_ac_id=%d and main_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn ); 
*/
	sql_execute("update mo_prdt_prot set sts='*' where prot_type='TZ'||substr(prot_type,3,1) and main_ac_id=%d and main_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn ); 
	sql_execute("update mo_prot_ac_rel set sts='*' where prot_type='TZ'||substr(prot_type,3,1) and main_ac_id=%d and main_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn ); 
	}
	
	set_zd_data("0310",g_pub_tx.ac_no);
	
	
	/***�ɿ����¿������Ͷ���֪ͨ**---add by zgf 2013-07-25**/
	if(mob_flag)
	{
		/**ԭ���ŷ��ͽ�Լ����֪ͨ**/
		pub_mob_sendmail("�ر�", g_pub_tx.tx_code,st_mob_acct_type.ac_no,"1",0.0, 0.0 );
		/**����ǩԼ���У��ɿ��Ÿ��³��¿��ţ����¿��ŷ���ǩԼ����֪ͨ**/
		sql_execute("update mob_acct_type set ac_no = '%s' where ac_no = '%s'",g_pub_tx.ac_no,st_mob_acct_type.ac_no ); 
		pub_mob_sendmail("��ͨ", g_pub_tx.tx_code,g_pub_tx.ac_no,"1",0.0, 0.0 );
	}
	/*end add by zgf*/

	 /* add begin �����˺���Ϣͬ�� lzy 20141127 */
			char today[9]={0};
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			nextday = pub_base_daynumLAT(today,1);

			if ( oldacno[0]=='6' )
				{
					sprintf( filename,"PersAcctSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::�˻�Ϊ[%s],����Ҫ������ͬ��.",__FILE__,__LINE__, oldacno );
					goto OkExit;
				}
			
			sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
			vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
			fp=fopen(pcfilename,"at");
			if( fp==NULL )
			{
				vtcp_log("[%s],[%d] �������ļ�[%s]",__LINE__,__FILE__,pcfilename);
				fp = fopen(pcfilename, "wt");
				if (fp == NULL)
				{
					vtcp_log("[%s],[%d] open file[%s] error",__LINE__,__FILE__,pcfilename);
					strcpy(g_pub_tx.reply,"SM83");
					goto ErrExit;
				}
			}
				/**fprintf(fp, "�˺�,���˺�\n");**/
				fprintf(fp, "%s,%s\n",oldacno,newacno);
				fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨����[%d]", __FILE__, __LINE__,filename,ret);
					strcpy(g_pub_tx.reply,"S047");
					goto ErrExit;
				}	
			vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨�ɹ�", __FILE__, __LINE__,filename);				
		  /* add end lzy 20141127 */


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��֤�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��֤ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
