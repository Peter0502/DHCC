/*************************************************
* �� �� ��: spL542.c
* ���������������ѺƷ����
*
* ��    ��: chenchao
* ������ڣ�2011-8-4 11:01:39
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
insert into tx_def values('4543','����ѺƷ����','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','2','2');
insert into tx_sub_def values('L543','����ѺƷ����','spL543','1','0');
insert into tx_flow_def values('4543','0','L543','#$');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "ln_gage_reg_c.h"
#include "pact_gaga_rel_c.h"
#include "ln_mst_c.h"

int spL543()
{
	int	ret=0;
	char	cKinbr[BRNO_LEN+1];
	char	cPactno[20];
	int	iUpd_flag=0;
	double	intst=0.00;
	double	tmp_amt=0.00;
	double	dGaga_max_bal=0.00;
	double	dOut_bal=0.00;/*** ǰ̨��������  ***/
	
	struct	pact_gaga_rel_c		sPact_gaga_rel;
	struct	ln_gage_reg_c		sLn_gage_reg;
	struct	ln_mst_c		sLn_mst;

	memset(cKinbr  , 0 , sizeof(cKinbr));

	memset(&sPact_gaga_rel,0x00,sizeof(struct pact_gaga_rel_c));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	memset(cPactno,0x00,sizeof(cPactno)); 

	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG

	/** ȡֵ����ֵ **/
	get_zd_data("0030",cKinbr);
	get_zd_data("0370",cPactno);
	get_zd_double("0390",&dOut_bal);

	sprintf(acErrMsg,"ȡֵ����ֵ PASS!dOut_bal=[%.2f]cPactno=[%s]",dOut_bal,cPactno);
	WRITEMSG
	/* ��Ϊ������ȫ��ͨ���Ŵ����Ƶ���˲��ڰ����ص��ļ����� */
	/** �������� **/
	ret = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no = '%s'",cPactno);
	if (ret != 100 && ret != 0)
	{
		sprintf(acErrMsg,"��ѯ�����ѺƷ�Ǽǲ�����");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	} 
	
	while(1)	/** �п���һ�ʺ�ͬ���Զ�Ƚ��(����),��������Ҫȫ������ **/
	{
		tmp_amt=0.00;
		intst=0.00;
		memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
		ret = Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);	
		if(ret == 100)
		{
			if(iUpd_flag)
			{
				break;
			}
			sprintf( acErrMsg, "û�иñʺ�ͬ��Э��ĳ�����¼!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"û�иñʺ�ͬ��Э��ĳ�����¼!!!");
			strcpy( g_pub_tx.reply, "L353" );
			goto ErrExit;
		}else if(ret)
		{
			sprintf( acErrMsg, "ȡ�α����!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ȡ�α����!!!");
			strcpy( g_pub_tx.reply, "D102" );
			goto ErrExit;
		}
        	if(sLn_gage_reg.ac_id>0)
        	{
        		memset(&sLn_mst,0x00,sizeof(sLn_mst));
        		ret = Ln_mst_Sel(g_pub_tx.reply,&sLn_mst,"ac_id=%ld",sLn_gage_reg.ac_id);
        		if(ret)
			{
				sprintf( acErrMsg, "ȡ�α����!!!");
				WRITEMSG
				set_zd_data(DC_GET_MSG,"ȡ�α����!!!");
				strcpy( g_pub_tx.reply, "D102" );
				goto ErrExit;
			}
			tmp_amt=sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst+sLn_mst.bal;
			if(pub_base_CompDblVal(tmp_amt,0.00)>0 && sLn_mst.intst_type[0]!='0')
			{
				sprintf( acErrMsg, "δ���岻��������!!!");
				WRITEMSG
				set_zd_data(DC_GET_MSG,"δ���岻��������!!!");
				strcpy( g_pub_tx.reply, "L409" );
				goto ErrExit;
			}
			if (pub_base_CompDblVal(sLn_mst.intst_acm+sLn_mst.in_lo_acm+sLn_mst.out_lo_acm+sLn_mst.cmpd_lo_acm , 0) && sLn_mst.intst_type[0]!='0')
			{
				sprintf( acErrMsg, "�����л���δ����,��������[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L051");
				ERR_DEAL
			}
				/* �ֶλ�����Ϣ */
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "1" , &intst , 1 );
			if( ret ) goto ErrExit;
			if ( pub_base_CompDblVal(intst , 0.00) )
			{
				sprintf( acErrMsg, "������Ϣδ����,��������[%lf]",intst);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "2" , &intst , 1 );	
			if( ret ) goto ErrExit;
			if (pub_base_CompDblVal(intst , 0.00) )
			{
				sprintf( acErrMsg, "������Ϣδ����,��������[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "3" , &intst , 1 );
			if( ret ) goto ErrExit;
			if (pub_base_CompDblVal(intst , 0.00))
			{
				sprintf( acErrMsg, "������Ϣδ����,��������[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			
			ret = pub_base_Get_SectIntst( sLn_mst.ac_id , sLn_mst.ac_seqn , "4" , &intst , 1 );
			if( ret ) goto ErrExit;
			if (pub_base_CompDblVal(intst , 0.00))
			{
				sprintf( acErrMsg, "������Ϣδ����,��������[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L052");
				ERR_DEAL
			}
			/* ��Ϊ���Ҵ��� */
			if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf(acErrMsg,"���Ҵ���");
				WRITEMSG
			
				/* ��ѯ����Ƿ��� */
				ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_pub_tx.ac_id,g_pub_tx.ac_seqn );
				if (ret)
				{
					sprintf( acErrMsg, "�ð��Ҵ������Ƿ��,��������[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"L053");
					ERR_DEAL
				}		
			}
	
        	}
		/*���³����־*/
		sLn_gage_reg.amt=0.00;
		if(memcmp(g_pub_tx.tx_code,"4374",4)==0)
		{
			ret = sql_count("pact_gaga_rel"," pact_no='%s' and stsvar ='1' and (io_ind<>'120' or io_ind is null)",cPactno);
			if(ret==0)
			{
				strcpy(sLn_gage_reg.sts,"1");
			}else if(ret>0){
				strcpy(sLn_gage_reg.sts,"0");
			}else{
					sprintf( acErrMsg, "ͳ����������!!!");
					WRITEMSG
					strcpy( g_pub_tx.reply, "D102" );
					goto ErrExit;
			}
  	
		}else{
			strcpy(sLn_gage_reg.sts,"1");
		}

		sLn_gage_reg.out_tx_date=g_pub_tx.tx_date;
		sLn_gage_reg.out_trace_no=g_pub_tx.trace_no;
		ret=Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"���´����ѺƷ�Ǽǲ�����",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D105");
			Ln_gage_reg_Clo_Upd();
			goto ErrExit;
		}
		iUpd_flag++;
	}
	Ln_gage_reg_Clo_Upd();
	iUpd_flag = 0;
	if(memcmp(g_pub_tx.tx_code,"4374",4)==0)
	{
		ret=Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and stsvar='1' and io_ind='120' and pact_type='2'",cPactno);
	}else{
		ret=Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and stsvar='1'",cPactno);
	}
	if ( ret )
	{
		sprintf(acErrMsg,"�����ݺź͵���ѺƷ��ϸ������ϵ���α����");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	while (1)
	{
		memset(&sPact_gaga_rel,0,sizeof(sPact_gaga_rel));
		ret=Pact_gaga_rel_Fet_Upd(&sPact_gaga_rel,g_pub_tx.reply);
		if (ret && ret != 100)
		{
			sprintf(acErrMsg,"ȡ�ý�ݺź͵���ѺƷ��ϸ������ϵ���α����");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L329");
			Pact_gaga_rel_Clo_Upd();
			goto ErrExit;
		}else if(ret == 100)
		{	if(iUpd_flag)
			{
				break;
			}
			sprintf( acErrMsg, "û�иñʺ�ͬ��Э��ĳ�����¼!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"û�иñʺ�ͬ��Э��ĳ�����¼!!!");
			strcpy( g_pub_tx.reply, "L353" );
			goto ErrExit;
		}
		memcpy(sPact_gaga_rel.stsvar,"2", 1);
		ret=Pact_gaga_rel_Upd_Upd(sPact_gaga_rel,g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"���½�ݺź͵���ѺƷ��ϸ������ϵ�����");
			WRITEMSG
			strcpy(g_pub_tx.reply, "L345");
			Pact_gaga_rel_Clo_Upd();
			goto ErrExit;
		}
		dGaga_max_bal += sPact_gaga_rel.gaga_bal;
		iUpd_flag++;
	}
	Pact_gaga_rel_Clo_Upd();
	if(pub_base_CompDblVal(dOut_bal,dGaga_max_bal) && sPact_gaga_rel.pact_type[0] == '1')
	{
		vtcp_log("[%s][%d]�����Ѻ����,[%lf],gaga[%lf]",__FILE__,__LINE__,dOut_bal,dGaga_max_bal);
		strcpy(g_pub_tx.reply,"SN07");
		goto ErrExit;
	}

	/* ���� ��70400 */
	set_zd_data( "0210" , "01" );
	memcpy(g_pub_tx.cur_no,"01",2);
	g_pub_tx.tx_amt1 = dOut_bal;
	memcpy(g_pub_tx.ac_no,cPactno,sizeof(g_pub_tx.ac_no)-1);
	set_zd_double("0390",dOut_bal);
	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.add_ind , "0" );        /* ����:0��1��2���� */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.ct_ind , "2" );        /* ��ת:1��2ת */
	strcpy ( g_pub_tx.prt_ind , "0" );/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	g_pub_tx.svc_ind = 3061;            /* �����ѺƷ������ */
	strcpy ( g_pub_tx.brf,"��ѺƷ����" );
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS![%.2f]",dGaga_max_bal);
	WRITEMSG

	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG

	/*** д��ˮ ***/
	g_pub_tx.tx_amt1 = dGaga_max_bal;
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"д��ˮ����!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ѺƷ����ɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		memcpy(g_pub_tx.reply,"H034",4);
	}	
	sprintf(acErrMsg,"�����ѺƷ����ʧ�ܣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

