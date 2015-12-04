/*************************************************
* �� �� ��: spC305.c
* ����������   
*           ��˾�ͻ���Ϣά��
*
* ��    ��: andy
* �������: 2004��02��05��
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
#include "cif_cop_inf_c.h"

static struct cif_cop_inf_c    	 cif_cop_inf, g_cif_cop_inf;
static struct cif_basic_inf_c    cif_basic_inf;

char memo[41],tbwhere[50];
char sts[2];
		
spC305()  
{ 	
	int ret=0;
	int cmp_code=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ���ݳ�ʼ�� **/
	memset (&cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset (&g_cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(sts,0x00,sizeof(sts));

	/** ȡֵ����ֵ **/
	get_zd_long("0280",&g_cif_cop_inf.cif_no);			/* �ͻ��� */
	vtcp_log("[%s] [%d] cif_no =[%ld]\n",__FILE__,__LINE__,g_cif_cop_inf.cif_no);
	get_zd_data( "0620",g_cif_cop_inf.crd_no);			/*����֤��*/
	vtcp_log("[%s] [%d] crd_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.crd_no);
	get_zd_data( "0630",g_cif_cop_inf.license);			/*Ӫҵִ��*/
	vtcp_log("[%s] [%d] license =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.license);
	get_zd_data( "0300",g_cif_cop_inf.units_license);   /*�Ǽ�֤��*/
	vtcp_log("[%s] [%d] units_license =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.units_license);
	get_zd_data( "0310",g_cif_cop_inf.g_tax_no);		/*��˰��*/
	vtcp_log("[%s] [%d] g_tax_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.g_tax_no);
	get_zd_data( "0320",g_cif_cop_inf.d_tax_no);		/*��˰��*/
	vtcp_log("[%s] [%d] d_tax_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.d_tax_no);
	get_zd_data( "0580",g_cif_cop_inf.credit_no);		/*�����*/
	vtcp_log("[%s] [%d] credit_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.credit_no);
	/**ǰ̨δ�����ֶ� delete by wanglei 20070409
	get_zd_data( "0660",g_cif_cop_inf.eco_attri);       *��������*
	**/
	get_zd_data( "0670",g_cif_cop_inf.ent_attri);       /*��ҵ����*/
	vtcp_log("[%s] [%d] ent_attri =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.ent_attri);
	/**ǰ̨δ�����ֶ� delete by wanglei 20070409
	get_zd_data( "0220",g_cif_cop_inf.cif_scopes);      *��������*
	**/
	get_zd_data( "0680",g_cif_cop_inf.belong_bank);     /*������ҵ(����)*/
	vtcp_log("[%s] [%d] belong_bank =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.belong_bank);
	get_zd_data( "0690",g_cif_cop_inf.belong_stat);     /*������ҵ(ͳ�ƾ�)*/
	vtcp_log("[%s] [%d] belong_stat =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.belong_stat);
	/**ǰ̨δ�����ֶ� delete by wanglei 20070409
	get_zd_data( "0700",g_cif_cop_inf.region);          *���򼶱�*
	**/
	get_zd_data( "0710",g_cif_cop_inf.mana_orga_moda);  /*��Ӫ��ʽ*/
	vtcp_log("[%s] [%d] mana_orga_moda =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.mana_orga_moda);
	get_zd_data( "0720",g_cif_cop_inf.bd_corp_orga_moda);/*��ҵ��ʽ*/
	vtcp_log("[%s] [%d] bd_corp_orga_moda =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.bd_corp_orga_moda);
	/**ǰ̨δ�����ֶ� delete by wanglei 20070409
	get_zd_data( "0230",g_cif_cop_inf.corp_act_mode);   *������ʽ*
	**/
	get_zd_long( "0440",&g_cif_cop_inf.reg_date);        /*ע������*/
	vtcp_log("[%s] [%d] reg_date =[%ld]\n",__FILE__,__LINE__,g_cif_cop_inf.reg_date);
	get_zd_data( "0890",g_cif_cop_inf.reg_country);    	/*ע�����*/
	vtcp_log("[%s] [%d] reg_country =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.reg_country);
	get_zd_data( "0210",g_cif_cop_inf.cur_no);          /*ע�����*/
	vtcp_log("[%s] [%d] cur_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.cur_no);
	get_zd_double( "0400",&g_cif_cop_inf.reg_fund);      /*ע���ʽ�*/
	vtcp_log("[%s] [%d] reg_fund =[%.2f]\n",__FILE__,__LINE__,g_cif_cop_inf.reg_fund);
	get_zd_double( "0410",&g_cif_cop_inf.paicl_up_capital);/*ʵ���ʱ�*/
	vtcp_log("[%s] [%d] paicl_up_capital =[%.2f]\n",__FILE__,__LINE__,g_cif_cop_inf.paicl_up_capital);
	get_zd_data( "0330",g_cif_cop_inf.ap_qualife);      /*�����ʸ�*/
	vtcp_log("[%s] [%d] ap_qualife =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.ap_qualife);
	get_zd_data( "0810",g_cif_cop_inf.artificial_person);/*��������*/
	vtcp_log("[%s] [%d] artificial_person =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.artificial_person);
	get_zd_data( "0820",g_cif_cop_inf.ap_id);           /*�������֤��*/
	vtcp_log("[%s] [%d] ap_id =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.ap_id);
	get_zd_data( "0900",g_cif_cop_inf.corp_size);       /*��ҵ��ģ*/
	vtcp_log("[%s] [%d] corp_size =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.corp_size);
	/**ǰ̨δ�����ֶ� delete by wanglei 20070409
	get_zd_data( "0240",g_cif_cop_inf.corp_mana_sts);   *��Ӫ״̬*
	**/
	get_zd_data( "0640",g_cif_cop_inf.credit);          /*������*/
	vtcp_log("[%s] [%d] credit =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.credit);
	get_zd_data( "0250",g_cif_cop_inf.supervior);       /*�ϼ���λ*/
	vtcp_log("[%s] [%d] supervior =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.supervior);
       /**26��ʹ�ô��󣬵������ݶ�ʧ modify 20120601
	get_zd_data( "0260",g_cif_cop_inf.director);         *���ܵ�λ*
       **/
	get_zd_data( "0830",g_cif_cop_inf.director);         /*���ܵ�λ*/
	vtcp_log("[%s] [%d] director =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.director);
	get_zd_data("0930",sts);							/* �������� */
	vtcp_log("[%s] [%d] sts =[%s]\n",__FILE__,__LINE__,sts);
	vtcp_log("[%s] [%d] sts =[%c]\n",__FILE__,__LINE__,sts[0]);
	/** �������� **/
	switch(sts[0])
	{
		case '1':									/*�޸�*/
				ret=Cif_cop_inf_Dec_Upd(g_pub_tx.reply,
				   			"cif_no=%ld ",g_cif_cop_inf.cif_no);
	vtcp_log("[%s] [%d] stsaaaaaaaaaaaa[0] =[%c]\n",__FILE__,__LINE__,sts[0]);
			    if (ret)
			    {
				   sprintf(acErrMsg,"DECLARE Cif_cop_inf_rel�α��쳣![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    ret=Cif_cop_inf_Fet_Upd(&cif_cop_inf,g_pub_tx.reply);
			    if (ret)
			    {
				   sprintf(acErrMsg,"FETCH Cif_cop_inf�α��쳣![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    strcpy(g_cif_cop_inf.rowid,cif_cop_inf.rowid);
				vtcp_log("update success g_rowid[%s],c_rowid[%s]",g_cif_cop_inf.rowid,cif_cop_inf.rowid);
	vtcp_log("[%s] [%d] upd credit_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.credit_no);
	/*get_zd_data( "0580",g_cif_cop_inf.credit_no);*/		/*�����*/
			    ret=Cif_cop_inf_Upd_Upd(g_cif_cop_inf,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg,"UPDATE Cif_cop_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_cop_inf_Clo_Upd( );
				
		  /* add begin �����ͻ���Ϣͬ�� lzy 20141204 */
			char today[9]={0};
			/*char nextday[9]={0};*/
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);

			sprintf( filename,"CorpCustSyn%ld.txt",nextday);			
			sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
			vtcp_log("%s %d:: pcfilename[%ld]",__FILE__,__LINE__,pcfilename);
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
			/**fprintf(fp, "�ͻ���,֤������,֤������,����,��ַ,�ʱ�,�绰,���˴���\n");**/
			fprintf(fp, "%ld,,,,,,,%s\n",g_cif_cop_inf.cif_no,g_cif_cop_inf.artificial_person);
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

				/*ɾ����˾�ͻ���Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld",g_cif_cop_inf.cif_no);
				ret =deldb("cif_cop_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ����˾�ͻ�������Ϣ���¼�쳣!![%s]",ret);
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
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"��˾�ͻ�������Ϣά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��˾�ͻ�������Ϣά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
