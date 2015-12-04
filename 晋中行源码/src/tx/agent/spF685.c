/*************************************************
* �� �� ��: spF685.c--spF610.c�޸�
* ����������������������-����(�˽�����686���ƶ�����Ϊ�����winqt��������������)
*
* ��    ��: jack
* �������: 2005��02��24��
* �޸�����: 2010-03-22 
* �� �� ��: ZHGF
* �޸�����: �ĳ�֧������Ҫ����첽ͨ�ſ���ģʽ
insert into tx_Def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('6642', '��Ԥ�������������ļ�', '10000000000000000000000001000000000000010001110000000000000000000010110000000000000000000010000000000000000000000000010000000000', '1', '6', '2');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('6642', 0, '6642', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('6642', '��Ԥ�������������ļ�', 'spF685', '0', '0');
-----------------------
insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('6642', 1, '', '', '', '', '', '', '', '', '0000', 'winqtԤ��������');

insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '6642', 1, 'winqtԤ����������Ȩ', 'sq(1237)');
* ǰ̨�����̵��ļ���ʽ����������ļ�Ҫ����У����£�
����|�ͻ���|�ֿ���֤������|�ֿ���֤������|�ֿ��˿ͻ���|�ֿ��˿ͻ�Ӣ����|�ֿ����Ա�|�ֿ��˼�ͥסַ|�ֿ��˼�ͥ�ʱ�|�ֿ��˼�ͥ�绰|�ֿ��˵�λ����|�ֿ��˵�λ��ַ|�ֿ��˵�λ�ʱ�|�ֿ��˵�λ�绰|�ֿ����ֻ�����|�ֿ�������|�ֿ��������ȼ�|������|��������|����״��|��λ���|���˴�����|�������|
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"/*--- ֤������Ĭ��Ϊ1 -�ָ�Ϊ�����κ�֤������fldstr[5]-*/
#include "public.h"
#include <errno.h>
#include "trace_log_c.h"
#include "note_mst_c.h"
#include "cif_basic_inf_c.h"
#include "cif_mger_rel_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_wrk_hst_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_per_inf_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
#include "note_min_no_c.h"
#include "com_branch_c.h"
#include "mdm_prdt_rel_c.h"
#include "cif_email_inf_c.h"
#include "mdm_ac_rel_c.h"
#include "card.h"
int transfer(char *buf,char *fdstr);
extern char *get_env_info(char *infoname);

