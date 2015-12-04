#include "find_debug.h"
/***************************************************************
* �� �� ��:     pubf_base_ac.c
* ����������
*              1��pub_base_CrtAcChkb�����˺�У��λ��
*              2��pub_base_CrtChkbAr�����˺�У��λ�㷨;
*              3��pub_base_CrtCifAc���ɿͻ���ʾ�˺ţ�
*              4��pub_base_CrtInAc�����ڲ����˺ţ�
*              5��pub_base_CrtAcSeqn	�����˻���ţ�
*
*			   6.pub_base_CurGetddFile
*							(����)����ת�����˻����
*			   7.pub_base_CurToSeqn
*							���ݱ��֡���ʾ�˺�ȡ�������ļ�
*			   8  pub_base_disac_file
*							������ʾ�˺�/���ȡ�����ļ�����Ʒ����
*			   9  pub_base_currentdis_file
*							���ݻ�����ʾ�˺ź��˺����ȡ�������ļ�����
*			  10  pub_base_termidis_file
*							���ݶ�����ʾ�˺ź��˺����ȡ�������ļ�����
*			  12  pub_base_currentid_file
*							���ݻ����˺�IDȡ�������ļ�����
*			  13  pub_base_termiid_file
*							���ݶ����˺�IDȡ�������ļ�����
*			  14  pub_base_idseqn_file
*   						�����˺�ID���˻���ŷ����˻����ͣ���ȡ���ļ�
*			  15  pub_base_actoseqn
*							�����ʺŷ����ʻ����
*			  16  pub_base_acgetname
*							�����˺ŵõ��ͻ��źͻ���
*			  17  pub_base_CrtDiscntAc	���������˺ţ�
*
*			  18  pub_base_ac_prdt()	�����ʺţ��ʺ����ȡ��Ʒ����
*
*			  19  pub_base_Exist_MainCrd()  �ж��Ƿ��������
*
*			  20  pub_base_opn_sub_ac()	�������ʺ�
*
*			  21  pub_base_ins_sub_ac_hst() �Ǽ����ʺ���ϸ̨��
*
*			  22  pub_base_sub_ac_trance()  �ӽ��״�ȡ������
*
*			  23  pub_base_getdd_mst_parm( ) �����˻�ȡ���ļ��Ͳ���

                          24  pub_base_old_acno()
* ��    ��:
* ������ڣ�    2003��12��16��
*
* �޸ļ�¼��
* 1. ��    ��: 	2003/12/30
*    �� �� ��: 	rob	/ lance
*    �޸�����: 	���� 5 - 11
*
**************************************************************/
#define ERR_DEAL if(ret){\
			sprintf( acErrMsg,"SQL ERROR[%d]",ret);\
			WRITEMSG\
			goto ErrExit;\
			}

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"				/*��Ʒ�ʺŶ��ձ�*/
#include "ac_seqn_id_c.h"				/*�˻����ID��*/
#include "cif_basic_inf_c.h"
#include "com_item_c.h"
#include "rsp_code_c.h"
#include "in_ac_seqn_c.h"
#include "mdm_attr_c.h"
#include "mo_coll_c.h"
#include "com_branch_c.h"
#include "mo_merge_ac_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "dc_acc_rel_c.h"

struct prdt_ac_id_c g_prdt_ac_id;		/*��Ʒ�ʺŶ��ձ�ṹ*/

#define     ACNO_LEN   13              /* �ͻ��ʺų��� */
#define     INACNO_LEN 13              /* �ڲ��ʺų��� */
#define     CARDNO_LEN 19              /* ���ʺų���   */
#define     CIFNO_LEN  8               /* �ͻ��ų���   */
#define     CHKBT_LEN  1               /* У��λ����   */

static int ret=0;
/**********************************************************************
* ��������    pub_base_CrtAcChkb
* �������ܣ�    �����˺�У��λ
* ����/ʱ�䣺    2003��12��16��
*
* ������
*     ���룺 acno       ����У����˺�
*           int mode   : �ʺŽ���(1 �ͻ��ʺ� 2 �ڲ��ʺ� 3 ���ʺ� 4 �ͻ���)
*           chkb_len   : У��λ�ĳ���
*
*     ����� chkb_acno  : ��У��λ���ʺ�
*
* �� �� ֵ: 0 �ɹ�,1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_CrtAcChkb( char *acno, char *chkb_acno, int mode, int chkb_len )
{
    int Des_Code ;      /* ���ص�У��λֵ */

    char fmtstr[10];    /* У��λ�����ʽ,ǰ�油0 */

    char Des_Str[4];    /* У��λ */

	char cAcno[32];
	int iLen = 0;

    /* ��������У��λ�㷨����У��λ */
    switch ( mode )
    {
        case 1 :  /* �ͻ��ʺ� */
            Des_Code = pub_base_CrtChkbAr( acno , ACNO_LEN - CHKBT_LEN );
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        case 2:  /* �ڲ��ʺ� */
            Des_Code = pub_base_CrtChkbAr( acno , INACNO_LEN - CHKBT_LEN );
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        case 3:  /* ���ʺ� */
			/*** modify by dgf ***/
			MEMSET_DEBUG( cAcno, 0x00, sizeof(cAcno) );
			strcpy( cAcno,acno );
			pub_base_strpack ( cAcno );
			iLen = strlen( cAcno );
            Des_Code = pub_base_CrtChkbAr( acno , iLen);

            /*Des_Code = pub_base_CrtChkbAr( acno , CARDNO_LEN - CHKBT_LEN );*/
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        case 4 :  /* �ͻ��� */
            Des_Code = pub_base_CrtChkbAr( acno , CIFNO_LEN - CHKBT_LEN );
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        default:  /* ���󷵻� */
            return (1);
            break;
    }

    /* ��У��λ�ӵ��ʺź��� */
    sprintf(acErrMsg,"������У����Ϊ [%d]",Des_Code);
    WRITEMSG

    sprintf( fmtstr , "%%0%dd" , chkb_len );
    sprintf(acErrMsg,"������fmtstrΪ [%s]",fmtstr);
    WRITEMSG

    sprintf( Des_Str , fmtstr , Des_Code );
    sprintf(acErrMsg,"������Des_StrΪ [%s]",Des_Str);
    WRITEMSG

    strncat( acno , Des_Str , chkb_len );
    sprintf(acErrMsg,"������acnoΪ [%s]",acno);
    WRITEMSG

    sprintf(acErrMsg,"00000�ͻ���Ϊ [%s]",chkb_acno);
    WRITEMSG
    sprintf( chkb_acno ,"%s", acno );
    sprintf(acErrMsg,"aaaaaaaaaaa");
    WRITEMSG
    sprintf(acErrMsg,"�ͻ���Ϊ [%s]",chkb_acno);
    WRITEMSG
    return 0;
}

/**********************************************************************
* �� �� ����    pub_base_CrtChkbAr
* �������ܣ�    ����У��λ�㷨(�ͻ��ʺš��ڲ��ʺš�����У��λ��������㷨)
* ����/ʱ�䣺    2003��12��16��
*
* ������
*     ���룺char * acno     ��У��λ���˺�
*          int len         ����У��λ���ʺų���
*
*     �����check_num:У��λ
*
* �� �� ֵ: >=0 �ɹ� ��<0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_CrtChkbAr( char * acno , int len )
{
    register     i, j;
    int          sum = 0;
    /*char      slstring[len+CHKBT_LEN+1];  *len+CHKBT_LEN��У��λ���ʺų���*/
    char      slstring[256];
    int          check_num;

	sprintf(acErrMsg,"acno[%s],len[%d]",acno,len);
    WRITEMSG

    MEMSET_DEBUG( slstring, 0x00, strlen(slstring) );

	sprintf(acErrMsg,"acno[%s],len[%d]",acno,len);
    WRITEMSG

    MEMCPY_DEBUG(slstring, acno, len);
    slstring[len] = '\0';

    for (i = len - 1, j = 1; i >= 0 ; i --, j ++) {
       if ((j % 2) != 0)
         sum = sum + (((slstring[i] - 48) >= 5)?
                   ((slstring[i] - 48 - 5) * 2 + 1):
                          ((slstring[i] - 48) * 2));
       else
         sum = sum + (slstring[i] - 48);
    }
      check_num = (((sum % 10) == 0)? 0: (10 - (sum % 10)));

    sprintf(acErrMsg,"У����Ϊ [%d]",check_num);
    WRITEMSG

   return(check_num);
}

