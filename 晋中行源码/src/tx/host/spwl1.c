/*************************************************
* �� �� ��: sp9602.c
* �����������޸Ĳ���Ա
*
* ��    ��: rob
* ������ڣ�20030309
*
* �޸ļ�¼���޸���Ȩ��Աʱ,��auth_tel_auth���¼ 
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

int sp9602( )
{
    int ret;
    char flag[2];
    char cbr_no[10];
    char ctel[10];
	char tx_type[25];
    struct com_tel_c com_tel_c,com_tel;
    struct com_branch_c com_branch_c;
    struct auth_tx_code_c auth_tx_code_c;
    struct cash_mst_c sCash_mst;
    struct com_cur_no_code_c com_cur_no_code_c;
    struct auth_tel_auth_c auth_tel_auth_c;

    memset(&com_tel_c,0x0,sizeof(struct com_tel_c));
    memset(&com_tel,0x0,sizeof(struct com_tel_c));
    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));
    memset(&auth_tx_code_c,0x0,sizeof(struct auth_tx_code_c));
    memset(&sCash_mst,0x0,sizeof(struct cash_mst_c));
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
    get_zd_data("0300",tx_type);
	get_zd_data("0680", com_tel_c.chk_ind);
	get_zd_data("0690", com_tel_c.csts);

	strncpy(com_tel_c.tx_type, tx_type, 10);
    pub_base_strpack(com_tel_c.br_no);
	com_tel_c.tx_type[10] = 0;
    pub_base_strpack(com_tel_c.tel);

	sprintf(acErrMsg, "******** chk_ind=[%s], tx_type=[%s]", 
		com_tel_c.chk_ind, tx_type);
	WRITEMSG

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
    ret = Com_tel_Sel(g_pub_tx.reply,&com_tel,"tel='%s'",com_tel_c.tel);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ��Ա��Ϣ�����!!");
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"��Ա�Ų�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O120");
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
        sprintf(acErrMsg,"�����������û�иû�����!! [%s]",com_branch_c.br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O116");
        goto ErrExit;
    }

	/* ��������仯�Ļ���Ҫ���Ǯ���ƾ֤ */
	if (strcmp(com_tel.br_no , com_tel_c.br_no) != 0)
	{
		ret = sql_count("note_mst","tel='%s' and sts = '0'", com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "ͳ�ƹ�Աƾ֤���쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret > 0)
		{
			sprintf(acErrMsg, "�ù�Ա����ƾ֤!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O258");
			goto ErrExit;
		}

		ret = sql_count("note_mst","tel='%s' and sts = '2'", com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "ͳ�ƹ�Աƾ֤���쳣 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret > 0 )
		{
			sprintf(acErrMsg, "�ù�Ա���е������յ�ƾ֤!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O259");
			goto ErrExit;
		}

		ret = sql_count("cash_mst", "tel='%s' and sts='0' and bal != 0.00",
			com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "ͳ�ƹ�ԱǮ���쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret > 0 )
		{
			sprintf(acErrMsg, "�ù�Ա��Ǯ���л���Ǯ���ܵ���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "O260");
			goto ErrExit;
		}

		/* ���û��Ǯ��������Ǯ�� */
		ret = sql_count("cash_mst", "tel='%s'", com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "ͳ�ƹ�ԱǮ���쳣!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret == 0 )
		{
			/* ����Ǯ�� */ 
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
			g_pub_tx.ac_no[10]=0;
			
			strcpy(sCash_mst.cur_no,"01"); 
			strcpy(sCash_mst.ac_no,g_pub_tx.ac_no); 
			strcpy(sCash_mst.br_no, com_tel_c.br_no);
			strcpy(sCash_mst.tel, com_tel_c.tel);
			sCash_mst.bal = 0.00;
			sCash_mst.ys_bal = 0.00;
			strcpy(sCash_mst.sts, "0");

			/* �Ǽ��ֽ�̨�� */ 
			ret = Cash_mst_Ins(sCash_mst,g_pub_tx.reply); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"�Ǽ��ֽ�̨�˴��� !!"); 
				WRITEMSG 
				goto ErrExit; 
			}
		}
	}

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
    /*sprintf(acErrMsg,"pwd =[%s]",com_tel_c.pwd);*/
    WRITEMSG

    if (strcmp(com_tel_c.pwd,"") == 0 || com_tel_c.pwd[0] == '\0')
    {
        sprintf(acErrMsg,"û���޸�����!!");
        WRITEMSG
        strcpy(com_tel_c.pwd,com_tel.pwd);
        com_tel_c.pwd_date = com_tel.pwd_date;
    }
    else 
    {
        pub_base_strpack(com_tel_c.pwd);
        pub_base_EnDes(g_pub_tx.tx_date,"",com_tel_c.pwd);
        com_tel_c.pwd_date = g_pub_tx.tx_date;
    }

    /*sprintf(acErrMsg,"pwd =[%s]",com_tel_c.pwd);*/
    WRITEMSG

    /* �޸Ĺ�Ա��Ϣ�� 
    strcpy(com_tel_c.csts, com_tel.csts);*/
  
    ret = Com_tel_Dec_Upd(g_pub_tx.reply,"tel='%s'",com_tel_c.tel);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Com_tel_Fet_Upd(&com_tel,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
vtcp_log("11111111111111111111111");
    /* ����Ƿ��޸��˻����� */
    if (strcmp(com_tel.br_no,com_tel_c.br_no) != 0)
    {
        /* ���� cash_mst */
		ret = sql_execute("update cash_mst set br_no='%s' where tel='%s'",
			com_tel_c.br_no, com_tel.tel);
		if ( ret )
		{
			sprintf(acErrMsg, "�޸ĸù�Ա��Ǯ�����!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
    }
    
vtcp_log("2222222222222222222222222");
    sprintf(acErrMsg,"bef upd !!! ");
    WRITEMSG

	strcpy(com_tel_c.rowid,com_tel.rowid);

    ret = Com_tel_Upd_Upd(com_tel_c,g_pub_tx.reply);
vtcp_log("3333333333333333333333333");
    if (ret != 0)
    {
        sprintf(acErrMsg,"�޸Ĺ�Ա��Ϣ�����!!!");
        WRITEMSG
        goto ErrExit;
    }
    Com_tel_Clo_Upd();


      
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
	else if(com_tel_c.tx_type[4] == '1')/***������Ա***/
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
	ret = Auth_tel_auth_Ins(auth_tel_auth_c,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Insert Auth_tel_auth error!!!");
		WRITEMSG
		goto ErrExit;
	}

vtcp_log("4444444444444444444444444");
    strcpy(g_pub_tx.ac_no,com_tel_c.tel);
    strcpy(g_pub_tx.brf,"�޸Ĺ�Ա");
vtcp_log("55555555555555555555555");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"����Ա�޸ĳɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����Ա�޸�ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
