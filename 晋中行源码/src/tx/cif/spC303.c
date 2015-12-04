/*************************************************
* �� �� ��: spC303.c
* �����������ͻ�������Ϣά��
*
* ��    ��: 
* ������ڣ�2004��02��05��
* �޸ļ�¼��   
*     1. ��    ��:	2004-05-31
*        �� �� ��:	andy
*        �޸�����:	�����޸ı�������ϵ�绰����ϵ��ַ
*     2. 
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_inf_chg_log_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_alias_inf_c.h"

static struct cif_basic_inf_c    g_cif_basic_inf;
static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_inf_chg_log_c  cif_inf_chg_log;
static struct cif_email_inf_c    cif_email_inf;
static struct cif_addr_inf_c    cif_addr_inf;
static struct cif_alias_inf_c    cif_alias_inf;
static struct cif_email_inf_c    cif_email_inf_tmp;
static struct cif_addr_inf_c    cif_addr_inf_tmp;
static struct cif_alias_inf_c    cif_alias_inf_tmp;

char memo[41],tbwhere[50];

		
spC303()  
{ 	
	int ret=0;
	int cmp_code=0;
	char sts[2];	
	char cName[61];
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ���ݳ�ʼ�� **/
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&g_cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_inf_chg_log, 0x00, sizeof(struct cif_inf_chg_log_c));	
	memset (&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
	memset (&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));
	memset (&cif_email_inf_tmp, 0x00, sizeof(struct cif_email_inf_c));
	memset (&cif_addr_inf_tmp, 0x00, sizeof(struct cif_addr_inf_c));
	memset (&cif_alias_inf, 0x00, sizeof(struct cif_alias_inf_c));
	memset (&cif_alias_inf_tmp, 0x00, sizeof(struct cif_alias_inf_c));
	strcpy( sts , "" );
	memset(cName,0x00,sizeof(cName));
				
	/** ȡֵ����ֵ **/
	get_zd_data("0930",g_cif_basic_inf.sts);			/* �ͻ���ǰ״̬*/
	g_cif_basic_inf.cif_seqn=0;							/* �ͻ��˻����*/
	get_zd_long("0280",&g_cif_basic_inf.cif_no);		/* �ͻ��� */
	
	get_zd_data("0720",sts);							/* �������� */
	get_zd_data("0670",g_cif_basic_inf.type);			/* �ͻ����� */
	/****
	get_zd_data("0260",g_cif_basic_inf.name);			
	****/
	get_zd_data("0260",cName);			/* �ͻ����� */

	get_zd_data("0680",g_cif_basic_inf.local);			/* ���ر�־ */
	get_zd_data("0690",g_cif_basic_inf.poten);			/* Ǳ�ڱ�־ */
	get_zd_data("0700",g_cif_basic_inf.shareholder_ind);/* ���йɶ� */
	get_zd_data("0710",g_cif_basic_inf.lvl);			/* �ͻ����� */
	
	get_zd_data( "0810",cif_email_inf_tmp.email);		/*�ͻ���ϵ�绰*/
	get_zd_data( "0980",cif_email_inf_tmp.addr_type);	/*��ϵ��ʽ*/
	get_zd_data( "0820",cif_addr_inf_tmp.addr);			/*�ͻ���ϵ��ַ*/		

	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*�޸�*/
				/* �޸Ŀͻ�������Ϣ�� */
				ret = Cif_basic_inf_Dec_Upd( g_pub_tx.reply,"cif_no=%ld", \
											g_cif_basic_inf.cif_no);
			    if( ret )
	            {
				   sprintf(acErrMsg,"DECLARE Cif_id_code_rel�쳣[%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    ret = Cif_basic_inf_Fet_Upd(&cif_basic_inf,g_pub_tx.reply);
			    if( ret )
			    {
				   sprintf(acErrMsg,"FETCH Cif_id_code_rel�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }

				g_cif_basic_inf.crt_date=cif_basic_inf.crt_date;
				g_cif_basic_inf.crt_time=cif_basic_inf.crt_time;
				g_cif_basic_inf.cif_seqn=cif_basic_inf.cif_seqn;
				
				
				if( strcmp(g_cif_basic_inf.sts ,cif_basic_inf.sts) && strlen(g_cif_basic_inf.sts)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"sts" ,cif_basic_inf.sts, g_cif_basic_inf.sts, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.sts ,cif_basic_inf.sts,sizeof(g_cif_basic_inf.sts));
					
				if( strcmp(g_cif_basic_inf.type ,cif_basic_inf.type)  && strlen(g_cif_basic_inf.type)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"type" ,cif_basic_inf.type, g_cif_basic_inf.type, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.type ,cif_basic_inf.type,sizeof(g_cif_basic_inf.type));
					

				if( strcmp(cName ,cif_basic_inf.name)  && strlen(cName)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no,"cif_basic_inf" ,"name" ,cif_basic_inf.name, cName, "" );
					memcpy(g_cif_basic_inf.name ,cName,sizeof(g_cif_basic_inf.name));

					if(ret)	ERR_DEAL
				}else
				{
					memcpy(g_cif_basic_inf.name ,cif_basic_inf.name,sizeof(g_cif_basic_inf.name));
				}
				if( strcmp(g_cif_basic_inf.local ,cif_basic_inf.local)  && strlen(g_cif_basic_inf.local)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"local" ,cif_basic_inf.local, g_cif_basic_inf.local, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.local ,cif_basic_inf.local,sizeof(g_cif_basic_inf.local));
				
				if( strcmp(g_cif_basic_inf.poten ,cif_basic_inf.poten)  && strlen(g_cif_basic_inf.poten)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"poten" ,cif_basic_inf.poten, g_cif_basic_inf.poten, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.poten ,cif_basic_inf.poten,sizeof(g_cif_basic_inf.poten));
					
				if( strcmp(g_cif_basic_inf.shareholder_ind ,cif_basic_inf.shareholder_ind) && strlen(g_cif_basic_inf.shareholder_ind)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"shareholder_ind" ,cif_basic_inf.shareholder_ind, g_cif_basic_inf.shareholder_ind, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.shareholder_ind ,cif_basic_inf.shareholder_ind,sizeof(g_cif_basic_inf.shareholder_ind));
					
				if( strcmp(g_cif_basic_inf.lvl ,cif_basic_inf.lvl)  && strlen(g_cif_basic_inf.lvl)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf(g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"lvl" ,cif_basic_inf.lvl, g_cif_basic_inf.lvl, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.lvl ,cif_basic_inf.lvl,sizeof(g_cif_basic_inf.lvl));
					
			   strcpy(g_cif_basic_inf.rowid,cif_basic_inf.rowid);	

				ret = Cif_basic_inf_Upd_Upd(g_cif_basic_inf,g_pub_tx.reply);
				if( ret )
				{
					sprintf(acErrMsg,"UPDATE Cif_id_code_rel�α��쳣![%s]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}

				Cif_basic_inf_Clo_Upd( );
				
				ret = sql_count("cif_alias_inf","cif_no=%ld and alias_seqn=1", \
								 g_cif_basic_inf.cif_no );
				if( ret<0 )
	            {
				   sprintf(acErrMsg,"TRANSFER SQL_COUNT�쳣![%ld]", ret);
				   WRITEMSG
				   ERR_DEAL;
			    }
				else if( ret>0 )
				{
					/* �޸Ŀͻ�����(������Ϣ��) */
					ret = Cif_alias_inf_Dec_Upd(g_pub_tx.reply,
						"cif_no=%ld and alias_seqn=1",g_cif_basic_inf.cif_no);
			    	if( ret )
	            	{
				   		sprintf(acErrMsg,"DECLARE Cif_alias_inf�쳣![%ld]",ret);
				  	 	WRITEMSG
				 	  	ERR_DEAL;
			    	}

				    ret = Cif_alias_inf_Fet_Upd(&cif_alias_inf,g_pub_tx.reply);
				    if( ret )
			    	{
					   	sprintf( acErrMsg,"FETCH Cif_alias_inf�쳣![%ld]",ret );
				  		WRITEMSG
				   		ERR_DEAL;
			    	}

					strcpy( cif_alias_inf.alias , cName);/*����*/

					ret = Cif_alias_inf_Upd_Upd(cif_alias_inf,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"UPDATE cif_alias_inf �쳣![%ld]",ret);
						WRITEMSG
						ERR_DEAL;
					}

					Cif_alias_inf_Clo_Upd( );				
				}
					
				/* �޸Ŀͻ���ϵ�绰 */
				ret = sql_count("cif_email_inf","cif_no=%ld and addr_seqn=1", \
								 g_cif_basic_inf.cif_no );
				if( ret<0 )
	            {
				   sprintf(acErrMsg,"TRANSFER SQL_COUNT�쳣![%ld]", ret);
				   WRITEMSG
				   ERR_DEAL;
			    }
				else if( ret>0 )
				{
					ret=Cif_email_inf_Dec_Upd(g_pub_tx.reply,
				   		"cif_no=%ld and addr_seqn=1",g_cif_basic_inf.cif_no);
			    	if( ret )
	            	{
				   		sprintf(acErrMsg,"DECLARE Cif_email_inf�쳣![%ld]",ret);
				   		WRITEMSG
				   		ERR_DEAL;
			    	}

				    ret = Cif_email_inf_Fet_Upd(&cif_email_inf,g_pub_tx.reply);
				    if( ret )
				    {
					   sprintf(acErrMsg,"FETCH cif_email_inf�쳣![%ld]",ret);
					   WRITEMSG
					   ERR_DEAL;
			    	}

					strcpy( cif_email_inf.email , cif_email_inf_tmp.email);
					strcpy( cif_email_inf.addr_type , cif_email_inf_tmp.addr_type);	
					ret = Cif_email_inf_Upd_Upd(cif_email_inf,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"UPDATE cif_email_inf�쳣![%ld]",ret );
						WRITEMSG
						ERR_DEAL;
					}

					Cif_email_inf_Clo_Upd( );				
				}
				
				/* �޸Ŀͻ���ϵ��ַ */
				ret = sql_count("cif_addr_inf","cif_no=%ld and addr_seqn=1", \
								 g_cif_basic_inf.cif_no );
				if( ret<0 )
	            {
				   sprintf(acErrMsg,"TRANSFER SQL_COUNT�쳣![%ld]", ret);
				   WRITEMSG
				   ERR_DEAL;
			    }
				else if( ret>0 )
				{
					ret = Cif_addr_inf_Dec_Upd(g_pub_tx.reply,
					   	"cif_no=%ld and addr_seqn=1",g_cif_basic_inf.cif_no);
			    	if( ret )
	            	{
				   		sprintf(acErrMsg,"DECLARE Cif_email_inf�쳣[%ld]",ret );
				   		WRITEMSG
				   		ERR_DEAL;
			    	}
			    	ret = Cif_addr_inf_Fet_Upd(&cif_addr_inf,g_pub_tx.reply);
			    	if( ret )
			    	{
				   		sprintf(acErrMsg,"FETCH cif_email_inf�쳣![%ld]", ret);
				   		WRITEMSG
				   		ERR_DEAL;
			    	}

					strcpy( cif_addr_inf.addr , cif_addr_inf_tmp.addr );

					ret = Cif_addr_inf_Upd_Upd(cif_addr_inf,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"UPDATE cif_addr_inf�쳣![%ld]",ret);
						WRITEMSG
						ERR_DEAL;
					}
					Cif_addr_inf_Clo_Upd( );
				}

		  /* add begin �����ͻ���Ϣͬ�� lzy 20141204 */
			char today[9]={0};
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			nextday = pub_base_daynumLAT(today,1);
			/**vtcp_log("%s %d:: nextday[%ld]  ",__FILE__,__LINE__,nextday);**/
			
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
					fprintf(fp, "%ld,,,%s,%s,,,,,\n",g_cif_basic_inf.cif_no,cName,cif_addr_inf_tmp.addr);
				}
			else if (cif_basic_inf.type[0] == '2' )  /**��˾�ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,���˴���\n");**/
					fprintf(fp, "%ld,,,%s,%s,,,\n",g_cif_basic_inf.cif_no,cName,cif_addr_inf_tmp.addr);
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
		  /* add end �����ͻ���Ϣͬ�� lzy 20141204 */

				break;
				
		case '2':									/*ɾ��*/
				/* ���ͻ����Ƿ������ļ����� */
				/*���ÿͻ�������̨��������ɾ��*/
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"cif_no=%ld", \
								 g_cif_basic_inf.cif_no);
				if( ret==0 )
				{
				 	sprintf(acErrMsg,"�ÿͻ��Ŵ��ڻ���̨��,����ɾ��!![%s],\
							g_dd_mst.cif_no=[%ld],g_cif_basic_inf.cif_no=[%ld]",
							g_pub_tx.reply,g_dd_mst.cif_no, \
							g_cif_basic_inf.cif_no);
				 	WRITEMSG
				 	strcpy(g_pub_tx.reply,"C048");
				 	ERR_DEAL;
				}
				else if( ret && ret!=100)
			 	{
				 	sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%ld]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W015");
					ERR_DEAL;
				}

				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"cif_no=%ld", \
								 g_cif_basic_inf.cif_no);
				if( ret==0 )
				{
				 	sprintf(acErrMsg,"�ÿͻ��Ŵ��ڶ���̨��,����ɾ��![%ld]",ret);
				 	WRITEMSG
				 	strcpy(g_pub_tx.reply,"C049");
				 	ERR_DEAL;
				 }
				 else if( ret && ret!=100)
				 {
				 	sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%ld]",ret );
					WRITEMSG
					strcpy(g_pub_tx.reply,"W017");
					ERR_DEAL;
				 }

				ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"cif_no=%ld", \
								 g_cif_basic_inf.cif_no);
				if( ret==0 )
				{
				 	sprintf(acErrMsg,"�ÿͻ��Ŵ��ڴ���̨��,����ɾ��![%s]",ret);
				 	WRITEMSG
				 	strcpy(g_pub_tx.reply,"C050");
				 	ERR_DEAL;
				}
				else if( ret && ret!=100 )
				{
				 	sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%ld]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W019");
					ERR_DEAL;
				}

				/*ȡ���ͻ�����*/
				ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,
							 		"cif_no=%ld",g_cif_basic_inf.cif_no);
				if( ret )
				{
				 	sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ���쳣![%ld]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W019");
					ERR_DEAL;
				}

				memset(tbwhere,0,sizeof(tbwhere));
				sprintf(tbwhere,"cif_no=%ld",g_cif_basic_inf.cif_no);

				if(g_cif_basic_inf.type[0]=='1')
				{
					ret =deldb("cif_per_inf",tbwhere);
					if( ret )
					{
				   		sprintf(acErrMsg,"ɾ�����˿ͻ�������Ϣ���쳣[%ld]",ret);
				  		WRITEMSG
				   		strcpy(g_pub_tx.reply,"C006");
				   		ERR_DEAL;
					}
				}
				else if(g_cif_basic_inf.type[0]=='2')
				{
					ret =deldb("cif_cop_inf",tbwhere);
					if( ret )
					{
				   		sprintf(acErrMsg,"ɾ����˾�ͻ�������Ϣ���쳣[%ld]",ret);
				  		WRITEMSG
				   		strcpy(g_pub_tx.reply,"C006");
				   		ERR_DEAL;
					}
				}

				/*ɾ���ͻ�֤����ͻ��Ŷ��ձ�����*/
				ret = deldb("cif_id_code_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ�֤����ͻ��Ŷ��ձ��쳣[%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ�������Ϣ������*/
				ret = deldb("cif_basic_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ�������Ϣ���¼�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ���ַ��Ϣ������*/
				ret = deldb("cif_addr_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ���ַ��Ϣ���¼�쳣!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ�������Ϣ������*/
				ret = deldb("cif_alias_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ��������Ϣ���¼�쳣!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ���������Ϣ������*/
				ret = deldb("cif_blklst_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ���������Ϣ���¼�쳣!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ�-�ͻ���ϵ������*/
				ret = deldb("cif_cif_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ�-�ͻ���ϵ���¼�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ�����ӵ�ַ��Ϣ������*/
				ret = deldb("cif_email_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ�����ӵ�ַ��Ϣ���¼�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ����ʽ��Ϣ������*/
				ret = deldb("cif_lifsty_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ����ʽ���¼�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ�-�ͻ������ϵ������*/
				ret = deldb("cif_mger_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ�-�ͻ������ϵ���쳣[%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ��������ڲ�Ʒ��Ϣ������*/
				ret = deldb("cif_oth_prdt_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ��������ڲ�Ʒ��Ϣ���쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ���Ʒ��ϵ������*/
				ret = deldb("cif_prdt_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ���Ʒ��ϵ���¼�쳣!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ�������ʷ��Ϣ������*/
				ret = deldb("cif_wrk_hst_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ�������ʷ��Ϣ���¼�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*ɾ���ͻ����Ŵ��Ŷ��ձ�����*/
				ret = deldb("cif_crdt_code",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ���ͻ����Ŵ��Ŷ��ձ��¼�쳣![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}
				break;
		default:
				sprintf(acErrMsg,"���������쳣,����ϵ���Ļ���![%s]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C108");
				ERR_DEAL;				
				break;
	}

	/* �Ǽǽ�����ˮ */
	g_pub_tx.ac_id = g_cif_basic_inf.cif_no;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ�������Ϣά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�ͻ�������Ϣά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
/**����ͻ�������Ϣ
Ins_cif_alias_inf()
{
		int iRc = 0;
		if(strlen(g_cif_basic_inf.name)!=0)
			{
				cif_alias_inf.cif_no=g_cif_basic_inf.cif_no;
				cif_alias_inf.alias_seqn=1;
				strcpy( cif_alias_inf.alias , g_cif_basic_inf.name);  *����*
				iRc=Cif_alias_inf_Ins(&cif_alias_inf,g_pub_tx.reply);
				if(iRc)
					{
						vtcp_log("%s,%d,�������ݿ����%d��",__FILE__, __LINE__,iRc);
					}
			}
		return iRc;
}
**�޸���ϵ�绰**
Ins_cif_email_inf()
{
	int iRc = 0;
	if(strmcp(cif_email_inf_tmp.email) != 0)
		{
			strcpy( cif_email_inf.email , cif_email_inf_tmp.email);
			strcpy(cif_email_inf.link_man, g_cif_basic_inf.name);
			cif_email_inf.cif_no=g_cif_basic_inf.cif_no;
			cif_email_inf.addr_seqn=1;
			iRc=Cif_email_inf_Ins(&cif_email_inf,g_pub_tx.reply);
				if(iRc)
					{
						vtcp_log("%s,%d,�������ݿ����%d��",__FILE__, __LINE__,iRc);
					}
		}
	return iRc;
}
**����ͻ���ַ**
Ins_cif_addr_inf()
{
	int iRc = 0;
	if(strmcp(cif_addr_inf_tmp.email) != 0)
		{
			strcpy( cif_email_inf.email , cif_email_inf_tmp.email);
			strcpy(cif_addr_inf.link_man, g_cif_basic_inf.name);
			cif_addr_inf.cif_no=g_cif_basic_inf.cif_no;
			cif_addr_inf.addr_seqn=1;
			iRc=Cif_addr_inf_Ins(&cif_email_inf,g_pub_tx.reply);
				if(iRc)
					{
						vtcp_log("%s,%d,�������ݿ����%d��",__FILE__, __LINE__,iRc);
					}
		}
	return iRc;
}**/

