/***************************************************************
* �� �� ��:     spJ005.c
* ����������    ���л�Ʊ��Ϣ¼��			
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-17 13:01
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"


#define LIMIT_MONEY 100.00


/*****����ǰ̨�õ���ֵ�ŵ����Լ������ݽṹ����*************/
int iGet_br_name(char *or_br_no, char *br_name);

spJ005()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	double money = 0.0;
	char cDeal_mode[2];/***ת������***/
	HV_FD123_AREA fd123;
	int    iExit = 0 ;	/**��¼�Ƿ���ڣ�1-����  0��������****/
	long ori_ac_id;
	
	vtcp_log("+++++++++[%s][%d]++++++++++\n",__FILE__,__LINE__);
	vtcp_log("+++++++++[%s][%d]FUCK!!!!\n",__FILE__,__LINE__);
	ori_ac_id=g_pub_tx.ac_id;
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	vtcp_log("+++++++++[%s][%d]shit!!!!\n",__FILE__,__LINE__);
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(&fd123,     0x00 ,  sizeof(fd123));

	/***********************
	 *PART 1 ��ǰ̨�õ�����*
	 ***********************/
	vtcp_log("+++++++++[%s][%d]+++++++\n",__FILE__,__LINE__);
	/**�õ����֧��ҵ��8583����***/
	/**���������漰�����ʺŷ����ֶβ���payin�ṹ��****/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		sprintf(acErrMsg,"��ǰ̨��ȡ����ʧ��!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG	
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	/**********************
        *PART 2 �����ж�      *
 	**********************/
	 /**1.����Ľ���Ϊ0 ����ĳ���޶�******/
	dot_str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),&money);
	if(money<LIMIT_MONEY)
	{
		sprintf(acErrMsg,"[%s][%d]�����������[%lf]!\n"
				,__FILE__,__LINE__,LIMIT_MONEY);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S042");
		goto ErrExit;
	}
	/****�жϸ��Ƿ�Ǽǹ�*****/
	/***����payin/notpayin�޷�����po_no,po_type�ֶ�,���Խ���Ҫ�ٶ�ȡ123��*/
	get_fd_data("1230",(char *)&fd123);
	get_zd_data("0720",cDeal_mode);

	vtcp_log("[%s][%d]fd123.po_ind==[%c]\n",__FILE__,__LINE__,fd123.po_type[0]);
	vtcp_log("[%s][%d]fd123.po_ind==[%c]\n",__FILE__,__LINE__,fd123.po_type[0]);
	vtcp_log("[%s][%d]fd123.po_ind==[%c]\n",__FILE__,__LINE__,fd123.po_type[0]);

	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));	
	/**Ʊ�ݺ���**/
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_payin.F_pay_opn_br_no);
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	
	ret = Hv_poinfo_Dec_Sel(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",			hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"�������ݿ����!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Hv_poinfo_Fet_Sel(&hv_poinfo,g_pub_tx.reply);
	if(ret!=100&&ret)
	{
		sprintf(acErrMsg,"ȡ�α����!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	/********¼��״̬ȡ���Ļ�Ʊ��Ȼ��������¼��
	else if(ret!=100)
	{
		sprintf(acErrMsg,"�ü�¼�Ѿ�����!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	**********************************/
	if(ret==100 &&!ret)
		iExit = 0;
	else if(ret==0)
		iExit = 1;
		
     /************************
     *PART 3 ׼��д���ݱ�   *
     ************************/	

	/****��¼������,��ʼд��Ʊ��Ϣ��****/
	dot_str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),&hv_poinfo.po_bal);	/***��Ʊ���**/
	MEMCPY_DEBUG(hv_poinfo.br_no,g_pub_tx.tx_br_no,sizeof(hv_poinfo.br_no)-1);
	/***ǩƱ��������**/
	hv_poinfo.bank_type[0] = '0';
        /**���л�Ʊ**/
	MEMCPY_DEBUG(hv_poinfo.or_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.or_br_no)-1);/***�������к�**/
	iGet_full_hv_no(hv_payin.F_pay_opn_br_no,"",hv_poinfo.pay_qs_no);
	vtcp_log("[%s][%d]hv_poinfo.pay_qs_no=[%s]\n",__FILE__,__LINE__,hv_poinfo.pay_qs_no);
	/***���������к�**/
	vtcp_log("[%s][%d]wt_date=[%s]\n",__FILE__,__LINE__,hv_payin.F_wt_date);
	hv_poinfo.l_tx_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
	/***��������***/
	MEMCPY_DEBUG(hv_poinfo.cur_id,"RMB",sizeof(hv_poinfo.cur_id)-1);
	/***�ϴν�������,�ŵ�ǰ����***/
	hv_poinfo.td_cnt    = 1;
	/***���ս��ױ���*/
	hv_poinfo.lost_cnt  = 0;
	/***��ʧ����***/
	MEMCPY_DEBUG(hv_poinfo.pay_ac_no,hv_payin.F_pay_ac_no,sizeof(hv_poinfo.pay_ac_no)-1);	/***�������ʺ�**/
	MEMCPY_DEBUG(hv_poinfo.pay_name,hv_payin.F_pay_name,sizeof(hv_poinfo.pay_name)-1);	/***����������**/
	MEMCPY_DEBUG(hv_poinfo.pay_addr,hv_payin.F_pay_addr,sizeof(hv_poinfo.pay_addr)-1);   /***�����˵�ַ***/
	MEMCPY_DEBUG(hv_poinfo.cash_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.cash_ac_no)-1);/***�տ����ʺ�**/
	MEMCPY_DEBUG(hv_poinfo.cash_name,hv_payin.F_cash_name,sizeof(hv_poinfo.cash_name)-1);/***�տ�������**/
	MEMCPY_DEBUG(hv_poinfo.hold_ac_no,hv_payin.F_cash_ac_no,sizeof(hv_poinfo.hold_ac_no)-1);/**��ǰ��Ʊ���ʺ�**/
	MEMCPY_DEBUG(hv_poinfo.hold_name,hv_payin.F_cash_name,sizeof(hv_poinfo.hold_name)-1);  /**��ǰ��Ʊ������**/
	hv_poinfo.reg_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
	/***�Ǽ�����***/
	hv_poinfo.po_ind[0]       = fd123.po_type[0];
	/***��Ʊ����***/
	hv_poinfo.deal_mode[0]    =cDeal_mode[0];
	/***�Ӻ�Ѻ��־***/
	hv_poinfo.add_pwd_ind[0] = '0';
	/***ת���ֽ���ʽ***/
	MEMCPY_DEBUG(hv_poinfo.cash_br_no,hv_payin.F_cash_opn_br_no,sizeof(hv_poinfo.cash_br_no)-1);/***�Ҹ����к�**/
	MEMCPY_DEBUG(hv_poinfo.op_tel,hv_payin.F_tel,sizeof(hv_poinfo.op_tel)-1);
	/*** ��������Ա**/
	MEMCPY_DEBUG(hv_poinfo.brf,   hv_payin.F_hv_brf,sizeof(hv_poinfo.brf)-1);
	/***����***/


	vtcp_log("[%s][%d]��ʼ�������ݱ�!\n",__FILE__,__LINE__);
	if(!iExit)/***�������¼��Ļ�Ʊ��Ҫ�����¼***/
	{
			vtcp_log("[%s][%d]��¼�ǲ����ڵ�!\n",__FILE__,__LINE__);
			hv_poinfo.po_sts[0] = HV_HP_REG;
			/***�ϴ��ʻ�״̬***/
			hv_poinfo.l_po_sts[0] =  HV_HP_UNREG;/***����Ϊ��Ч״̬,����ȡ��ʱ����***/
			/***��ǰ��Ʊ��״̬***/

			ret = Hv_poinfo_Ins(hv_poinfo,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"д��Ʊ������Ϣ�����ret=[%d]\n",ret);
				set_zd_data("0130",acErrMsg);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P009");
				goto ErrExit;
			}
	}
	else if(iExit==1)/***˵���˻�Ʊ�Ǵ��ڵ�***/
	{
			vtcp_log("[%s][%d]��¼�Ǵ��ڵ�!\n",__FILE__,__LINE__);
			if((hv_poinfo.l_po_sts[0]==HV_HP_REG || hv_poinfo.l_po_sts[0]==HV_HP_SIGN)  &&hv_poinfo.po_sts[0]==HV_HP_UNREG)
			{
					hv_poinfo.l_po_sts[0] = HV_HP_UNREG;
					/***�ϴ��ʻ�״̬***/
					hv_poinfo.po_sts[0] =HV_HP_REG;
					/***��ǰ��Ʊ��״̬***/
					ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
					if(ret)
					{
						vtcp_log("[%s][%d]�Ǽ�ȡ��״̬�Ļ�Ʊ���µǼǴ���!\n",__FILE__,__LINE__);
						sprintf(acErrMsg,"�Ǽ�ȡ��״̬�Ļ�Ʊ���µǼǴ���",50);
						set_zd_data("0130",acErrMsg);
						strcpy(g_pub_tx.reply,"P009");
						goto ErrExit;	
					}
			}	
			else
			{
					vtcp_log("[%s][%d]��״̬�Ļ�Ʊ���ǵǼ�ȡ�����޷����µǼ�!\n",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"P001");
					sprintf(acErrMsg,"��״̬�Ļ�Ʊ���ǵǼ�ȡ�����޷����µǼ�",50);
					set_zd_data("0130",acErrMsg);
					goto ErrExit;
			}
	}
	/**********************
     *PART 4 д��ˮ������ *
     **********************/ 
	/* д��ҵ����ˮ */
	vtcp_log("+++++++++[%s][%d]++++++++++\n",__FILE__,__LINE__);
	MEMCPY_DEBUG(g_pub_tx.beg_note_no,hv_poinfo.po_no,sizeof(g_pub_tx.beg_note_no));
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	/* ���Ӷ�ҵ��ί��ƾ֤�Ĵ���  added by liuxuan 2007-3-31*/
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_int("1022",&g_pub_tx.ac_seqn);
	g_pub_tx.ac_id=ori_ac_id;
	vtcp_log("��ʼƾ֤���� ac_no[%s] ac_id[%ld] type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);

	/******** ���ﻻ����ͬ־��001��д�ĺ���  20090910 chenhw 
	if(pub_sub_note())
	{
		sprintf(acErrMsg,"����ƾ֤����!");
		WRITEMSG
		goto ErrExit;
	}**/

	vtcp_log("��ʼƾ֤���� type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
	if ((memcmp(g_pub_tx.note_type,"005",3)==0) )
	{
		ret=J005_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
		if(ret)
		{
			sprintf(acErrMsg,"����ƾ֤����!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/*add by gong 2011/9/30 15:22:03 �������У����Ŵ�ϵͳ�޸� */
	char m_draw_no[25];
	memset(m_draw_no,'\0',sizeof(m_draw_no));
	WRITEMSG
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	vtcp_log("[%s][%d],draw_no=[%s]",__FILE__,__LINE__,m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='YH%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_date,hv_poinfo.po_no,m_draw_no);/*�����������*/
		if( ret < 0 )
		{
				sprintf( acErrMsg, "����draw_loan_hst�����!ret=[%d]",ret );
				WRITEMSG
				goto ErrExit;
		}
	}

	/*** ������� ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int J005_com_NoteUse(
 long    ac_id,
 int     ac_seqn,
 char    *note_type,
 char    *beg_no,
 char    *end_no,
 char    *tel)
{
	struct note_parm_c sNoteParm;

	int     ret;

	MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );

	/* ȡƾ֤������:����ƾ֤����ȡƾ֤�ṹ */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
		 sprintf(acErrMsg,"ȡƾ֤����note_parm���!note_type=[%s]",note_type);
		 WRITEMSG
		 strcpy( g_pub_tx.reply, "N001" );
		 return (-1);
	 }else if ( ret )
	 {
		 sprintf( acErrMsg, "ִ��Note_parm_Sel����![%d]", ret );
		 WRITEMSG
		  return(-1);
	}

        /* �ж��Ƿ�������������ֱ�ӷ�����ȷ */
        if ( sNoteParm.ctl_no_ind[0] == 'N' )
        {
                return(0);      /* ��������룬ֱ�ӷ�����ȷ0 */
        }
        sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
        WRITEMSG
        sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
        WRITEMSG

        /* �ж��Ƿ����������ͻ� */
        if ( sNoteParm.sell_ind[0] == 'N' )     /* ��������۸��ͻ� */
        {
                sprintf( acErrMsg, "ƾ֤���ܳ��۸��ͻ�" );
                        WRITEMSG
                        return(-1);

        }else if ( sNoteParm.sell_ind[0] == 'Y' )
        {
                ret = J005_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
                if ( ret )
                {
                        sprintf( acErrMsg, "���ͻ�ƾ֤��" );
                        WRITEMSG
                        return(-1);
                }
        }else
        {
                sprintf( acErrMsg, "ƾ֤������۱�־��sell_ind=[%s]",sNoteParm.sell_ind );
                WRITEMSG
                return(-1);
        }

	return(0);
}


int J005_com_CheqClsNo(long ac_id, int ac_seqn, char *note_type, char *cheq_beg_no, char *cheq_end_no)
{
        struct  note_cheq_mst_c sCheqMst;       /* ��¼�ͻ�ƾ̨֤�� */
        struct  note_mst_c      sNoteMst;
        char    sts[51];        /* ֧Ʊ״̬ */
        long    tx_date;        /* �������� */
        char    tw_br_no[6];/* �Է������� */
        char    tw_tel[6];      /* �Է���Ա�� */
        int     ret;

        MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
        MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );

        /* �����ʼ���롢��ֹ�����������,����ͷ�Ƿ���ͬ */
        ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
        if ( ret == -1 )
        {
                sprintf( acErrMsg, "���������,����ͷʧ��!ret=[%d]",ret );
                WRITEMSG
                return(-1);
        }
        /*******************************/

        /* �����ʻ�ID���֧Ʊ�Ƿ�Ϊ���ʺ����� */
        ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
        if( ret == -1 )
        {
                sprintf( acErrMsg, "�ͻ��޴�ƾ֤��ƾ֤״̬��һ��!ac_id=[%d],\
                         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
                         ac_id,note_type,cheq_beg_no, cheq_end_no );
                WRITEMSG
                return(-1);
        }

        if ( sts[0] == '0' )
        {
                sprintf( acErrMsg, "�ͻ��޴�ƾ֤!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N080" );
                return(-1);
        }else if ( sts[0] == '2' )
        {
                sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N081" );
                return(-1);
        }else if ( sts[0] == '3' )
        {
                sprintf( acErrMsg, "��ƾ֤�ѻ���!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N082" );
                return(-1);
        }else if ( sts[0] == '4' )
        {
                sprintf( acErrMsg, "��ƾ֤������!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N083" );
                return(-1);
        }else if ( sts[0] == '5' )
        {
                sprintf( acErrMsg, "��ƾ֤��ʹ��!sts=[%s]", sts);
                WRITEMSG
                strcpy( g_pub_tx.reply, "N084" );
                return(-1);
        }

        /*****************************/
	  /* ���ͻ�ƾ̨֤���и�֧Ʊ�����־��Ϊ'5'��ʹ��״̬�� */
 strcpy( sNoteMst.note_type, note_type );
 strcpy( sNoteMst.beg_note_no, cheq_beg_no );
 strcpy( sNoteMst.end_note_no, cheq_end_no );
 tx_date=g_pub_tx.tx_date; /*JYW JINZHONG20070421*/

  ret=pub_com_RegCheq( sNoteMst, ac_id, ac_seqn, tx_date, "G" );
 if( ret == -1 )
 {
                sprintf( acErrMsg, "�޸�֧Ʊ״̬λ����!ret=[%d]",ret );
                WRITEMSG
                return(-1);
        }


        /* ����ƾ֤���� */
        ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
        if ( ret == -1 )
        {
                sprintf( acErrMsg, "����ƾ֤������ret = [%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N010" );
                return(-1);
       }
        sCheqMst.cnt = ret ;
        sCheqMst.ac_id = g_pub_tx.ac_id;
        sCheqMst.ac_seq = g_pub_tx.ac_seqn;
        strcpy( sCheqMst.note_type, note_type );
        strcpy( sCheqMst.beg_note_no, cheq_beg_no );
        strcpy( sCheqMst.end_note_no ,cheq_end_no );

        /* �Ǽǿͻ�ƾ̨֤����ϸ C��ʾ�ͻ�ʹ��ƾ֤ */
        strcpy( tw_br_no , "" );
        strcpy( tw_tel, "" );

        ret=pub_com_RegCheqHst( "C", sCheqMst, tw_br_no, tw_tel );
        if( ret == -1 )
        {
                sprintf( acErrMsg, "�Ǽǿͻ�ƾ̨֤����ϸʧ��!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N021" );
                return(-1);
        }


        return 0;
}
