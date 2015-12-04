/*************************************************
* �� �� ��: spC308.c
* �����������ͻ���ַ��Ϣά��
*
* ��    ��:
* ������ڣ�2004��01��26��
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

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_addr_inf_c		cif_addr_inf;
	static struct cif_addr_inf_c		g_cif_addr_inf;

	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC308()
{
	int ret=0;
	int cmp_code=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_addr_inf,0x00,sizeof(struct cif_addr_inf_c));
	memset(&g_cif_addr_inf,0x00,sizeof(struct cif_addr_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_data("0690",sts);							/*��������*/
	get_zd_long("0280",&g_cif_addr_inf.cif_no);			/*�ͻ���*/
	get_zd_long("0490",&g_cif_addr_inf.addr_seqn);		/*��ַ���*/
	get_zd_data("0230",g_cif_addr_inf.country);			/*���ڹ���*/
	get_zd_data("0670",g_cif_addr_inf.addr_type);		/*��ַ����*/
	get_zd_data("0260",g_cif_addr_inf.addr);			/*�ͻ���ַ*/
	get_zd_data("0640",g_cif_addr_inf.post_code);		/*��������*/

	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*1-�޸�*/
				ret=Cif_addr_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and addr_seqn=%d",g_cif_addr_inf.cif_no,
					g_cif_addr_inf.addr_seqn);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_addr_inf_rel�α��쳣![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_addr_inf_Fet_Upd(&cif_addr_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_addr_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				vtcp_log("rowid=======!!!!!s======[%20l]\n",g_cif_addr_inf.cif_no);
				vtcp_log("rowid=======?????/???????/////??=[%20s]\n",cif_addr_inf.rowid);
				memcpy(g_cif_addr_inf.rowid, cif_addr_inf.rowid,sizeof(g_cif_addr_inf.rowid));
				ret=Cif_addr_inf_Upd_Upd(g_cif_addr_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_addr_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_addr_inf_Clo_Upd( );
				
		  /* add begin �����ͻ���Ϣͬ�� lzy 20141204 */
			char today[9]={0};
			/*char nextday[9]={0};*/
			long nextday = 0;
			char cifno[9]={0};
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_cif_addr_inf.cif_no,8,cifno);
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);

			if ( cifno[0] == '1' && g_cif_addr_inf.addr_type[0] == '2' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
				}
			else if ( cifno[0] == '5' && g_cif_addr_inf.addr_type[0] == '1' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::�ͻ���[%s],��ַ����[%s],����Ҫ������ͬ��.",__FILE__,__LINE__,cifno,g_cif_addr_inf.addr_type);
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

			if ( cifno[0] == '1' )  /**���˿ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,�ֻ�,����,�Ա�\n");**/
					fprintf(fp, "%ld,,,,%s,%s,,,,\n",g_cif_addr_inf.cif_no,g_cif_addr_inf.addr,g_cif_addr_inf.post_code);
				}
			else if (cifno[0] == '5' )  /**��˾�ͻ�**/
				{
					/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,���˴���\n");**/
					fprintf(fp, "%ld,,,,%s,%s,,\n",g_cif_addr_inf.cif_no,g_cif_addr_inf.addr,g_cif_addr_inf.post_code);
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
				
		case '2':										/*2-ɾ��*/
				/*ɾ���ͻ�������Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld and addr_seqn=%d",
						g_cif_addr_inf.cif_no,g_cif_addr_inf.addr_seqn);
				ret =deldb("cif_addr_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ���ַ��Ϣ���¼�쳣!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
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

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ���ַ��Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ���ַ��Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

