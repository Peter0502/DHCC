/*************************************************
* �� �� ��: sp9707.c
* ���������������ʺŻ��������Ϣ_ת�浥����_NO.1
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_prdt_rel_c.h"
#include "mdm_attr_c.h"

sp9707()  
{ 		
	int i=0,ret=0,ac_seqn=0,tag=0;
	char title[40],cur_no[3];
	char ac_no[21],name[60],tmpname[100];
	char note_type[4];
	double bal;
	FILE *fp;

	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_prdt_rel_c g_mdm_prdt_rel;
	struct mdm_attr_c vmdm_attr;

	memset(&vmdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_prdt_rel,0x00,sizeof(struct mdm_prdt_rel_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1021",ac_no);							/*�ʺ�*/
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"���ҳ���!!ret=[%d]",ret);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*��Ϊ������λ����֤ʵ�顢����һ��ͨʱ���������˽���*/
	if( strncmp(g_mdm_ac_rel.mdm_code,"0020",4) && 
		strncmp(g_mdm_ac_rel.mdm_code,"0007",4) && 
		strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
	{
	    sprintf(acErrMsg,"���ʺŲ�����ת�浥����!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"O187");
		goto ErrExit;
	}

	/* ���ҽ������� */
	ret = Mdm_attr_Sel( g_pub_tx.reply,&vmdm_attr,"mdm_code='%s'", 
						g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
	    sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"���ҳ���!!ret=[%d]",ret);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy( note_type,vmdm_attr.note_type );

	/*���˻����ʵ�������ж�*/
	if( vmdm_attr.ac_num_ind[0]=='N' )
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld and ac_seqn>=200", 
					  g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==0 )
		{
			sprintf(acErrMsg,"�ÿ������ڶ��ڲ�Ʒ!!");
			WRITEMSG
		 	strcpy(g_pub_tx.reply,"O184");
			goto ErrExit;
		}
		else if( ret )
		{
			tag = ret;
			ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, 
								"ac_id=%ld and ac_seqn>=200", 
								 g_mdm_ac_rel.ac_id);
   			if( ret==100 )
			{
				sprintf(acErrMsg,"�����ڸ��˻�!ac_id=[%ld]",g_mdm_ac_rel.ac_id);
		 		WRITEMSG
		 		strcpy(g_pub_tx.reply,"M003");
		 		goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"���Ҵ���======[%d]",ret );
		 		WRITEMSG
		 		strcpy(g_pub_tx.reply,"D103");
		 		goto ErrExit;
			}

			if( tag==1 )
				ac_seqn = g_prdt_ac_id.ac_seqn;
			else 
				set_zd_long("0340",g_prdt_ac_id.ac_seqn);
		}
	}
	else 
	{
		ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", 
							 g_mdm_ac_rel.ac_id);
   		if( ret==100 )
		{
			sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
		 			 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"M003");
		 	goto ErrExit;
		}
		ac_seqn = g_prdt_ac_id.ac_seqn;
	}

	/*** ����Ψһ���ڻ����ߵ�λ����֤���������� ***/
	if( ( vmdm_attr.ac_num_ind[0]=='N' && tag==1) || 
		  !strncmp(g_mdm_ac_rel.mdm_code,"0007",4) )
	{
		ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d", 
			 		   g_mdm_ac_rel.ac_id,ac_seqn);
		if( ret==100 )
		{
		 	sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
					g_mdm_ac_rel.ac_id,ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
	 	}

		ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", 
						g_prdt_ac_id.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
			 		g_prdt_ac_id.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P145");
			goto ErrExit;
		}

 		if( g_td_mst.ac_sts[0]=='*' )
		{
			sprintf(acErrMsg,"���˻�����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}

		bal=g_td_mst.bal;
		strcpy(name,g_td_mst.name);
		strcpy(title,g_td_parm.title);
		strcpy(cur_no,g_td_parm.cur_no);
	
		memset( &vmdm_attr, 0x00, sizeof(struct mdm_attr_c) );

		/* ���ؽ������Ͳ˵� */
		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"a");

		ret = Mdm_prdt_rel_Dec_Sel( g_pub_tx.reply,"prdt_no='%s'", 
									 g_prdt_ac_id.prdt_no);
		if( ret ) goto ErrExit;

		while(1)
		{
			ret = Mdm_prdt_rel_Fet_Sel(&g_mdm_prdt_rel,g_pub_tx.reply);
			if( ret==100 ) break;
			else if( ret ) goto ErrExit;

			if( !strncmp(g_mdm_ac_rel.mdm_code,g_mdm_prdt_rel.mdm_code1,4) )
			{
				continue;
			}

			ret = Mdm_attr_Sel(g_pub_tx.reply,&vmdm_attr,"mdm_code='%s' and \
							   mdm_code<>'0016' and mdm_code<>'0020'", 
							   g_mdm_prdt_rel.mdm_code1);

			pub_base_strpack(vmdm_attr.mdm_name);
			pub_base_strpack(vmdm_attr.mdm_code);

			if( i==0 )  set_zd_data("1161",vmdm_attr.mdm_code);
			fprintf(fp,"1161|%s|%s|\n",vmdm_attr.mdm_name,vmdm_attr.mdm_code);
			i++;
		}
		Mdm_prdt_rel_Clo_Sel();
		fclose(fp);
		set_zd_data(DC_FILE_SND,"2");
		/*���ʲ˵��������*/

		/* ������� */
		set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
		set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
		set_zd_data("1148",g_mdm_ac_rel.id_type);
		set_zd_double("0420",bal);
		set_zd_data("0210",cur_no);
		set_zd_data("0240",g_td_mst.prdt_no);
		set_zd_data("0270",title);
		set_zd_data("1166",name);

		set_zd_data("0890",note_type);

                /*added by liuyong 2009-9-23 ƾ֤����*/
                set_zd_data("0610",g_mdm_ac_rel.note_no);
                sprintf(acErrMsg,"ԭʼƾ֤���룺[%s]",g_mdm_ac_rel.note_no);
                WRITEMSG
	}

	set_zd_data("1021",ac_no);

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
