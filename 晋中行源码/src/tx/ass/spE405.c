/*************************************************
* �� �� ��:  spE405.c
* ���������� ����ѯ
*
* ��    ��:  mike
* ������ڣ� 2004��3��05��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_mst_c.h"
#include "com_branch_c.h"
#include "zjgl_mst_c.h"
		int  ttlnum1=0;			/**��ȡ��������**/
		char filename1[100];
		FILE *fp1;
int spE405()
{
    int ret;
    long lpact_no=0;
    struct ass_mst_c ass_mst;
    struct com_branch_c sComBranch;
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char cSign[5];
		char prt_flag[2];
		char cBr_no[6];
		char cStat[9];
		int ttlnum=0;			/**��ȡ��������**/
		FILE *fp;
    memset(&ass_mst,0x0,sizeof(struct ass_mst_c));
    memset(&sComBranch,0x0,sizeof(struct com_branch_c));
		memset( wherelist,0x0,sizeof(wherelist) );
		memset( tmpstr,0x0,sizeof(tmpstr) );
		memset( prt_flag,0x0,sizeof(prt_flag) );
		memset( cBr_no,0x0,sizeof(cBr_no) );
		memset( filename,0x0,sizeof(filename) );
		memset(cSign,0x0,sizeof(cSign));
		memset(cStat,0x0,sizeof(cStat));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_long("0470",&lpact_no);
    get_zd_data("0670",prt_flag);
    get_zd_data("0910",cBr_no);
	  vtcp_log("��ʼ�ǺǺ�![%s][%s]\n",prt_flag,cBr_no);

 if(prt_flag[0]!='1')
 {  
  	  vtcp_log("��ʼ�ǺǺ�![%s][%s]\n",prt_flag,cBr_no);

    if(lpact_no)
    {
    			sprintf( tmpstr," pact_no=%ld and",lpact_no );
					strcat( wherelist,tmpstr );

    }
    /**Add by hxc 091123 ��������Ҫ��ֻ���������ֻ��ѯ����������������(10001)�ɲ�ѯ������Ϣ Begin**/
     if( strlen(cBr_no) && memcmp(cBr_no,"10001",5))
   {
    			sprintf( tmpstr," br_no='%s' and",cBr_no );
					strcat( wherelist,tmpstr );
   }
   /**Add by hxc 091123 ��������Ҫ��ֻ���������ֻ��ѯ����������������(10001)�ɲ�ѯ������Ϣ End**/
    strcat(wherelist," 1=1 ORDER BY br_no,pact_no") ;
    ret=Ass_mst_Dec_Sel(g_pub_tx.reply,wherelist);
    ERR_DEAL  
    ttlnum=0;
    while(1)
		{
				memset(cSign,0x0,sizeof(cSign));
				memset(cStat,0x0,sizeof(cStat));	
				memset(&ass_mst,0x0,sizeof(struct ass_mst_c));
				ret=Ass_mst_Fet_Sel(&ass_mst, g_pub_tx.reply);
				if( ret==100 ) break;
				ERR_DEAL
				if( !ttlnum )
				{
					pub_base_AllDwnFilName( filename );
					fp = fopen( filename,"w" );
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",filename );
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					/**��ʾ�б���**/
					fprintf( fp,"~��ͬ��|������|�ꡡ��־|$�ࡡ����|$�������|@��Ϣ����|�ϱʷ�����|�����ڡ���|@��������|@��������|@�ѽ���Ϣ|@��  ��|@״����̬|\n" );
				}
			   	if(ass_mst.sign[0]=='1')
			   	{
			   		memcpy(cSign,"���",4);
			   	}
			   	else
			   	{
			   		memcpy(cSign,"�ϴ�",4);
			   	}
			   	if(ass_mst.sts[0]=='*')
			   	{
			   		memcpy(cStat,"�Ѿ�����",8);
			   	}
			   	else
			   	{
			   		memcpy(cStat,"��δ����",8);
			   	}
			   	
			   	fprintf(fp, "%ld|%s|%s|%.2lf|%.2lf|%ld|%ld|%ld|%.4f|%.4f|%.2lf|%.2lf|%s|\n",ass_mst.pact_no,ass_mst.sub_br_no,cSign,ass_mst.bal,ass_mst.ys_bal,ass_mst.ic_date,ass_mst.lst_date,ass_mst.end_date,ass_mst.rate,ass_mst.over_rate,ass_mst.intst,ass_mst.intst_acm,cStat);
		ttlnum++;
   
	}
	ret=Ass_mst_Clo_Sel();
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
	 		fclose(fp);
  		set_zd_data( DC_FILE_SND,"1" );
	}
 } 
 else /***��ʼ��ӡ***/
 {
	vtcp_log("��ʼ�ǺǺ�!\n");
	ret=prt_dzd(cBr_no,lpact_no);
	if( !ttlnum1 )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
	 		fclose(fp1);
  		set_zd_data( DC_FILE_SND,"1" );
	}
 } 
  /*
    ret = Ass_mst_Sel(g_pub_tx.reply,&ass_mst,"pact_no=%ld",ass_mst.pact_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ�������ļ�����!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O104");
        goto ErrExit;
    }   
    else if (ret == 100)
    {
        sprintf(acErrMsg,"û�иú�ͬ�ŵ�ϵͳ�ڲ����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O111");
        goto ErrExit;
    }
    if (ass_mst.sts[0] == '*')
    {
        sprintf(acErrMsg,"�ú�ͬ�ŵ��ڲ�����Ѿ�����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O112");
        goto ErrExit;
    }
   
    ret=Com_branch_Sel( g_pub_tx.reply , &sComBranch ,
             "br_no='%s'" , ass_mst.sub_br_no );
    if( ret != 0 )
    {
        sprintf( acErrMsg, "ȡ������������!\
            br_no=[%s]", g_pub_tx.tx_br_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "S019" );
        goto    ErrExit;
    }


    set_zd_data("0910",ass_mst.sub_br_no);
    set_zd_data("0250",sComBranch.br_name);
    set_zd_double("0390",ass_mst.bal);
    set_zd_long("0450",ass_mst.end_date);
    set_zd_data("0380",ass_mst.br_no);
    if (ass_mst.sign[0] == '1')
    {
        set_zd_data("0370","���");
    }
    else
    {
        set_zd_data("0370","�ϴ�");
    }
    set_zd_data("0670",ass_mst.sign);
    set_zd_long("0440",ass_mst.ic_date);
    sprintf(acErrMsg,"���˻���=[%s]",ass_mst.br_no);
    WRITEMSG
   */
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int prt_dzd(char * br_no, long pact_no)
{
    struct ass_mst_c	 s_ass_mst;
    struct zjgl_mst_c	 s_zjgl_mst;
  	char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
  	char wherelist1[1024];  /**��ѯ����**/
		char tmpstr1[512];
		int ret=0;
		char tmp_bal[21];
    memset(&s_ass_mst,0x00,sizeof(s_ass_mst));
    memset(&wherelist,0x00,sizeof(wherelist));
    memset(&tmpstr,0x00,sizeof(tmpstr));
    memset(&tmp_bal,0x00,sizeof(tmp_bal));
    memset(&wherelist1,0x00,sizeof(wherelist1));
    memset(&tmpstr1,0x00,sizeof(tmpstr1));
    memset(&s_zjgl_mst,0x00,sizeof(s_zjgl_mst));
	  vtcp_log("jinrule!\n");
	 if(pact_no)
   {
    			sprintf( tmpstr," pact_no=%ld and",pact_no );
					strcat( wherelist,tmpstr );
   }
   pub_base_strpack(br_no);
   if( strlen(br_no) )
   {
    			sprintf( tmpstr," br_no='%s' and",br_no );
					strcat( wherelist,tmpstr );
   }
    strcat(wherelist," sts!='*' and 1=1 ORDER BY br_no,pact_no") ;
	  ret=Ass_mst_Dec_Sel(g_pub_tx.reply,wherelist);
    ERR_DEAL  
    ttlnum1=0;
    while(1)
		{
			  vtcp_log("��ʼ��ѯ��!\n");
				memset(&s_ass_mst,0x0,sizeof(struct ass_mst_c));
				ret=Ass_mst_Fet_Sel(&s_ass_mst, g_pub_tx.reply);
				if( ret==100 )
				{
	 		       fclose(fp1);
				     break;
			  }
				ERR_DEAL
				if( !ttlnum1 )
				{
					pub_base_AllDwnFilName( filename1 );
					fp1 = fopen( filename1,"w" );
					if( fp1==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",filename1 );
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					/**��ʾ�б���**/
				}
          memset(&tmp_bal,0x00,sizeof(tmp_bal));
					sprintf(tmp_bal,"%.2lf",s_ass_mst.bal);
				  pub_rpt_flttomon(tmp_bal,tmp_bal);
				  char dc_ind[3];
				  memset(dc_ind,0x00,sizeof(dc_ind));
				  if(s_ass_mst.sign[0]=='1')
				  {
				  	strcpy(dc_ind,"��");
				  }
				  else
				  {
				  	strcpy(dc_ind,"��");
				  }
          /*fprintf( fp1,"^[@^[3^P^\v1\n");*/
          fprintf( fp1," \n\n\n" );
          fprintf( fp1,"                                                       �������ж��ʵ�\n" );
          fprintf( fp1,"            ��ͬ��: %ld                                                                                                  \n",s_ass_mst.pact_no );
          fprintf( fp1,"            ������: %5s                                                                        ��ӡ����: %8ld            \n",s_ass_mst.br_no,g_pub_tx.tx_date);
          fprintf( fp1,"           �����������ө������ө����������������������������ө����������ө��������ө������ө�����������������������������\n" );
          fprintf( fp1,"           �� ��   �ک� ��� ��    ��  ��  ��  ��  ��      ��   δ��   �� ��   �ک� ��� ��    ��  ��  ��  ��  ��      ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥����������������������������          �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��%8ld�� %2s   ��       %21s��   ����   ��        ��      ��                            ��\n",s_ass_mst.lst_date,dc_ind,tmp_bal);
          fprintf( fp1,"           �����������㩥�����㩥���������������������������㩥���ө����㩥�������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            �� �� �� �� ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥���������������������������� �� �� ҵ �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            �� δ �� δ ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥���������������������������� �� �� �� �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            �� �� �� �� ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥���������������������������� �� �� �� �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            ��    ��    ��        ��      ��                            ��\n" );
      /*  fprintf( fp1,"           �����������㩥�����㩥����������������������������    ��    �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            ��    ��    ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥����������������������������    ��    �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            ��    ��    ��        ��      ��                            ��\n" );*/
          fprintf( fp1,"           �����������㩥�����㩥���������������������������㩥���۩����㩥�������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           �������Ժ�      ��                            ��          �������Ժ�      ��                            ��\n" );
          fprintf( fp1,"           �����������۩������۩����������������������������۩����������۩��������۩������۩�����������������������������\n" );
          fprintf( fp1,"                                                                                                                         \n" );
          fprintf( fp1,"             1.����ҵ����˶������ʣ����������ڽ��˶��˵���һ�����º��ͻؽ�����        ����������������������������      \n" );
          fprintf( fp1,"                                                                                       ��                        ��      \n" );
          fprintf( fp1,"               ��ҵ���С�                                                              ��                        ��      \n" );
          fprintf( fp1,"                                                                                       ��     ��  ҵ  ǩ  ��     ��      \n" );
          fprintf( fp1,"                                                                                       ��                        ��      \n" );
          fprintf( fp1,"                                                                                       ��                        ��      \n" );
          fprintf( fp1,"                                                                                       ����������������������������      \n" );
          fprintf( fp1,"                                                                                                                          \n" );       
          fprintf( fp1," \n" );
   		    ttlnum1++;
          if(ttlnum1%2==0)      
          fprintf( fp1," \f" );
	}
	ret=Ass_mst_Clo_Sel();
   pub_base_strpack(br_no);
   if( strlen(br_no) )
   {
    			sprintf( tmpstr1," opn_br_no='%s' and",br_no);
					strcat( wherelist1,tmpstr1 );
   }
    strcat(wherelist1," substr(acc_no,1,3)='407' and 1=1 ORDER BY opn_br_no,acc_no") ;
	  ret=Zjgl_mst_Dec_Sel(g_pub_tx.reply,wherelist1);
    ERR_DEAL  
    ttlnum1=0;
    while(1)
		{
			  vtcp_log("��ʼ��ѯ��!\n");
				memset(&s_zjgl_mst,0x0,sizeof(struct zjgl_mst_c));
				ret=Zjgl_mst_Fet_Sel(&s_zjgl_mst, g_pub_tx.reply);
				if( ret==100 )
				{
	 		       fclose(fp1);
				     break;
			  }
				ERR_DEAL
				if( !ttlnum1 )
				{
					pub_base_AllDwnFilName( filename1 );
					fp1 = fopen( filename1,"a+" );
					if( fp1==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",filename1 );
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					/**��ʾ�б���**/
				}
           memset(&tmp_bal,0x00,sizeof(tmp_bal));
					sprintf(tmp_bal,"%.2lf",s_zjgl_mst.dr_bal);
				  pub_rpt_flttomon(tmp_bal,tmp_bal);

          /*fprintf( fp1,"^[@^[3^P^\v1\n");*/
          fprintf( fp1," \n\n\n" );
          fprintf( fp1,"                                                          �������ж��ʵ�\n" );
          fprintf( fp1,"            ��Ŀ��: %7s                                                                                                  \n",s_zjgl_mst.acc_no );
          fprintf( fp1,"            ������: %5s                                                                        ��ӡ����: %8ld            \n",s_zjgl_mst.opn_br_no,g_pub_tx.tx_date);
          fprintf( fp1,"           �����������ө������ө����������������������������ө����������ө��������ө������ө�����������������������������\n" );
          fprintf( fp1,"           �� ��   �ک� ��� ��    ��  ��  ��  ��  ��      ��   δ��   �� ��   �ک� ��� ��    ��  ��  ��  ��  ��      ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥����������������������������          �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��%8ld�� ��   ��       %21s��   ����   ��        ��      ��                            ��\n",s_zjgl_mst.lst_date,tmp_bal);
          fprintf( fp1,"           �����������㩥�����㩥���������������������������㩥���ө����㩥�������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            �� �� �� �� ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥���������������������������� �� �� ҵ �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            �� δ �� δ ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥���������������������������� �� �� �� �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            �� �� �� �� ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥���������������������������� �� �� �� �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            ��    ��    ��        ��      ��                            ��\n" );
        /*fprintf( fp1,"           �����������㩥�����㩥����������������������������    ��    �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            ��    ��    ��        ��      ��                            ��\n" );
          fprintf( fp1,"           �����������㩥�����㩥����������������������������    ��    �����������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           ��        ��      ��                            ��    ��    ��        ��      ��                            ��\n" );*/
          fprintf( fp1,"           �����������㩥�����㩥���������������������������㩥���۩����㩥�������㩥�����㩥����������������������������\n" );
          fprintf( fp1,"           �������Ժ�      ��                            ��          �������Ժ�      ��                            ��\n" );
          fprintf( fp1,"           �����������۩������۩����������������������������۩����������۩��������۩������۩�����������������������������\n" );
          fprintf( fp1,"                                                                                                                         \n" );
          fprintf( fp1,"             1.����ҵ����˶������ʣ����������ڽ��˶��˵���һ�����º��ͻؽ�����        ����������������������������      \n" );
          fprintf( fp1,"                                                                                       ��                        ��      \n" );
          fprintf( fp1,"               ��ҵ���С�                                                              ��                        ��      \n" );
          fprintf( fp1,"                                                                                       ��     ��  ҵ  ǩ  ��     ��      \n" );
          fprintf( fp1,"                                                                                       ��                        ��      \n" );
          fprintf( fp1,"                                                                                       ��                        ��      \n" );
          fprintf( fp1,"                                                                                       ����������������������������      \n" );
          fprintf( fp1,"                                                                                                                         \n" );       
          fprintf( fp1," \n" );
   		    ttlnum1++;
          if(ttlnum1%2==0)      
          fprintf( fp1," \f" );
	}
	ret=Zjgl_mst_Clo_Sel();
OkExit:	
	  return 0;
ErrExit:
    return 1;
}
