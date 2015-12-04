/***************************************************************
* �� �� ��: spD232.c
* ��������: ���Ĵ�ӡ����(�ֹ�����)
*
* ��    ��: jack
* �������: 2004��3��31��
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"
#include "com_branch_c.h"

spD232()
{
    int ret;
    int bat_no;
	int flag;
	char reprt[2];
    char br_no[6];
    char cFileName[50];
    FILE *fp;
	char wherelist[500];
	long beg_bat_no=0, end_bat_no=0;

    struct mo_city_c mo_city_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
	struct com_branch_c com_branch_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));
	memset(wherelist, 0x0, sizeof(wherelist));
	memset(reprt, 0x0, sizeof(reprt));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0910",br_no);
    get_zd_data("0670",reprt);
    strcpy(mo_city_c.in_out_ind,"2");
    strcpy(mo_city_c.sts,"A");
    mo_city_c.tx_date = g_pub_tx.tx_date;


    if (strcmp(br_no, "") != 0)
	{
		sprintf(wherelist, "br_no='%s' and br_type in ('3','4','5')", br_no);
	}
	else
	{
		strcat(wherelist, " br_type in ('3','4','5') ");
	}

	strcat(wherelist, " order by br_no");

	sprintf(acErrMsg,"wherelist = [%s]\n", wherelist);
	WRITEMSG

    ret = pub_base_AllDwnFilName( cFileName );
    if (ret != 0)
    {
        sprintf( acErrMsg, "�����ļ��������!!" );
        WRITEMSG
        strcpy(g_pub_tx.reply,"D126");
        goto ErrExit;
    }

    fp = fopen( cFileName, "w" );
    if( fp==NULL )
    {
        sprintf( acErrMsg, "��ʱ�ļ�����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B080" );
        goto ErrExit;
    }

   	fprintf(fp,"~%s|%s|%s|%s|%s|%s|%s|%s|\n","���κ�","�������","��������",
			"�������˺�", "�տ����˺�","���","��������","��ӡ����");

	ret = Com_branch_Dec_Sel(g_pub_tx.reply, wherelist);
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECALRE FOR SELECT COM_BRANCH!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	flag = 0;

	while(1)
	{
		ret = Com_branch_Fet_Sel(&com_branch_c, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH FOR SELECT COM_BRANCH!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			/***
			if (strcmp(br_no, "") != 0)
			{
				sprintf(acErrMsg,"û�иû�����!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D173");
				goto ErrExit;
			}
			****/
			if (flag == 0)
			{
				sprintf(acErrMsg,"û�и�Ʊ��!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D160");
				goto ErrExit;
			}
			break;
		}

		/** delete by rob at 20051013 for ���ĺͷ����ĵĴ��� 
    	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
        	      and br_no='%s'", com_branch_c.br_no);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d] [%s]",
				ret, com_branch_c.br_no);
        	WRITEMSG
			goto ErrExit;
    	}
		else if (ret == 100)
		{
			sprintf(acErrMsg, "���׻�����ϵ����û�м�¼!!");
			WRITEMSG
			continue;
		}

    	if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    	{
        	* �û������Ǹñ�ҵ����ϼ��������ܴ�ӡ *
        	sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",
					mo_city_c.bat_no, com_branch_c.br_no);
        	WRITEMSG
			if (strcmp(br_no,"") != 0)
			{
        		strcpy(g_pub_tx.reply,"D144");
        		goto ErrExit;
			}
			continue;
    	}
		*/

		/****** 
		if (strcmp(com_tx_br_rel_c.up_br_no,com_branch_c.br_no) == 0)
		{
			sprintf(acErrMsg,"�Լ�����Ͳ��ô�ӡ��!!");
			WRITEMSG
			* ������Щ��¼��״̬ *
			ret = sql_execute("update mo_city set sts='B' where cd_ind='1' \
					and payer_br_no= '%s' and tx_date=%ld and sts='A'", 
					com_branch_c.br_no, g_pub_tx.tx_date); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"����ͬ�ǵǼǲ��еļ�¼����!! [%d]",ret); 
				WRITEMSG 
				strcpy(g_pub_tx.reply,"D171"); 
				goto ErrExit;
			} 

			ret = sql_execute("update mo_city set sts='B' where cd_ind='2' \
					and payee_br_no= '%s' and tx_date=%ld and sts='A'", 
					com_branch_c.br_no, g_pub_tx.tx_date); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"����ͬ�ǵǼǲ��еļ�¼����!! [%d]",ret); 
				WRITEMSG 
				strcpy(g_pub_tx.reply,"D171"); 
				goto ErrExit;
			} 
			else 
			{ 
				if (strcmp(br_no, "") != 0)
				{
					strcpy(g_pub_tx.reply,"D170"); 
					set_zd_data("0130","�Լ�����Լ����ô�ӡ!!");
					goto OkExit;
				}
			} 
        	continue;
		}
		****/

    	/* �������κ� */
    	ret = sql_max_int("mo_city","bat_no","trans_mode='1' and \
				in_out_ind='2'");
    	if (ret < 0)
    	{
        	sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}

    	bat_no = ret + 1;

		/* if (strcmp(br_no, "") != 0) */
		if (reprt[0] == 'N')
		{
			/* �ش� */
			beg_bat_no = 0;
			get_zd_long("0530", &beg_bat_no);
			if ( beg_bat_no == 0 )
			{
   				ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_br_no='%s' and \
					sts='B'", g_pub_tx.tx_br_no);
   				if (ret != 0)
   				{
       				sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       				WRITEMSG
       				goto ErrExit;
   				}
			}
			else
			{
   				ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_br_no='%s' and \
					sts='B' and bat_no=%ld", g_pub_tx.tx_br_no, beg_bat_no);
   				if (ret != 0)
   				{
       				sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       				WRITEMSG
       				goto ErrExit;
   				}
			}
		}
		else
		{
			/* ��һ�δ�ӡ */
   			ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_br_no='%s' and \
					sts='A'", g_pub_tx.tx_br_no);
   			if (ret != 0)
   			{
       			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
       			WRITEMSG
       			goto ErrExit;
   			}
		}

    	while(1)
    	{
        	ret = Mo_city_Fet_Upd(&mo_city_c, g_pub_tx.reply);
        	if (ret != 0 && ret != 100)
        	{
            	sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�����!! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
        	else if (ret == 100)
        	{
				/****
				if (flag == 0)
				{
					sprintf(acErrMsg,"û�и���Ʊ��!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D160");
					goto ErrExit;
				}
				*****/
            	break;
        	}
 
			if (strcmp(mo_city_c.sts, "A") == 0)
			{
        		mo_city_c.bat_no = bat_no;
				if (flag == 0)
				{
					beg_bat_no = bat_no;
					end_bat_no = bat_no;
				}
				else
				{
					end_bat_no = bat_no;
				}
			}
			else
			{
				beg_bat_no = end_bat_no = mo_city_c.bat_no;
			}

			mo_city_c.pr_cnt = mo_city_c.pr_cnt + 1;
        	if (mo_city_c.cd_ind[0] == '1')
        	{
				/* ����跽 */
				if (strcmp(mo_city_c.payer_br_no,com_branch_c.br_no) != 0)
				{
					continue;
				}
				flag = flag + 1;
            	fprintf(fp,"%d|%s|%s|%s|%s|%lf|%s|%d|\n",mo_city_c.bat_no,
					com_branch_c.br_no,com_branch_c.br_name, 
					mo_city_c.payer_ac_no, mo_city_c.payee_ac_no, 
					mo_city_c.bill_amt, "����跽", mo_city_c.pr_cnt);
        	}
        	else
        	{
				/* ������� */
				if (strcmp(mo_city_c.payee_br_no,com_branch_c.br_no) != 0)
				{
					continue;
				}
				flag = flag + 1;
            	fprintf(fp,"%d|%s|%s|%s|%s|%lf|%s|%d|\n",mo_city_c.bat_no,
					com_branch_c.br_no, com_branch_c.br_name, 
					mo_city_c.payer_ac_no, mo_city_c.payee_ac_no, 
					mo_city_c.bill_amt, "�������", mo_city_c.pr_cnt);
        	}

        	strcpy(mo_city_c.sts,"B");
        	ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
        	if (ret != 0)
        	{
            	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            	WRITEMSG
            	goto ErrExit;
        	}
    	}
    	Mo_city_Clo_Upd();
	}

    fclose(fp);
	
   	set_zd_data( DC_FILE_SND, "1" );        /* �������ͱ�־ */


	/* set_zd_long("0480",flag); */

	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�ֹ��������Ĵ�ӡ");
	g_pub_tx.ac_seqn = beg_bat_no;
	g_pub_tx.svc_ind = end_bat_no;
	sprintf(acErrMsg, "beg=[%ld] , [%ld]", beg_bat_no, end_bat_no);
	WRITEMSG
	strcpy(g_pub_tx.no_show, "1");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���������ֹ���ӡ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���������ֹ���ӡ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

