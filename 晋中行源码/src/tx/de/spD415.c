/***************************************************************
* �� �� ��: spD415.c
* ������������ע����
* ��    ��: 
* ������ڣ�2003��08��19��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "remark_ctl_c.h"

spD415()
{
	int ret = 0;
	int	i=0;
	struct	remark_ctl_c	rem;
	struct	remark_ctl_c	remark_ctl_c;
	char	msg[51];
	
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&rem, 0x00, sizeof(struct remark_ctl_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*** ȡ�������  ***/
	get_zd_data("0660", g_pub_tx.tmp_ind);
	get_zd_data("1011", g_pub_tx.ac_no);
	get_zd_int ("1012",&g_pub_tx.ac_seqn);
	get_zd_data("0860", g_pub_tx.id_no);
 	get_zd_data("0250", g_pub_tx.name);
	get_zd_double("1013",&g_pub_tx.tx_amt1);

vtcp_log("CHK tmp_ind=[%s] ac_no=[%s] seqn=[%d] name=[%s] id_no=[%s]",
g_pub_tx.tmp_ind,g_pub_tx.ac_no,g_pub_tx.ac_seqn,g_pub_tx.name,g_pub_tx.id_no);

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
					 g_pub_tx.ac_no);
	if( ret==100 )
	{
	  	sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼!");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P237");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	if ( g_pub_tx.tmp_ind[0]=='1' )
	{
		strcpy(rem.br_no,g_pub_tx.tx_br_no);
		strcpy(rem.ac_no,g_pub_tx.ac_no);
		rem.ac_id=g_mdm_ac_rel.ac_id;
		rem.ac_seqn=g_pub_tx.ac_seqn;
		rem.amt = g_pub_tx.tx_amt1;
		strcpy(rem.rem_code,g_pub_tx.id_no);
		strcpy(rem.rem , g_pub_tx.name);
		rem.tx_date = g_pub_tx.tx_date;
		strcpy(rem.tx_tel,g_pub_tx.tel);
		rem.rem_sts[0]='1';
		ret = Remark_ctl_Ins( rem , g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"���Ӽ�¼�� ");
			WRITEMSG
			goto ErrExit;
		}
		ret = sql_execute("UPDATE dd_mst SET ctl_amt=ctl_amt+%f \
			WHERE ac_id=%d AND ac_seqn=%d", \
			g_pub_tx.tx_amt1,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if ( ret )
		{
			sprintf(acErrMsg,"�޸ļ�¼�� ");
			WRITEMSG
			goto ErrExit;
		}
		ret = sql_execute("UPDATE td_mst SET ctl_amt=ctl_amt+%f \
			WHERE ac_id=%d AND ac_seqn=%d", \
			g_pub_tx.tx_amt1,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if ( ret )
		{
			sprintf(acErrMsg,"�޸ļ�¼�� ");
			WRITEMSG
			goto ErrExit;
		}
	}
	if ( g_pub_tx.tmp_ind[0]=='2' )
	{
		ret=Remark_ctl_Dec_Upd( g_pub_tx.reply,
			"ac_id=%ld AND ac_seqn=%d AND rem_code='%s' ",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.id_no );
		if( ret )
		{
			TRACE
			goto ErrExit;
		}
		ret=Remark_ctl_Fet_Upd( &remark_ctl_c,g_pub_tx.reply );
		if( ret )
		{
			strcpy(g_pub_tx.reply,"S049");
			TRACE
			goto ErrExit;
		}
		if( strcmp(remark_ctl_c.br_no,g_pub_tx.tx_br_no) )
		{
			sprintf( acErrMsg,"������[%s][%s]���ı�ע",
				remark_ctl_c.br_no,remark_ctl_c.tx_tel);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"BBCL");
			goto ErrExit;
		}
		remark_ctl_c.lst_date=g_pub_tx.tx_date;
		strcpy( remark_ctl_c.lst_tel,g_pub_tx.tel );
		remark_ctl_c.rem_sts[0]='2';

		ret=Remark_ctl_Upd_Upd( remark_ctl_c,g_pub_tx.reply );
		if( ret )
		{
			strcpy(g_pub_tx.reply,"S049");
			TRACE
			goto ErrExit;
		}
		Remark_ctl_Clo_Upd();

		/**
		ret = sql_execute("UPDATE remark_ctl SET lst_date=%d,lst_tel='%s', \
			rem_sts='2' WHERE ac_id=%ld and ac_seqn=%d AND rem_code='%s' ", \
			g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.id_no);
		if ( ret )
		{
			sprintf(acErrMsg,"�޸ļ�¼�� ");
			WRITEMSG
			goto ErrExit;
		}
		******/
		ret = sql_execute("UPDATE dd_mst SET ctl_amt=ctl_amt-%f \
			WHERE ac_id=%d AND ac_seqn=%d", \
			g_pub_tx.tx_amt1,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if ( ret )
		{
			sprintf(acErrMsg,"�޸ļ�¼�� ");
			WRITEMSG
			goto ErrExit;
		}
		ret = sql_execute("UPDATE td_mst SET ctl_amt=ctl_amt-%f \
			WHERE ac_id=%d AND ac_seqn=%d", \
			g_pub_tx.tx_amt1,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if ( ret )
		{
			sprintf(acErrMsg,"�޸ļ�¼�� ");
			WRITEMSG
			goto ErrExit;
		}
	}
	if ( g_pub_tx.tmp_ind[0]=='3' )
	{
		ret = Remark_ctl_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d AND rem_code!='%s' AND rem_sts='1'",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.id_no);

		memset(msg,0x00,sizeof(msg));
		strcpy(msg,"������ţ�");
		while(1)
		{
			ret = Remark_ctl_Fet_Sel(&rem,g_pub_tx.reply);
			if ( ret ) { strcat(msg,"��"); break;}
			if ( i>=3 ) { strcat(msg,"......");break;} else strcat(msg," ");
			strcat(msg,rem.rem_code);
			i++;
		}
		if ( i == 0 )
			set_zd_data("0130","����Ϊ��ѯ����");
		else
			set_zd_data("0130",msg);
	}

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ע����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ע����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
