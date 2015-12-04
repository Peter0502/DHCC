/*************************************************
* �� �� ��: spC302.c
* ����������ά���ͻ�ʶ��ID
* ��    ��: andy
* ������ڣ�2004��01��10��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_inf_c.h"
#include "cif_sfhc_inf_c.h"

static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_id_code_rel_c  g_cif_id_code_rel;
static struct cif_id_code_rel_c  cif_id_code_rel_tmp;
	
char tmp_sts[2],type[2],num[21],tbwhere[50];
long cif_no;
		
spC302()  
{
	int ret=0,g_reply_int=0;
	char num[21], new_num[21];
	int count_tmp;
	char new_type[2];
	char hcjg[3];
	
	struct cif_inf_c         sCif_inf;
	struct cif_sfhc_inf_c    sCif_sfhc_inf;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ���ݳ�ʼ�� **/
	memset(&sCif_inf,       0x00,sizeof(sCif_inf));
	memset(&sCif_sfhc_inf,0x00,sizeof(sCif_sfhc_inf));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_id_code_rel_tmp, 0x00, sizeof(struct cif_id_code_rel_c));
	memset(num, 0x0, sizeof(num));
	memset(new_num, 0x0, sizeof(new_num));
	memset(new_type, 0x0, sizeof(new_type));
	memset(hcjg, 0x0, sizeof(hcjg));
				
	/** ȡֵ����ֵ **/
	get_zd_data("0670",tmp_sts);					/* �������� */
	get_zd_long("0280",&cif_no);					/* �ͻ��� */
	get_zd_data("0680",type);						/* ֤������ */
	get_zd_data("0620",num);						/* ֤������ */
	get_zd_data("0690", new_type);                  /* ��֤������ */
	get_zd_data("0630", new_num);					/* ��֤������ */
	get_zd_data("0220", hcjg);                  /* �˲��� */
	/** �������� **/
	/* ���ͻ����Ƿ��Ѿ����ڣ���ȡ����Ϣ */
	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
							 "cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	/*����ѡ�����ʹ���*/
	switch(tmp_sts[0])
	{
		case '0':									/*1-����*/
				/*���ͻ���\֤�������Ƿ����*/
				ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,
								 "cif_no=%ld ",cif_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,"�ͻ�֤����ͻ��Ŷ��ձ��в�����!![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C007");
					ERR_DEAL;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���ͻ�֤����ͻ��Ŷ��ձ��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}

				pub_base_strpack(g_cif_id_code_rel.id_type);
				pub_base_strpack(type);
				pub_base_strpack(num);

				if(!strcmp(g_cif_id_code_rel.id_type,type))
				{
					sprintf(acErrMsg,"�ÿͻ��Ѵ�������֤������![%s]"
							,g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C044");
					ERR_DEAL;
				}

				/*check id_typ and id_no whether exist*/
				ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,
								 "id_type='%s' and id_no='%s'",type,num);
				if( ret==0 )
				{
					sprintf(acErrMsg,"֤����Ŵ���!![%s]",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}

			memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

				g_cif_id_code_rel.cif_no=cif_no;
				strcpy(g_cif_id_code_rel.id_type,type);
				strcpy(g_cif_id_code_rel.id_no,num);

				/*�Ǽǿͻ�֤����ͻ��Ŷ��ձ�*/
				ret = Cif_id_code_rel_Ins(g_cif_id_code_rel,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}

				break;
		case '1':			/*2-�޸�*/
			sprintf(acErrMsg, "id_type=[%s], id_no=[%s]", type, num);
			WRITEMSG
			ret= Cif_id_code_rel_Sel(g_pub_tx.reply, &g_cif_id_code_rel,
				"id_type='%s' and id_no='%s'", new_type, new_num);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "SELECT cif_id_code_rel error [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			else if (ret == 0)
			{
				if (cif_no != g_cif_id_code_rel.cif_no)
				{
					sprintf(acErrMsg, "��֤����֤�������Ѿ��Ǽ�!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "C125");
					goto ErrExit;
				}
				else
				{
					if (strcmp(type, g_cif_id_code_rel.id_type) != 0)
					{
						sprintf(acErrMsg, "��֤����֤�������Ѿ��Ǽ�!!");
						WRITEMSG
						strcpy(g_pub_tx.reply, "C125");
						goto ErrExit;
					}
				}
			}
			memset(&g_cif_id_code_rel, 0x0, sizeof(struct cif_id_code_rel_c));

			ret=Cif_id_code_rel_Dec_Upd(g_pub_tx.reply, \
				"cif_no=%ld and id_type='%s' and id_no='%s'",
				cif_no, type, num);
			if (ret)
			{
				sprintf(acErrMsg,"DECLARE Cif_id_code_rel�α��쳣![%s]",
						g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				ERR_DEAL;
			}

			ret=Cif_id_code_rel_Fet_Upd(&g_cif_id_code_rel,g_pub_tx.reply);
			if (ret)
			{
				sprintf(acErrMsg,"FETCH Cif_id_code_rel�α��쳣![%s]",
						g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				ERR_DEAL;
			}

			strcpy(g_cif_id_code_rel.id_type, new_type);
			strcpy(g_cif_id_code_rel.id_no, new_num);

			ret=Cif_id_code_rel_Upd_Upd(g_cif_id_code_rel,g_pub_tx.reply);
			if (ret)
			{
				sprintf(acErrMsg,"UPDATE Cif_id_code_rel�α��쳣![%s]",
						g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				ERR_DEAL;
			}

			Cif_id_code_rel_Clo_Upd( );
	
	/* ����֤����Ϣ��ά����ݺ˲��� wudawei 20131014 */
	g_reply_int=Cif_sfhc_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� Cif_sfhc_inf �����α���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	/***���˿ͻ���Ϣ��ά��**/
	g_reply_int=Cif_sfhc_inf_Fet_Upd(&sCif_sfhc_inf,g_pub_tx.reply);
	if(g_reply_int==100)
	{
		sCif_sfhc_inf.cif_no=cif_no;
		strcpy(sCif_sfhc_inf.country,hcjg);
		strcpy(sCif_sfhc_inf.clerk_no,g_pub_tx.tel);
		sCif_sfhc_inf.mar_date=g_pub_tx.tx_date;
		g_reply_int=Cif_sfhc_inf_Ins(sCif_sfhc_inf,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_sfhc_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D107");
			goto ErrExit;
		}
		
		g_reply_int=sql_execute("update cif_inf set sts='2' where cif_no=%ld", cif_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"����cif_inf ʧ�ܣ�");
			WRITEMSG
			goto ErrExit;
		}
		
		Cif_sfhc_inf_Clo_Upd();
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d] ȡ Cif_sfhc_inf ���¼�¼���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}

	strcpy(sCif_sfhc_inf.country,hcjg);
	strcpy(sCif_sfhc_inf.clerk_no,g_pub_tx.tel);
	sCif_sfhc_inf.mar_date=g_pub_tx.tx_date;
	g_reply_int=Cif_sfhc_inf_Upd_Upd(sCif_sfhc_inf,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� Cif_sfhc_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Cif_sfhc_inf_Clo_Upd();
	 /**add end wudawei 20131014 **/
	
		  /* add begin �����ͻ���Ϣͬ�� lzy 20141112 */
			char today[9]={0};
			/*char nextday[9]={0};*/
			long nextday =0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);

			if (cif_basic_inf.type[0] == '1' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
				}
			else if (cif_basic_inf.type[0] == '2' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::�ͻ�����Ϊ[%s],����Ҫ������ͬ��.",__FILE__,__LINE__,cif_basic_inf.type);
					break;
				}
			
			sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
			vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
			fp=fopen(pcfilename,"at");
			if( fp==NULL )
			{
				vtcp_log("[%s],[%d] �������ļ�[%s]",__LINE__,__FILE__,pcfilename);
				fp = fopen(pcfilename, "wt");
				if (fp == NULL)
				{
					vtcp_log("[%s],[%d] open file[%s] error",__LINE__,__FILE__,pcfilename);
					strcpy(g_pub_tx.reply,"SM83");
					goto ErrExit;
				}
			}

			if (cif_basic_inf.type[0] == '1' )  /**���˿ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,�ֻ�,����,�Ա�\n");**/
					fprintf(fp, "%ld,%s,%s,,,,,,,\n",cif_no,new_type,new_num);
				}
			else if (cif_basic_inf.type[0] == '2' )  /**��˾�ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,���˴���\n");**/
					fprintf(fp, "%ld,%s,%s,,,,,\n",cif_no,new_type,new_num);
				}	
				fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨����[%d]", __FILE__, __LINE__,filename,ret);
					strcpy(g_pub_tx.reply,"S047");
					goto ErrExit;
				}	
			vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨�ɹ�", __FILE__, __LINE__,filename);				
		  /* add end �����ͻ���Ϣͬ�� lzy 20141112 */
		  
			break;
			
		case '2':									/*3-ɾ��*/
				/*���ͻ���\֤������\�����Ƿ����*/
				ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,
				"cif_no=%ld and id_type='%s' and id_no='%s'",cif_no,type,num);
				if( ret==100 )
				{
					sprintf(acErrMsg,"�ÿͻ���+֤������+֤���Ų�����![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C045");
					ERR_DEAL;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}

				sprintf(tbwhere,"cif_no=%ld and id_type='%s' and id_no='%s'",
						cif_no,type,num);
				ret =deldb("cif_id_code_rel",tbwhere);
				if( ret )
				{
					sprintf(acErrMsg,"ɾ�����¼�쳣!![%s]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				/* ��ѯ�ÿͻ�����������֤����¼ */
				count_tmp = sql_count( cif_id_code_rel_tmp , "cif_no" , cif_no );
				if( count_tmp ==0 )
				{					
					sprintf(acErrMsg,"�˼�¼Ϊ�ÿͻ����һ��֤����¼,����ɾ��![%s]",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C096");
					ERR_DEAL;
				}				
				break;
		default: 
				break;
	}
	
	/* �Ǽǽ�����ˮ */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
/**************
	set_zd_data("0690",cif_basic_inf.sts);		* �ͻ����� *
		****************/
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ�ʶ��IDά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"�ͻ�ʶ��IDά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}    

