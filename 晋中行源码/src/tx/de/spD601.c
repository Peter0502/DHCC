/***************************************************************
* �� �� ��: spD601.c
* �����������ʻ���Ϣά��
* ��    ��: mike
* ������ڣ�2003��3��9��
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
#include "mo_infrm_c.h"
#include "prdt_ac_id_c.h"

spD601()
{
	int ret;
	char ac_no[20],ac_type[2],intst_type[2];
	char name[61];
	long tmp_date1,tmp_date2;
	double tmp_amt;
	char kt_flag[2];

	struct mo_infrm_c	g_mo_infrm;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_c     g_dd_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	memset(kt_flag,0x00,sizeof(kt_flag));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* �˺� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);  			/* �˻���� */
	get_zd_data("0230",g_pub_tx.prdt_code);  		/* ��Ʒ���� */
	get_zd_data("0680",ac_type);         			/* �˻����� */
	get_zd_data("0670",intst_type);         		/* ��Ϣ���� */
	get_zd_data("0250",name);         				/* �˻����� */
	get_zd_data("0710",kt_flag);                    /* �Ƿ�ͨ�����˻���־ */

	pub_base_strpack( name );

	sprintf(acErrMsg,"�µĻ�����-[%s]",name);
	WRITEMSG

	/*�����˺Ų����˺�ID*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'", \
						 g_pub_tx.ac_no);
  	if( ret==100 )
  	{
	   sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** del by xxxxx 2003-12-20
	if( g_pub_tx.ac_no[0]!='5' )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%s]",g_pub_tx.ac_no);
		WRITEMSG
		set_zd_data( DC_GET_MSG,"�������޸ĸ��˻�����������Ҫֻ�������ؿ�");
		strcpy(g_pub_tx.reply,"BBCL");
		goto ErrExit;
	}
	********/

	/*����˻�״̬�Ƿ�����*/
	if( g_mdm_ac_rel.note_sts[0]=='*'||g_in_mst.sts[0]=='*' )
	{
	 	sprintf(acErrMsg,"���˻�������!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"P172");
	    goto ErrExit;
	}
	
	strcpy(g_mdm_ac_rel.draw_pwd_chg,kt_flag);
	ret=sql_execute("update mdm_ac_rel set draw_pwd_chg='%s' where ac_no='%s'",\
		g_mdm_ac_rel.draw_pwd_chg,g_pub_tx.ac_no);
	if(ret)
	{
		vtcp_log("���±�mdm_ac_relʧ�� �ʺ�:[%s],[%s] [%d]",g_pub_tx.ac_no,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O087");
		goto ErrExit;
	}

	/*�����˻����͵���Ӧ�ı���������*/
	switch( g_pub_tx.prdt_code[0] )
	{
		case '1':                                          /*1-����*/
				ret=Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", \
								g_pub_tx.prdt_code);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"��Ʒ��Ų�����!");
	  				WRITEMSG
					strcpy(g_pub_tx.reply,"M003");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}

				ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"�����ļ����޴˼�¼");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_dd_mst.ac_id;
				strcpy( g_dd_mst.name, name );

				if(g_dd_parm.cif_type[0]=='1')
					strcpy(g_dd_mst.sttl_type,ac_type);
				else
				{
					strcpy(g_dd_mst.ac_type,ac_type);

					if(ac_type[0]=='1')							/*** ������ ***/
						strcpy(g_dd_mst.cash_ind,"Y");
					else if(ac_type[0]=='2')					/*** һ�㻧 ***/
						strcpy(g_dd_mst.cash_ind,"N");
					else
						strcpy(g_dd_mst.cash_ind,"Y");
				}
				if(g_mdm_ac_rel.ac_no[0]=='5'){			/* �Թ��ĸĴ���Ĳ���,���Ա������ */
					g_dd_mst.intst_type[0]=intst_type[0];
				}

				ret = Dd_mst_Upd_Upd( g_dd_mst,g_pub_tx.reply );
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}

				Dd_mst_Clo_Upd();
				break;
	case '2':												/*����*/
				ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				ret = Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"�����ļ����޴˼�¼");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_td_mst.ac_id;
				/*���ڵ�Ҳ �ܸļ�Ϣ��ʽ wudawei 20140715*/
				strcpy(g_td_mst.intst_type,intst_type);
				/***strcpy( g_td_mst.name, name );del by martin 2008/12/8 14:28***/
				vtcp_log("ic_date=[%ld]",g_td_mst.ic_date);

				ret = Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}

				Td_mst_Clo_Upd();
				break;
		case '9':
			ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				ret = In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"�����ļ����޴˼�¼");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_in_mst.ac_id;
				strcpy( g_in_mst.name, name );/***del by martin 2008/12/8 14:28***/
				vtcp_log("ic_date=[%ld]",g_in_mst.ic_date);
				g_in_mst.intst_knd[0]=intst_type[0];
				ret = In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				In_mst_Clo_Upd();
				break;
		default:
				break;
	}
	/*switch����*/
	strcpy( g_pub_tx.brf,"�ʻ���Ϣά��" );


    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

	 /* add begin lzy 20150108 */
			char today[9]={0};
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,atol(pub_base_daynumLAT(today,1)),8);*/
			nextday = pub_base_daynumLAT(today,1);

			if ( g_pub_tx.ac_no[0]=='5' )
				{
					sprintf( filename,"CorpAcctSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::�˻�Ϊ[%s],����Ҫ������ͬ��.",__FILE__,__LINE__, g_pub_tx.ac_no );
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
				/**fprintf(fp, "�˺�,����\n");**/
				fprintf(fp, "%s,%s\n",g_pub_tx.ac_no,name);
				fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨����[%d]", __FILE__, __LINE__,filename,ret);
				}	
			vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨�ɹ�", __FILE__, __LINE__,filename);				
		  /* add end lzy 20150108 */


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ʻ���Ϣά���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ʻ���Ϣά��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
