/*************************************************
* �� �� ��: sp9731.c
* ���������������ʺŻ����˻���ż���Ϣ[����ת��ȡ����]
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
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "mdm_attr_c.h"
#include "gl_mst_c.h"
#include "hv_brno_orno_c.h"/**YHBP**/

sp9731()
{
	int i=0,ret=0,flag=0,tag=0;
	long ac_seqn= 0;
	double bal;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61],tmp_inf[3];
	char ac_type[2],tmp_memo[100],tmpname[100],prdt_name[31];
	char node0[8];   /*��ʼ�ڵ�,��������ר�� add by hzh 20130604*/
	FILE *fp;
  char in_out='0'; /**0.��Ŀ  �ڲ��� 1.�Թ������ʻ�**/

  
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
  struct hv_brno_orno_c hv_brno_orno;/**YHBP**/

	struct com_item_c g_com_item;
	struct gl_mst_c     g_gl_mst;
	char spe_ind[21];
	char opn_br_no[6];/**YHBP**/
	struct mo_prdt_prot_c sMo_prdt_prot;
	struct mo_prot_ac_rel_c sMo_prot_ac_rel;
	struct dd_mst_c sDd_mst_virtual;
	memset(opn_br_no,'\0',sizeof(opn_br_no));
	memset(&hv_brno_orno, 0 , sizeof(hv_brno_orno));
	memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));

	memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
	memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    
	/**YHBP**/
	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(node0, 0x0, sizeof(node0));
	 
	/** ȡֵ����ֵ **/
	get_zd_data("0030",opn_br_no); /**YHBP**/
	get_zd_data("1021",ac_no);
	get_zd_data("0270",node0); /*��ʼ�ڵ�,��������ר�� add by hzh 20130604 */
	vtcp_log("%s,%d,��ʼ�ڵ�[%s]",__FILE__,__LINE__,node0);
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/
  vtcp_log("�ʺ�Ϊ��[%s]",ac_no);
	get_zd_data("102L",tmp_inf);				/* ժҪ */
  i=strlen(ac_no);
	if(i==3 || i==5 || i==7)							/*��Ŀ���ڲ��ʺ�*/
	{
    vtcp_log("�Կ�Ŀ�Ĵ���");
    in_out='0';
    set_zd_data("0690",&in_out);
    /**begin YHBP ���ݻ����Ų�ѯ�к�**/
    ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",opn_br_no);
    if( ret )
    {
        sprintf(acErrMsg,"�鿪�����кż�¼������!![%s]",ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"M003");
        goto ErrExit;
    }
    vtcp_log("�кţ�[%s]",hv_brno_orno.or_br_no);
    /**end YHBP**/
	 	/*���ݿ�Ŀ�Ų�ѯ��Ŀ����*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_no=[%s]", ac_no);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "��ѯ������Ŀ���쳣!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		if( g_com_item.acc_knd[0]!='3' )
		{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�����˻������ӿ�Ŀ!!acc_knd=[%s]",g_com_item.acc_knd);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P406");
		 	goto ErrExit;
		}

	 	/*���ݿ�Ŀ�Ų�ѯ���������*/
	 	ret=Gl_mst_Sel(g_pub_tx.reply,&g_gl_mst,"acc_hrt='%s'",
					   g_com_item.acc_hrt);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]",g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"��ѯ������Ŀ���쳣!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
                set_zd_double("1203",g_gl_mst.dr_bal);   /*���*/
		/*��Ŀ�������*/
		if(g_com_item.dc_ind[0]=='1')
		{
			set_zd_data("0220","��");
			set_zd_double("0410",g_gl_mst.dr_bal);

                        set_zd_double("0390",g_gl_mst.dr_bal);  /*��ǰ�跽���*/
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("0220","��");
			set_zd_double("0410",g_gl_mst.cr_bal);

                        set_zd_double("0390",g_gl_mst.cr_bal);  /*��ǰ�������*/
         	}
		else if(g_com_item.dc_ind[0]=='0')		/*˫��*/
		{
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("0220","��");
				set_zd_double("0410",g_gl_mst.dr_bal);
			}
			else if(ret==-1)
			{
				set_zd_data("0220","��");
				set_zd_double("0410",g_gl_mst.cr_bal);
			}
		}
		set_zd_data("0250",g_com_item.acc_name);       /*����*/
		set_zd_data("0210",g_gl_mst.cur_no);           /*����*/


   /**������FD120�ĸ�ֵ**/
                set_zd_data("1201",ac_no);
                set_zd_data("1202",g_com_item.acc_name); /*����*/
                                                         /*1203������������*/
                set_zd_data("1204",g_gl_mst.cur_no);     /*����*/
                set_zd_data("1205",  "2");          /*��ת��־2��ת 1 ��*/

		/** add.begin.gyf.20060920 Ϊ�˺ͼ���ʻ�ʱ����һ�£��������Ӷ�102�򲿷���Ϣ��ֵ **/
		set_zd_data("102H",g_com_item.acc_name);
                set_zd_double("102I",g_gl_mst.dr_bal);   /*���*/
                set_zd_data("102J",g_gl_mst.cur_no);     /*����*/
                set_zd_data("102K","2");             /*��ת��־2��ת 1 ��*/
		strcpy(cif_type,"2");	/** ���Թ������ʹ��� **/
		strcpy(ac_type,cif_type);
		set_zd_data("102M",ac_type);
		set_zd_data("1029","N");
		set_zd_data("1026","N");
		strcpy(prdt_name,"��Ŀ��:");
		strcat(prdt_name,g_com_item.acc_name);
		set_zd_data("102V",prdt_name);
		/** add.end.gyf.20060920 **/

		set_zd_data("0520",hv_brno_orno.or_br_no);/**YHBP �����к� **/
	}
