/*************************************************
* �� �� ��: spC106.c
* �����������ͻ���ַ��Ϣ¼��
*
* ��    ��: 
* ������ڣ�2004��01��08��
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
#include "cif_addr_inf_c.h"

static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_addr_inf_c     cif_addr_inf;

spC106()  
{ 	
	int ret = 0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ���ݳ�ʼ�� **/
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_long("0280",&cif_addr_inf.cif_no);		/* �ͻ��� */
	get_zd_data("0670",cif_addr_inf.addr_type);		/* ��ַ���� */
	get_zd_data("0230",cif_addr_inf.country);		/* ���ڹ��� */
	get_zd_data("0810",cif_addr_inf.addr);			/* ��ַ */
	get_zd_data("0650",cif_addr_inf.post_code);		/* �������� */

		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]", \
				cif_addr_inf.post_code);
		WRITEMSG
	/**��������**/
	/*���ͻ����Ƿ��Ѿ�����*/
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf , \
						  "cif_no=%ld",cif_addr_inf.cif_no);
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

	/*��ѯ��ַ��Ϣ���Ƿ���ڼ�¼*/
	ret=sql_count("cif_addr_inf","cif_no=%ld",cif_addr_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		cif_addr_inf.addr_seqn=1;

		/* �Ǽǿͻ���ַ��Ϣ��*/
		ret=Cif_addr_inf_Ins(cif_addr_inf);
		if(ret)
		{
			sprintf(acErrMsg,"�Ǽǿͻ���ַ��Ϣ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
			goto ErrExit;
		}
	}
	else
	{
		ret = sql_max_int( "Cif_addr_inf", "addr_seqn", " cif_no=%ld ", 
							cif_addr_inf.cif_no);
		if ( ret < 0 )
		{
			sprintf(acErrMsg,"��ȡ�ͻ���ַ��Ϣ���ַ�������쳣![%s]", ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			ERR_DEAL;
		}
		else if(ret)
		{

  			cif_addr_inf.addr_seqn = ret + 1;
	
			/* �Ǽǿͻ���ַ��Ϣ�� */
			ret = Cif_addr_inf_Ins( cif_addr_inf );
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽǿͻ���ַ��Ϣ��ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C023");
				goto ErrExit;
			}
		}
	}
	/* �Ǽǽ�����ˮ */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	/* ǰ̨��� */
	set_zd_long("0350",cif_addr_inf.addr_seqn);
		  /* add begin �����ͻ���Ϣͬ�� lzy 20141216 */
			char today[9]={0};
			long nextday = 0;
			char cifno[9]={0};
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(cif_addr_inf.cif_no,8,cifno);
			apitoa(g_pub_tx.tx_date,8,today);
			nextday = pub_base_daynumLAT(today,1);

			if ( cifno[0] == '1' && cif_addr_inf.addr_type[0] == '2' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
				}
			else if ( cifno[0] == '5' && cif_addr_inf.addr_type[0] == '1' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::�ͻ���[%s],��ַ����[%s],����Ҫ������ͬ��.",__FILE__,__LINE__,cifno,cif_addr_inf.addr_type);
					goto OkExit;
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

			if ( cifno[0] == '1' )  /**���˿ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,�ֻ�,����,�Ա�\n");**/
					fprintf(fp, "%ld,,,,%s,%s,,,,\n",cif_addr_inf.cif_no,cif_addr_inf.addr,cif_addr_inf.post_code);
				}
			else if (cifno[0] == '5' )  /**��˾�ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,���˴���\n");**/
					fprintf(fp, "%ld,,,,%s,%s,,\n",cif_addr_inf.cif_no,cif_addr_inf.addr,cif_addr_inf.post_code);
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
		  /* add end �����ͻ���Ϣͬ�� lzy 20141216 */

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ���ַ��Ϣ¼��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�ͻ���ַ��Ϣ¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