/**********************************************************************
* ��������    pub_base_CrtCifAc
* �������ܣ�    ���ɿͻ����˺ţ���У��λ��
* ����/ʱ�䣺    2003��12��16��
*
* ������
*     ���룺cif_no    :    �ͻ���
*
*     �����acno            :    ���ݿͻ������ɵĴ�У����˺�
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_CrtCifAc(int cif_no, char *acno)
{
    char    s_acno[21];       /* ����У��λ���˺� */
    char    s_tmp1[65];     /* ��ʱ����1 */
    char    s_tmp2[65];     /* ��ʱ����2 */
    int    ac_seqn;     /* �ͻ��ŵ����˳��� */
    int    ret;
	struct cif_basic_inf_c	cif_basic_inf_c;
	MEMSET_DEBUG(&cif_basic_inf_c,0x00,sizeof(struct cif_basic_inf_c));

	sprintf(acErrMsg,"�ͻ���![%d]",cif_no);
	WRITEMSG
    /* ���ݿͻ���ȡ���˳��� */
	g_reply_int=Cif_basic_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",cif_no);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ�쳣![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P135");
		return 1;
	}

	g_reply_int=Cif_basic_inf_Fet_Upd(&cif_basic_inf_c,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ������![%d][%ld]", \
				g_reply_int,cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P134");
		return 1;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ�쳣![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P135");
		return 1;
	}

    /* ˳���+1 */
	cif_basic_inf_c.cif_seqn++;

    /* �����ַ���cif_no��˳��� */
    sprintf(s_tmp1,"%04d", cif_basic_inf_c.cif_seqn);   /* ��˳��Ų���Ϊ4λ */
    sprintf(s_tmp2,"%08d", cif_no);        		/* ���ͻ��Ų���Ϊ8λ */
    strcat(s_tmp2 , s_tmp1);
    strcpy(s_acno, s_tmp2);

    /* ����У��λ */
    ret=pub_base_CrtAcChkb(s_acno , acno , 1, CHKBT_LEN);
    if (ret < 0 ){
        return (1);
    }

    /* �����޸Ŀͻ���˳��ű� */
	g_reply_int=Cif_basic_inf_Upd_Upd(cif_basic_inf_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ�쳣![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P135");
		return 1;
	}
	Cif_basic_inf_Clo_Upd();
    return 0;
}

/**********************************************************************
* ��������    pub_base_CrtInAc
* �������ܣ�    �����ڲ����˺ţ���У��λ��
* ����/ʱ�䣺    jane/2003��12��16��
*
* ������
*     ���룺acc_hrt    :    ��Ŀ������
*
*     �����acno            :    ���ݿ�Ŀ���������ɵ���У����˺�
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_CrtInAc( char *acc_hrt, char *acno )
{
	struct com_item_c sComItem;
	struct  in_ac_seqn_c sInAcSeqn;

    char    s_acno[21];       /* ����У��λ���˺� */
    char    s_tmp[65];     /* ��ʱ��������˳��Ų���Ϊ4λ */
    char    app_code[5];     /* Ӧ�ô��� */
    int     ac_seqn;     /* ˳��� */
    int     ret;

    MEMSET_DEBUG( &sComItem, 0x00, sizeof( struct com_item_c ) );
    MEMSET_DEBUG( &sInAcSeqn, 0x00, sizeof( struct in_ac_seqn_c ) );

    /* ���ݿ�Ŀ������ȡӦ�ô��� */
    ret = Com_item_Sel( g_pub_tx.reply , &sComItem , "acc_hrt = '%s'" , acc_hrt );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "�޴˿�Ŀ,acchrt=[%s]", acc_hrt );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S033" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "���ҹ�����Ŀ���acchrt=[%s],[%d]", acc_hrt, ret );
    	WRITEMSG
    	return(-1);
    }

    /* ����Ӧ�ô���ȡ���˳��� */
    pub_base_strpack( sComItem.app_cod );
    ret = In_ac_seqn_Sel( g_pub_tx.reply , &sInAcSeqn , "app_cod = '%s'", \
    				sComItem.app_cod );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "�޴�Ӧ�ô���,app_cod=[%s]", sComItem.app_cod );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S044" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "�����ڲ��ʺ���ű��app_cod=[%s],[%d]", sComItem.app_cod, ret );
    	WRITEMSG
    	return(-1);
    }

    /* ˳���+1 */
    sInAcSeqn.seque++;

    /* �����ַ���app_code��˳��� */
    sprintf(s_tmp,"%07d", sInAcSeqn.seque);
    s_acno[0]='9';               /* ��һλ9��ʾ�ڲ��ʺ� */
	s_acno[1]='\0';
    strcat(s_acno, sComItem.app_cod);
    strcat(s_acno, s_tmp);

    /* ����У��λ */
    ret=pub_base_CrtAcChkb( s_acno, acno, 2, CHKBT_LEN );
    if (ret < 0 ){
        return (1);
    }

    /* �����޸��ڲ��ʺ�˳��ű� */
    ret = sql_execute("update in_ac_seqn set seque=%d where app_cod='%s'",sInAcSeqn.seque,sInAcSeqn.app_cod);
    if ( ret )
    {
    	sprintf( acErrMsg, "�޸�	in_ac_seqn��ret = [%d]" , ret );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "E021" );
    	return(-1);
    }

    return 0;
}

/**********************************************************************
* ��������    pub_base_disac_file
* �������ܣ�  ������ʾ�˺�/��ŷ����˻�����,��ȡ�����ļ�����Ʒ����
* ����/ʱ�䣺 rob/2003/12/31
*
* ������
*		����: ��ʾ�ʺ�			ac_no			char[24];
*			  �˻����			ac_seqn			long;
*     	������˻�����			ac_type			char[2];
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��rob 2003/01/14
*
********************************************************************/
int pub_base_disac_file ( ac_no, ac_seqn, ac_type )
char *ac_no;
int ac_seqn;
char *ac_type;
{
	/*������ʾ�˺�,���ȡ���˻�id*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no ='%s'", ac_no);
	if ( ret == 100 )
	{
		sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	/*�����˻�id���˻����ȡ���ʺ�����*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id,
			   "ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W012");
		return 1;
	}
	else if( ret )
	{
	    	sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ��쳣[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W013");
		return 1;
	}

	/*�����ʺ�����*/
    MEMSET_DEBUG(ac_type, '0', sizeof(ac_type));
	strcpy(ac_type , g_prdt_ac_id.ac_id_type);

    /***** add by rob at 05/01/14*****/
    strcpy(g_pub_tx.ac_id_type,ac_type);
    strcpy(g_pub_tx.prdt_code,g_prdt_ac_id.prdt_no);

    sprintf(acErrMsg,"g_pub_tx.prdt_code=[%s]!!",g_pub_tx.prdt_code);
    WRITEMSG

    if (pub_base_get_prdtparm(g_pub_tx.prdt_code))
    {
        strcpy(acErrMsg,"ȡ��Ʒ��������!!");
        WRITEMSG
        return 1;
    }
    /***** add end *****/

	switch ( atoi(ac_type) )
	{
		case 1:									/*1-����*/
			MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );
			MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );

			/*���ļ�*/
			ret = Dd_mst_Sel(&g_pub_tx.reply, &g_dd_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}

			/*��ѯ���ڲ�����*/
			ret = Dd_parm_Sel(&g_pub_tx.reply, &g_dd_parm,
							"prdt_no='%s'", g_dd_mst.prdt_no);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 2:									/*2-����*/
			MEMSET_DEBUG( &g_td_mst, 0x00, sizeof(struct td_mst_c) );
			MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );

			ret = Td_mst_Sel(&g_pub_tx.reply, &g_td_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W016");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W017");
				return 1;
			}

			/*��ѯ���ڲ�����*/
			ret = Td_parm_Sel(&g_pub_tx.reply, &g_td_parm,
							"prdt_no='%s'", g_td_mst.prdt_no);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 3:									/*3-����*/
			MEMSET_DEBUG( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );

			ret = Ln_mst_Sel(&g_pub_tx.reply, &g_ln_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W018");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W019");
				return 1;
			}
			break;
		case 4:									/*4-͸֧*/
			MEMSET_DEBUG( &g_od_mst, 0x00, sizeof(struct od_mst_c) );

			ret = Od_mst_Sel(&g_pub_tx.reply, &g_od_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"͸֧���ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W020");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ͸֧���ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W021");
				return 1;
			}
			break;
		case 9:									/*5-�ڲ���Ʒ*/
			MEMSET_DEBUG( &g_in_mst, 0x00, sizeof(struct in_mst_c) );
			ret = In_mst_Sel(&g_pub_tx.reply, &g_in_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�ڲ���Ʒ���ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W022");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"��ѯ�ڲ����ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W023");
				return 1;
			}
			break;
		default:
			sprintf( acErrMsg, "�ʻ����ʹ�" );
			WRITEMSG
			return 1;
	}
	sprintf(acErrMsg,"g_mdm_ac_rel.opn_br_no=[%s],g_mdm_ac_rel.ac_id=[%ld]", g_mdm_ac_rel.opn_br_no, g_mdm_ac_rel.ac_id );
	WRITEMSG
	return 0;
}

/**********************************************************************
* ��������    pub_base_currentdis_file
* �������ܣ�  ���ݻ�����ʾ�˺ź��˺����ȡ�������ļ�����
* ����/ʱ�䣺 rob/2003/12/31
*
* ������
*     ���룺������ʾ�ʺ�		char(24);
*			�ʺ����			long ;
*
*     ������������ļ��ṹ
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_currentdis_file( ac_no,ac_seqn,f_dd_mst)
char *ac_no;
int ac_seqn;
struct dd_mst_c *f_dd_mst;
{
	/*������ʾ�˺�,���ȡ���˻�id*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no='%s' ", ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺű��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"��ѯ�����ʺű��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	/*���ݻ����ʻ�id���˻����ȡ�������ļ�*/
	MEMSET_DEBUG(f_dd_mst, 0x00, sizeof(struct dd_mst_c) );

vtcp_log("[%ld],[%d]",g_mdm_ac_rel.ac_id,ac_seqn);
	ret = Dd_mst_Sel(g_pub_tx.reply,f_dd_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�����˺Ų�����![%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}

	return 0;
}