else {
					
         ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
         if( ret )
         {
                sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"M003");
                goto ErrExit;
         }
         /**begin YHBP ���ݻ����Ų��к�**/
         set_zd_data("0880",g_mdm_ac_rel.draw_pwd_chg);
         
        /** begin Ϊ�������������ر���,lzy 20141226**/
				char net_ind[10]={0};
				get_zd_data("0100",net_ind); /**lzy 20141226**/
				vtcp_log("9731net_ind[%s]",net_ind);
				if ( memcmp(net_ind,"netb",4) == 0 )
				{
					set_zd_data("0860",g_mdm_ac_rel.opn_br_no);
				}
        /** end Ϊ�������������ر���,lzy 20141226**/

        ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
        if( ret )
        {
            sprintf(acErrMsg,"�鿪�����кż�¼������!![%s]",ac_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"M003");
            goto ErrExit;
        }
        vtcp_log("�к�: [%s]",hv_brno_orno.or_br_no);
        /**end YHBP ����**/
         /****����Ϊ����Ļ�������ر�־����ʾ��Ա add by wanglei 20070621****/
         ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id = %ld and ac_sts in('1','2','3','4','5') \
                  and (bal >0 or in_lo_intst >0 or out_lo_intst >0 or cmpd_lo_intst >0) ", g_mdm_ac_rel.ac_id);
         if(ret == 0)
         {
                  sprintf(acErrMsg,"������ʺ����б������Ϣδ��!!");
                  WRITEMSG
                  set_zd_data("0710","1"); /****������ʾ��־****/
         }
	/****ended 20070621****/

         ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
                                              g_mdm_ac_rel.ac_id);
         if( ret )
         {
                 sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"M003");
                 goto ErrExit;
         }

         if(prdt_ac_id_c.ac_id_type[0] == '1')
         {
                 in_out='2';
                 set_zd_data("0690",&in_out);
         }
         else if(prdt_ac_id_c.ac_id_type[0] == '9')
         {
                 in_out='1';
                 set_zd_data("0690",&in_out);
         }
/*	set_zd_int("1022",prdt_ac_id_c.ac_seqn);* ���μ� */

