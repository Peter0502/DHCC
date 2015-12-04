/***************************************************************
* �� �� ��:     pubf_acct_loss.c
* ����������
*              1������˺š��˺�ID���˺���ŵĺϷ��ԣ�
*              2���Ǽǽ�����ˮ��
*
* ��ڲ�����	mode ��ʧ��ҷ�ʽ 11-�ڹң�12-��ң�13-�ܹң�14-��Ҽ��ܹ�
*								  15-ӡ�ң�16-�������ʧ��
*								  21-����ڹң�22-�����ң�23-����ܹ�
*								  24-����ܹҼ����
*				iAuto �Ƿ��Զ���� 0-������̨��ң�1-�����Զ����
*
* ��    ��:	xxx
* ������ڣ�    2003��01��20��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "mo_loss_c.h"
#include "mo_coll_c.h"
#include "com_branch_c.h"
#include "trace_log_c.h"

int	LOSS;	/*** 1-��ʧ��0-��� ***/
int  tmpcs=0;
int	 tmpcs1=0;

pub_acct_loss(int mode,int iAuto)
{
	struct	mo_coll_c		g_mo_coll;
	struct	com_branch_c	g_com_branch;

	char br_name[6];
	char tmpstr[6];
	g_reply_int=0;

	memset( &g_mo_coll, 0x00, sizeof(struct mo_coll_c) );
	memset( &g_com_branch, 0x00, sizeof(struct com_branch_c) );

	if( mode==11 ) strcpy(g_pub_tx.brf,"�ڹ�");
	else if( mode==12 ) strcpy(g_pub_tx.brf,"���");
	else if( mode==13 ) strcpy(g_pub_tx.brf,"�ܹ�");
	else if( mode==14 ) strcpy(g_pub_tx.brf,"�ܹҼ����");
	else if( mode==15 ) strcpy(g_pub_tx.brf,"ӡ��");
	else if( mode==16 ) strcpy(g_pub_tx.brf,"�������ʧ");
	else if( mode==21 ) strcpy(g_pub_tx.brf,"��ڹ�");
	else if( mode==22 ) strcpy(g_pub_tx.brf,"�����");
	else if( mode==23 ) strcpy(g_pub_tx.brf,"���ܹ�");
	else if( mode==24 ) strcpy(g_pub_tx.brf,"���ܹҼ����");
	else if( mode==25 ) strcpy(g_pub_tx.brf,"��ӡ��");

	if( mode>20 )												/*���*/
	{
		LOSS=0;
		if( mode%10==2 || mode%10==4 ) strcpy(tmpstr,"SGTS"); 	/*�����*/
		else if( mode%10==3 )	strcpy(tmpstr,"MGTS");			/*���ܹ�*/
		else	strcpy(tmpstr,"KGTS");							/*��ڹ�*/

		g_reply_int=pub_base_GetParm_int(tmpstr,1,&tmpcs);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ��������[%s]",tmpstr);
			WRITEMSG
			goto ErrExit;
		}
		g_reply_int=pub_base_GetParm_int("SGTS",2,&tmpcs1);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ��������[%s]",tmpstr);
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		LOSS=1;
	}

	g_reply_int=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
								g_pub_tx.ac_no);	
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"�˺Ų�����[%s][%d]",g_pub_tx.ac_no,g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	
	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);
	pub_base_strpack(g_mdm_ac_rel.id_type);				/*֤������*/
	pub_base_strpack(g_mdm_ac_rel.id_no);				/*֤������*/

	/*
	if( mode!=13 && mode!=23 && mode!=15 && mode!=25 &&
		!strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) )
	{
		sprintf(acErrMsg,"���˺�û�н��ʲ�������ʧ���![%s]",
				g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P187");
		goto ErrExit;
	}
	*/

	if( g_mdm_ac_rel.note_sts[0]=='*' || g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�˺��Ѿ�����![%s]",g_mdm_ac_rel.note_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}

	/*** Add commence by xxx ***/
	if( g_mdm_ac_rel.coll_sts[0]=='1' && LOSS==1 )
	{
		if( pub_base_checkcollyn( g_mdm_ac_rel.ac_no ) )
		{
			sprintf(acErrMsg,"���ü�ʰ��麯������");
			WRITEMSG
			goto ErrExit;
		}
	}

	/**���ڹ��ⲻ��ͨ��**/
	if( strncmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,5) 
	&& mode!=11 && mode!=21 ) 
		/* XRB LOSS==1 ) */
	{
		if( pub_base_getbrname( g_mdm_ac_rel.opn_br_no,br_name ) )
		{
			sprintf(acErrMsg,"ȡ����������!");
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack(br_name);
		strcpy(g_pub_tx.reply,"D209");
		sprintf(acErrMsg,"���׻���Ϊ[%s],��������Ϊ[%s]",g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no);
		set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		return(2);
	}
	/*** complete ***/
	
	if( (mode==13 || mode==14) && g_mdm_ac_rel.draw_pwd_yn[0]=='N' )
	{
		sprintf(acErrMsg,"�û������ܻ��������ܹ�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P436");
		goto ErrExit;
	}

	if( LOSS==1 )									/*1-��ʧ*/
	{
		if( pub_loss( mode ) )
		{
			sprintf(acErrMsg,"��ʧ��������!");
			WRITEMSG
			goto ErrExit;
		}
	}
	else if( pub_un_loss( mode, iAuto ) )				/*0-���*/
	{
		sprintf(acErrMsg,"��Һ�������!");
		WRITEMSG
		goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ʧ������ش���ɹ�!");
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
	
ErrExit:
	sprintf(acErrMsg,"��ʧ������ش���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int pub_loss( int mode )							/*��ʧ����*/
{
	struct mo_loss_c	mo_loss_c;
	struct mdm_ac_rel_c	f_mdm_ac_rel;

	memset(&mo_loss_c,0x00,sizeof(struct mo_loss_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	if( g_mdm_ac_rel.ac_no[0]=='5' && (mode==13 ||mode==14) )
	{
		sprintf(acErrMsg,"ac_no[%s]!",g_mdm_ac_rel.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P508");
		return 1;
	}
	if( g_mdm_ac_rel.ac_no[0]!='5' && mode==15 )
	{
		sprintf(acErrMsg,"ac_no[%s]!",g_mdm_ac_rel.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P506");
		return 1;
	}
	if( !strcmp(g_mdm_ac_rel.mdm_code,"0000") && mode!=15 && mode!=16 )
	{
		sprintf(acErrMsg,"ac_no[%s]!",g_mdm_ac_rel.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P507");
		return 1;
	}

	/*** ֤����� *** ǰ̨�̽����Ѿ������,����������������,ǰ̨û��֤����**/
	if(strcmp(g_pub_tx.tx_code,"4405")==0 && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)!=0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0 )
	{/**���۹�ʧ����У������--wangwenkui-20101230**/
		if (strcmp(g_mdm_ac_rel.id_no,g_pub_tx.id_no)
		|| strcmp(g_mdm_ac_rel.id_type,g_pub_tx.id_type) )
		{
			vtcp_log("������[%s][%s]",g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no);
			vtcp_log("�����[%s][%s]",g_pub_tx.id_type,g_pub_tx.id_no);
			sprintf(acErrMsg,"֤������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P186");
			return 1;
		}
	}
	/******************************************************/

	if( g_mdm_ac_rel.note_sts[0]=='1' )				/*������˻��ѹ�ʧ*/
	{
		g_reply_int=Mo_loss_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and unloss_ind='0' ",g_pub_tx.ac_no);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}

		g_reply_int=Mo_loss_Fet_Upd(&mo_loss_c,g_pub_tx.reply);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"��ʧ�Ǽǲ��޴˼�¼!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P188");
			return 1;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}

		/*** �ڹҿ��Զ�ι�ʧ ***/
		if( mode!=11 && (mode%10)==atoi(mo_loss_c.loss_ind) )
		{
			sprintf(acErrMsg,"���˻��Ѿ��������ֹ�ʧ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P190");
			return 1;
		}
		
		if( mo_loss_c.loss_ind[0]=='1' )
		{
			if( mode==11 || mode==12 || mode==13 || mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P501" );
				return 1;
			}
		}
		else if( mo_loss_c.loss_ind[0]=='2' )
		{
			if( mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P502" );
				return 1;
			}
		}
		else if( mo_loss_c.loss_ind[0]=='3' )
		{
			if( mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P503" );
				return 1;
			}
		}
		else if( mo_loss_c.loss_ind[0]=='4' )
		{
			if( mode==14 || mode==16 )
			{
				;
			}
			else
			{
				strcpy( g_pub_tx.reply,"P504" );
				return 1;
			}
		}

		if( mo_loss_c.loss_ind[0]=='4' && mode!=16)
		{
			sprintf(acErrMsg,"���˻��Ѿ���ʧ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P170");
			return 1;
		}
		else if( (mo_loss_c.loss_ind[0]=='2' || mo_loss_c.loss_ind[0]=='3')
			&&  mode!=14 && mode!=16 )
		{
			sprintf(acErrMsg,"���˻��Ѿ���ʧ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P170");
			return 1;
		}

		if( mode!=16 )
		{
			strcpy(mo_loss_c.unloss_ind,mo_loss_c.loss_ind);
			mo_loss_c.unloss_date=g_pub_tx.tx_date;
			strcpy(mo_loss_c.unloss_tel,g_pub_tx.tel);
			strcpy(mo_loss_c.unloss_chk,g_pub_tx.chk);
			strcpy(mo_loss_c.unloss_auth,g_pub_tx.auth);
			strcpy(mo_loss_c.new_ac_no,mo_loss_c.ac_no);
			mo_loss_c.wrk_date=g_pub_tx.tx_date;
			mo_loss_c.trace_no=g_pub_tx.trace_no;
		}
		else	/*** �������ʧ ***/
		{
			mo_loss_c.loss_date=g_pub_tx.tx_date;
			mo_loss_c.wrk_date=g_pub_tx.tx_date;
			mo_loss_c.trace_no=g_pub_tx.trace_no;
			strcpy(mo_loss_c.prop_book_loss,"1");
		}
		g_reply_int=Mo_loss_Upd_Upd(mo_loss_c,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}
		Mo_loss_Clo_Upd();	
	}
	else if( mode==16 )	/*** �û�û�й�ʧ,���������ʧ ***/
	{
		sprintf(acErrMsg,"�û�û�й�ʧ�����ܹ�ʧ������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P441");
		return 1;
	}

	/*** ��������� ***/
	/***Ӧ����Ҫ��ȥ��������--���Ŀ�--20101217
	TODO:�绰������Ҫ��������**/
	/**if( mode!=11 && mode!=13 && mode!=14 
		&& g_mdm_ac_rel.draw_pwd_yn[0]=='Y' ) 
	{
		if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0)   *���Ĳ���ҪУ�鿨���� Add by hxc 100620
			{
					sprintf(acErrMsg,"[%s][%d],ac_no[%s]�ǿ�,��ʧʱ���Ĳ���ҪУ������!",__FILE__,__LINE__,g_pub_tx.ac_no);
		      WRITEMSG
			}
		else
			{
					** �ܹһ���Ҽ��ܹ��ٹ�ʧ�������ʱ��У������ **
		      if(mode==16&&(mo_loss_c.loss_ind[0]=='3'||mo_loss_c.loss_ind[0]=='4')) 
		      {
			     ;
		      }
		      else if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
		     {
			     sprintf(acErrMsg,"���벻��!");
			     WRITEMSG
			     strcpy(g_pub_tx.reply,"M005");
			     return 1;
		      }
	     }
	}**/
	/**���Ӵ��۵绰���й�ʧ���뽻��wangwk-20101221**/
	vtcp_log("%s %d g_pub_tx.tx_code===[%s],_pub_tx.draw_pwd_yn[%s] ",
		__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.draw_pwd_yn);
	if(strcmp(g_pub_tx.tx_code,"4405") == 0
	   && strcmp(g_pub_tx.draw_pwd_yn,"N") != 0
	   && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) !=0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0 )
	{
		vtcp_log("%s %d ������� ",__FILE__,__LINE__);
	   if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
	 {
	     sprintf(acErrMsg,"���벻��!");
	     WRITEMSG
	     strcpy(g_pub_tx.reply,"M005");
	     return 1;
	  }
	}
	/**add end --wangwk**/
	if( mode!=16 )
	{
		strcpy(mo_loss_c.ac_no,g_pub_tx.ac_no);
		strcpy(mo_loss_c.id_type,g_pub_tx.tmp_ind);	/** ������֤������ **/
		strcpy(mo_loss_c.id_no,g_pub_tx.cal_code);	/** ������֤������ **/
		strcpy(mo_loss_c.name,g_pub_tx.name);
		sprintf(mo_loss_c.loss_ind,"%1d",mode%10);
		strcpy(mo_loss_c.prop_book_loss,"0");
		strcpy(mo_loss_c.unloss_ind,"0");
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		strcpy(mo_loss_c.opn_br_no,g_pub_tx.tx_br_no);
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		mo_loss_c.loss_date=g_pub_tx.tx_date;
		strcpy(mo_loss_c.loss_br_no,g_pub_tx.tx_br_no);
		strcpy(mo_loss_c.loss_tel,g_pub_tx.tel);
		strcpy(mo_loss_c.loss_chk,g_pub_tx.chk);
		strcpy(mo_loss_c.loss_auth,g_pub_tx.auth);
		mo_loss_c.unloss_date=0;
		strcpy(mo_loss_c.unloss_tel,"");
		strcpy(mo_loss_c.unloss_chk,"");
		strcpy(mo_loss_c.unloss_auth,"");
		strcpy(mo_loss_c.new_ac_no,"");
		mo_loss_c.wrk_date=g_pub_tx.tx_date;
		mo_loss_c.trace_no=g_pub_tx.trace_no;

		g_reply_int=Mo_loss_Ins(mo_loss_c,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P189");
			return 1;
		}

	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
  		if(g_reply_int)
		{
	   		sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_mdm_ac_rel.note_sts,"1");

		g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}

	return 0;
}

/**** flag=0 ������� flag=1 �ڹҺ͹�ʧ��֤�Զ���� ****/
/*--------------------------------------------------
* �޸�����: �����ͷ��ʧ�Զ����
*-------------------------------------------------*/
int pub_un_loss( int mode ,int flag )
{
	int	tmpts=0;
	char   br_name[6];
	char   f_loss_ind[2];					/*** ԭ��ʧ���� ***/
	struct mo_loss_c	mo_loss_c;
	struct mdm_ac_rel_c	f_mdm_ac_rel;
	char tmpstr[21];

	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&mo_loss_c,0x00,sizeof(struct mo_loss_c));
	vtcp_log("%s,%d,mode=[%d][%d]",__FILE__,__LINE__,mode,mode%10);
	sprintf(f_loss_ind,"%1d",mode%10);
	vtcp_log("f_loss_ind=[%s]",f_loss_ind);
	
	/**XRB***/
	{
		if( mode%10==2 || mode%10==4 ) strcpy(tmpstr,"SGTS"); 	/*�����*/
		else if( mode%10==3 )	strcpy(tmpstr,"MGTS");			/*���ܹ�*/
		else	strcpy(tmpstr,"KGTS");							/*��ڹ�*/

		g_reply_int=pub_base_GetParm_int(tmpstr,1,&tmpcs);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ��������[%s]",tmpstr);
			WRITEMSG
			return -1;
		}
		g_reply_int=pub_base_GetParm_int("SGTS",2,&tmpcs1);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ��������[%s]",tmpstr);
			WRITEMSG
			return -1;
		}
	}

	g_reply_int=Mo_loss_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and unloss_ind='0'",
								g_pub_tx.ac_no);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P189");
		return 1;
	}

	g_reply_int=Mo_loss_Fet_Upd(&mo_loss_c,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"��ʧ�Ǽǲ��޴˼�¼!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P188");
		return 1;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P189");
		return 1;
	}
	vtcp_log("%s %d tx_br_no ===[%s] ",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	pub_base_strpack(mo_loss_c.opn_br_no);
	/**���й�ʧ�����뵽���������**/
	/*if( strcmp(mo_loss_c.opn_br_no,g_pub_tx.tx_br_no) && mode!=21 )*/
	if( strcmp(mo_loss_c.opn_br_no,g_pub_tx.tx_br_no) && mode!=21 )
	{
		if( pub_base_getbrname( mo_loss_c.opn_br_no,br_name ) )
		{
			sprintf(acErrMsg,"ȡ����������!");
			WRITEMSG
			return 1;
		}

		pub_base_strpack(br_name);
		sprintf(acErrMsg,"�뵽ԭ��ʧ�������������![%s][%s]", \
				mo_loss_c.opn_br_no,g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D210");
		sprintf( acErrMsg,"��������Ϊ:(%s).",br_name);
		set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		return(2);
	}

	pub_base_strpack(mo_loss_c.id_type);
	pub_base_strpack(mo_loss_c.id_no);
			
	/**
	if( atoi(mo_loss_c.loss_ind)!=(mode%10) && flag==0 )
	**/
	if( atoi(mo_loss_c.loss_ind)!=(mode%10) )
	{
		sprintf(acErrMsg,"��ҷ�ʽ����![%s][%d]",mo_loss_c.loss_ind,mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P191");
		return 1;
	}

	/*����������*/
	tmpts=pub_base_CalTrueDays(mo_loss_c.loss_date,g_pub_tx.tx_date);
	if( (mode==22 || mode==24) && tmpts>=tmpcs && tmpcs1==0 && flag==0 )
	{
		sprintf(acErrMsg,"��ҽ�������ѹ�������ʧ��֤���ʧ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P192");
		return 1;
	}

	if( flag==1 && tmpts < tmpcs )
	{
		sprintf(acErrMsg,"δ����ʧ�������,���ܽ��![%ld]",mo_loss_c.loss_date);
		WRITEMSG
		sprintf(acErrMsg,"ts=[%d],tmpcs=[%d]",tmpts,tmpcs);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P198");
		return 1;
	}
	vtcp_log("%s,%d,ts=[%d],tmpcs=[%d]",__FILE__,__LINE__,tmpts,tmpcs);

	if (flag == 0)	/*----- �Զ���Ҳ����֤�������� -----*/
	{
		/*** ��������� ***/
		/**����֤����-wangwenkui-20110106**
		if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && ( mode==21 || mode==22
			|| ( (mode==23 || mode==24) && tmpts<tmpcs) ) ) 

		**�ܻ��������ұ���ƾ��**
		if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && (mode==21 || mode==22) )
		{
			if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0 )   **���Ĳ���ҪУ�鿨���� Add by hxc 100620**
				{
					sprintf(acErrMsg,"[%s][%d],ac_no[%s]�ǿ�,���ʱ���Ĳ���ҪУ������!",__FILE__,__LINE__,g_pub_tx.ac_no);
		      WRITEMSG
				}
		  else
		  	{ *** ��������� ***
					***Ӧ����Ҫ��ȥ��������--���Ŀ�--20101217
					TODO:�绰������Ҫ��������**
		  		***if( pub_base_DesChk(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
			     {
				     sprintf(acErrMsg,"���벻��!");
				     WRITEMSG
				     strcpy(g_pub_tx.reply,"M005");
				     return 1;
			      }
			     **
		     }
		}
		**/
		
	vtcp_log("%s,%d,ts=[%d],tmpcs=[%d]",__FILE__,__LINE__,tmpts,tmpcs);
	if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && (mode==23 || mode==24) )
	{
		if( tmpts<tmpcs )
		{
			if( pub_base_DesChk(g_pub_tx.tx_date,
				g_pub_tx.ac_no,g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd) )
			{
				sprintf(acErrMsg,"�����ʧ�ڹ�ʧ�����ڽ�ң�����ƾԭ����");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P505" );
				return 1;
			}
		}
	}

		/*** ֤����� ***/
		if( ( strcmp(mo_loss_c.id_no,g_pub_tx.cal_code)
			||  strcmp(mo_loss_c.id_type,g_pub_tx.tmp_ind) ) 
			&& ( strcmp(g_mdm_ac_rel.id_no,g_pub_tx.cal_code)
			|| strcmp(g_mdm_ac_rel.id_type,g_pub_tx.tmp_ind) )
			&& mo_loss_c.loss_date>=20050827 )
		{
			if( mode!=21 )
			{
			vtcp_log("[%s][%s]",mo_loss_c.id_type,mo_loss_c.id_no);
			vtcp_log("[%s][%s]",g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no);
			vtcp_log("[%s][%s]",g_pub_tx.tmp_ind,g_pub_tx.cal_code);
			sprintf(acErrMsg,"֤������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P186");
			return 1;
			}
		}
	}

	strcpy(mo_loss_c.unloss_ind,mo_loss_c.loss_ind);
	mo_loss_c.unloss_date=g_pub_tx.tx_date;
	strcpy(mo_loss_c.unloss_tel,g_pub_tx.tel);
	strcpy(mo_loss_c.unloss_chk,g_pub_tx.chk);
	strcpy(mo_loss_c.unloss_auth,g_pub_tx.auth);
	strcpy(mo_loss_c.new_ac_no,mo_loss_c.ac_no);
	mo_loss_c.wrk_date=g_pub_tx.tx_date;
	mo_loss_c.trace_no=g_pub_tx.trace_no;

	g_reply_int=Mo_loss_Upd_Upd(mo_loss_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��ʧ�Ǽǲ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P189");
		return 1;
	}
	Mo_loss_Clo_Upd();	
	vtcp_log("%s,%d,mode=[%d]", __FILE__, __LINE__, mode);
	/**ֻҪ�ڹ���ҵĵ�ʱ�������������--wangwenkui-20110106**/
	if( (mode==21 || mode==22 || mode==23 || mode==24) /**&& tmpts>=tmpcs**/ )
	{
		/*vtcp_log("%s,%d,draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
		pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.ac_no,g_pub_tx.draw_pwd);

		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										g_pub_tx.ac_no);
  		if(g_reply_int)
		{
	   		sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_mdm_ac_rel.note_sts,"0");
		vtcp_log("[%s][%d] Pwd_yn = [%s] \n",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
		/****���ܹ�ʱ�����ѡ��'N'(����)�������mdm_ac_rel �����־Ϊ'N'  modify by wanglei 20070517****/
		if(flag == 0)
			{
				if(g_pub_tx.draw_pwd_yn[0] == 'Y')
				{
					strcpy(f_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
					/**����Է����ܸ�Ϊ�����������־����-wangwk2010-3-23 17:13**/
					f_mdm_ac_rel.draw_pwd_yn[0] = 'Y';       
				}else{
					f_mdm_ac_rel.draw_pwd_yn[0] = 'N';
				}
			}
		/**** end  20070517 23:00****/

		g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}
	else
	{
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										g_pub_tx.ac_no);
  		if(g_reply_int)
		{
	   		sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_mdm_ac_rel.note_sts,"0");

		g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}

	if( g_reply_int )
	{
		sprintf(acErrMsg,"���½����˻����ձ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		return 1;
	}

	return 0;
}