/**********************************************************************
* ��������    pub_base_termidis_file();
* �������ܣ�  ���ݶ�����ʾ�˺ź��˺����ȡ�������ļ�����
* ����/ʱ�䣺 rob/2003/12/31
*
* ������
*     ���룺������ʾ�ʺ�		char(24);
*			�ʺ����			int ;
*
*     ������������ļ��ṹ
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_termidis_file(char ac_no[24],int ac_seqn,struct td_mst_c *f_td_mst)
{
	/*������ʾ�˺�,���ȡ���˻�id*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(&g_pub_tx.reply, &g_mdm_ac_rel,
						"ac_no='%s' ", ac_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if (ret && ret!=100)
	{
 		sprintf(acErrMsg,"��ѯ�����˻����ձ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	/*���ݶ����ʻ�id���˻����ȡ�������ļ�*/
	MEMSET_DEBUG( f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret = Td_mst_Sel(&g_pub_tx.reply, f_td_mst,
					"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=0)
	{
 		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}
	return 0;
}

/**********************************************************************
* ��������    pub_base_currentid_file
* �������ܣ�  ���ݻ����˺�IDȡ�������ļ�����
* ����/ʱ�䣺 rob/2003/12/31
*
* ������
*     ���룺�����ʺ�id			ac_id		long;
*			�����ʺ����		ac_seqn		int;
*
*     ������������ļ��ṹ		f_dd_mst;
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_currentid_file( long ac_id,int ac_seqn,struct dd_mst_c *f_dd_mst )
{
	/*���ݻ����ʻ�id���˻����ȡ�������ļ�*/
	MEMSET_DEBUG(f_dd_mst, 0x00, sizeof(struct dd_mst_c) );
	sprintf( acErrMsg , "ac_id[%ld]ac_seqn[%d]",ac_id,ac_seqn);
		WRITEMSG

	TRACE
	ret = Dd_mst_Sel(g_pub_tx.reply,f_dd_mst,
					"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
	TRACE
		sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if(ret && ret!=100)
	{
	TRACE
 		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}
	TRACE
	return 0;
}

/**********************************************************************
* ��������    pub_base_termiid_file
* �������ܣ�  ���ݶ����˺�IDȡ�������ļ�����?
* ����/ʱ�䣺 rob/2003/12/31
*
* ������
*     ���룺�����ʺ�id			ac_id			long;
*			�����ʺ����		ac_seqn     	int;
*
*     ������������ļ��ṹ		f_td_mst;
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_termiid_file( ac_id,ac_seqn,f_td_mst )
	long ac_id;
	int ac_seqn;
	struct td_mst_c	*f_td_mst;
{
	/*���ݶ����ʻ�id���˻����ȡ�������ļ�*/
	MEMSET_DEBUG( f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret=Td_mst_Sel(g_pub_tx.reply,f_td_mst, \
				   "ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}
	return 0;
}


/**********************************************************************
* ��������    pub_base_getparm
* �������ܣ�  ȡ��Ʒ��������
* ����/ʱ�䣺 rob/2003/04/01
*
* ������
*     ���룺�����ʺ�id			ac_id			long;
*			�����ʺ����		ac_seqn     	int;
*
*     ������������ļ��ṹ		f_td_mst;
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_getparm( ac_id,ac_seqn,cif_type )
	long ac_id;
	int ac_seqn;
	char *cif_type;
{
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, \
	   "ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�ʺű��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"��ѯ�쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}

	switch(g_prdt_ac_id.prdt_no[0])
	{
		case '1':
				ret=Dd_parm_Sel(g_pub_tx.reply,g_dd_parm, \
	   				"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"���ڲ������в����ڸü�¼[%d]",ret);
					WRITEMSG
					strcpy (g_pub_tx.reply,"W016");
					return 1;
				}
				else if(ret && ret!=100)
				{
 					sprintf(acErrMsg,"��ѯ�쳣[%d]",ret);
 					WRITEMSG
					strcpy (g_pub_tx.reply,"W017");
					return 1;
				}
				strcpy(cif_type,g_dd_parm.cif_type);

				break;
		case '2':
				ret=Td_parm_Sel(g_pub_tx.reply,g_td_parm, \
	   				"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"���ڲ������в����ڸü�¼[%d]",ret);
					WRITEMSG
					strcpy (g_pub_tx.reply,"W016");
					return 1;
				}
				else if(ret && ret!=100)
				{
 					sprintf(acErrMsg,"��ѯ�쳣[%d]",ret);
 					WRITEMSG
					strcpy (g_pub_tx.reply,"W017");
					return 1;
				}
				strcpy(cif_type,g_dd_parm.cif_type);
				break;
		default:
				break;
	}

	return 0;
}

/**********************************************************************
* ��������    pub_base_idseqn_file
* �������ܣ�  �����˺�ID���˻���ŷ����˻����ͣ���ȡ���ļ�����
* ����/ʱ�䣺 rob/2003/12/31
*
* ������
*     ���룺�ʺ�id			ac_id			long;
*			�ʺ����		ac_seqn     	int;
*
*     ������˻�����		ac_type			char[2];
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_idseqn_file( ac_id,ac_seqn,ac_type )
	long ac_id;
	int ac_seqn;
	char *ac_type;
{
	/*�����ʻ�id���˻����ȡ�˻�����*/
	struct prdt_ac_id_c  g_prdt_ac_id;
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id,
						"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�˻����ձ��в����ڸü�¼[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"��ѯ�˻����ձ��쳣!![%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}

	strcpy(ac_type,g_prdt_ac_id.ac_id_type);

	switch(atoi(ac_type))
	{
		case 1:									/*1-����*/
			MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );
			ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 2:									/*2-����*/
			MEMSET_DEBUG( &g_td_mst, 0x00, sizeof(struct td_mst_c) );

			ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W016");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W017");
				return 1;
			}
			break;
		case 3:									/*3-����*/
			MEMSET_DEBUG( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );

			ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�������ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W018");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W019");
				return 1;
			}
			break;
		case 4:									/*4-͸֧*/
			MEMSET_DEBUG( &g_od_mst, 0x00, sizeof(struct od_mst_c) );

			ret = Od_mst_Sel(g_pub_tx.reply, &g_od_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"͸֧���ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W020");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"��ѯ͸֧���ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W021");
				return 1;
			}
			break;
		case 9:									/*5-�ڲ���Ʒ*/
			MEMSET_DEBUG( &g_in_mst, 0x00, sizeof(struct in_mst_c) );
			ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�ڲ���Ʒ���ļ��в����ڸü�¼[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W022");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"��ѯ�ڲ����ļ��쳣[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W023");
				return 1;
			}
			break;
		default:
			sprintf( acErrMsg, "�ʻ����ʹ�" );
			WRITEMSG
			return 1;
	}
	return 0;
}
/**********************************************************************
* �� �� ����  pub_base_odid_file
* �������ܣ�  ����͸֧�˺�ID���˺����ȡ͸֧���ļ�
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��12��30��
*
* ��    ����
*     ��  �룺  ac_id       integer   �˺�ID
*               ac_seqn     smallint  �˺����
*
*     ��  ��:                         ͸֧���ļ��ṹ
*                           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_odid_file(long ac_id ,int ac_seqn ,struct od_mst_c *od_mst )
{

	int ret = 0 ;

	MEMSET_DEBUG( od_mst, 0x00, sizeof( struct od_mst_c ) );

    		/* ȡ͸֧���ļ���Ϣ */
	ret = Od_mst_Sel(g_pub_tx.reply, od_mst , "ac_id=%d and ac_seqn=%d" ,
			        ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"͸֧���ļ������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"T001" );
		return 1;
	}
	else if( ret !=0 )
	{
   		sprintf( acErrMsg,"ȡ͸֧���ļ��쳣 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}
  	return 0 ;
}

/**********************************************************************
 * ��  �� ����  pub_base_ChkAc
 * ���� ����:   ������ʾ�˺�/���ȡ�����ļ�;
 *		�ʻ���Ч�Լ��:����ʺ��Ƿ�Ϊ����״̬��(ƾ֤�����ã�
 *
 * ����/ʱ�䣺  jane/2003��1��8��
 *
 * ��  ����
 *     ���룺  char	*ac_no			�ʺ�
 *	       int	ac_seqn			�ʺ����
 *
 *     �����  char	*open_br_no		����������
 *	      long	*ac_id			�ʺ�id
 *   ����ֵ��  0: �ɹ�   -1: ʧ��
 *
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_base_ChkAc( char *ac_no, int ac_seqn, char *open_br_no, long *ac_id )
{
	char	cAcType[2];			/* �˻����� */
	char	cAcSts[2];			/* �ʻ�״̬ */
	int	iRet;

	MEMSET_DEBUG( cAcType, 0x00, sizeof( cAcType ) );
	MEMSET_DEBUG( cAcSts, 0x00, sizeof( cAcSts ) );

	/* ������ʾ�˺�/���ȡ�����ļ� */
	iRet = pub_base_disac_file( ac_no, ac_seqn, cAcType );
	if( iRet )
	{
		sprintf( acErrMsg, "������ʾ�˺�/���ȡ�����ļ�ʧ��!" );
		WRITEMSG
		return(-1);
	}
	vtcp_log("g_pub_tx.tx_code[%s]\n",g_pub_tx.tx_code);
	switch( cAcType[0] )
	{
		case '1':
			strcpy( open_br_no, g_dd_mst.opn_br_no );
			strcpy( cAcSts, g_dd_mst.ac_sts );
			*ac_id = g_dd_mst.ac_id;
			break;
		case '9':
			if(!memcmp(g_pub_tx.tx_code,"5105",4))
			{
			vtcp_log("g_pub_tx.tx_code1[%s]\n",g_pub_tx.tx_code);
			strcpy(open_br_no,g_in_mst.opn_br_no);
			strcpy(cAcSts,g_in_mst.sts);
			*ac_id =g_in_mst.ac_id;
			break;
			}
		case '2':
		case '3':
		case '4':
		case '5':
		default:
			strcpy( g_pub_tx.reply,"M018" );
			sprintf( acErrMsg, "�޴��ʻ�����" );
			WRITEMSG
			return(-1);
	}

	if( cAcSts[0] == '3' )
	{
		sprintf( acErrMsg, "�˻�Ϊ��ʧ����״̬�����������˽���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P247" ) ;
		return(-1);
	}else if( cAcSts[0] == '4' )
	{
		sprintf( acErrMsg, "�˻�Ϊ��������״̬�����������˽���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P248" ) ;
		return(-1);
	}else if( cAcSts[0] == '5' )
	{
		sprintf( acErrMsg, "�˻�Ϊ��ʱ����״̬�����������˽���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P249" ) ;
		return(-1);
	}else if( cAcSts[0] == '*' )
	{
		sprintf( acErrMsg, "�˻�Ϊ����״̬�����������˽���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P250" ) ;
		return(-1);
	}

	sprintf( acErrMsg, "g_dd_mst.ac_type=[%s]",g_dd_mst.ac_type );
	WRITEMSG

	/**��ƾ֤���׻�������**
	if( (strcmp( g_dd_mst.ac_type,"1") != 0)
		&& (strcmp( g_dd_mst.ac_type, "3" ) != 0) )
	{
		sprintf( acErrMsg, "�˻��ǶԹ���������ר�û�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N072" );
		return(-1);
	}
	**/
	return 0;
}