spF685()
{ 	
	int ret=0;
	char execstr[256];

	char cPrdt_code[4];
	char cBeg_card_no[20];
	char cEnd_card_no[20];
	char cCard_no[20];
	char cNote_no[20];
	char cz_acno[20];
	char cUnit_no[5];
	char cNote_type[4];
	char cCpfilename[200];
	char cFtpfile[200];

	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;	

	double dTram = 0.00;

	char cPass[7];
	long lLen = 0;

	char filename[100],cFilename[100];
	char filename1[100];
	char fldstr[30][150];/**����Ҫ����20��������ʱ����Ϊ30�����Ϊ140��������ʱ��Ϊ150***/
	char tmpstr[1024];
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	int i=0;

	char filename2[100];
	FILE *fp2 = NULL;
	char filename3[100];
	FILE *fp3 = NULL;	
	char filename4[100];	/*����ȷ�����������*/
	long lSnum = 0;
	double dSamt = 0.00;
	int iErr_flag = 0;	/*---��־������¼���Ƿ��д������---*/
	char cReturn[64];
	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	long lBeg_note_no = 0;
	char cNote_tmp[16];
	char cNote_tmp1[9];
	int iLen = 0;
	long lCount = 0;

	char cHand_flag[2];	/*---��Ϣ¼�뷽ʽ---*/
	double dT_amt = 0.00;	/*---ǰ̨�����ܽ��---*/
	long lCount_i = 0;	/*---ǰ̨�����ܱ���---*/
	long lRnum = 0,iOpcnt=0;

	char cRctp[9];

	struct cif_basic_inf_c cif_basic_inf;
	struct cif_mger_rel_c cif_mger_rel;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;
	struct note_min_no_c sNote_min_no;
	struct com_branch_c sCom_branch;
	struct mdm_prdt_rel_c sMdm_prdt_rel;
	struct cif_addr_inf_c	z_cif_addr_inf;
	struct cif_per_inf_c	z_cif_per_inf;
	struct note_mst_c	note_mst_z;
	struct cif_email_inf_c z_cif_email_inf;
	struct cif_email_inf_c z_cif_email_inf1;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(&note_mst_z,0x00,sizeof(struct note_mst_c));

	memset(&cif_basic_inf,0x00,sizeof(cif_basic_inf));
	memset(&cif_mger_rel,0x00,sizeof(cif_mger_rel));
	memset(&cif_id_code_rel,0x00,sizeof(cif_id_code_rel));
	memset(&sAg_unitinfo,0x00,sizeof(sAg_unitinfo));
	memset(&sAg_peritemmap,0x00,sizeof(sAg_peritemmap));
	memset(&sPay_agent,0x00,sizeof(sPay_agent));
	memset(&sNote_min_no,0x00,sizeof(sNote_min_no));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(cPrdt_code,0x0,sizeof(cPrdt_code));
	memset(cBeg_card_no,0x0,sizeof(cBeg_card_no));
	memset(cEnd_card_no,0x0,sizeof(cEnd_card_no));
	memset(cCard_no,0x0,sizeof(cCard_no));
	memset(cNote_no,0x0,sizeof(cNote_no));
	memset(cz_acno,0x0,sizeof(cz_acno));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cNote_type,0x0,sizeof(cNote_type));
	memset( cReturn, 0x0, sizeof(cReturn) );
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );
	memset( filename,0x0,sizeof(filename));
	memset( cFilename,0x0,sizeof(cFilename));
	memset( filename1,0x0,sizeof(filename1));
	memset( filename2,0x0,sizeof(filename2));
	memset( filename3,0x0,sizeof(filename3));
	memset( filename4,0x0,sizeof(filename4));
	memset(cCpfilename,0x00,sizeof(cCpfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	memset( cPass, 0x0, sizeof(cPass) );

	get_zd_data("0230",cPrdt_code);
	get_zd_data("0300",cz_acno);
	pub_base_old_acno( cz_acno );

	get_zd_data("0920",cUnit_no);
	get_zd_data("0900",cNote_type);

	get_zd_data( "0700", cHand_flag );

	get_zd_data( "0790", cPass );
	lLen = strlen( cPass );
	if ( lLen == 0 )
	{
		sprintf( acErrMsg, "���벻��Ϊ��!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( "0130", "���벻��Ϊ��!!!" );
		goto ErrExit;
	}
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	/**
����|�ͻ���|�ֿ���֤������|�ֿ���֤������|�ֿ��˿ͻ���|�ֿ��˿ͻ�Ӣ����|�ֿ����Ա�|�ֿ��˼�ͥסַ|�ֿ��˼�ͥ�ʱ�|�ֿ��˼�ͥ�绰|�ֿ��˵�λ����|�ֿ��˵�λ��ַ|�ֿ��˵�λ�ʱ�|�ֿ��˵�λ�绰|�ֿ����ֻ�����|�ֿ�������|�ֿ��������ȼ�|������|��������|����״��|����λ���|���˴�����|�������|
	**/
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "020") || !strcmp(cNote_type, "024")   || !strcmp(cNote_type, "027")|| !strcmp(cNote_type, "028")  || !strcmp(cNote_type, "029") || !strcmp(cNote_type, "031") || !strcmp(cNote_type, "032")) /* �� $IC��*/
	{
		strcpy( cRctp, "��" );
	}else{/**ע��**�м��ֿ��أ����������������������Ҫ��������޸Ĵ˴�**/
		sprintf( acErrMsg, "�������ʹ��󣬲��ǿ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O188" );
		set_zd_data( "0130", "�������ʹ���!!!" );
		goto ErrExit;
	}

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "û�в�ѯ���õ�λ��Ŷ�Ӧ��¼!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	/**�������ļ����Ѿ��������˻����� 20130510**/
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s'", sAg_unitinfo.bank_acct_no);
	if( ret )
	{
		sprintf( acErrMsg, "ִ��mdm_ac_rel����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	vtcp_log("%s,%d,s_mdm_ac_rel.note_sts =[%s]\n",__FILE__,__LINE__,s_mdm_ac_rel.note_sts);
	/*if(s_mdm_ac_rel.note_sts == '*')*/
	if(strcmp(s_mdm_ac_rel.note_sts,"*") == 0)
	{
		vtcp_log("�ô�������ʺ������������� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		sprintf( acErrMsg, "�ô�������ʺ�������������!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P245" );
		goto	ErrExit;	
	}
	/**�������ļ����Ѿ��������˻����� 20130510 **/
	pub_base_strpack( sAg_unitinfo.unit_name );

	memset( filename1, 0x0, sizeof(filename1) );
	sprintf( filename1,"%s/0%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	vtcp_log("%s,%d,��ȷ�ļ���filename1[%s]\n",__FILE__,__LINE__,filename1);
	sprintf(cFilename,"0%s%ld%s%06d.txt",cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	fp1 = fopen( filename1,"w" );   /*��������Ҫ�������ļ�*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	sprintf( filename2,"%s/1%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	vtcp_log("%s,%d,�д�����Ϣ�ļ�¼д��filename2[%s]\n",__FILE__,__LINE__,filename2);
	fp2 = fopen(filename2,"w" );   /*����ǰ̨��Ա�Ĵ�����ʾ�ļ�*/
	if(fp2==NULL)
	{
		sprintf( acErrMsg, " open file [%s] error ", filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fprintf(fp2,"@3v1\n");
  	
	fprintf( fp2, "                         ����������������Ϣ��[%s-%s]			\n", \
    	sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
	fprintf( fp2, "    �������������ө����������ө��������������������ө��������������������ө���������������������������������������������\n" );
	fprintf( fp2, "    ��%10s��%10s��%20s��%20s��%44s��\n", "������", "�ͻ�����", "��������", "֤������", "������Ϣ" );
	fprintf( fp2, "    �������������㩥���������㩥�������������������㩥�������������������㩥��������������������������������������������\n" );

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]",filename);
	WRITEMSG
	/**��ǰ̨�������ļ����б���**/
	sprintf(cCpfilename,"cp %s %s/CARD%s%d%sOK.txt",filename,getenv("FILDIR"),g_pub_tx.tel,g_pub_tx.tx_date,cUnit_no);
	vtcp_log("%s,%d cCpfilename=[%s]\n",__FILE__,__LINE__,cCpfilename);
	ret=system(cCpfilename);
	if (ret)
	{
		sprintf(acErrMsg,"����ǰ̨�������ļ�����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	if ( cHand_flag[0] == '0' )	/*---�ֹ�---*/
	{
		get_zd_double("0400",&dT_amt);
		get_zd_long( "0550", &lCount_i );
	}
	else				/*---���ֹ���ʽ�ļ���һ��Ϊ������Ϣ---��Ҫ�����������̵��ļ�*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		/*-	�����ļ������ݵ�һ�л��ܸ�ʽ����
			fldstr[0] ��λ���
		    fldstr[1] ���ܱ���
		    fldstr[2] ���ܽ��
			
		----�����ļ�������ÿһ�ж�Ӧ��һ����¼����ʽ���ֹ�¼��һ��������
			
		����|�ͻ���|�ֿ���֤������ |�ֿ���֤������|�ֿ��˿ͻ���|�ֿ��˿ͻ�Ӣ����|�ֿ����Ա�|�ֿ��˼�ͥסַ|�ֿ��˼�ͥ�ʱ�|�ֿ��˼�ͥ�绰|�ֿ��˵�λ����|�ֿ��˵�λ��ַ |�ֿ��˵�λ�ʱ�|�ֿ��˵�λ�绰|�ֿ����ֻ�����|�ֿ�������|�ֿ��������ȼ�|������|��������|����״��|��λ���|���˴�����|�������|
			
		----------------------------------------------*/
		memset(fldstr,0x0,sizeof(fldstr));
		for(i=0;i<3;i++)/**��һ�о��������� ��λ���|�ܱ���|�ܽ��|**/
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" fldstr[%d]=[%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/**���ƿ����ܽ��ֻ��Ϊ�� 20130502**/
		if( atof(fldstr[2])!=0){
			sprintf(acErrMsg,"����Ԥ����������0���!!!" );
			WRITEMSG
			sprintf(acErrMsg,"����Ԥ����������0���!!!");
			set_zd_data( "0130", acErrMsg );
			strcpy( g_pub_tx.reply, "H019" );
			goto ErrExit;
		}
		/**���ƿ����ܽ��ֻ��Ϊ�� 20130502**/
		long    fd55=0;
		get_zd_long( "0550", &fd55);/**�ܱ���****/
		if(fd55 != atoi(fldstr[1]))
		{
			sprintf(acErrMsg,"�ܱ��������ڱ��������!!!" );
			WRITEMSG
			sprintf(acErrMsg,"���ڻ��ܱ���Ϊ[%d]", atoi(fldstr[1]) );
			set_zd_data( "0130", acErrMsg );
			strcpy( g_pub_tx.reply, "H019" );
			goto ErrExit;
		}
		lCount_i = atoi(fldstr[1]);	
		dT_amt = atof(fldstr[2]);
		vtcp_log("%s,%d,�ļ��еĻ��ܽ��fldstr[2]=[%s]",__FILE__,__LINE__,fldstr[2]);
		vtcp_log("%s,%d,�ļ��еĻ��ܽ��Ϊ[%lf]",__FILE__,__LINE__,dT_amt);
	}
	
	vtcp_log("%s,%d,ͳ�Ʊ���[%d]ǰ̨������[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	/*------ �жϹ�Ա�Ƿ�ӵ������������ƾ֤ -----*/
	lCount=0;/**�տ�ʼΪ1**/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			vtcp_log("%s,%d,�������һ��!",__FILE__,__LINE__);
			memset(fldstr,0x0,sizeof(fldstr));
			if ( cHand_flag[0] != '0' )
				{
					for( i=0;i<23;i++ )/*����������ļ���ʽ ����˵�λ��� ���˴����� �������**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
				}else
				{/**�ֹ�����**/
					transfer(tmpstr,fldstr[0]);
				}
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,���һ��Ϊ�Ƿ���!",__FILE__,__LINE__);
				vtcp_log("%s,%d,ͳ�Ʊ���lCount=[%d]",__FILE__,__LINE__,lCount);	
				break;
			}
			/**�Ƚ����˴���������ǰ���0ȥ��**/
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("[%s][%d]��ʾ��������Ϣ\n",__FILE__,__LINE__);
		vtcp_log("fldstr0===[%s]\n",fldstr[0]);
		vtcp_log("fldstr1===[%s]\n",fldstr[1]);
		vtcp_log("fldstr2===[%s]\n",fldstr[2]);
		vtcp_log("fldstr3===[%s]\n",fldstr[3]);
		vtcp_log("fldstr4===[%s]\n",fldstr[4]);
		vtcp_log("fldstr5===[%s]\n",fldstr[5]);
		vtcp_log("fldstr6===[%s]\n",fldstr[6]);
		vtcp_log("fldstr7===[%s]\n",fldstr[7]);
		vtcp_log("fldstr8===[%s]\n",fldstr[8]);
		vtcp_log("fldstr9===[%s]\n",fldstr[9]);
		vtcp_log("fldstr10===[%s]\n",fldstr[10]);
		vtcp_log("fldstr11===[%s]\n",fldstr[11]);
		vtcp_log("fldstr12===[%s]\n",fldstr[12]);
		vtcp_log("fldstr13===[%s]\n",fldstr[13]);
		vtcp_log("fldstr14===[%s]\n",fldstr[14]);
		vtcp_log("fldstr15===[%s]\n",fldstr[15]);
		vtcp_log("fldstr16===[%s]\n",fldstr[16]);
		vtcp_log("fldstr17===[%s]\n",fldstr[17]);
		vtcp_log("fldstr18===[%s]\n",fldstr[18]);
		vtcp_log("fldstr19===[%s]\n",fldstr[19]);
		vtcp_log("fldstr20===[%s]\n",fldstr[20]);
		vtcp_log("fldstr21===[%s]\n",fldstr[21]);
		vtcp_log("fldstr22===[%s]\n",fldstr[22]);
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[22] );
		dTmp_amt = dTmp_amt + dTemp_amt;
		lCount = lCount + 1;
		vtcp_log("%s,%d,ͳ�Ʊ���lCount=[%d]",__FILE__,__LINE__,lCount);	
		break;
		}
		memset(fldstr,0x0,sizeof(fldstr));
		if( cHand_flag[0] != '0' )
			{
				for( i=0;i<23;i++ )/*����������ļ���ʽ ����˵�λ��� ���˴����� �������**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
			}else
			{			
				vtcp_log("[%s][%d]��ʾ��������Ϣ\n",__FILE__,__LINE__);
				transfer(tmpstr,fldstr[0]);
			}
			
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,����Ϊ�Ƿ���!",__FILE__,__LINE__);
				continue;
			}
		/*
		����|�ͻ���|�ֿ���֤������|�ֿ���֤������|�ֿ��˿ͻ���|�ֿ��˿ͻ�Ӣ����|�ֿ����Ա�|�ֿ��˼�ͥסַ|�ֿ��˼�ͥ�ʱ�|�ֿ��˼�ͥ�绰|�ֿ��˵�λ����|�ֿ��˵�λ��ַ|�ֿ��˵�λ�ʱ�|�ֿ��˵�λ�绰|�ֿ����ֻ�����|�ֿ�������|�ֿ��������ȼ�|������|��������|����״��|��λ���|���˴�����|�������|
		*/
		/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
		    fldstr[4]   �ֿ��˿ͻ���     ok
		    fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
		    fldstr[6]   �ֿ����Ա�       
		    fldstr[7]   �ֿ��˼�ͥסַ   ok
		    fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
		    fldstr[9]   �ֿ��˼�ͥ�绰
		    fldstr[10]  �ֿ��˵�λ����   ok
		    fldstr[11]  �ֿ��˵�λ��ַ   ok
		    fldstr[12]  �ֿ��˵�λ�ʱ�   ok
		    fldstr[13]  �ֿ��˵�λ�绰
		    fldstr[14]  �ֿ����ֻ�����   ok
		    fldstr[15]  �ֿ�������
		    fldstr[16]  �ֿ��������ȼ�
		    fldstr[17]  ������
		    fldstr[18]  ��������
		    fldstr[19]  ����״��
		    fldstr[20]  ��λ���depid    ok
		    fldstr[21]  ���˴�����itemno    ok
		    fldstr[22]  �������opnamt    ok
		----------------------------------------------*/
		{
			/**�Ƚ����˴���������ǰ���0ȥ��**/
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("[%s][%d]��ʾ��������Ϣ\n",__FILE__,__LINE__);
		vtcp_log("fldstr0===[%s]\n",fldstr[0]);
		vtcp_log("fldstr1===[%s]\n",fldstr[1]);
		vtcp_log("fldstr2===[%s]\n",fldstr[2]);
		vtcp_log("fldstr3===[%s]\n",fldstr[3]);
		vtcp_log("fldstr4===[%s]\n",fldstr[4]);
		vtcp_log("fldstr5===[%s]\n",fldstr[5]);
		vtcp_log("fldstr6===[%s]\n",fldstr[6]);
		vtcp_log("fldstr7===[%s]\n",fldstr[7]);
		vtcp_log("fldstr8===[%s]\n",fldstr[8]);
		vtcp_log("fldstr9===[%s]\n",fldstr[9]);
		vtcp_log("fldstr10===[%s]\n",fldstr[10]);
		vtcp_log("fldstr11===[%s]\n",fldstr[11]);
		vtcp_log("fldstr12===[%s]\n",fldstr[12]);
		vtcp_log("fldstr13===[%s]\n",fldstr[13]);
		vtcp_log("fldstr14===[%s]\n",fldstr[14]);
		vtcp_log("fldstr15===[%s]\n",fldstr[15]);
		vtcp_log("fldstr16===[%s]\n",fldstr[16]);
		vtcp_log("fldstr17===[%s]\n",fldstr[17]);
		vtcp_log("fldstr18===[%s]\n",fldstr[18]);
		vtcp_log("fldstr19===[%s]\n",fldstr[19]);
		vtcp_log("fldstr20===[%s]\n",fldstr[20]);
		vtcp_log("fldstr21===[%s]\n",fldstr[21]);
		vtcp_log("fldstr22===[%s]\n",fldstr[22]);
		}
		
		/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
		    fldstr[4]   �ֿ��˿ͻ���     ok
		    fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
		    fldstr[6]   �ֿ����Ա�       
		    fldstr[7]   �ֿ��˼�ͥסַ   ok
		    fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
		    fldstr[9]   �ֿ��˼�ͥ�绰
		    fldstr[10]  �ֿ��˵�λ����   ok
		    fldstr[11]  �ֿ��˵�λ��ַ   ok
		    fldstr[12]  �ֿ��˵�λ�ʱ�   ok
		    fldstr[13]  �ֿ��˵�λ�绰
		    fldstr[14]  �ֿ����ֻ�����   ok
		    fldstr[15]  �ֿ�������
		    fldstr[16]  �ֿ��������ȼ�
		    fldstr[17]  ������
		    fldstr[18]  ��������
		    fldstr[19]  ����״��
		    fldstr[20]  ��λ���depid    ok
		    fldstr[21]  ���˴�����itemno    ok
		    fldstr[22]  �������opnamt    ok
		------------------------------------------*/
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[22] );
		/**����ÿ�ʵĿ������ֻ��Ϊ�� 20130502**/
		if(dTemp_amt != 0.00)
		{
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "��[%d]�У���������ÿ�ʽ�����Ϊ��",lCount);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "��[%d]�У���������ÿ�ʽ�����Ϊ��",lCount);
			WRITEMSG
			goto TrLog;	
		}
		/**����ÿ�ʵĿ������ֻ��Ϊ�� 20130502**/
		dTmp_amt = dTmp_amt + dTemp_amt;
		lCount = lCount + 1;
		vtcp_log("%s,%d,ͳ�Ʊ���lCount=[%d]",__FILE__,__LINE__,lCount);
	}
	/**ȥ����������wangwk-20110117
	if ( cHand_flag[0] == '1' )	---����---
		lCount=lCount-1;
		**/
	vtcp_log("%s,%d,ͳ�Ʊ���[%d]ǰ̨������[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	if ( lCount == 0 )
	{
		sprintf(acErrMsg,"û����Ҫ���������ļ�¼!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H017" );
		goto ErrExit;
	}
	fclose ( fp );
	vtcp_log("%s,%d,ͳ�Ʊ���[%d]ǰ̨������[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"�ܱ�����ֻ���ϸ�ϼƱ��������!!!" );
		WRITEMSG
		sprintf(acErrMsg,"��ϸ����Ϊ[%d]", lCount );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"��ϸ���Ϊ[%.2lf]dT_amt=[%.2lf]", dTmp_amt,dT_amt);
	WRITEMSG
	ret = pub_base_CompDblVal( dT_amt, dTmp_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"���ܽ��[%.2lf]����ϸ�ܽ��Ϊ[%.2lf]",dTmp_amt,dT_amt);
		WRITEMSG
		sprintf(acErrMsg,"���ܽ��[%.2lf]����ϸ�ܽ��Ϊ[%.2lf]",dTmp_amt,dT_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	memset(&sMdm_prdt_rel,0,sizeof(struct mdm_prdt_rel_c));
	ret = Mdm_prdt_rel_Sel(g_pub_tx.reply,&sMdm_prdt_rel,"prdt_no='%s' and mdm_code1 ='0'||'%s'",cPrdt_code,cNote_type);
	if(ret)
	{
		vtcp_log("%s,%d,��Ʒ����ʲ����ڹ�����ϵ!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"Z905");
		goto ErrExit;
	}
	if ( strcmp( cNote_type, "020" ) == 0 || strcmp( cNote_type, "024" ) == 0  || strcmp( cNote_type, "027" ) == 0 || strcmp( cNote_type, "028" ) == 0  || strcmp( cNote_type, "029" ) == 0 || strcmp( cNote_type, "031" ) == 0 || strcmp( cNote_type, "032" ) == 0)		/**��������Ҫ��ƾ֤**/
	{
		ret = sql_sum_long ( "note_mst", "cnt", &lRnum, "tel = '%s' \
					and note_type = '%s' and sts = '0'", \
					g_pub_tx.tel, cNote_type );
		if ( lRnum < lCount )
		{
			sprintf(acErrMsg, "��������[%d]ƾ֤��[%d]", lCount, lRnum );
			WRITEMSG
        	
			sprintf(acErrMsg, "��Ա����ƾ֤���������������!!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H018" );
			goto ErrExit;
		}
        	
		sprintf(acErrMsg, "��������[%d]ƾ֤��[%d]", lCount, lRnum );
		WRITEMSG
	}
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	if ( cHand_flag[0] != '0' )	/*---���ֹ�ȥ����һ��---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
	}
	/****����ģʽ�Ļ���Ҫ���¼����Ա����ƾ֤�ˣ���Ϊ���������¹�Աƾ֤**/
	{
		ret=Note_mst_Dec_Sel(g_pub_tx.reply,"tel='%s' and note_type='%s' and sts='0' and cnt>0\
			order by beg_note_no",g_pub_tx.tel,cNote_type);
		if(ret){
			sprintf(acErrMsg,"��ѯ��Աƾ̨֤�˱����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		
		ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
		if(ret){
			sprintf(acErrMsg,"��ѯ��Աƾ̨֤�˱����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
	}
	iOpcnt=0;
	lCount=0;
	/***�������****/
	while(1)
	{
		memset(tmpstr,0,sizeof(tmpstr));
		if(feof(fp))
		{
			break;
		}
		fgets( tmpstr,1024,fp );
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		memset(fldstr,0x0,sizeof(fldstr));
		if( cHand_flag[0] != '0')
		{
			for( i=0;i<23;i++ )/*�ĳ�23 ����Ҫ����ϵ�λ����,���˴����ţ������������23��**/
			{
				ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
				pub_base_strpack( fldstr[i] );
			}
		}else
		{
			/**�ֹ�����**/
						/**ת��һ��ǰ̨����˳��-wangwenkui-20101226**/
						transfer(tmpstr,fldstr[0]);
		}
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,����Ϊ�Ƿ���!",__FILE__,__LINE__);
				continue;
			}
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
		    fldstr[4]   �ֿ��˿ͻ���     ok
		    fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
		    fldstr[6]   �ֿ����Ա�       
		    fldstr[7]   �ֿ��˼�ͥסַ   ok
		    fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
		    fldstr[9]   �ֿ��˼�ͥ�绰
		    fldstr[10]  �ֿ��˵�λ����   ok
		    fldstr[11]  �ֿ��˵�λ��ַ   ok
		    fldstr[12]  �ֿ��˵�λ�ʱ�   ok
		    fldstr[13]  �ֿ��˵�λ�绰
		    fldstr[14]  �ֿ����ֻ�����   ok
		    fldstr[15]  �ֿ�������
		    fldstr[16]  �ֿ��������ȼ�
		    fldstr[17]  ������
		    fldstr[18]  ��������
		    fldstr[19]  ����״��
		    fldstr[20]  ��λ���depid    ok
		    fldstr[21]  ���˴�����itemno    ok
		    fldstr[22]  �������opnamt    ok
		----------------------------------------------*/
		if ( strlen(fldstr[0]) == 0  || strlen(fldstr[2]) == 0
		   ||strlen(fldstr[3]) == 0  || strlen(fldstr[4]) == 0
		   ||strlen(fldstr[14]) == 0 || strlen(fldstr[20]) == 0
		   ||strlen(fldstr[21]) == 0 )
		/**Ӧ����Ҫ��,����Ϊ��**/
		{
			if(strlen(fldstr[0]) == 0){
				sprintf( cReturn, "��[%d]�У����Ų���Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У����Ų���Ϊ��",lCount);
			}
			if(strlen(fldstr[2]) == 0){
				sprintf( cReturn, "��[%d]�У�֤�����಻��Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У�֤�����಻��Ϊ��",lCount);
			}
			if(strlen(fldstr[3]) == 0){
				sprintf( cReturn, "��[%d]�У�֤�����벻��Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У�֤�����벻��Ϊ��",lCount);
			}
			if(strlen(fldstr[4]) == 0){
				sprintf( cReturn, "��[%d]�У��ͻ�������Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У��ͻ�������Ϊ��",lCount);
			}
			if(strlen(fldstr[14]) == 0){
				sprintf( cReturn, "��[%d]�У��ֻ����벻��Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У��ֻ����벻��Ϊ��",lCount);
			}
			if(strlen(fldstr[20]) == 0){
				sprintf( cReturn, "��[%d]�У���λ��Ų���Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У���λ��Ų���Ϊ��",lCount);
			}
			if(strlen(fldstr[21]) == 0){
				sprintf( cReturn, "��[%d]�У����˴����Ų���Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У����˴����Ų���Ϊ��",lCount);
			}
			if(strlen(fldstr[22]) != 0&& atof(fldstr[22])!= 0.0)
			{
				sprintf( cReturn, "��[%d]�У�����������Ϊ��",lCount);
				sprintf( acErrMsg, "��[%d]�У�����������Ϊ��",lCount);
			}
			/*�����κ�һ��Ϊ����Ϊ��ʾ����*/
			iErr_flag = 1;
			ret = 1;
			strcpy( g_pub_tx.reply, "H033" );
			WRITEMSG
			goto TrLog;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
			pub_base_strpack( fldstr[20] );
		if(memcmp(cUnit_no,fldstr[20],strlen(fldstr[20]))){
			/**��λ����������*��ʾ����**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "��[%d]�У���λ������벻��ȷ",lCount);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "��[%d]�У���λ������벻��ȷ",lCount);
			WRITEMSG
			goto TrLog;
		}
		
		/**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		memset(&z_cif_email_inf,0x00,sizeof(z_cif_email_inf));
		memset(&z_cif_email_inf1,0x00,sizeof(z_cif_email_inf1));
		memset(&cif_basic_inf,0x00,sizeof(cif_basic_inf));
		memset( &sAg_peritemmap, 0x00,sizeof(sAg_peritemmap) );
		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_mger_rel, 0x0, sizeof(cif_mger_rel));
		memset(&z_cif_addr_inf,0x00,sizeof(z_cif_addr_inf));
		memset(&z_cif_per_inf,0x00,sizeof(z_cif_per_inf));
		
	    ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[21] );
			vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if ( ret == 0 )
		{
			/*----ͬһ��Ŷ�Ӧ�µ����֤�����Ѿ�������������--����ط�Ҫ��ϣ�������ﶨһ�£����Բ�һ���˶࿪��������࿪�Ļ���ԭ���ķ������˺���δ���--*/
			/*pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[3] ) == 0 )
			{
				iErr_flag = 1;
				ret = 1;
				pub_base_strpack( sAg_peritemmap.ac_no );
				sprintf( cReturn, "���˴�����%s���ж�Ӧ�˺Ų������¿���", fldstr[21]);
				strcpy( g_pub_tx.reply, "H033" );
				sprintf( acErrMsg, "���˴�����%s���ж�Ӧ�˺Ų������¿���", fldstr[21]);
				WRITEMSG
				goto TrLog;
			}*/
			/*---------------------------------------------*/
							/*----ͬһ��λ��� ��������˴������ظ�----*/

				iErr_flag = 1;
				ret = 1;
				dT_amt = dT_amt - dTx_amt;
				pub_base_strpack( sAg_peritemmap.item_no );
				sprintf( cReturn, " ���˱��[%s]�ظ�", sAg_peritemmap.item_no );
				/** 
				strcpy( g_pub_tx.reply, "H033" ); 
				**/
				sprintf( acErrMsg, "���˱��[%s]�ظ�", sAg_peritemmap.item_no );				
				WRITEMSG
				
				goto TrLog;
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"��ѯag_peritemmap����[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}
		/**��ʼ��ѯ�ͻ����Ƿ���ڣ������������¿���**/
		
	    /*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
		    fldstr[4]   �ֿ��˿ͻ���     ok
		    fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
		    fldstr[6]   �ֿ����Ա�       
		    fldstr[7]   �ֿ��˼�ͥסַ   ok
		    fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
		    fldstr[9]   �ֿ��˼�ͥ�绰
		    fldstr[10]  �ֿ��˵�λ����   ok
		    fldstr[11]  �ֿ��˵�λ��ַ   ok
		    fldstr[12]  �ֿ��˵�λ�ʱ�   ok
		    fldstr[13]  �ֿ��˵�λ�绰
		    fldstr[14]  �ֿ����ֻ�����   ok
		    fldstr[15]  �ֿ�������
		    fldstr[16]  �ֿ��������ȼ�
		    fldstr[17]  ������
		    fldstr[18]  ��������
		    fldstr[19]  ����״��
		    fldstr[20]  ��λ���depid    ok
		    fldstr[21]  ���˴�����itemno    ok
		    fldstr[21]  �������    ok
		----------------------------------------------*/
		
	    
		strcpy(cif_id_code_rel.id_type, fldstr[2]);/* ֤������ */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* ֤������ */
		pub_base_strpack(cif_id_code_rel.id_type);
		pub_base_strpack(cif_id_code_rel.id_no);
		vtcp_log("[%s][%d]֤������[%s]\n",__FILE__,__LINE__,fldstr[2]);
		/**MOD AT 20130528 ��������֤����ϢУ�飬���֤����18λ������֤�����ܳ���20λ**/
		if(memcmp(fldstr[2],"1",1) == 0)
				{
					if(strlen(fldstr[3]) != 18 )
						{
							iErr_flag = 1;
							ret = 1;
							sprintf( cReturn, "��[%d]�У���������֤���������Ϊ18λ",lCount);
							strcpy( g_pub_tx.reply, "H033" );
							sprintf( acErrMsg, "��[%d]�У���������֤���������Ϊ18λ",lCount);
							WRITEMSG
							goto TrLog;
					}	
				}
		else
			{
					if(strlen(fldstr[3]) > 20 )
						{
							iErr_flag = 1;
							ret = 1;
							sprintf( cReturn, "��[%d]�У�֤�����볬����20λ",lCount);
							strcpy( g_pub_tx.reply, "H033" );
							sprintf( acErrMsg, "��[%d]�У�֤�����볬����20λ",lCount);
							WRITEMSG
							goto TrLog;
						}
			}
		/**MOD AT 20130528 ��������֤����ϢУ�飬���֤����18λ������֤�����ܳ���20λ**/
		vtcp_log("[%s][%d]֤������[%s]\n",__FILE__,__LINE__,fldstr[3]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		strcpy(cif_basic_inf.type,"1");		      /* �ͻ����� */
		strcpy(cif_basic_inf.name,fldstr[4]);	  /* �ͻ����� */
		strcpy(cif_basic_inf.local,"1");	/* ���ر�־ */
		strcpy(cif_basic_inf.poten,"1");	/* Ǳ�ڱ�־1-����2-Ǳ�� */
		strcpy(cif_basic_inf.shareholder_ind,"1");/*�Ƿ��йɶ�2-�� 1-�� */
		strcpy(cif_basic_inf.lvl,"4"); /*�ͻ�����*/

		/*--------��ʼ�����ͻ���Ϣ----------*/
		cif_no = 0;
		/* ���ͻ�֤�����ͣ�֤�������Ƿ��Ѿ����� */
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" id_type='%s' and id_no='%s' " , cif_id_code_rel.id_type ,
					  cif_id_code_rel.id_no );
		if( ret==0 )
		{
			sprintf(acErrMsg,"�ÿͻ��Ѿ�ӵ�пͻ�ID![%s][%s]",fldstr[5],fldstr[3]);
			WRITEMSG
			/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
			fldstr[4]   �ֿ��˿ͻ���     ok
			fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
			fldstr[6]   �ֿ����Ա�       
			fldstr[7]   �ֿ��˼�ͥסַ   ok
			fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
			fldstr[9]   �ֿ��˼�ͥ�绰
			fldstr[10]  �ֿ��˵�λ����   ok
			fldstr[11]  �ֿ��˵�λ��ַ   ok
			fldstr[12]  �ֿ��˵�λ�ʱ�   ok
			fldstr[13]  �ֿ��˵�λ�绰
			fldstr[14]  �ֿ����ֻ�����   ok
			fldstr[15]  �ֿ�������
			fldstr[16]  �ֿ��������ȼ�
			fldstr[17]  ������
			fldstr[18]  ��������
			fldstr[19]  ����״��
			fldstr[20]  ��λ���depid    ok
			fldstr[21]  ���˴�����itemno    ok
			fldstr[22]  �������    ok
			----------------------------------------------*/
			/****���ӶԻ������ж�****/
			ret = Cif_basic_inf_Sel(g_pub_tx.reply, &cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(ret == 0)
			{
				pub_base_strpack(fldstr[4]);
				pub_base_strpack(cif_basic_inf.name);				
				if(strcmp(cif_basic_inf.name,fldstr[4]) != 0)
				{
					/**�ͻ���������*��ʾ����**/
					iErr_flag = 1;
					ret = 1;
					sprintf( cReturn, "��[%d]�У����������ϣ����ݿ���Ϊ��%s��",lCount,cif_basic_inf.name);
					strcpy( g_pub_tx.reply, "H033" );
					sprintf( acErrMsg, "��[%d]�У����������ϣ����ݿ���Ϊ��%s��",lCount,cif_basic_inf.name);
					WRITEMSG
					goto TrLog;
				}
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ���Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
			cif_no = cif_id_code_rel.cif_no; /*���Ѵ��ڵĿͻ��Ÿ�ֵ*/
			/***����Ҫ����Ϣ�����ݿ��в�ѯ����**/
			/* �Ǽǿͻ�������Ϣ�� */
			ret = Cif_addr_inf_Sel(g_pub_tx.reply,&z_cif_addr_inf,"cif_no=%d",cif_no);
			if( ret && ret!=100)
			{
				sprintf(acErrMsg,"�Ǽǿͻ���ַ��Ϣ�����![%d]",ret);
				WRITEMSG
				/**
				strcpy(g_pub_tx.reply,"C067");
				goto ErrExit;**/
			}
			ret = Cif_per_inf_Sel(g_pub_tx.reply,&z_cif_per_inf,"cif_no=%d",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"��ѯ���˿ͻ���Ϣ��![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C008");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf,"cif_no=%d and addr_type='5'",cif_no);
			if( ret )/**�ֻ�**/
			{
				sprintf(acErrMsg,"���˿ͻ�������Ϣ��![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C062");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf1,"cif_no=%d and addr_type='6'",cif_no);
			if( ret )/**����**/
			{
				sprintf(acErrMsg,"�ͻ����ӵ�ַ��Ϣ��![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;**/
			}
			goto OK_cif_no;	/*ֱ�ӿ���*/
		}else if( ret!=100 )
		{
			sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
			goto ErrExit;
		}
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		cif_basic_inf.cif_no = cif_no ;
		cif_id_code_rel.cif_no = cif_no ;	

		ret = Cif_id_code_rel_Ins( cif_id_code_rel ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C004");
			goto ErrExit;
		}
	
		/* �Ǽǿͻ�������Ϣ�� */
		cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* �������� */
		cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ����ʱ�� */
		strcpy( cif_basic_inf.sts , "1" ) ;         /* �ͻ�״̬��1���� 2ע�� */
		cif_basic_inf.cif_seqn = 1 ;                /* �ͻ���� */
		ret = Cif_basic_inf_Ins( cif_basic_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		/**�Ǽǿͻ���ַ��Ϣ��**/
		z_cif_addr_inf.cif_no=cif_no;
		z_cif_addr_inf.addr_seqn=1;
		z_cif_addr_inf.addr_type[0]='2';
		memcpy(z_cif_addr_inf.addr,fldstr[11],sizeof(z_cif_addr_inf.addr)-1);
		memcpy(z_cif_addr_inf.post_code,fldstr[8],sizeof(z_cif_addr_inf.post_code)-1);
		ret = Cif_addr_inf_Ins( z_cif_addr_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ���ַ��Ϣ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
			goto ErrExit;
		}
		/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
			fldstr[4]   �ֿ��˿ͻ���     ok
			fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
			fldstr[6]   �ֿ����Ա�       
			fldstr[7]   �ֿ��˼�ͥסַ   ok
			fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
			fldstr[9]   �ֿ��˼�ͥ�绰
			fldstr[10]  �ֿ��˵�λ����   ok
			fldstr[11]  �ֿ��˵�λ��ַ   ok
			fldstr[12]  �ֿ��˵�λ�ʱ�   ok
			fldstr[13]  �ֿ��˵�λ�绰
			fldstr[14]  �ֿ����ֻ�����   ok
			fldstr[15]  �ֿ�������
			fldstr[16]  �ֿ��������ȼ�
			fldstr[17]  ������
			fldstr[18]  ��������
			fldstr[19]  ����״��
			fldstr[20]  ��λ���depid    ok
			fldstr[21]  ���˴�����itemno    ok
			fldstr[22]  ������� opnamt
		----------------------------------------------*/
		/**�ǼǸ��˿ͻ���Ϣ��**/
		z_cif_per_inf.cif_no=cif_no;
		z_cif_per_inf.g_bir=atol(fldstr[18]);
		memcpy(z_cif_per_inf.sex,fldstr[6],1);
		memcpy(z_cif_per_inf.marriage,fldstr[19],1);/**��Ҫ������Լ������ֵ ����ʲô��˼�������һ����Ҫת��**/
		memcpy(z_cif_per_inf.name,fldstr[4],sizeof(z_cif_per_inf.name)-1);
		memcpy(z_cif_per_inf.nationality,"01",sizeof(z_cif_per_inf.nationality)-1);
		memcpy(z_cif_per_inf.country,"01",sizeof(z_cif_per_inf.country)-1);
		z_cif_per_inf.mon_income=atof(fldstr[17]);
		ret = Cif_per_inf_Ins( z_cif_per_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"���˿ͻ���Ϣ��![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C008");
			goto ErrExit;
		}
		sprintf(acErrMsg,"�����ͻ���Ϣ�ɹ�,�ͻ���[%ld]",cif_no);/*���ɿͻ���*/
		WRITEMSG
		/**�ͻ����ӵ�ַ��Ϣ��**/
		z_cif_email_inf.cif_no=cif_no;
		ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
		z_cif_email_inf.addr_seqn=ret+1;
		z_cif_email_inf.addr_type[0]='5';/**�ֻ�**/
		memcpy(z_cif_email_inf.email,fldstr[14],sizeof(z_cif_email_inf.email)-1);
		ret = Cif_email_inf_Ins( z_cif_email_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"���˿ͻ���Ϣ��![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C024");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if(strlen(fldstr[15])){/**�����ʼ�**/
			z_cif_email_inf1.cif_no=cif_no;
			ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
			z_cif_email_inf1.addr_seqn=ret+1;
			z_cif_email_inf1.addr_type[0]='6';/**�����ʼ�**/
			memcpy(z_cif_email_inf1.email,fldstr[15],sizeof(z_cif_email_inf1.email)-1);
			ret = Cif_email_inf_Ins( z_cif_email_inf1 ,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"���˿ͻ���Ϣ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;
			}
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
OK_cif_no:
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		/**���տ����������м���ƾ֤����**/
		vtcp_log("%s,%d iOpcnt=[%d],note_mst_z.cnt=[%d]\n",__FILE__,__LINE__,iOpcnt,note_mst_z.cnt);
		if(iOpcnt>note_mst_z.cnt){/***��Ҫ����һ��ƾ֤��**/
			ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
			if(ret){
				sprintf(acErrMsg,"��ѯ��Աƾ̨֤�˱����![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			if(ret==100){
				vtcp_log("%s,%d ƾ֤�����ˣ���Ӧ�ð�������\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			iOpcnt=iOpcnt-note_mst_z.cnt;/***���¼���**/
		}
		vtcp_log("%s,%d sNote_min_no.note_min_no={%s},iOpcnt={%d}\n",__FILE__,__LINE__,sNote_min_no.note_min_no,iOpcnt);
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
		ret=pub_com_CalNoteNo("020",note_mst_z.beg_note_no,sNote_min_no.note_min_no,iOpcnt);
		if(ret){
			sprintf(acErrMsg,"��ѯ��Աƾ̨֤�˱����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		/***��ʼ�ȽϿ��ŵ�ƾ֤�����Ա��ƾ֤���Ƿ�һ��**/
		vtcp_log("%s,%d ���š�%s����ƾ֤��[%s]\n",__FILE__,__LINE__,fldstr[0],sNote_min_no.note_min_no+4);
		if(memcmp(fldstr[0]+6,sNote_min_no.note_min_no+4,12)){/*����7-18λΪƾ֤����**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "��[%d]�У����š�%s����ƾ֤�Ų���[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "��[%d]�У�����[%s]��ƾ֤�Ų���[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			WRITEMSG
			goto TrLog;
		}
		vtcp_log("%s,%d ��������ɣ�ret=[%d]\n",__FILE__,__LINE__,ret);
TrLog:	
		vtcp_log("%s,%d ��������ɣ�ret=[%d]\n",__FILE__,__LINE__,ret);
		if ( ret == 0 )
		{
			/***�����Ҫ�������ļ�fp1*
			����|�ͻ���|�ֿ���֤������|�ֿ���֤������|�ֿ��˿ͻ���|
			�ֿ��˿ͻ�Ӣ����|�ֿ����Ա�|�ֿ��˼�ͥסַ|�ֿ��˼�ͥ�ʱ�|
			�ֿ��˼�ͥ�绰|�ֿ��˵�λ����|�ֿ��˵�λ��ַ|�ֿ��˵�λ�ʱ�|
			�ֿ��˵�λ�绰|�ֿ����ֻ�����|�ֿ�������|�ֿ��������ȼ�|������|��������|����״��***/
			/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
			fldstr[4]   �ֿ��˿ͻ���     ok
			fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
			fldstr[6]   �ֿ����Ա�       
			fldstr[7]   �ֿ��˼�ͥסַ   ok
			fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
			fldstr[9]   �ֿ��˼�ͥ�绰
			fldstr[10]  �ֿ��˵�λ����   ok
			fldstr[11]  �ֿ��˵�λ��ַ   ok
			fldstr[12]  �ֿ��˵�λ�ʱ�   ok
			fldstr[13]  �ֿ��˵�λ�绰
			fldstr[14]  �ֿ����ֻ�����   ok
			fldstr[15]  �ֿ�������
			fldstr[16]  �ֿ��������ȼ�
			fldstr[17]  ������
			fldstr[18]  ��������
			fldstr[19]  ����״��
			fldstr[20]  ��λ���depid    ok
			fldstr[21]  ���˴�����itemno    ok
			fldstr[22]  �������opnamt    ok
		----------------------------------------------*/
			fprintf( fp1, "%19s|%12d|%1s|%20s|%30s|\
				%30s|%1s|%140s|%6s|\
				%15s|%60s|%50s|%6s|\
				%15s|%15s|%100s|%1s|%17.2f|%8s|%1s|\
				%4s|%20s|%17s|\n",
				fldstr[0],cif_no,fldstr[2],fldstr[3],fldstr[4],\
				"",z_cif_per_inf.sex,z_cif_addr_inf.addr,z_cif_addr_inf.post_code,\
				"",sAg_unitinfo.unit_name,"","",\
				"",fldstr[14],fldstr[15],fldstr[16],z_cif_per_inf.mon_income,fldstr[18],z_cif_per_inf.marriage,\
				cUnit_no,fldstr[21],fldstr[22]);
		}
		else
		{
			/**fprintf( fp2, "    ��%10s��%10s��%20s��%20s��%44s��\n", "������", "�ͻ�����", "��������", "֤������", "������Ϣ" );**/
			fprintf( fp2, "    ��%10s��%10s��%20s��%20s��%44s��\n", fldstr[21], fldstr[4], \
			fldstr[0], fldstr[3], cReturn );
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		lCount = lCount + 1;
		iOpcnt++;
	}
	
	fprintf( fp2, "    �������������۩����������۩��������������������۩��������������������۩���������������������������������������������\n" );

	fprintf(fp2,"\n\n");
	fprintf(fp2,"������������ʧ��!!!�������ϼ�¼����!!!!\n");

	fclose( fp );
	fclose( fp1 );
	fclose( fp2 );
	if ( iErr_flag == 1 )/**���ٱ�֤¼�����ʱ���еļ�¼���ܹ�ȥ,����������Ǳ߸��鷳**/
	{
		/**����д�����ϸʱ��ǰ̨���ʹ�����ļ� 20130510**/
		memset( execstr, 0x0, sizeof(execstr) );
		sprintf( execstr, "cp %s %s", filename2, filename );
		ret = system( execstr );
		if ( ret )
		{
			sprintf(acErrMsg,"�����´��ļ���ʧ��[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;		
		}
		set_zd_data( DC_FILE_SND, "1" );	
		vtcp_log("%s,%d ��ǰ̨���ʹ�����ϸfilename=��%s��",__FILE__,__LINE__,filename);
		/**����д�����ϸʱ��ǰ̨���ʹ�����ļ� 20130510**/
		strcpy( g_pub_tx.reply, "H086" );
		goto ErrExit;
	}
	set_zd_data("0260",cFilename);/**���ڸ�ƽ̨�����ļ�**/
	/**���һ��shell���� ���Ļ������͵����ڽ���ƽ̨***/
	vtcp_log("%s,%d cFilename=[%s]\n",__FILE__,__LINE__,cFilename);
        /****�޸�Ϊ�µļ��ܷ�ʽ 20120604
	sprintf(cFtpfile,"%s/bin/opncardtosw.sh %s",getenv("WORKDIR"),cFilename);
	****/
	sprintf(cFtpfile,"%s/bin/opncardtosw.sh %s %s %s",getenv("WORKDIR"),get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨����[%d]",cFilename,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨�ɹ�",cFilename);
	WRITEMSG

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    fclose( fp );
    fclose( fp1 );
    fclose( fp2 );
    sprintf(acErrMsg,"��������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
/**
**ת��ǰ̨���ļ����˳��
**/
int transfer(char *buf,char *fdstr)
{
		int i;
		int ret;
		char cFldstr[30][150];/**����Ҫ����20��������ʱ����Ϊ30�����Ϊ140��������ʱ��Ϊ150***/
		memset(cFldstr, 0, sizeof(cFldstr));
		vtcp_log("[%s][%d] �������ݡ�%s�� ", __FILE__, __LINE__,buf);
		for( i=0;i<9;i++ )/*�ĳ�23 ����Ҫ����ϵ�λ����,���˴����ţ������������23��**/
		{
			ret=pub_base_cut_str( buf,cFldstr[i],"|",i+1 );
			vtcp_log("[%s][%d] [%d] [%s] ", __FILE__, __LINE__, i+1, cFldstr[i]);
			pub_base_strpack(cFldstr[i]);
		}
		vtcp_log("[%s][%d] cFldstr[7]->%s ���� ", __FILE__, __LINE__,cFldstr[7]);
		memcpy(fdstr, cFldstr[7],  strlen(cFldstr[7]));/**����**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+2*150, cFldstr[6],  strlen(cFldstr[6]));/**�ֿ���֤������**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+3*150, cFldstr[4],  strlen(cFldstr[4]));/**�ֿ���֤������**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+4*150, cFldstr[2],  strlen(cFldstr[2]));/**�ֿ��˿ͻ���**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+14*150, cFldstr[8], strlen(cFldstr[8]));/**�ֿ����ֻ�����**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+20*150, cFldstr[5], strlen(cFldstr[5]));/**��λ���**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+21*150, cFldstr[0], strlen(cFldstr[0]));/**���˴�����itemno**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
		memcpy(fdstr+22*150, cFldstr[3], strlen(cFldstr[3]));/**�������opnamt**/
		vtcp_log("[%s][%d] transfer ���� ", __FILE__, __LINE__);
			
		return 0;
}
	/*-	fldstr[0]   ���� ------------ok------------
			fldstr[1]   �ͻ���           ok
			fldstr[2]   �ֿ���֤������   ok
			fldstr[3]   �ֿ���֤������   ok
			fldstr[4]   �ֿ��˿ͻ���     ok
			fldstr[5]   �ֿ��˿ͻ�Ӣ���� 
			fldstr[6]   �ֿ����Ա�       
			fldstr[7]   �ֿ��˼�ͥסַ   ok
			fldstr[8]   �ֿ��˼�ͥ�ʱ�   ok
			fldstr[9]   �ֿ��˼�ͥ�绰
			fldstr[10]  �ֿ��˵�λ����   ok
			fldstr[11]  �ֿ��˵�λ��ַ   ok
			fldstr[12]  �ֿ��˵�λ�ʱ�   ok
			fldstr[13]  �ֿ��˵�λ�绰
			fldstr[14]  �ֿ����ֻ�����   ok
			fldstr[15]  �ֿ�������
			fldstr[16]  �ֿ��������ȼ�
			fldstr[17]  ������
			fldstr[18]  ��������
			fldstr[19]  ����״��
			fldstr[20]  ��λ���depid    ok
			fldstr[21]  ���˴�����itemno    ok
			fldstr[22]  �������opnamt    ok
		----------------------------------------------*/
