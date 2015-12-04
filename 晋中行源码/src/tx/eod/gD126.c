/*************************************************************
* �� �� ��: gD126.c
* ��������������������������
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "ass_mst_c.h"
#include "mo_jl_bal_c.h"


gD126()
{
	int ret = 0;


    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();

    ret = pub_eod_odmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"����͸֧�ʻ����ļ�������������!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_cashmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ֽ�̨�ʵ�����������!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_inmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ڲ��ʻ����ļ�������������!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_assmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"���¿������ļ�������������!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_notemst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"����ƾ֤���ļ�������������!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_submst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�������ʺ�̨�ʵ�����������!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * �� �� ����   pub_eod_odmst_ysbal_upd
 * �������ܣ�   �޸�͸֧�ʻ����ļ����������
 * ����/ʱ�䣺  rob/20030315
********************************************************************/
int pub_eod_odmst_ysbal_upd()
{
    int ret;
    struct od_mst_c od_mst_c;
	int i=0;

	vtcp_log("BEGIN OD_MST");

    memset(&od_mst_c, 0x0, sizeof(struct od_mst_c));

    ret = Od_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Od_mst_Fet_Upd(&od_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

		i++;
		vtcp_log("[%d-%d,%d]",i,od_mst_c.ac_id,od_mst_c.ac_seqn);

		ret = pub_eod_ye_bal ( od_mst_c.ys_bal , od_mst_c.bal , 
			od_mst_c.ac_id , od_mst_c.ac_seqn);
		if(ret)		return ret;

        od_mst_c.ys_bal = od_mst_c.bal;
        ret = Od_mst_Upd_Upd(od_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Od_mst_Clo_Upd();

	vtcp_log("OVER  OD_MST");

    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_cashmst_ysbal_upd
 * �������ܣ�   �޸��ֽ�̨�ʵ��������
 * ����/ʱ�䣺  rob/20030315
********************************************************************/
int pub_eod_cashmst_ysbal_upd()
{
    int ret;
    struct cash_mst_c cash_mst_c;
	int i=0;

	vtcp_log( "BEGIN CASH_MST" );

    memset(&cash_mst_c, 0x0, sizeof(struct cash_mst_c));

    ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

		i++;
		vtcp_log("[%d-%s]",i,cash_mst_c.ac_no);
	
        cash_mst_c.ys_bal = cash_mst_c.bal;
        ret = Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Cash_mst_Clo_Upd();

	vtcp_log( "OVER  CASH_MST" );

    return 0;
}

/**********************************************************************
 * �� �� ����   pub_eod_inmst_ysbal_upd
 * �������ܣ�   �޸��ڲ��ʻ����ļ����������
 * ����/ʱ�䣺  rob/20030315
********************************************************************/
int pub_eod_inmst_ysbal_upd()
{
    int ret;
    struct in_mst_c in_mst_c;
	int i=0;

	vtcp_log("BEIGN IN_MST");

    memset(&in_mst_c, 0x0, sizeof(struct in_mst_c));

    ret = In_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = In_mst_Fet_Upd(&in_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

		i++;
		vtcp_log("[%d-%d,%d]",i,in_mst_c.ac_id,in_mst_c.ac_seqn);

		ret = pub_eod_ye_bal ( in_mst_c.ys_bal , in_mst_c.bal , 
			in_mst_c.ac_id , in_mst_c.ac_seqn);
		if(ret)		return ret;

        in_mst_c.ys_bal = in_mst_c.bal;
        ret = In_mst_Upd_Upd(in_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    In_mst_Clo_Upd();

	vtcp_log("BEIGN IN_MST");
    return 0;
}

/**********************************************************************
 * �� �� ����   pub_eod_assmst_ysbal_upd
 * �������ܣ�   �޸Ŀ���̨�ʵ��������
 * ����/ʱ�䣺  rob/20030315
********************************************************************/
int pub_eod_assmst_ysbal_upd()
{
    int ret;
    struct ass_mst_c ass_mst_c;
	int i=0;

	vtcp_log("BEGIN ASS_MST");
    memset(&ass_mst_c, 0x0, sizeof(struct ass_mst_c));

    ret = Ass_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Ass_mst_Fet_Upd(&ass_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

		i++;
		vtcp_log("[%d-]",i);
	        
        ass_mst_c.ys_bal = ass_mst_c.bal;
        ret = Ass_mst_Upd_Upd(ass_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Ass_mst_Clo_Upd();

	vtcp_log("OVER  ASS_MST");

    return 0;
}

/**********************************************************************
 * �� �� ����   pub_eod_submst_ysbal_upd 
 * �������ܣ�   �޸����ʺ�̨�ʵ��������
 * ����/ʱ�䣺  rob/20030711
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_submst_ysbal_upd()
{
    int ret;
	
	vtcp_log("BEGIN SUB_DD_MST");

	ret=sql_execute("update sub_dd_mst set ys_bal=bal where 1=1");
	if( ret )
	{
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
			strcpy(g_pub_tx.reply,"P434");
            return ret;
	}
	vtcp_log("OVER  SUB_DD_MST");

    return 0;
}
int pub_eod_notemst_ysbal_upd()
{
    int ret;

	vtcp_log("BEGIN NOTE_MST");
	ret=sql_execute("update note_mst set ys_cnt=cnt where ys_cnt!=cnt");
    if ( ret )
    {
        sprintf(acErrMsg,"����ƾ֤���ļ�������������!!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"N019");
        return ret;
    }
	vtcp_log("OVER  NOTE_MST");
}