/**********************************************************************
 * ��  �� ����  pub_base_ChkAcBal
 * ���� ����:   �����˺ź��ʻ����ȡ�����ļ�;
 *		�ʻ���Ч�Լ��:����ʻ���(���л�Ʊ�ã�
 *
 * ����/ʱ�䣺  jane/2003��1��17��
 *
 * ��  ����
 *     ���룺  char	*ac_no			�������ʺ�
 *	       int	ac_seqn			�ʺ����
 *
 *     �����  double	*dAcBal			�ʻ����
 *
 *   ����ֵ��  0: �ɹ�   -1: ʧ��
 *
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_base_ChkAcBal( char *ac_no, int ac_seqn, double *dAcBal )
{
	char	cAcType[2];			/* �˻����� */
	int	iRet;

	MEMSET_DEBUG( cAcType, 0x00, sizeof( cAcType ) );

	/* ������ʾ�˺�/���ȡ�����ļ� */
	iRet = pub_base_disac_file( ac_no, ac_seqn, cAcType );
	if( iRet )
	{
		sprintf( acErrMsg, "������ʾ�˺�/���ȡ�����ļ�ʧ��!" );
		WRITEMSG
		return(-1);
	}

	switch( cAcType[0] )
	{
		case '1':
			*dAcBal = g_dd_mst.bal;
			break;
		case '2':
			*dAcBal = g_td_mst.bal;
			break;
		case '3':
			*dAcBal = g_ln_mst.bal;
			break;
		case '4':
			*dAcBal= g_od_mst.bal;
			break;
		case '5':
			*dAcBal = g_in_mst.bal;
			break;
		default:
			sprintf( acErrMsg, "�޴��ʻ�����" );
			WRITEMSG
			return(-1);
	}
	return 0;
}

/**********************************************************************
* �� �� ����  pub_base_CrtAcSeqn
* �������ܣ�  �����˻����
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��12��
*
* ��    ����
*     ��  �룺  ac_id       integer   �˺�ID
*
*     ��  ��:
*               ac_seqn     smallint  �˺����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_CrtAcSeqn(long ac_id , int *ac_seqn )
{
	int g_reply_int =0;

	struct ac_seqn_id_c ac_seqn_id;		/* �˺����ID��ṹ */

	g_reply_int=Ac_seqn_id_Dec_Upd(g_pub_tx.reply,"ac_id=%ld", ac_id);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�˺����ID���쳣����![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P140");
		goto ErrExit;
	}
	g_reply_int=Ac_seqn_id_Fet_Upd(&ac_seqn_id,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����![%d][%ld]",g_reply_int,ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"�˺����ID���쳣����![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P140");
		goto ErrExit;
	}

	ac_seqn_id.ac_seqn_id++;
	g_reply_int=Ac_seqn_id_Upd_Upd(ac_seqn_id,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�˺����ID���쳣����![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P140");
		goto ErrExit;
	}
	Ac_seqn_id_Clo_Upd();

	if( ac_seqn_id.ac_seqn_id>=9998 )
	{
		sprintf(acErrMsg,"�ý��ʲ��Ѿ������ٿ����˻�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P165");
		goto ErrExit;
	}

	*ac_seqn = ac_seqn_id.ac_seqn_id ;

	return 0;
ErrExit:
	return 1;
}


/**********************************************************************
* �� �� ����  pub_base_CurToSeqn
* �������ܣ�  (����)����ת�����˻����
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��13��
*
* ��    ����
*     ��  �룺  cur_no      char      ����
*
*     ��  ��:
*               ac_seqn     smallint  �˺����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_CurToSeqn(char *cur_no , int *ac_seqn )
{
	*ac_seqn = atoi( cur_no );	/* �˻���� */

	return 0;
}


/**********************************************************************
* �� �� ����  pub_base_CurGetddFile
* �������ܣ�  ���ݱ��֡���ʾ�˺�ȡ�������ļ�
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��13��
*
* ��    ����
*     ��  �룺  cur_no      char      ����
*		ac_no	    char      ��ʾ�˺�
*
*     ��  ��:
*               �������ļ�
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_CurGetddFile(char ac_no[25],char cur_no[3],struct dd_mst_c *f_dd_mst )
{
	int ac_seqn=0;
	MEMSET_DEBUG( f_dd_mst , 0x00 ,sizeof( struct dd_mst_c ));
	ret = pub_base_CurToSeqn( cur_no , &ac_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���ݱ��������˻���� PASS[%d]!",ac_seqn);
	WRITEMSG


	ret = pub_base_currentdis_file( ac_no, ac_seqn, f_dd_mst);
	if ( ret )
		{
			sprintf(acErrMsg,"���ú���������ʾ�˻����˻���ŷ��ػ������ļ�����!");
			WRITEMSG
			return 1;
		}
	sprintf(acErrMsg,"������ʾ�˻����˻���ŷ��ػ������ļ� PASS!");
	WRITEMSG
	vtcp_log( "xxxxxx__name[%s]",f_dd_mst->name);
	return 0;
}

/**********************************************************************
* �� �� ����  pub_base_CheckCrdNo
* �������ܣ�  ��鿨�ŵĺϷ���
* ��    �ߣ�  chengguohong
* ���ʱ�䣺  2003��02��18��
*
* ��    ����
*     ��  �룺
*	         g_pub_tx.crd_no ����
*
*     ��  ��:
*               �������ļ�
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_CheckCrdNo()
{
     char sCard_flag[60];
     memset(sCard_flag,0,sizeof(sCard_flag));
     get_zd_data("0830",sCard_flag);
     ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",
             g_pub_tx.crd_no);
    if ( ret == 100)
    {
        sprintf(acErrMsg,"�ÿ��Ų�����!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT20");
        return ret;
    }
    else if ( ret )
    {
        sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ����!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT21");
        return ret;
    }

    /* ��鿨��״̬ */
    /**��������Ҫ���ʧ״̬��ת��ת����Ȼ���Գɹ�������ӱ�־���� 20130811 hzh**/
    if ( g_mdm_ac_rel.note_sts[0] == '1' && memcmp(sCard_flag,"tdbzr",5) !=0 && memcmp(g_pub_tx.tx_code,"6203",4) == 0)
    {
        sprintf(acErrMsg,"�ÿ��Ѿ���ʧ!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT22");
        return 1;
    }

    if ( g_mdm_ac_rel.note_sts[0] == '2' )
    {
        sprintf(acErrMsg,"�ÿ��Ѿ���ʧ��֤!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT23");
        return 1;
    }

    if ( g_mdm_ac_rel.note_sts[0] == '3' || g_mdm_ac_rel.note_sts[0] == '4')
    {
        sprintf(acErrMsg,"�ÿ��Ѿ�������֤!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT24");
        return 1;
    }

    if ( g_mdm_ac_rel.note_sts[0] == '*' )
    {
        sprintf(acErrMsg,"���ʺ��Ѿ�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT25");
        return 1;
    }

    if ( g_mdm_ac_rel.coll_sts[0] == '1')
    {
        sprintf(acErrMsg,"�ÿ���״̬Ϊ��ʰ״̬!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT26");
        return 1;
    }

    if ( g_mdm_ac_rel.coll_sts[0] == '2')
    {
        sprintf(acErrMsg,"�ÿ���״̬Ϊ�̿�״̬!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT27");
        return 1;
    }

    /**��������Ҫ���ʧ״̬��ת��ת����Ȼ���Գɹ�������ӱ�־���� 20130811 hzh**/
    if ((g_mdm_ac_rel.note_sts[0] == '1' && memcmp(sCard_flag,"tdbzr",5) !=0 && memcmp(g_pub_tx.tx_code,"6203",4) == 0) || (g_mdm_ac_rel.note_sts[0] != '0' && g_mdm_ac_rel.note_sts[0] != '1') || (g_mdm_ac_rel.coll_sts[0] != '0'))
    {
        sprintf(acErrMsg,"�ÿ���״̬Ϊ������״̬!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT01");
        return 1;
    }

    /* ��鿨����Ч�� */
    if ((g_pub_tx.tx_date < g_mdm_ac_rel.beg_date)
            || (g_pub_tx.tx_date >= g_mdm_ac_rel.end_date))
    {
        sprintf(acErrMsg,"������Ч�����ѹ�!![%ld] [%ld] [%ld]",g_pub_tx.tx_date,g_mdm_ac_rel.beg_date,g_mdm_ac_rel.end_date);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT28");
        return 1;
    }

    /* ��鿨��֧ȡ��ʽ *
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        return 1;
    }

    * ���֧ȡ��ʽ�ĺϷ��� *
    if (strcmp(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd) != 0)
    {
        sprintf(acErrMsg,"����֧ȡ�������!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        return 1;
    }
    ***/
    return 0;
}

