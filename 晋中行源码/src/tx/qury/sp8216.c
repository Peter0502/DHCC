/*************************************************
* �� �� ��:  sp8216.c
* ���������� ��ӡ���˴��֤��
*
* ��    ��:  �Ź���
* ������ڣ� 20140226
*
�������ã�
insert into tx_def values('8216','��ӡ���˴��֤��1','00000001000000000000111111111111110011111111111110000100010001100000000001000000000100000000000000000000000000000000000000000000','1','6','2');
insert into tx_flow_def values('8216',0,'8216','#$');
insert into tx_sub_def values('8216','��ӡ���˴��֤��1','sp8216','0','0');
ƾ֤���ã�
insert into mdm_attr (MDM_CODE, MDM_NAME, MDM_TYPE, AC_NUM_IND, DRAW_UNCON_YN, QRY_PWD_YN, DRAW_PWD_YN, DRAW_ID_YN, DRAW_SEAL_YN, PWD_MACH_YN, NOTE_TYPE, ONLY_IND, NO_IND, PRT_IND, PG_NUM, PG_LINE)
values ('0114', '���˴��֤��', '6', 'Y', 'O', 'O', 'O', 'O', 'O', 'O', '114', 'Y', 'N', 'N', 0, 0);
insert into note_parm (NOTE_TYPE, ACC_HRT, NAME, SELL_IND, DRAW_PWD_IND, DC_IND, CT_IND, CTL_NO_IND, ORDER_IND, CUR_NO, UNIT_AMT, DC_UNIT_MODE, USE_IND, CIF_TYPE, UNIT_CNT, OCP_CNT, BUY_UNIT, BUY_CNT, UNIT_LMT, CNT_LNT, HEAD_LEN)
values ('114', '70200', '���˴��֤��', 'N', 'N', 'N', '1', 'Y', 'Y', '01', 0, '0', '0', '1', 1, 1, 1, 1, 1, 1, 8);
insert into Dic_data (PR_CODE, PR_SEQ, CONTENT)
values ('note_type', '114', '���˴��֤��');
������
insert into com_def_chrg (TX_CODE, SEQN, CHRG_RATE_CODE, CHRG_AC_NO, CHRG_MODE, GET_IND, COND, AMT_NO, MO)
values ('8216', 1, '111', '0830', '0', '1', '1==1', '0400', '���˴��֤���շ�');
insert into mo_def_rate (CHRG_CODE, CHRG_TYPE, MIN_AMT, MAX_AMT, CHRG_RATE_NO)
values ('111', 0, 0.00, 99999999.00, '111');
insert into com_chrg_rate (CUR_NO, CHRG_RATE_CODE, BEG_DATE, END_DATE, DC_CODE, NAME, TYPE, VAL, MIN_AMT, MAX_AMT)
values ('01', '111', 20140401, 99991231, '511A', '���˴��֤������', '1', 20.00000, 20.00, 99999999.00);

*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"
#include "prdt_ac_id_c.h"

int sp8216()
{
	struct dd_mst_c vdd_mst;
	struct td_mst_c vtd_mst;
	struct dd_parm_c vdd_parm;
	struct td_parm_c vtd_parm;
	struct prdt_ac_id_c prdt_ac_id_c;
  struct mdm_ac_rel_c mdm_ac_rel;
	struct note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	char	who[6];				/* ʹ���� */
	char  cNoteSts[2];            	/* ƾ֤״̬ */
	char  cPNoteSts[2];            	/* ƾ֤״̬ */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	int	ret;
	char ac_no1[20] = {0};
	char ac_no2[20] = {0};
	char ac_no3[20] = {0};
	char ac_no4[20] = {0};
	char ac_no5[20] = {0};
	char t_name[41];
	char ac_sts[41];
	char parm_title[41];
	double  t_bal=0.00;
	long end_date=0;
	long opn_date=0;
	char cur_no[4]={0};

	memset(&sNoteMst, 0x00, sizeof(struct note_mst_c));
	memset(who, 0x00, sizeof( who));
	memset(cNoteSts, 0x00, sizeof(cNoteSts));
	memset(cPNoteSts, 0x00, sizeof(cPNoteSts));
	memset(cNoteAct, 0x00, sizeof(cNoteAct));
	
	pub_base_sysinit();

	get_zd_data("0300",ac_no1 );		/*���Ӻ���*/
	get_zd_data("0310",ac_no2 );		/*���Ӻ���*/
	get_zd_data("0320",ac_no3 );		/*���Ӻ���*/
	get_zd_data("0330",ac_no4 );		/*���Ӻ���*/
	get_zd_data("0370",ac_no5 );		/*���Ӻ���*/
	
	/*get_zd_data( "0020", g_pub_tx.tx_br_no );		   ���� */
	/*get_zd_data( "0070", g_pub_tx.tel );		  		 ��Ա */
	/*get_zd_long( "0040", &g_pub_tx.trace_no );		   ��ˮ */
	get_zd_data( "0890", g_pub_tx.note_type );		  /* ƾ֤���� */
	get_zd_data( "0580", g_pub_tx.beg_note_no );		/* ��ʼ���� */
	
	
	if(strlen(ac_no1) != 0)
	{
		memset(&mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ));
		memset(&prdt_ac_id_c,0x00,sizeof(prdt_ac_id_c));
		memset(&vdd_mst,0x00,sizeof(vdd_mst));
		memset(&vdd_parm,0x00,sizeof(vdd_parm));
		memset(&vtd_mst,0x00,sizeof(vtd_mst));
		memset(&vtd_parm,0x00,sizeof(vtd_parm));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' and note_sts<>'*' order by opn_br_no,ac_id,ac_seqn", ac_no1);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 	{
		 case '1':	/*����*/
		 
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				opn_date=vdd_mst.opn_date;
				strcpy(cur_no,vdd_parm.cur_no);
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,"����");
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*����*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				opn_date=vtd_mst.opn_date;
				strcpy(cur_no,vtd_parm.cur_no);
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,"����");
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"�޴��˺�����![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   }
   set_zd_data("0300",ac_no1);/**����**/
   set_zd_data("0250",parm_title);/**�������**/
   set_zd_data("0210",cur_no);/**����**/
   set_zd_double("0390",t_bal);/**���**/
   set_zd_long("0440",opn_date);/**������**/
	}
	if(strlen(ac_no2) != 0)
	{
		memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
		memset(&prdt_ac_id_c,0x00,sizeof(prdt_ac_id_c));
		memset(&vdd_mst,0x00,sizeof(vdd_mst));
		memset(&vdd_parm,0x00,sizeof(vdd_parm));
		memset(&vtd_mst,0x00,sizeof(vtd_mst));
		memset(&vtd_parm,0x00,sizeof(vtd_parm));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' and note_sts<>'*' order by opn_br_no,ac_id,ac_seqn", ac_no2);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 	{
		 case '1':	/*����*/
		 
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				opn_date=vdd_mst.opn_date;
				strcpy(cur_no,vdd_parm.cur_no);
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,"����");
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*����*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				opn_date=vtd_mst.opn_date;
				strcpy(cur_no,vtd_parm.cur_no);
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,"����");
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"�޴��˺�����![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   }
   set_zd_data("0310",ac_no2);/**����**/
   set_zd_data("0260",parm_title);/**�������**/
   set_zd_data("0220",cur_no);/**����**/
   set_zd_double("0400",t_bal);/**���**/
   set_zd_long("0450",opn_date);/**������**/
	}
	if(strlen(ac_no3) != 0)
	{
		memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
		memset(&prdt_ac_id_c,0x00,sizeof(prdt_ac_id_c));
		memset(&vdd_mst,0x00,sizeof(vdd_mst));
		memset(&vdd_parm,0x00,sizeof(vdd_parm));
		memset(&vtd_mst,0x00,sizeof(vtd_mst));
		memset(&vtd_parm,0x00,sizeof(vtd_parm));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' and note_sts<>'*' order by opn_br_no,ac_id,ac_seqn", ac_no3);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 	{
		 case '1':	/*����*/
		 
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				opn_date=vdd_mst.opn_date;
				strcpy(cur_no,vdd_parm.cur_no);
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,"����");
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*����*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				opn_date=vtd_mst.opn_date;
				strcpy(cur_no,vtd_parm.cur_no);
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,"����");
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"�޴��˺�����![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   }
   set_zd_data("0320",ac_no3);/**����**/
   set_zd_data("0270",parm_title);/**�������**/
   set_zd_data("0230",cur_no);/**����**/
   set_zd_double("0410",t_bal);/**���**/
   set_zd_long("0460",opn_date);/**������**/
	}
	if(strlen(ac_no4) != 0)
	{
		memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
		memset(&prdt_ac_id_c,0x00,sizeof(prdt_ac_id_c));
		memset(&vdd_mst,0x00,sizeof(vdd_mst));
		memset(&vdd_parm,0x00,sizeof(vdd_parm));
		memset(&vtd_mst,0x00,sizeof(vtd_mst));
		memset(&vtd_parm,0x00,sizeof(vtd_parm));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' and note_sts<>'*' order by opn_br_no,ac_id,ac_seqn", ac_no4);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 	{
		 case '1':	/*����*/
		 
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				opn_date=vdd_mst.opn_date;
				strcpy(cur_no,vdd_parm.cur_no);
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,"����");
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*����*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				opn_date=vtd_mst.opn_date;
				strcpy(cur_no,vtd_parm.cur_no);
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,"����");
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"�޴��˺�����![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   } 
   set_zd_data("0330",ac_no4);/**����**/
   set_zd_data("0380",parm_title);/**�������**/
   set_zd_data("0240",cur_no);/**����**/
   set_zd_double("0420",t_bal);/**���**/
   set_zd_long("0470",opn_date);/**������**/
	}
	if(strlen(ac_no5) != 0)
	{
		memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
		memset(&prdt_ac_id_c,0x00,sizeof(prdt_ac_id_c));
		memset(&vdd_mst,0x00,sizeof(vdd_mst));
		memset(&vdd_parm,0x00,sizeof(vdd_parm));
		memset(&vtd_mst,0x00,sizeof(vtd_mst));
		memset(&vtd_parm,0x00,sizeof(vtd_parm));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' and note_sts<>'*' order by opn_br_no,ac_id,ac_seqn", ac_no5);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 	{
		 case '1':	/*����*/
		 
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				opn_date=vdd_mst.opn_date;
				strcpy(cur_no,vdd_parm.cur_no);
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,"����");
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*����*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**��ȡ��Ʒ����**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				opn_date=vtd_mst.opn_date;
				strcpy(cur_no,vtd_parm.cur_no);
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,"����");
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"�޴��˺�����![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   } 
   set_zd_data("0370",ac_no5);/**����**/
   set_zd_data("0620",parm_title);/**�������**/
   set_zd_data("0580",cur_no);/**����**/
   set_zd_double("0430",t_bal);/**���**/
   set_zd_long("0280",opn_date);/**������**/
	}
	

	strcpy(g_pub_tx.end_note_no ,g_pub_tx.beg_note_no);		/* ��ֹ���� */
	strcpy( cNoteAct, "D" );		/* "D"��ʾʹ�� */
	strcpy(g_pub_tx.brf,"���˴��֤��");
	/* ����ƾ֤��Ϊ���ض�Ӧƾ֤״̬ */
	strcpy( who, "T" );			/* "T"��ʾ��Ա��Ϊ */
	strcpy( g_pub_tx.add_ind,"0" );
	strcpy( g_pub_tx.ac_no,"702" );
	
	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "����ƾ֤��Ϊ���ض�Ӧƾ֤״̬��" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ��Աƾ֤��� */
	ret=pub_com_ChkNoteUse( g_pub_tx.note_type );
	if( ret )
	{
		sprintf( acErrMsg, "��Աƾ֤����" );
		WRITEMSG
		goto ErrExit;
	}
	if( cPNoteSts[0]!='0' )				/* ƾ֤״̬ӦΪ���� */
	{
		sprintf( acErrMsg, "ƾ֤״̬��" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N022" );
		goto ErrExit;
	}
	sprintf( acErrMsg, "note_type='%s' and beg_note_no='%s'\
	and end_note_no='%s' and tel='%s'", g_pub_tx.note_type, g_pub_tx.beg_note_no,\
	g_pub_tx.end_note_no, g_pub_tx.tel );
	WRITEMSG
	/* �������ƾ֤�Ƿ����ڽ��׹�Ա */
	ret = pub_com_ChkNtExt( g_pub_tx.beg_note_no, g_pub_tx.end_note_no, \
				g_pub_tx.note_type, g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "����pub_com_ChkNtExt����!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ��Աƾ֤���Ŵ��� */
	ret=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn, g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "ƾ֤ʹ�ô����" );
		WRITEMSG
		goto ErrExit;
	}


GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
