/*************************************************
* �� �� ��: sp9953.c
* ���������������ʺŻ����˻���ż���Ϣ[����ύ��]
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#include "public.h"
#include "prdt_ac_id_c.h"

#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"
#include "mo_infrm_c.h"

sp9953()  
{ 		
	int i=0,ret=0,tag=0;
	long ac_seqn=0;
	double bal;
	long cif_no=0;
	char spe_ind[21];
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61];
	char tmpname[100],prdt_name[31];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct mo_infrm_c g_mo_infrm;

	memset( &g_mo_infrm, 0x00, sizeof(struct mo_infrm_c) );
	memset( spe_ind,0,sizeof(spe_ind) );
	strcpy( spe_ind,"000000" );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("1021",ac_no);					/*�ʺ�*/
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����������ᣬ�����벿�������˺�![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M017");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý����ѱ���֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����Ѳ��ỻ֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������������![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						 g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]", \
				g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** ���˻����ʵ���� ***/
	if( g_mdm_attr.ac_num_ind[0]=='N' )
	{
		ret = sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==1) 
		{
			vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
			ac_seqn = prdt_ac_id_c.ac_seqn;
			vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
		}
		else if(ret>1)
		{
			vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
			set_zd_long("102N",prdt_ac_id_c.ac_seqn);
			vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
			ac_seqn = prdt_ac_id_c.ac_seqn;
			set_zd_long("1022",ac_seqn);
			tag = 1;
			vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
		}
	}
	else
		ac_seqn = g_mdm_ac_rel.ac_seqn;
	set_zd_long("1022",ac_seqn);
	if( tag==0 )
	{
		if( prdt_ac_id_c.prdt_no[0]=='2' )
		{
		vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"td_mst [%d][%d]",prdt_ac_id_c.ac_id,ac_seqn);
				WRITEMSG
				goto ErrExit;
			}

			if(g_td_mst.ac_sts[0]=='*')
			{
				sprintf(acErrMsg,"���˺�������!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

		vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
			ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
							  g_td_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}

			if( g_td_parm.wdr_ind[0]=='N' )
			{
				sprintf(acErrMsg,"���ʻ���������!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O193");
				goto ErrExit;
			}

			/*** ֪ͨ�����֪ͨ�ǼǱ� ***/
			if( g_td_parm.td_type[0]=='4' )
			{
				ret = Mo_infrm_Sel(g_pub_tx.reply,&g_mo_infrm,"ac_id=%ld and \
								   ac_seqn=%d and sts='0'", g_td_mst.ac_id, \
								   g_td_mst.ac_seqn );
				vtcp_log("XXXXXXXXXXXXXXX");
				if( ret==100 )
				{
					vtcp_log("YYYYYYYYYY");
					strcpy( spe_ind ,"0001" );			/*** δ�Ǽ�֧ȡ ***/
				}
				else if( ret==0 )
				{
					vtcp_log("ZZZZZZZ");
					vtcp_log("֪ͨ���ڣ�g_mo_infrm.draw_date = [%ld], \
							 ϵͳ���ڣ�g_pub_tx.tx_date = [%ld] ", \
							 g_mo_infrm.prm_draw_date, g_pub_tx.tx_date );

					if( g_pub_tx.tx_date < g_mo_infrm.prm_draw_date )
					strcpy( spe_ind , "0002");			/*** ��ǰ֧ȡ ***/
					if( g_pub_tx.tx_date > g_mo_infrm.prm_draw_date )
					strcpy( spe_ind , "0003");			/*** ����֧ȡ ***/
				}
				else if( ret )
				{
					sprintf(acErrMsg,"read dd_mst error" );
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
			}

			strcpy( name,g_td_mst.name );
			cif_no = g_td_mst.cif_no;
			bal = g_td_mst.bal;
			strcpy( prdt_name,g_td_parm.title );
			strcpy( cur_no,g_td_parm.cur_no );
			strcpy( cif_type,g_td_parm.cif_type );
		}
		else 
		{
			sprintf(acErrMsg,"���ʻ�����������ҵ��! [%s]",prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O193");
			goto ErrExit;
		}

	   	set_zd_data("102M",cif_type);

		if( cif_type[0]!='1' && cif_type[0]!='0' )
		{
			sprintf(acErrMsg,"�����봢���˺�! [%s]",cif_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O164");
			goto ErrExit;
		}

		ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
									"cif_no=%ld",cif_no);
		if( ret )
		{	
			sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "C051");
			goto ErrExit;
		}

		/*����ƾ֤����*/
		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"w");

		ret=Dic_data_Sel(g_pub_tx.reply,&vdic_data, \
			"pr_code='note_type' and pr_seq='%s'",g_mdm_attr.note_type);
		if( ret==100 )
			goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		fprintf(fp,"1023|%s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		set_zd_data("1023",g_mdm_attr.note_type);		/*ƾ֤����*/
		set_zd_data(DC_FILE_SND,"2");
		fclose(fp);
		/*�������*/

		pub_base_strpack(spe_ind);
		pub_base_strpack(prdt_name);

		vtcp_log("��Ʒ����Ϊ��prdt_name = [%s], ", prdt_name );
		vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);

		pub_base_strpack(name);
    	set_zd_data("0020",g_td_mst.opn_br_no);			/*��������*/
    	set_zd_data("102H",name);						/*����*/
    	set_zd_data("102U",spe_ind);					/*֪֧ͨȡ��־*/
    	set_zd_data("102V",prdt_name);					/*��Ʒ����*/
		set_zd_data("102J",cur_no);						/*����*/
		set_zd_data("102K","1");						/*Ĭ����ת*/
		set_zd_double("102I",bal);						/*���*/
		set_zd_data("1023",g_mdm_attr.note_type);		/*ƾ֤����*/
		set_zd_data("1024",g_mdm_ac_rel.note_no);		/*ƾ֤����*/
		set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn);	/*����֧ȡ*/

		vtcp_log("++++++++++++++++++++++++++++g_mdm_ac_rel.draw_id_yn=[%s]", \
				  g_mdm_ac_rel.draw_id_yn);

		if(g_mdm_ac_rel.draw_id_yn[0]=='N')
	 		set_zd_data("1029","N");                    /*֤��֧ȡ*/
		else if(g_mdm_ac_rel.draw_id_yn[0]=='Y')
			set_zd_data("1029","Y");                    /*֤��֧ȡ*/

		set_zd_data("102G",g_mdm_ac_rel.id_type);      	/*֤������*/
	}

	set_zd_data("1021",ac_no);      /*������ʺ�*/
	vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	vtcp_log("[%s][%d] ac_seqn=%d,prdt_seqn=%d\n",__FILE__,__LINE__,ac_seqn,prdt_ac_id_c.ac_seqn);
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