/**********************************************************************
* �� �� ����  pub_base_actoseqn
* �������ܣ�  �����ʺŷ����˻����(���˻����ڶ����ŷ����˻����Ϊ0)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��02��21��
* ��    ����
*     ��  �룺
*				ac_no	    char      	��ʾ�˺�
*     ��  ��:
*               ac_seqn		smallint	�˻����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_actoseqn(char *ac_no,int *ac_seqn)
{
	MEMSET_DEBUG(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
    if ( ret == 100)
    {
        sprintf(acErrMsg,"���ʺŲ�����!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"M003");
        return ret;
    }
	else if(ret)
    {
        sprintf(acErrMsg,"��ѯ�����ʺŶ��ձ��쳣!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W011");
        return ret;
    }

	/*��ղ�Ʒ�ʺŽṹ*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c) );

	if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)
	{
TRACE
		ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(ret)
    	{
        	sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ��쳣!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"W013");
        	return ret;
		}

		ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if(ret==100)
    	{
       		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ��в����ڸü�¼!! [%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"W012");
       		return ret;
		}
		else if(ret)
    	{
       		sprintf(acErrMsg,"FETCH��Ʒ�ʺŶ��ձ��쳣!! [%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"W059");
       		return ret;
		}

		ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if(ret==100)
			*ac_seqn=g_prdt_ac_id.ac_seqn;
		else if(ret)
    	{
       		sprintf(acErrMsg,"FETCH��Ʒ�ʺŶ��ձ��쳣!! [%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"W059");
       		return ret;
		}
		else if(ret==0)
			*ac_seqn=0;
		Prdt_ac_id_Clo_Sel();
	}
	else
    {
		*ac_seqn=g_mdm_ac_rel.ac_seqn;
    }

	return 0;
}
/**
* ����ac_id �� ac_seqnȡ��ac_no
*/
int pub_base_acid_acno( long ac_id,int ac_seqn,char *ac_no )
{
	int ret=0;
	int i=0;
	struct mdm_ac_rel_c vmdm_ac;
	struct mdm_ac_rel_c smdm_ac;

	ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_id=%ld and ac_seqn=%d order by ac_no",
		ac_id,ac_seqn );
	ERR_DEAL

	ret = Mdm_ac_rel_Fet_Sel( &vmdm_ac , g_pub_tx.reply );
	if( ret==100 )
	{
		Mdm_ac_rel_Clo_Sel();

		ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_id=%ld and ac_seqn=%d order by ac_no",
			ac_id,ALL_SEQN );
		ERR_DEAL
		ret = Mdm_ac_rel_Fet_Sel( &vmdm_ac , g_pub_tx.reply );
		if( ret==100 )
		{
			Mdm_ac_rel_Clo_Sel();

			ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_id=%ld order by ac_no",
				ac_id );
			ERR_DEAL
			ret = Mdm_ac_rel_Fet_Sel( &vmdm_ac , g_pub_tx.reply );
			if( ret==100 )
			{
				strcpy( ac_no, "" );
				goto ErrExit;
			}
		}
	}
	if ( ret!=100 ) { ERR_DEAL }

	if( vmdm_ac.note_sts[0]=='0' || vmdm_ac.note_sts[0]=='1' )
	{
		TRACE
		pub_base_strpack( vmdm_ac.ac_no );
		strcpy( ac_no,vmdm_ac.ac_no );
		goto OkExit;
	}

	while( 1 )
	{
		ret = Mdm_ac_rel_Fet_Sel( &smdm_ac , g_pub_tx.reply );
		if( ret ) break;

		if( smdm_ac.note_sts[0]=='0' || smdm_ac.note_sts[0]=='1' )
		{
			TRACE
			pub_base_strpack( smdm_ac.ac_no );
			strcpy( ac_no,smdm_ac.ac_no );
			goto OkExit;
		}

		MEMCPY_DEBUG( &vmdm_ac,&smdm_ac,sizeof(vmdm_ac) );
	}

	Mdm_ac_rel_Clo_Sel();

		pub_base_strpack( vmdm_ac.ac_no );
		strcpy( ac_no,vmdm_ac.ac_no );

OkExit:
	return 0;
ErrExit:
	return 1;
}
/**
* ���� ac_no ȡ�� ac_id �� ac_seqn
*/
int pub_base_acno_acid( long * ac_id,char *ac_no )
{
	int ret=0;
	struct mdm_ac_rel_c vmdm_ac;

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac," ac_no='%s' ",
		ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"ac_no='%s'",ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"L014" );
		return 1;
	}
	else if( ret )
	{
		return 1;
	}
	else
	{
		*ac_id = vmdm_ac.ac_id ;
	}

return 0;
}
/******************************************************
*
* �������������˺Ų��ҿͻ��š�����
*
*****************************************************/
int pub_base_acgetname_mst( long ac_id,int ac_seqn,char *name )
{
	int ret = 0;
	struct dd_mst_c dd_mst_c;
	struct td_mst_c td_mst_c;

		TRACE
	ret=Dd_mst_Sel( g_pub_tx.reply,&dd_mst_c,"ac_id=%d and ac_seqn=1",ac_id );
	if( ret )
	{
		TRACE
		ret=Td_mst_Sel( g_pub_tx.reply,&td_mst_c,"ac_id=%d",ac_id );
		if( ret )
		{
		TRACE
			strcpy( g_pub_tx.reply,"W011" );
			return ret;
		}
		else
		{
			strcpy( name,td_mst_c.name );
		}
	}
	else
	{
			strcpy( name,dd_mst_c.name );
	}

	return (0);
}

/******************************************************
*
* �������������˺Ų��ҿͻ��š�����
*
*****************************************************/
int pub_base_acgetname(char *ac_no, long *cif_no, char *name)
{
	int ret = 0;

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct cif_basic_inf_c sCif_basic_inf;

	MEMSET_DEBUG( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no = '%s'", ac_no );
	if( ret != 0 )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return 1;
	}

	MEMSET_DEBUG( &sPrdt_ac_id, 0x00, sizeof(sPrdt_ac_id) );
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &sPrdt_ac_id, \
		"ac_id = '%ld' order by ac_id_type", sMdm_ac_rel.ac_id );
	if( ret != 0 )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return ret;
	}

	MEMSET_DEBUG( &sCif_basic_inf, 0x00, sizeof(sCif_basic_inf) );
	ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCif_basic_inf, \
		"cif_no = '%ld'", sPrdt_ac_id.cif_no );
	if( ret != 0 )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return 3;
	}

	sprintf(acErrMsg,"ac_id is [%ld];cif_no is [%ld]-------------", sMdm_ac_rel.ac_id, sPrdt_ac_id.cif_no );
	WRITEMSG

	*cif_no = sPrdt_ac_id.cif_no;
	strcpy( name, sCif_basic_inf.name );

	return (0);
}


