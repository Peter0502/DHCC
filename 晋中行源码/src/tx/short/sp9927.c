/*************************************************
* �� �� ��: sp9927.c
* ����������������ʾ�˺ź���Ų�ѯ���������[��������]
* ��    ��:  
* ������ڣ�2003��1��17�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

int sp9927()
{
    int ret=0;
	int	ac_seqn;
	char name[61],cur_no[3],note_no[5],cif_type[2],prdt_name[31];
	double bal=0.00;
	char spe_ind[7];
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct mdm_attr_c	g_mdm_attr;
   
    memset( spe_ind,'0',sizeof(spe_ind)-1 );
	spe_ind[6] = 0;
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	g_reply_int=0;

	pub_base_sysinit();

    get_zd_data("1021",g_mdm_ac_rel.ac_no);
    get_zd_int("1022",&ac_seqn);
    
vtcp_log("ac_no=[%s],ac_seqn=[%d]",g_mdm_ac_rel.ac_no,ac_seqn);
    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
								"ac_no='%s' and ac_seqn=%d", \
								g_mdm_ac_rel.ac_no,ac_seqn);
    if( g_reply_int==100 )
    {
		g_reply_int=0;
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_no='%s' and ac_seqn=%d ",g_mdm_ac_rel.ac_no,ALL_SEQN);
		if( ret==100 )
		{
        	sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
		}
    }

	if( g_reply_int || ret ) 
	{
       	sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���!! [%d][%d]",g_reply_int,ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	ret=Prdt_ac_id_Sel( g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_seqn=%d",
						g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret )
	{
       	sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���![%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}
vtcp_log("prdt_no=[%s]",prdt_ac_id_c.prdt_no);

 	if(strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE))
	{
		ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						 g_mdm_ac_rel.mdm_code);
		if( ret )
		{
		 	sprintf(acErrMsg,"��ѯ�������Ա����![%d]",ret);
					WRITEMSG
		 	strcpy(g_pub_tx.reply, "B004");
		 	goto ErrExit;
		}
	 	strcpy(note_no,g_mdm_attr.note_type);
 	}
 	else strcpy(note_no,"");

	switch(prdt_ac_id_c.prdt_no[0])
	{
		case '1': ret = sql_count("mdm_unprt_hst","ac_id=%ld and ac_seqn=%d", \
					 	 		   prdt_ac_id_c.ac_id,ac_seqn );
				  if( ret<0 )
				  {
					 sprintf(acErrMsg,"TRANSFER sql_count error" );
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"D103");
					 goto ErrExit;
				  }
				  else if( ret )
				  {
					 sprintf(acErrMsg,"���ʻ�����δ����ϸ�����Ȳ��Ǵ���!" );
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"D239");
					 goto ErrExit;
				  }

				 	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
									"ac_id=%ld and ac_seqn=%d", 
									prdt_ac_id_c.ac_id,ac_seqn);
					strcpy(name,g_dd_mst.name);
					bal=g_dd_mst.bal;

					if( g_dd_mst.ac_sts[0]=='*' )
					{
						sprintf(acErrMsg,"���˺�������!! [%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P245");
						goto ErrExit;
					}

					ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
									"prdt_no='%s'",g_dd_mst.prdt_no);
					strcpy(prdt_name,g_dd_parm.title);
					strcpy(cur_no,g_dd_parm.cur_no);
					strcpy(cif_type,g_dd_parm.cif_type);
					break;
		case '2':	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
									"ac_id=%ld and ac_seqn=%d", 
									prdt_ac_id_c.ac_id,ac_seqn);
					strcpy(name,g_td_mst.name);
					bal=g_td_mst.bal;

					if( g_td_mst.ac_sts[0]=='*' )
					{
						sprintf(acErrMsg,"���˺�������!! [%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P245");
						goto ErrExit;
					}

					ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
									"prdt_no='%s'",g_td_mst.prdt_no);
					if( ret )
					{
						sprintf(acErrMsg,"���Ҷ��ڲ������!! [%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P245");
						goto ErrExit;
					}

vtcp_log("tx_date=[%d],mtr_ate=[%d]",g_pub_tx.tx_date,g_td_mst.mtr_date);
					/***�����Ҫ�����ա���������ǰ֧ȡ***/
					if( g_td_parm.atterm_date_ind[0]=='Y' && \
						g_td_parm.fwd_draw_ind[0]=='Y' )
					{
						if( g_pub_tx.tx_date < g_td_mst.mtr_date )
							spe_ind[2]='1';
					}
vtcp_log("spe_ind=[%s]",spe_ind);

					strcpy(prdt_name,g_td_parm.title);
					strcpy(cur_no,g_td_parm.cur_no);
					strcpy(cif_type,g_td_parm.cif_type);
					break;
		default:
				sprintf(acErrMsg,"��Ʒ���ʹ���![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
	}

    if( ret )
    {
        sprintf(acErrMsg,"��ѯ�Է���������!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
    
vtcp_log("spe_ind=[%s]",spe_ind);
	pub_base_strpack(name);
    set_zd_data("102U",spe_ind); 					/*�����־*/
    set_zd_data("102H",name); 						/*����*/
    set_zd_data("102J",cur_no); 					/*����*/
    set_zd_double("102I",bal); 						/*���*/
    set_zd_data("102V",prdt_name); 					/*��Ʒ����*/
    set_zd_data("1023",note_no); 					/*ƾ֤����*/
    set_zd_data("1024",g_mdm_ac_rel.note_no); 		/*ƾ֤����*/
    set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn); 
    set_zd_data("1029",g_mdm_ac_rel.draw_id_yn); 
    set_zd_data("102G",g_mdm_ac_rel.id_no); 		/*֤������*/

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
