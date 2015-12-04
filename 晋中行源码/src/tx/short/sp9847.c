/*************************************************
* �� �� ��:  sp9847.c
* ���������� ������ʾ�˺ź��˻���Ų�ѯ���������__����ת��ȡ�ר��
*
* ��    ��:  
* ������ڣ� 2003��1��17�� 
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
#include "cif_id_code_rel_c.h"

int sp9847()
{
    int ret=0;
	int	ac_seqn;
	char name[51],cur_no[3],note_no[3];
	char cif_type[3],prdt_name[31];
	int flag=0;
	double bal=0.00;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct mdm_attr_c	g_mdm_attr;
	struct cif_id_code_rel_c	g_cif_id_code_rel;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	g_reply_int=0;

    get_zd_data("1021",g_mdm_ac_rel.ac_no);
    get_zd_int("1022",&ac_seqn);
    
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
								"ac_no='%s' and ac_seqn=%d ",
								g_mdm_ac_rel.ac_no,ac_seqn);
    if( g_reply_int==100 )
    {
		g_reply_int=0;
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
							"ac_no='%s' and ac_seqn=%d ",
							 g_mdm_ac_rel.ac_no,ALL_SEQN);
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

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
		"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret )
	{
       	sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���![%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
							"cif_no=%ld",prdt_ac_id_c.cif_no);
	if( ret==100 )
	{
       	sprintf(acErrMsg,"�ÿͻ�������!");
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "C007");
       	goto ErrExit;
	}

	if(strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE))
	{
		ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,
						"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
		if( ret )
		{
       		sprintf(acErrMsg,"��ѯ�������Ա����![%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply, "B004");
       		goto ErrExit;
		}
		strcpy(note_no,g_mdm_attr.note_type);
		vtcp_log("WWWWWWWWWWWWWWWWnote_no=[%s]",note_no);
	}
	else strcpy(note_no,"");

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", 
						prdt_ac_id_c.ac_id,ac_seqn);
    	if( ret )
    	{
        	sprintf(acErrMsg,"��ѯ�Է���������!! [%d]",ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "B005"); 
			goto ErrExit; 
		}
		strcpy(name,g_dd_mst.name);
		bal=g_dd_mst.bal;

		ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						  g_dd_mst.prdt_no);
    	if( ret )
    	{ 
			sprintf(acErrMsg,"��ѯ�Է���������!! [%d]",ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply, "B005"); 
			goto ErrExit; 
		}
		strcpy(prdt_name,g_dd_parm.title);
		strcpy(cur_no,g_dd_parm.cur_no);
		strcpy(cif_type,g_dd_parm.cif_type);
	}
	else
	{
		sprintf(acErrMsg,"����������ʺţ������˻�������ת�ʷ�!![%s]", \
				prdt_ac_id_c.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O227");
		goto ErrExit;
	}
    
	pub_base_strpack(name);
	vtcp_log("name=[%s],bal=[%lf]",name,bal);
	vtcp_log("pwd_yn=[%s]",g_mdm_ac_rel.draw_pwd_yn);
	vtcp_log("id_type=[%s]",g_mdm_ac_rel.id_type);
	vtcp_log("id_no=[%s]",g_mdm_ac_rel.id_no);
	vtcp_log("mach_yn=[%s]",g_mdm_ac_rel.pwd_mach_yn);

    set_zd_data("102H",name); 							/*����*/
    set_zd_data("102J",cur_no); 						/*����*/
    set_zd_data("102V",prdt_name); 						/*��Ʒ����*/
    set_zd_data("102K","1"); 							/*Ĭ����ת*/
    set_zd_double("102I",bal); 							/*���*/
    set_zd_data("1023",note_no); 						/*ƾ֤����*/
    set_zd_data("1024",g_mdm_ac_rel.note_no); 			/*ƾ֤����*/
    set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn); 		/*�Ƿ�����֧ȡ*/

   	set_zd_data("1029",g_mdm_ac_rel.draw_id_yn ); 		/*�Ƿ�֤��֧ȡ*/
   	set_zd_data("102G",g_mdm_ac_rel.id_type); 			/*֤������*/

    set_zd_data("102C",g_mdm_ac_rel.pwd_mach_yn); 		/*�Ƿ�������֧ȡ*/
   	set_zd_double("102F",g_td_mst.td_amt); /*�汾ȡϢ�����ȡ��ÿ�η�����*/
	
	set_zd_data("102M",cif_type);    		           /*�ʻ�����*/
		vtcp_log("YYYYYYYYYYYYYYYcif_type=[%s]",cif_type);

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