/**********************************************************************
* �� �� ����  pub_base_getprdtparm
* �������ܣ�  ȡ��Ʒ����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��05��21��
*
* ��    ����
*     ��  �룺
*				prdt_no    	char      	��Ʒ���
*     ��  ��:
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_getprdtparm(prdt_no,term,term_type,deadline,interest)
char *prdt_no;
int  *term;
char *term_type;
long *deadline;
double *interest;
{

	switch(prdt_no[0])
	{
		case '1':
				 *term=g_td_parm.term;
				 strcpy(term_type,g_td_parm.term_type);
				 *deadline=g_td_mst.mtr_date;
				 *interest=0.00;
		case '2':
				 *term=g_td_parm.term;
				 strcpy(term_type,g_td_parm.term_type);
				 *deadline=g_td_mst.mtr_date;
				 *interest=0.00;
		default :
				 return 1;
	}

	return 0;
}

/**********************************************************************
* �� �� ����  pub_base_checkopnbr
* �������ܣ�  �жϰ���ҵ��������Ƿ�Ϊ�˻��Ŀ�����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��05��21��
*
* ��    ����
*     ��  �룺
*				ac_no    	char      	�ʺ�
*     ��  ��:
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_checkopnbr(ac_no)
char *ac_no;
{
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no );
	if( ret==100 )
	{
		strcpy( g_pub_tx.reply,"M003" );
		return 1;
	}
	else if( ret )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return 1;
	}

	if(strncmp(g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no,5))
	{
		strcpy( g_pub_tx.reply,"O171" );
		return 1;
	}

	return 0;
}

/**********************************************************************
* ��������    pub_base_CrtDiscntAc
* �������ܣ�    ���������˺ţ���У��λ��
* ����/ʱ�䣺    jane/2003��6��13��
*
* ������
*     ���룺acc_hrt    :    ��Ŀ������
*
*     �����acno       :    ���ݿ�Ŀ���������ɵ���У����˺�
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_CrtDiscntAc( char *acc_hrt, char *acno )
{
	struct	com_item_c 		sComItem;
	struct  in_ac_seqn_c 	sInAcSeqn;

    char    s_acno[21];		/* ����У��λ���˺� */
    char    s_tmp[65];     	/* ��ʱ��������˳��Ų���Ϊ4λ */
    char    app_code[5];    /* Ӧ�ô��� */
    int     ac_seqn;     	/* ˳��� */
    int     ret;

    MEMSET_DEBUG( &sComItem, 0x00, sizeof( struct com_item_c ) );
    MEMSET_DEBUG( &sInAcSeqn, 0x00, sizeof( struct in_ac_seqn_c ) );

    /* ���ݿ�Ŀ������ȡӦ�ô��� */
    ret = Com_item_Sel( g_pub_tx.reply, &sComItem, "acc_hrt = '%s'", acc_hrt );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "�޴˿�Ŀ,acchrt=[%s]", acc_hrt );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S033" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "���ҹ�����Ŀ���acchrt=[%s],[%d]", acc_hrt, ret );
    	WRITEMSG
    	return(-1);
    }

    /* ����Ӧ�ô���ȡ���˳��� */
    pub_base_strpack( sComItem.app_cod );
    ret = In_ac_seqn_Sel( g_pub_tx.reply , &sInAcSeqn , "app_cod = '%s'", \
    				sComItem.app_cod );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "�޴�Ӧ�ô���,app_cod=[%s]", sComItem.app_cod );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S044" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "�����ڲ��ʺ���ű��app_cod=[%s],[%d]", sComItem.app_cod, ret );
    	WRITEMSG
    	return(-1);
    }

    /* ˳���+1 */
    sInAcSeqn.seque++;

    /* �����ַ���app_code��˳��� */
    sprintf(s_tmp,"%04d", sInAcSeqn.seque);
    s_acno[0]='5';               /* ��һλ5��ʾ�Թ��ʺ� */
	s_acno[1]='\0';
    strcat(s_acno, sComItem.app_cod);
    strcat(s_acno, s_tmp);

    /* ����У��λ */
    ret=pub_base_CrtAcChkb( s_acno, acno, 2, CHKBT_LEN );
    if (ret < 0 )
        return (1);

    /* �����޸��ڲ��ʺ�˳���*/
    ret = sql_execute("update in_ac_seqn set seque = %d where app_cod = '%s'", \
    			sInAcSeqn.seque, sInAcSeqn.app_cod );
    if ( ret )
    {
    	sprintf( acErrMsg, "�޸�in_ac_seqn��ret = [%d]" , ret );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "E021" );
    	return(-1);
    }

    return 0;
}

/*������Ӧ����ȡ������Ϣ*/
int pub_base_regetinf(char *rsp_code, char *inf)
{
	int ret = 0;

	struct rsp_code_c g_rsp_code;

	MEMSET_DEBUG( &g_rsp_code, 0x00, sizeof(struct rsp_code_c) );

	ret = Rsp_code_Sel(g_pub_tx.reply,&g_rsp_code,"code='%s'",rsp_code);
	if( ret )
	{
		strcpy( g_pub_tx.reply,"O206" );
		return 1;
	}

	strcpy(inf,g_rsp_code.memo);

	return (0);
}

int pub_base_ac_prdt(struct prdt_ac_id_c	*sPrdt_ac_id)
{
	int ret=0;

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",g_pub_tx.ac_no);
	if( ret==100 )
	{
    	sprintf( acErrMsg, "�ʺŲ�����!ac_no=[%s]",g_pub_tx.ac_no);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}
	if( ret )
	{
    	sprintf( acErrMsg, "ȡ�����ʺŶ��ձ����!ac_no=[%s],ret=[%d]",g_pub_tx.ac_no,ret);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,sPrdt_ac_id,"ac_id='%ld' and ac_seqn='%d' ",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret==100 )
	{
    	sprintf( acErrMsg, "�ʺŲ�����!ac_id=[%d],ac_seqn=[%d]",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}
	if( ret )
	{
    	sprintf( acErrMsg, "ȡ��Ʒ�ʺŶ��ձ����!ac_id=[%d],ac_seqn=[%d],ret=[%d]",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}
	return 0;
}

int pub_base_Exist_MainCrd(long ac_id,int *flag)
{
	int	ret=0;
	struct mdm_ac_rel_c Tmp_mdm_ac_rel;
	struct mdm_attr_c 	Tmp_mdm_attr;

	MEMSET_DEBUG(&Tmp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply," ac_id=%d and main_ind='1'",ac_id);
	if( ret )
	{
    	sprintf(acErrMsg,"ȡ�����ʺŶ��ձ����!ac_id=[%d],ret=[%d]",ac_id,ret);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "P147" );
    	return (-1);
	}

	*flag=1;		/** Ĭ������ **/
	while(1)
	{
		ret=Mdm_ac_rel_Fet_Sel(&Tmp_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )	break;
		if( ret )
		{
    		sprintf(acErrMsg,"�����ʺű����!ac_id=[%d],ret=[%d]",ac_id,ret);
    		WRITEMSG
    		sprintf( g_pub_tx.reply, "P147" );
    		return (-1);
		}

		/**
		if( !strncmp(Tmp_mdm_ac_rel.ac_no,CRD_HEAD,9) )	 ** �������� **
			*flag=0;
		**/
		ret=Mdm_attr_Sel(g_pub_tx.reply,&Tmp_mdm_attr,"mdm_code='%s'",Tmp_mdm_ac_rel.mdm_code);
		if( ret )
		{
    		sprintf(acErrMsg,"ȡ�������Ա����!mdm_code=[%s],ret=[%d]",Tmp_mdm_ac_rel.mdm_code,ret);
    		WRITEMSG
    		sprintf( g_pub_tx.reply, "P133" );
    		return (-1);
		}

		if( Tmp_mdm_attr.mdm_type[0]=='3' || Tmp_mdm_attr.mdm_type[0]=='4'
			|| Tmp_mdm_attr.mdm_type[0]=='5' )
			*flag=0;
	}
	Mdm_ac_rel_Clo_Sel();

	return 0;
}
/**�����¾��˺�**/
int pub_base_old_acno( char * ac_no )
{
	int ret=0;
	struct mo_merge_ac_c mo_merge_ac_c;

	/**���˺�**/
	if( strlen(ac_no)==18 )
		;
	else if( !strncmp(ac_no,"7861",4) && strlen(ac_no)==20 )
		;
	else if( !strncmp(ac_no,"7861",4) && strlen(ac_no)==19 )
		;
	else
		return 0;

	ret=Mo_merge_ac_Sel( g_pub_tx.reply , &mo_merge_ac_c , "old_ac_no='%s'" , \
						 ac_no );
	if( ret==100 )
	{
		return 0;
	}
	else if( ret )
	{
		return 1;
	}
	pub_base_strpack(mo_merge_ac_c.ac_no);
	strcpy( ac_no,mo_merge_ac_c.ac_no );
        return 0;
}

int pub_base_opn_sub_ac(int sub_ac_seqn,char *intst_type,double rate)
{
	int ret=0;
	struct prdt_ac_id_c	sPrdt_ac_id;
	struct sub_dd_mst_c	sSub_dd_mst;
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	if( g_pub_tx.ac_seqn==0 )		/*** ��Ҫ���ɷ��ʺ� ***/
	{
		ret = pub_base_CurToSeqn( g_pub_tx.cur_no , &g_pub_tx.ac_seqn );
		if( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			    return 1;
		}
	}

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ�����ʺŶ��ձ����!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	if(memcmp(g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no,strlen(g_pub_tx.tx_br_no))){
		sprintf(acErrMsg,"ֻ����һ���˻��������������˻�!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P427");
		return 1;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ��Ʒ�ʺŶ��ձ����!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	if( sPrdt_ac_id.ac_id_type[0]!='1' )
	{
		sprintf(acErrMsg,"�ǻ��ڲ�Ʒ�ʺŲ��ܿ����ʺ�!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P426");
		return 1;
	}
	if( strcmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no) )
	{
		sprintf(acErrMsg,"ֻ���ڸ�ĸ�ʺſ������������ʺ�!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P427");
		return 1;
	}
	strcpy(g_pub_tx.prdt_code,sPrdt_ac_id.prdt_no);

	strcpy(sSub_dd_mst.br_no,g_mdm_ac_rel.opn_br_no);
	strcpy(sSub_dd_mst.ac_no,g_pub_tx.ac_no);
	sSub_dd_mst.sub_ac_seqn=sub_ac_seqn;
	sprintf(sSub_dd_mst.sub_ac_no,"%s-%d",g_pub_tx.ac_no,sub_ac_seqn);
	sSub_dd_mst.bal=0.00;
	sSub_dd_mst.ys_bal=0.00;
	sSub_dd_mst.intst_acm=0.00;
	sSub_dd_mst.opn_date=g_pub_tx.tx_date;
	sSub_dd_mst.ic_date=g_pub_tx.tx_date;
	sSub_dd_mst.lst_date=g_pub_tx.tx_date;
	sSub_dd_mst.hst_cnt=1;
	sSub_dd_mst.hst_pg=0;
	strcpy(sSub_dd_mst.name,g_pub_tx.name);
	strcpy(sSub_dd_mst.ac_sts,"1");
	strcpy(sSub_dd_mst.intst_type,intst_type);
	sSub_dd_mst.rate=rate;
	/**------------ �Ǽ����ʺ�̨�� -------------**/
	ret=Sub_dd_mst_Ins(sSub_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ����ʺ�̨�ʴ���!");
		WRITEMSG
		Sub_dd_mst_Debug(&sSub_dd_mst);
		    strcpy(g_pub_tx.reply,"P429");
		return 1;
	}
	/**------------- �Ǽ����ʺ���ϸ̨�� ------------**/
	ret=pub_base_ins_sub_ac_hst(sSub_dd_mst);
	if( ret )
	{
		WRITEMSG
		   return 1;
	}
	/**------------- �Ǽ�ҵ����ˮ --------------**/
	ret=pub_ins_trace_log();
	if( ret )
	{
		WRITEMSG
		    return 1;
	}

	return 0;
}

int pub_base_ins_sub_ac_hst(struct sub_dd_mst_c	sSub_dd_mst)
{
	int ret=0;
	struct sub_dd_mst_hst_c	sSub_dd_mst_hst;
	memset(&sSub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));

	sSub_dd_mst_hst.trace_no=g_pub_tx.trace_no;
	get_zd_long( DC_TRACE_CNT , &sSub_dd_mst_hst.trace_cnt);
	sSub_dd_mst_hst.trace_cnt++;
	strcpy(sSub_dd_mst_hst.opn_br_no,sSub_dd_mst.br_no);
	strcpy(sSub_dd_mst_hst.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sSub_dd_mst_hst.ac_no,sSub_dd_mst.ac_no);
	strcpy(sSub_dd_mst_hst.sub_ac_no,sSub_dd_mst.sub_ac_no);
	strcpy(sSub_dd_mst_hst.add_ind,g_pub_tx.add_ind);
	strcpy(sSub_dd_mst_hst.ct_ind,g_pub_tx.ct_ind);
	sSub_dd_mst_hst.tx_amt=g_pub_tx.tx_amt1;
	sSub_dd_mst_hst.bal=sSub_dd_mst.bal;
	sSub_dd_mst_hst.intst_acm=sSub_dd_mst.intst_acm;
	sSub_dd_mst_hst.tx_date=g_pub_tx.tx_date;
	sSub_dd_mst_hst.tx_time=g_pub_tx.tx_time;
	sSub_dd_mst_hst.hst_cnt=sSub_dd_mst.hst_cnt;
	strcpy(sSub_dd_mst_hst.brf,g_pub_tx.brf);
	strcpy(sSub_dd_mst_hst.tel,g_pub_tx.tel);
	strcpy(sSub_dd_mst_hst.chk,g_pub_tx.chk);
	strcpy(sSub_dd_mst_hst.auth,g_pub_tx.auth);
	strcpy(sSub_dd_mst_hst.note_type,g_pub_tx.note_type);
	strcpy(sSub_dd_mst_hst.note_no,g_pub_tx.beg_note_no);
	get_zd_long("1261",&sSub_dd_mst_hst.tt_no);/*add by cgx 20090818 nxhhyh */
	sSub_dd_mst_hst.sub_ac_seqn=sSub_dd_mst.sub_ac_seqn;

	ret=Sub_dd_mst_hst_Ins(sSub_dd_mst_hst,g_pub_tx.reply);
	if( ret )
	{
		Sub_dd_mst_hst_Debug(&sSub_dd_mst_hst);
		sprintf(acErrMsg,"�Ǽ����ʺ���ϸ̨�ʴ���!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P430");
		return 1;
	}
	return 0;
}

