/*************************************************
* �� �� ��: sp9601.c
* �������������Ӳ���Ա
*
* ��    ��: rob
* ������ڣ�20030309
*
* �޸ļ�¼��������Ȩ��Աʱ,��auth_tel_auth���¼
* ��    ��: 20061201
* �� �� ��: wanglei
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_tx_code_c.h"
#include "cash_mst_c.h"
#include "com_cur_no_code_c.h"
#include "auth_tel_auth_c.h"

int sp9601( )
{
    int ret;
    char cbr_no[10];
    char ctel[10];
    struct com_tel_c com_tel_c;
    struct com_branch_c com_branch_c;
    struct auth_tx_code_c auth_tx_code_c;
    struct cash_mst_c cash_mst_c;
    struct com_cur_no_code_c com_cur_no_code_c;
    struct auth_tel_auth_c auth_tel_auth_c;

    memset(&com_tel_c,0x0,sizeof(struct com_tel_c));
    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));
    memset(&auth_tx_code_c,0x0,sizeof(struct auth_tx_code_c));
    memset(&cash_mst_c,0x0,sizeof(struct cash_mst_c));
    memset(&com_cur_no_code_c,0x0,sizeof(struct com_cur_no_code_c));
    memset(&auth_tel_auth_c,0x0,sizeof(struct auth_tel_auth_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /******
    get_zd_data("0920",ctel);
    get_zd_data("0910",cbr_no);
    *******/
    get_zd_data("0920",com_tel_c.tel);
    get_zd_data("0910",com_tel_c.br_no);
    get_zd_data("0250",com_tel_c.name);
    get_zd_data("0790",com_tel_c.pwd);
    get_zd_data("0860",com_tel_c.lvl);
    get_zd_data("0640",com_tel_c.beg_menu);
    get_zd_data("0300",com_tel_c.tx_type);
    get_zd_data("0680", com_tel_c.chk_ind);
    get_zd_data("0690", com_tel_c.csts);

    pub_base_strpack(com_tel_c.br_no);
    pub_base_strpack(com_tel_c.tel);

    if (strcmp(com_tel_c.pwd,"") == 0 || com_tel_c.pwd[0] == '\0')
    {
        strcpy(com_tel_c.pwd,"111111"); /* Ĭ��ֵ */
    }

    if (strcmp(com_tel_c.beg_menu,"") == 0 || com_tel_c.beg_menu[0] == '\0')
    {
        strcpy(com_tel_c.beg_menu,"0000"); /* Ĭ��ֵ */
    }

    /*******
    if (strlen(ctel) != 4)
    {
        sprintf(acErrMsg,"��Ա����ĳ���ӦΪ4λ!! [%s] ",ctel);
        WRITEMSG
        goto ErrExit;
    }

    if (strlen(cbr_no) != 5)
    {
        sprintf(acErrMsg,"�����ŵĳ���ӦΪ5λ!! [%s] ",cbr_no);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(com_tel_c.tel,ctel);
    strcpy(com_tel_c.br_no,cbr_no);
    ********/

    /* ����Ա���Ƿ�Ψһ */
    ret = Com_tel_Sel(g_pub_tx.reply,&com_tel_c,"tel='%s'",com_tel_c.tel);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ��Ա��Ϣ�����!!");
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 0)
    {
        sprintf(acErrMsg,"��Ա���Ѿ�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O115");
        goto ErrExit;
    }
	
    /* �����������Ƿ���� */
    strcpy(com_branch_c.br_no,com_tel_c.br_no);

    ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
          com_branch_c.br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ�������������!!!!");
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"�����������û�иû�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O116");
        goto ErrExit;
    }

    /* ��齻�������Ƿ���� */
    /**********
    auth_tx_code_c.tx_type = com_tel_c.tx_type;

    ret = Auth_tx_code_Sel(g_pub_tx.reply,&auth_tx_code_c,"tx_type=%d",
          auth_tx_code_c.tx_type);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ��Ա�������ͱ�������!!!");
        WRITEMSG
        goto ErrExit;   
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"��Ա�������ͱ������û�иý�������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O117");
        goto ErrExit;
    }
    ********/

    /* �жϹ�Ա�����Ƿ���ȷ */
    if ( atoi(com_tel_c.lvl) != 1 && atoi(com_tel_c.lvl) != 2
      && atoi(com_tel_c.lvl) != 3 && atoi(com_tel_c.lvl) != 4)
    {
        sprintf(acErrMsg,"��Ա�������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O118");
        goto ErrExit;
    }

    /* ��������� */
    pub_base_EnDes(g_pub_tx.tx_date,"",com_tel_c.pwd);

    /* ���ӹ�Ա��Ϣ�� */
    /* strcpy(com_tel_c.csts,"2"); */
    com_tel_c.pwd_date = g_pub_tx.tx_date;
  
    ret = Com_tel_Ins(com_tel_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"���ӹ�Ա��Ϣ�����!!!");
        WRITEMSG
        goto ErrExit;
    }

	/***��auth_tel_auth���в��¼  add by wanglei 20061201***/
	if(com_tel_c.tx_type[1] == '1')/***֧�������Ա***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[2] == '1')/***����������ͨ��Ա***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[3] == '1')/***������������***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZJ1");
		strcpy(auth_tel_auth_c.au_xj,"ZJ1");
		strcpy(auth_tel_auth_c.au_zz,"ZJ2");
		strcpy(auth_tel_auth_c.au_tc,"ZJ3");
		strcpy(auth_tel_auth_c.au_de,"ZJ3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[4] == '1')/***���������Ա***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[5] == '1')/***����ϵͳ����Ա***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"HZ1");
		strcpy(auth_tel_auth_c.au_xj,"HZ1");
		strcpy(auth_tel_auth_c.au_zz,"HZ1");
		strcpy(auth_tel_auth_c.au_tc,"HZ1");
		strcpy(auth_tel_auth_c.au_de,"HZ1");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[6] == '1')/***�������ʹ�Ա***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"GZ1");
		strcpy(auth_tel_auth_c.au_xj,"GZ1");
		strcpy(auth_tel_auth_c.au_zz,"GZ1");
		strcpy(auth_tel_auth_c.au_tc,"GZ1");
		strcpy(auth_tel_auth_c.au_de,"GZ1");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[7] == '1')/*֧�й���Ա*/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	if(com_tel_c.tx_type[0] != '1'){
		ret = Auth_tel_auth_Ins(auth_tel_auth_c,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"Insert Auth_tel_auth error!!!");
			WRITEMSG
			goto ErrExit;
		}
	}
       
    strcpy(cash_mst_c.br_no,com_tel_c.br_no);
    strcpy(cash_mst_c.tel,com_tel_c.tel);
    cash_mst_c.bal = 0.00;
    cash_mst_c.ys_bal = 0.00;
    strcpy(cash_mst_c.sts,"0");

    ret = Com_cur_no_code_Dec_Sel(g_pub_tx.reply,"1=1 order by cur_no");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        ret = Com_cur_no_code_Fet_Sel(&com_cur_no_code_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            break;
        }

        /* ����һ���ڲ��˺� *
        ret=pub_base_CrtInAc("10101",g_pub_tx.ac_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"���ɸù�Ա���ֽ��˺Ŵ���!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O119");
            goto ErrExit;
        }
		**/

		strcpy(g_pub_tx.ac_no, "910110");
		strcat(g_pub_tx.ac_no, com_tel_c.tel);
		g_pub_tx.ac_no[10] = 0;
  
        strcpy(cash_mst_c.cur_no,com_cur_no_code_c.cur_no);
        strcpy(cash_mst_c.ac_no,g_pub_tx.ac_no);

    TRACE
        /* �Ǽ��ֽ�̨�� */
        ret = Cash_mst_Ins(cash_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"�Ǽ��ֽ�̨�˴��� !!");
            WRITEMSG
            goto ErrExit;
        }
    }
    Com_cur_no_code_Clo_Sel();

    TRACE
    strcpy(g_pub_tx.ac_no,com_tel_c.tel);
    strcpy(g_pub_tx.brf,"���ӹ�Ա");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"����Ա���ӳɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����Ա����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