vtcp_log("in_out is [%c]",in_out);

	/** delete.begin.gyf.20060920
	if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') )
	delete.end.gyf.20060920 **/

	/** modify.gyf.20060920 ����ת�ʶ��ڲ��ʻ���֧�� **/
	if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') && prdt_ac_id_c.prdt_no[0]!='9' )
	{
		sprintf(acErrMsg,"�Ǵ�ȡ�����˺�! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
		goto ErrExit;
	}
	set_zd_data("0930",prdt_ac_id_c.ac_id_type);
	set_zd_data("0240",prdt_ac_id_c.prdt_no); /*�������ز�Ʒ��*/
	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					    g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ʲ�����! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** ���˻����ʵ���� ***/
	if( g_mdm_attr.ac_num_ind[0]=='N' )
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(ret<0)
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==1)
		{
			ac_seqn = prdt_ac_id_c.ac_seqn;
		}
		else if(ret>1)
		{
			set_zd_long("102N",prdt_ac_id_c.ac_seqn);
			ac_seqn = prdt_ac_id_c.ac_seqn;
			set_zd_long("1022",ac_seqn);
			tag = 1;
		}
	}
	else
		ac_seqn = g_mdm_ac_rel.ac_seqn;

	if( tag==0 )
	{
		/** begin.gyf.20060920
		if( prdt_ac_id_c.prdt_no[0]=='1' )
		{
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_dd_mst.name);
			cif_no = g_dd_mst.cif_no;
			bal = g_dd_mst.bal;

			if(g_dd_mst.ac_sts[0]=='*')
			{
				sprintf(acErrMsg,"���˺�������!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
							"prdt_no='%s'",g_dd_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);
		}
		else
		{
			sprintf(acErrMsg,"����������ʺţ������˻�������ת�ʷ�![%s]", \
					prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O227");
			goto ErrExit;
		}

		strcpy(ac_type,cif_type);
		set_zd_data("102M",ac_type);

		ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
									"cif_no=%ld",cif_no);
		if( ret )
		{
			sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "G102");
			goto ErrExit;
		}

		end.gyf.20060920 ***/

		/** add.begin.gyf.20060920 ���Ӷ��ڲ��ʻ���֧�� **/
		switch( prdt_ac_id_c.prdt_no[0])
		{
		case '1':
			ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_dd_mst.name);
			cif_no = g_dd_mst.cif_no;
			bal = g_dd_mst.bal;

			if(g_dd_mst.ac_sts[0]=='*')
			{
				sprintf(acErrMsg,"���˺�������!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
							"prdt_no='%s'",g_dd_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
				
	             /**1.���.�Ƿ����������˻�.**/
             vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_dd_mst.ac_id);
             ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_dd_mst.ac_id);
             if(ret == 100)
             {
                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 /**��������ǩԼ.����ԭ����.**/
             }
             else if(ret)
             {
                 sprintf(acErrMsg,"Mo_prdt_prot_Sel error ret=[%d]",ret);
                 WRITEMSG
                 strcpy( g_pub_tx.reply,"D103");
                 goto ErrExit;
             }
             else
             {
                 vtcp_log("[%s][%d]���˻� zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);

                 vtcp_log("[%s][%d] �ʺ�=[%s] �ʻ�ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_dd_mst.ac_id);
                 if(ret == 100)
                 {
                   /**������ǩԼ��Ϣ����.�������˻����չ�ϵ������.**/
                     sprintf(acErrMsg,"������ǩԼ��Ϣ����.�������˻����չ�ϵ������.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 
                 /**2.�������˻���Ϣ..ǩԼ�˻�,�����˻���Ϣ��ȡ��.**/
                 ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
                 if(ret == 100)
                 {
                     sprintf(acErrMsg,"dd_mst���������˻���Ϣ.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Dd_mst_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 bal=g_dd_mst.bal+sDd_mst_virtual.bal;
                               vtcp_log("[%s][%d]�����˻����bal=[%17.2f]",__LINE__,__FILE__,bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
             }
             
                 /* end 20130306*/
			strcpy(prdt_name,g_dd_parm.title);
			strcpy(cur_no,g_dd_parm.cur_no);
			strcpy(cif_type,g_dd_parm.cif_type);

			strcpy(ac_type,cif_type);
			set_zd_data("102M",ac_type);
			set_zd_double("0430",bal);
			set_zd_data("0730",g_dd_mst.od_ind);          /*͸֧��־*/

			ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
										"cif_no=%ld",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
			}
			set_zd_data("0720",g_dd_mst.hold_sts);/* �Ƿ񶳽� */
			vtcp_log("[%s][%d]��Цg_dd_mst.hold_sts==[%s]",__FILE__,__LINE__,g_dd_mst.hold_sts);
			break;
		case '2':
				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_td_mst.name);
			cif_no = g_td_mst.cif_no;
			bal = g_td_mst.bal;

			if(g_td_mst.ac_sts[0]=='*')
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
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(prdt_name,g_td_parm.title);
			strcpy(cur_no,g_td_parm.cur_no);
			strcpy(cif_type,g_td_parm.cif_type);

			strcpy(ac_type,cif_type);
			set_zd_data("102M",ac_type);
			set_zd_double("0430",g_td_mst.bal);

			ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
										"cif_no=%ld",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "G102");
				goto ErrExit;
			}
			set_zd_data("0720",g_td_mst.hold_sts);/* �Ƿ񶳽� */
			vtcp_log("[%s][%d]��Цg_td_mst.hold_sts==[%s]",__FILE__,__LINE__,g_td_mst.hold_sts);
			break;
		case '9':
			ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							prdt_ac_id_c.ac_id,ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_in_mst.name);
			bal = g_in_mst.bal;

			if(g_in_mst.sts[0]=='*')
			{
				sprintf(acErrMsg,"���˺�������!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P245");
				goto ErrExit;
			}

			ret = In_parm_Sel(g_pub_tx.reply,&g_in_parm,
							"prdt_no='%s'",g_in_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				goto ErrExit;
			}
			strcpy(prdt_name,g_in_parm.prdt_mo);
			strcpy(cur_no,g_in_parm.cur_no);
			strcpy(cif_type,"2");	/** ���Թ������ʹ��� **/
			strcpy(ac_type,cif_type);
			set_zd_data("102M",ac_type);
			set_zd_double("0430",g_in_mst.bal);
      set_zd_data("0730",g_in_mst.od_ind);          /*͸֧��־*/
			g_mdm_ac_rel.draw_id_yn[0] = 'N';	/** ��Ϊ�ڲ��ʻ�û��ֵ�����Ը�һ�� **/
			set_zd_data("0720","0");/* �Ƿ񶳽� */
			vtcp_log("[%s][%d]��Цg_td_mst.hold_sts==[%s]",__FILE__,__LINE__,g_td_mst.hold_sts);
			break;
		default:
			sprintf(acErrMsg,"����������ʺţ������˻�������ת�ʷ�![%s]", \
					prdt_ac_id_c.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O227");
			goto ErrExit;
		}


		/*** add.end.gyf.20060920 ***/
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);

		pub_base_strpack(name);
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);

       set_zd_data("0250",name);
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);
        /**���������������ﲻ��Ҫ�ı�39���ֵ����֤ƽ̨��39�����Ľ��׽��������� 20130604 hzh**/
        if(memcmp(node0, "nbanklvhv", 9)) 
		{
        set_zd_double("0390",bal);
    }
        /**���������������ﲻ��Ҫ�ı�39���ֵ����֤ƽ̨��39�����Ľ��׽��������� 20130604 hzh**/
    	set_zd_data("102C","N");						/*�Ƿ�֧��������*/
    	set_zd_data("102H",name);						/*����*/
        vtcp_log("[%s][%d]name==[%s]",__FILE__,__LINE__,name);
    	set_zd_data("102V",prdt_name);					/*��Ʒ����*/
		set_zd_data("102J",cur_no);						/*����*/
		set_zd_data("102K","1");						/*Ĭ����ת*/
                set_zd_data("0680","1");

		set_zd_double("102I",bal);						/*���*/
		set_zd_data("1024",g_mdm_ac_rel.note_no);		/*ƾ֤����*/
                set_zd_data("1025",g_mdm_ac_rel.draw_uncon_yn); /*�Ƿ�����֧ȡ*/
		set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn);	/*����֧ȡ*/
                set_zd_data("1027",g_mdm_ac_rel.qry_pwd);  /*��ѯ����*/
		vtcp_log("[%d]SPE_IND[%s]",__LINE__,spe_ind);
		set_zd_data("1028",g_mdm_ac_rel.draw_pwd); /*֧ȡ����*/
		set_zd_data("102U",spe_ind);
  		/*�����־*/
                set_zd_data("102B",g_mdm_ac_rel.draw_seal_yn); /*�Ƿ�ƾӡ��֧ȡ*/

		if(flag==1&&g_mdm_ac_rel.draw_id_yn[0]=='N')
		{
	 		set_zd_data("1029","N");                        /*֤��֧ȡ*/
			set_zd_data("102G",g_cif_id_code_rel.id_type);  /*֤������*/
			set_zd_data("102A",g_cif_id_code_rel.id_no);    /*֤������*/
		}
		else if(flag==1)
		{
			set_zd_data("1029","Y");                        /*֤��֧ȡ*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*֤������*/
			set_zd_data("102A",g_mdm_ac_rel.id_no);         /*֤������*/
		}
		else
		{
			set_zd_data("1029",g_mdm_ac_rel.draw_id_yn);    /*֤��֧ȡ*/
			set_zd_data("102G",g_mdm_ac_rel.id_type);       /*֤������*/
			set_zd_data("102A",g_mdm_ac_rel.id_no);         /*֤������*/
		}
        set_zd_data("0250",name);
        set_zd_data("0620",g_mdm_ac_rel.id_no);
        set_zd_data("0520",hv_brno_orno.or_br_no);/**YHBP �����к� **/
	}

}

	/***��ʼ���ز˵�***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	if( ac_type[0]=='2' )
	{
		if( strcmp(tmp_inf,"01" ) )	/* �г�ֵ */
		{
			ret=Dic_data_Sel( g_pub_tx.reply,&vdic_data,"pr_code='tran_memo' and pr_seq='%s' " , tmp_inf);
			if( ret==100 )
			{
				strcpy( tmp_inf , "01" );
			}else if(ret){
				goto ErrExit;
			}else{
				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);
				set_zd_data("102L",vdic_data.pr_seq);
				fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			}
		}
		if( strcmp(tmp_inf,"01")==0)
		{
			/* ���޳�ֵ */
			/***���ضԹ�ժҪ�б�***/
			ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='tran_memo'");
			if( ret ) goto ErrExit;

			while(1)
			{
				ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
				if( ret==100 ) break;
				if( ret ) goto ErrExit;

				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);
				if(i==0)
				{
					set_zd_data("102L",vdic_data.pr_seq);
					strcpy(tmp_memo,vdic_data.pr_seq);
				}

				fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
	}
    if( strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) && ac_type[0]=='1')
	{
		memset( &g_mdm_attr, 0x00, sizeof(struct mdm_attr_c) );
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

		/***ѡ���б�***/
		ret = Dic_data_Dec_Sel( g_pub_tx.reply, \
							 "pr_code='per_dety' and pr_seq='%s'",note_no);
		if( ret ) goto ErrExit;
		i=0;
		while(1)
		{
			ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
			if( ret==100 ) break;
			if( ret ) goto ErrExit;

			pub_base_strpack(vdic_data.pr_seq);
			pub_base_strpack(vdic_data.content);

			if( i==0 )
			{
				set_zd_data("1023",vdic_data.pr_seq);
				strcpy(note_no,vdic_data.pr_seq);
			}
			fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			i++;
		}
		Dic_data_Clo_Sel();
	}
	else
	{
		if( ac_type[0]=='2' )			/*���ضԹ�ȡ��ƾ֤����*/
		{
			/***ѡ���б�***/
			i=0;
			ret=Dic_data_Dec_Sel( g_pub_tx.reply, \
				"pr_code='pub_taty' and pr_seq[1,3]<>'001' order by pr_seq");
			if( ret ) goto ErrExit;

			while(1)
			{
				ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
				if( ret==100 ) break;
				if( ret ) goto ErrExit;

				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);

				if( i==0 )
				{
					strcpy(note_no,vdic_data.pr_seq);
					set_zd_data("1023",vdic_data.pr_seq);
				}
				fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***ѡ���б��������***/

	set_zd_data("1021",ac_no);

    double tmp_bal=0;
    get_zd_double("0410",&tmp_bal);
    if(tmp_bal<0){
        vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n",tmp_bal);
        tmp_bal=0;
        set_zd_double("0410",tmp_bal);
    }
    tmp_bal=0;
    get_zd_double("0390",&tmp_bal);
    if(tmp_bal<0){
        vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n",tmp_bal);
        tmp_bal=0;
        set_zd_double("0390",tmp_bal);
    }
    tmp_bal=0;
    get_zd_double("102I",&tmp_bal);
    if(tmp_bal<0){
        vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n",tmp_bal);
        tmp_bal=0;
        set_zd_double("102I",tmp_bal);
    }

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

