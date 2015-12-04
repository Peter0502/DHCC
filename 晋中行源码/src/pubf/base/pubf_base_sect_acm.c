#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_sect_acm.c
* ����������
*             �ppubf_base_InsertSectAcm�ǼǷֶλ���
* ���ú���˵����
*               
*
* ��    ��:     rob
* ������ڣ�    2003��12��28��
*
* �޸ļ�¼��
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sect_acm_c.h"

/**********************************************************************
* ��������    pub_base_InsertSectAcm()
* �������ܣ�  ��ֶλ������в�������
* ����/ʱ�䣺 2003��12��28��
*
* ������
*     ���룺
*          �˻�ID				integer
*          �˻����				smallint
*          �˻�����				char(1)��1����2������������ 3���������ǷϢ������4���������ǷϢ������5�������������6��͸֧��
*          ��Ϣ����				integer
*          �Ǽ�����				integer
*          ��Ϣ����				decimal(19,2)
*          ����					decimal(8,6)
*     ���:
*          ��Ӧ��				char(4)
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_InsertSectAcm(long ac_id, int ac_seqn, char ac_type,
		  long ic_date, long wrk_date, double intst_acm,
		  double rate, char *reply)
{
    struct com_sect_acm_c com_sect_acm_c;
	int ret;

	if ( pub_base_CompDblVal( intst_acm , 0.00 )==0 )
	{
		sprintf(acErrMsg,"����Ϊ��,������!");
    	WRITEMSG
		return 0;
	}

    sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d],ac_type=[%c]",
                      ac_id,ac_seqn,ac_type);
    WRITEMSG
    
	MEMSET_DEBUG(&com_sect_acm_c, 0x0, sizeof(struct com_sect_acm_c));

	com_sect_acm_c.ac_id = ac_id;
	com_sect_acm_c.ac_seqn = ac_seqn;
	com_sect_acm_c.beg_date = ic_date;
	com_sect_acm_c.end_date = wrk_date;
	com_sect_acm_c.rate = rate;
	com_sect_acm_c.intst_acm = intst_acm;

    
	if (ac_type == '1')
	{
		com_sect_acm_c.intst_type[0] = '0';
	}
	else if (ac_type == '2')
	{
		com_sect_acm_c.intst_type[0] = '1';
	}
	else if (ac_type == '3')
	{
		com_sect_acm_c.intst_type[0] = '2';
	}
	else if (ac_type == '4')
	{
		com_sect_acm_c.intst_type[0] = '3';
	}
	else if (ac_type == '5')
	{
		com_sect_acm_c.intst_type[0] = '4';
	}
	else if (ac_type == '6')
	{
		com_sect_acm_c.intst_type[0] = '5';
	}
	else
	{
		
        sprintf(acErrMsg,"  ac_type error!! %d",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "W003" );
        return 1;

	}
	com_sect_acm_c.sts[0] = '0';

    ret = Com_sect_acm_Ins(com_sect_acm_c, reply);
    if (ret && ret != -239)
	{
		sprintf(acErrMsg, "INSERT com_sect_acm error %d", ret);
		WRITEMSG
		strcpy(reply, "D107");
		return 1;
	}
	else if (ret == -239)
	{
		sprintf(acErrMsg, "INSERT com_sect_acm error %d", ret);
		WRITEMSG
		strcpy(reply, "D108");
		return 1;
	}

	return 0;
}