/* ע��ļ������� pub_tx.ac_no   ���˻�
				  pub_tx.ac_no1  ���ʺ�
                  pub_tx.add_ind �����־
                  pub_tx.cd_ind  ��ת��־
				  pub_tx.brf     ժҪ
				  pub_tx.cur_no  ����
*/
int pub_base_sub_ac_trance(int sub_ac_seqn)
{
	int	ret=0;
	char sub_ac_no[26];

	char add_ind[2];
	struct sub_dd_mst_c	sSub_dd_mst;
	struct prdt_ac_id_c	sPrdt_ac_id;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct dd_parm_c    s_dd_parm;

	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(sub_ac_no,0,sizeof(sub_ac_no));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&s_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(add_ind,0,sizeof(add_ind));
	vtcp_log(" %s, %d, function[pub_base_sub_ac_trance] ac_seqn[%ld] cur_no[%s] ac_seqn[%d] ",__FILE__,__LINE__,g_pub_tx.ac_seqn,g_pub_tx.cur_no,sub_ac_seqn);
	if( g_pub_tx.ac_seqn==0 )
	{
		ret = pub_base_CurToSeqn( g_pub_tx.cur_no , &g_pub_tx.ac_seqn );
		if( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			    return 1;
		}
	}

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ�����ʺŶ��ձ����![%s]",g_pub_tx.ac_no1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	/* �����ж�һ�����ǿ�ͨ��־û�п�ֱͨ�ӱ���! 201011114*/
	/* if(s_mdm_ac_rel.kt_flag[0]!='1') */
	if(strcmp(s_mdm_ac_rel.draw_pwd_chg,"1"))
	{
		vtcp_log("%s,%d,���˻�û�п�ͨ�����˻�!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"SN09");
		return 1;
	}
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",s_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ��Ʒ�ʺŶ��ձ����!");
		WRITEMSG
	    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}

	ret=Dd_parm_Sel(g_pub_tx.reply,&s_dd_parm,"prdt_no='%s'",sPrdt_ac_id.prdt_no);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ��Ʒ���������!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}

	/**--------- ȡĸ�ʺ����ļ� -----------**
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%d and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ��Ʒ���������!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	*/
	/**--------- ȡ���ʺ�̨�� -----------**/
	ret=Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%d",g_pub_tx.ac_no,sub_ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ���ʺ�̨�ʴ���!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P431");
		return 1;
	}
	ret=Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ���ʺ�̨�ʴ���!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P431");
		return 1;
	}
	memcpy(g_pub_tx.opn_br_no ,sSub_dd_mst.br_no,sizeof(g_pub_tx.opn_br_no)-1);
	if( sSub_dd_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"�����ʺ��Ѿ�����!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P432");
		return 1;
	}
	vtcp_log(" %s, %d, add_ind[%s] bal[%.2lf] ",__FILE__,__LINE__,g_pub_tx.add_ind,sSub_dd_mst.bal);
	if( g_pub_tx.add_ind[0]=='1' )		/*** ��� ***/
	{
		vtcp_log(" %s, %d, g_pub_tx.tx_amt1[%.2lf] bal[%.2lf] ",__FILE__,__LINE__,g_pub_tx.tx_amt1,sSub_dd_mst.bal);
		if( pub_base_CalAcm(sSub_dd_mst.lst_date,g_pub_tx.tx_date,
		    s_dd_parm.intst_term_type,s_dd_parm.acm_calc_type,
		    sSub_dd_mst.bal,g_pub_tx.tx_amt1,&sSub_dd_mst.intst_acm,
		    sSub_dd_mst.intst_type,s_dd_parm.intst_mon,s_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			    return 1;
		}
		sSub_dd_mst.bal+=g_pub_tx.tx_amt1;
		vtcp_log(" %s, %d, sSub_dd_mst.bal=[%f]",__FILE__,__LINE__,sSub_dd_mst.bal);
	}
	else	/* ȡ��,���� */
	{
		vtcp_log(" %s, %d, g_pub_tx.tx_amt1[%.2lf] bal[%.2lf] ",__FILE__,__LINE__,g_pub_tx.tx_amt1,sSub_dd_mst.bal);
		if( pub_base_CalAcm(sSub_dd_mst.lst_date,g_pub_tx.tx_date,
		    s_dd_parm.intst_term_type,s_dd_parm.acm_calc_type,
		    sSub_dd_mst.bal,-g_pub_tx.tx_amt1,&sSub_dd_mst.intst_acm,
		    sSub_dd_mst.intst_type,s_dd_parm.intst_mon,s_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			    return 1;
		}
		vtcp_log(" %s, %d,   sSub_dd_mst.bal=%.2f",__FILE__,__LINE__,sSub_dd_mst.bal);
		vtcp_log(" %s, %d,   g_pub_tx.tx_amt1=%.2f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		sSub_dd_mst.bal-=g_pub_tx.tx_amt1;
		vtcp_log(" %s, %d,   sSub_dd_mst.bal=%.2f",__FILE__,__LINE__,sSub_dd_mst.bal);
		if(pub_base_CompDblVal(sSub_dd_mst.bal,0.0)<0)
		{
				sprintf(acErrMsg,"���˻�����͸֧!,sub_dd_mst.bal=%.2f",sSub_dd_mst.bal);
				WRITEMSG
				    strcpy(g_pub_tx.reply,"T002");
				return 1;
		}
		if( g_pub_tx.add_ind[0]=='3' )	/*** ���� ***/
		{
			if( pub_base_CompDblVal(sSub_dd_mst.bal,0.00)!=0 )
			{
				sprintf(acErrMsg,"���ʺ����������ʺ���Ϊ��!");
				WRITEMSG
				    strcpy(g_pub_tx.reply,"P433");
				return 1;
			}
			strcpy(sSub_dd_mst.ac_sts,"*");
		}
		strcpy(g_pub_tx.add_ind,"0");
	}
	sSub_dd_mst.hst_cnt++;
	sSub_dd_mst.lst_date=g_pub_tx.tx_date;

	ret=Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"ȡ���ʺ�̨�ʴ���!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P431");
		return 1;
	}
	Sub_dd_mst_Clo_Upd();

	/**------------- �Ǽ����ʺ���ϸ̨�� ------------**/
	ret=pub_base_ins_sub_ac_hst(sSub_dd_mst);
	if( ret )
	{
		WRITEMSG
		return 1;
	}
	/**------------- �Ǽ�ҵ����ˮ --------------**/
	/* by Wang Yongwei Ϊʲô��Ҫ���˻���ŷŵ�ժҪ���� 20101214*/
	/* strcpy(g_pub_tx.brf,g_pub_tx.ac_no1); */
	/* modify begin by wzs 20120514 reason: ���˻������������8λ,trace_log��ac_no������,�������˺ŵ���ˮ��ŷ���ac_id����,ac_no�����˺� */
	/* g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
	sprintf(sub_ac_no,"%.13s-%d",g_pub_tx.ac_no,sub_ac_seqn);
	memcpy(g_pub_tx.ac_no,sub_ac_no,sizeof(g_pub_tx.ac_no)-1);*//*�Ǽ����˺��� add by chenchao 20110110 23:48:30*/
	g_pub_tx.ac_id=sub_ac_seqn;
	/* modify end by wzs 20120514 */
	ret=pub_ins_trace_log();
	if( ret )
	{
		WRITEMSG
		    return 1;
	}
	strcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no);/* ĸ�˺Ÿ���ȫ�ֱ���Ϊ��ĸ�˺ż��� */

	return 0;
}
/**********************************************************************
* �� �� ����  pub_base_checkcollyn
* �������ܣ�  �ж��Ƿ��ʰ
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��21��
*
* ��    ����
*     ��  �룺
*				ac_no    	char      	�ʺ�
*     ��  ��:
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_checkcollyn( char *ac_no )
{
	struct mo_coll_c		g_mo_coll;
	struct com_branch_c		g_com_branch;

	MEMSET_DEBUG( &g_mo_coll,0x00,sizeof(struct mo_coll_c) );
	MEMSET_DEBUG( &g_com_branch,0x00,sizeof(struct com_branch_c) );

	g_reply_int = Mo_coll_Sel(g_pub_tx.reply,&g_mo_coll, \
				   			  "ac_no='%s' and sts='1'",g_mdm_ac_rel.ac_no);
   	if( g_reply_int==100 )
   	{
	   	sprintf(acErrMsg,"��ʰ�ǼǱ��޸ñʼ�ʰ��¼");
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply,"D141");
		return 1;
   	}
   	else if( g_reply_int==0 )
   	{
	   	g_reply_int = Com_branch_Sel(g_pub_tx.reply,&g_com_branch, \
									 "br_no='%s'",g_mo_coll.pick_br_no);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"������벻����[%s]",g_mo_coll.pick_br_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D141");
			return 1;
		}
	   	else if( g_reply_int )
		{
		  	sprintf(acErrMsg,"��ѯ����[%d]",g_reply_int);
			WRITEMSG
		   	strcpy(g_pub_tx.reply,"D103");
			return 1;
		}

	   	pub_base_strpack(g_com_branch.br_name);
	   	sprintf( acErrMsg,"�ý��ʱ�[%s]��ʰ",g_com_branch.br_name);
	  	WRITEMSG
   		strcpy(g_pub_tx.reply,"D141");
		sprintf( acErrMsg,"��ʰ����:(%s),�����黹.", \
		g_com_branch.br_name);
		set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		return(2);
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"���Ҵ���");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	return 0;
}
/**********************************************************************
* �� �� ����  pub_base_getbrname
* �������ܣ�  �����к�ȡ����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��28��
*
* ��    ����
*     ��  �룺
*				br_no    	char      	������
*     ��  ��:
*				br_name		char 		��������
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_getbrname( char *br_no ,char *br_name )
{
	struct com_branch_c		g_com_branch;

	MEMSET_DEBUG( &g_com_branch,0x00,sizeof(struct com_branch_c) );

	g_reply_int = Com_branch_Sel(g_pub_tx.reply,&g_com_branch, "br_no='%s'", \
								 br_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"������벻����[%s]",br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D141");
		return 1;
	}
   	else if( g_reply_int )
	{
	  	sprintf(acErrMsg,"��ѯ����[%d]",g_reply_int);
		WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	strcpy( br_name,g_com_branch.br_name );
	return 0;
}
/**********************************************************************
* �� �� ����  pub_base_getbrtele
* �������ܣ�  �����к�ȡ�绰����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��28��
* ��    ����
*     ��  �룺br_no    	char      	������
*     ��  ��: br_name	char 		��������
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
* �޸���ʷ��
*
********************************************************************/
int pub_base_getbrtele( char *br_no ,char *br_tele )
{
	struct com_branch_c		g_com_branch;

	MEMSET_DEBUG( &g_com_branch,0x00,sizeof(struct com_branch_c) );

	g_reply_int = Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'", \
								 br_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"������벻����[%s]",br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D141");
		return 1;
	}
   	else if( g_reply_int )
	{
	  	sprintf(acErrMsg,"��ѯ����[%d]",g_reply_int);
		WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	strcpy( br_tele,g_com_branch.br_tele );
	return 0;
}
/**ȡ�ò�Ʒ�Ŀ�Ŀ**/
int pub_base_prdt_accno( char *prdt_no,char *acc_no )
{
	int ret;
	struct dd_parm_c dd_parm;
	struct td_parm_c td_parm;
	struct ln_parm_c ln_parm;
	struct in_parm_c in_parm;
	struct dc_acc_rel_c dc_acc_rel;
	struct com_item_c com_item;
	char dc_code[5];

	if( prdt_no[0]=='1' )
	{
		ret=Dd_parm_Sel( g_pub_tx.reply,&dd_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,dd_parm.dc_code );
	}
	else if( prdt_no[0]=='2' )
	{
		ret=Td_parm_Sel( g_pub_tx.reply,&td_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,td_parm.dc_code );
	}
	else if( prdt_no[0]=='3' )
	{
		ret=Ln_parm_Sel( g_pub_tx.reply,&ln_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,ln_parm.dc_code );
	}
	else if( prdt_no[0]=='9' )
	{
		ret=In_parm_Sel( g_pub_tx.reply,&in_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,in_parm.dc_code );
	}
	else
	{
		strcpy( acc_no,"" );
		return 0;
	}
	pub_base_strpack(acc_no);
	ret=Dc_acc_rel_Sel( g_pub_tx.reply,&dc_acc_rel,"dc_code='%s' and data_code='0152'",dc_code);
	if( ret ) { strcpy( acc_no,"" ); return 0; }
	pub_base_strpack(dc_acc_rel.acc_hrt);
	ret=Com_item_Sel( g_pub_tx.reply,&com_item,"acc_hrt='%s'",dc_acc_rel.acc_hrt);
	if( ret ) { strcpy( acc_no,"" ); return 0; }
	strcpy( acc_no,com_item.acc_no );

	return 0;
}
/**********************************************************************
* �� �� ����  pub_base_getacname
* �������ܣ�  �����˻�IDȡ����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��28��
* ��    ����
*     ��  �룺br_no    	char      	������
*     ��  ��: br_name	char 		��������
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
* �޸���ʷ��
*
********************************************************************/
int pub_base_getacname( long *ac_id,char *name )
{
	int ret;

	MEMSET_DEBUG( &g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c) );

	g_reply_int =Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld",ac_id);

	switch( g_prdt_ac_id.prdt_no[0] )
	{
		case '1':
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=1",ac_id);
				strcpy( name,g_dd_mst.name );
				break;
		case '2':
				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld",ac_id);
				strcpy( name,g_td_mst.name );
				break;
		case '3':
				ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=0",ac_id);
				strcpy( name,g_ln_mst.name );
				break;
		case '9':
				ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld",ac_id);
				strcpy( name,g_in_mst.name );
				break;
		default:
				break;
	}

	if( ret==100 || g_reply_int==100)
	{
		sprintf(acErrMsg,"�˻�������[%ld][%d][%d]",ac_id,ret,g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
   	else if( ret || g_reply_int )
	{
	  	sprintf(acErrMsg,"��ѯ����[%d][%d]",ret,g_reply_int);
		WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	return 0;
}
/**********************************************************************
* ��������    pub_base_getdd_mst_parm
* �������ܣ�  ���ݻ����˺š����ȡ���ļ��Ͳ�Ʒ����
* ����/ʱ�䣺 rob
*
* ������
*     ���룺�����ʺ�			char(24);
*			�ʺ����			int ;
*
*     ������������ļ��ṹ
*     		���ڲ����ṹ
*
* �� �� ֵ: 0 �ɹ� 1 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_getdd_mst_parm(char *ac_no, int ac_seqn,
							struct dd_mst_c *f_dd_mst,
							struct dd_parm_c *f_dd_parm )
{
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );
	MEMSET_DEBUG( f_dd_mst, 0x00, sizeof(struct dd_mst_c) );
	MEMSET_DEBUG( f_dd_parm, 0x00, sizeof(struct dd_parm_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺű��в����ڸü�¼");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"��ѯ�����ʺű��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	ret = Dd_mst_Sel(g_pub_tx.reply,f_dd_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�����˺Ų�����!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}

	ret = Dd_parm_Sel( g_pub_tx.reply,f_dd_parm,"prdt_no='%s'", \
						f_dd_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ڲ�Ʒ������!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}

	return 0;
}
