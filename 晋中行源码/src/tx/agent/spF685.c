/*************************************************
* ÎÄ ¼ş Ãû: spF685.c--spF610.cĞŞ¸Ä
* ¹¦ÄÜÃèÊö£ºÅúÁ¿¿ª»§½»Ò×-ÉñÂë(´Ë½»Ò×ÓÉ686¸´ÖÆ¶øÀ´£¬ÎªÁËÅäºÏwinqtµÄĞÂÅúÁ¿´úÀí½»Ò×)
*
* ×÷    Õß: jack
* Íê³ÉÈÕÆÚ: 2005Äê02ÔÂ24ÈÕ
* ĞŞ¸ÄÈÕÆÚ: 2010-03-22 
* ĞŞ ¸Ä ÈË: ZHGF
* ĞŞ¸ÄÄÚÈİ: ¸Ä³ÉÖ§³ÖÉñÂëÒªÇóµÄÒì²½Í¨ĞÅ¿ª¿¨Ä£Ê½
insert into tx_Def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('6642', 'ĞÂÔ¤ÅúÁ¿¿ª¿¨Éú³ÉÎÄ¼ş', '10000000000000000000000001000000000000010001110000000000000000000010110000000000000000000010000000000000000000000000010000000000', '1', '6', '2');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('6642', 0, '6642', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('6642', 'ĞÂÔ¤ÅúÁ¿¿ª¿¨Éú³ÉÎÄ¼ş', 'spF685', '0', '0');
-----------------------
insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('6642', 1, '', '', '', '', '', '', '', '', '0000', 'winqtÔ¤ÅúÁ¿¿ª¿¨');

insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '6642', 1, 'winqtÔ¤ÅúÁ¿¿ª¿¨ÊÚÈ¨', 'sq(1237)');
* Ç°Ì¨µ¼´ÅÅÌµÄÎÄ¼ş¸ñÊ½°´ÕÕÉñÂëµÄÎÄ¼şÒªÇó½øĞĞ£¬ÈçÏÂ£º
¿¨ºÅ|¿Í»§ºÅ|³Ö¿¨ÈËÖ¤¼şÖÖÀà|³Ö¿¨ÈËÖ¤¼şºÅÂë|³Ö¿¨ÈË¿Í»§Ãû|³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû|³Ö¿¨ÈËĞÔ±ğ|³Ö¿¨ÈË¼ÒÍ¥×¡Ö·|³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à|³Ö¿¨ÈË¼ÒÍ¥µç»°|³Ö¿¨ÈËµ¥Î»Ãû³Æ|³Ö¿¨ÈËµ¥Î»µØÖ·|³Ö¿¨ÈËµ¥Î»ÓÊ±à|³Ö¿¨ÈËµ¥Î»µç»°|³Ö¿¨ÈËÊÖ»úºÅÂë|³Ö¿¨ÈËÓÊÏä|³Ö¿¨ÈËĞÅÓşµÈ¼¶|ÔÂÊÕÈë|³öÉúÈÕÆÚ|»éÒö×´¿ö|µ¥Î»±àºÅ|¸öÈË´úÀí±àºÅ|¿ª»§½ğ¶î|
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"/*--- Ö¤¼şÀàĞÍÄ¬ÈÏÎª1 -ÏÖ¸ÄÎª¿ÉÒÔÈÎºÎÖ¤¼şÀàĞÍfldstr[5]-*/
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
	char fldstr[30][150];/**ÉñÂëÒªÇóÊÇ20¸öÄÚÈİÔİÊ±¶¨ÒåÎª30£¬×î³¤µÄÎª140£¬ËùÒÔÔİÊ±¶¨Îª150***/
	char tmpstr[1024];
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	int i=0;

	char filename2[100];
	FILE *fp2 = NULL;
	char filename3[100];
	FILE *fp3 = NULL;	
	char filename4[100];	/*½«ÕıÈ·¿ª»§½á¹û±¸·İ*/
	long lSnum = 0;
	double dSamt = 0.00;
	int iErr_flag = 0;	/*---±êÖ¾¸ÃÅú¼ÇÂ¼ÖĞÊÇ·ñÓĞ´íÎó³öÏÖ---*/
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

	char cHand_flag[2];	/*---ĞÅÏ¢Â¼Èë·½Ê½---*/
	double dT_amt = 0.00;	/*---Ç°Ì¨ÊäÈë×Ü½ğ¶î---*/
	long lCount_i = 0;	/*---Ç°Ì¨ÊäÈë×Ü±ÊÊı---*/
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
		sprintf( acErrMsg, "ÃÜÂë²»ÄÜÎª¿Õ!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( "0130", "ÃÜÂë²»ÄÜÎª¿Õ!!!" );
		goto ErrExit;
	}
	/** ³õÊ¼»¯È«¾Ö±äÁ¿ **/
	pub_base_sysinit();
	/**
¿¨ºÅ|¿Í»§ºÅ|³Ö¿¨ÈËÖ¤¼şÖÖÀà|³Ö¿¨ÈËÖ¤¼şºÅÂë|³Ö¿¨ÈË¿Í»§Ãû|³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû|³Ö¿¨ÈËĞÔ±ğ|³Ö¿¨ÈË¼ÒÍ¥×¡Ö·|³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à|³Ö¿¨ÈË¼ÒÍ¥µç»°|³Ö¿¨ÈËµ¥Î»Ãû³Æ|³Ö¿¨ÈËµ¥Î»µØÖ·|³Ö¿¨ÈËµ¥Î»ÓÊ±à|³Ö¿¨ÈËµ¥Î»µç»°|³Ö¿¨ÈËÊÖ»úºÅÂë|³Ö¿¨ÈËÓÊÏä|³Ö¿¨ÈËĞÅÓşµÈ¼¶|ÔÂÊÕÈë|³öÉúÈÕÆÚ|»éÒö×´¿ö|´úÀíµ¥Î»±àºÅ|¸öÈË´úÀí±àºÅ|¿ª»§½ğ¶î|
	**/
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "020") || !strcmp(cNote_type, "024")   || !strcmp(cNote_type, "027")|| !strcmp(cNote_type, "028")  || !strcmp(cNote_type, "029") || !strcmp(cNote_type, "031") || !strcmp(cNote_type, "032")) /* ¿¨ $IC¿¨*/
	{
		strcpy( cRctp, "¿¨" );
	}else{/**×¢Òâ**ÓĞ¼¸ÖÖ¿¨ÄØ£¿£¿£¿£¿Èç¹û»¹ÓĞÆäËû¿¨ĞèÒª½øĞĞÌí¼ÓĞŞ¸Ä´Ë´¦**/
		sprintf( acErrMsg, "½éÖÊÀàĞÍ´íÎó£¬²»ÊÇ¿¨!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O188" );
		set_zd_data( "0130", "½éÖÊÀàĞÍ´íÎó!!!" );
		goto ErrExit;
	}

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "Ã»ÓĞ²éÑ¯µ½¸Ãµ¥Î»±àºÅ¶ÔÓ¦¼ÇÂ¼!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "Ö´ĞĞAg_unitinfo´íÎó!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	/**¶ÔÅú¿ªÎÄ¼şÖĞÒÑ¾­Ïú»§µÄÕË»§±¨´í 20130510**/
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel, "ac_no='%s'", sAg_unitinfo.bank_acct_no);
	if( ret )
	{
		sprintf( acErrMsg, "Ö´ĞĞmdm_ac_rel´íÎó!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	vtcp_log("%s,%d,s_mdm_ac_rel.note_sts =[%s]\n",__FILE__,__LINE__,s_mdm_ac_rel.note_sts);
	/*if(s_mdm_ac_rel.note_sts == '*')*/
	if(strcmp(s_mdm_ac_rel.note_sts,"*") == 0)
	{
		vtcp_log("¸Ã´úÀí¹ØÁªÕÊºÅÒÑ×öÏú»§´¦Àí [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		sprintf( acErrMsg, "¸Ã´úÀí¹ØÁªÕÊºÅÒÑ×öÏú»§´¦Àí!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P245" );
		goto	ErrExit;	
	}
	/**¶ÔÅú¿ªÎÄ¼şÖĞÒÑ¾­Ïú»§µÄÕË»§±¨´í 20130510 **/
	pub_base_strpack( sAg_unitinfo.unit_name );

	memset( filename1, 0x0, sizeof(filename1) );
	sprintf( filename1,"%s/0%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	vtcp_log("%s,%d,ÕıÈ·ÎÄ¼şÃûfilename1[%s]\n",__FILE__,__LINE__,filename1);
	sprintf(cFilename,"0%s%ld%s%06d.txt",cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	fp1 = fopen( filename1,"w" );   /*°´ÕÕÉñÂëÒªÇóÉú³ÉÎÄ¼ş*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	sprintf( filename2,"%s/1%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
	vtcp_log("%s,%d,ÓĞ´íÎóĞÅÏ¢µÄ¼ÇÂ¼Ğ´³Éfilename2[%s]\n",__FILE__,__LINE__,filename2);
	fp2 = fopen(filename2,"w" );   /*Éú³ÉÇ°Ì¨¹ñÔ±µÄ´íÎóÌáÊ¾ÎÄ¼ş*/
	if(fp2==NULL)
	{
		sprintf( acErrMsg, " open file [%s] error ", filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fprintf(fp2,"@3v1\n");
  	
	fprintf( fp2, "                         ÅúÁ¿¿ª»§´íÎó½á¹ûĞÅÏ¢±í[%s-%s]			\n", \
    	sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
	fprintf( fp2, "    ©±©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
	fprintf( fp2, "    ©¦%10s©¦%10s©¦%20s©¦%20s©¦%44s©¦\n", "´úÀí±àºÅ", "¿Í»§ĞÕÃû", "´ı¿ª¿¨ºÅ", "Ö¤¼şºÅÂë", "´íÎóĞÅÏ¢" );
	fprintf( fp2, "    ©Á©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©É\n" );

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"È¡µÃµÄÎÄ¼şÃûÎª[%s]",filename);
	WRITEMSG
	/**½«Ç°Ì¨´«À´µÄÎÄ¼ş½øĞĞ±¸·İ**/
	sprintf(cCpfilename,"cp %s %s/CARD%s%d%sOK.txt",filename,getenv("FILDIR"),g_pub_tx.tel,g_pub_tx.tx_date,cUnit_no);
	vtcp_log("%s,%d cCpfilename=[%s]\n",__FILE__,__LINE__,cCpfilename);
	ret=system(cCpfilename);
	if (ret)
	{
		sprintf(acErrMsg,"¿½±´Ç°Ì¨´«À´µÄÎÄ¼ş³ö´í[%d]",ret);
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

	if ( cHand_flag[0] == '0' )	/*---ÊÖ¹¤---*/
	{
		get_zd_double("0400",&dT_amt);
		get_zd_long( "0550", &lCount_i );
	}
	else				/*---·ÇÊÖ¹¤·½Ê½ÎÄ¼şµÚÒ»ĞĞÎª»ã×ÜĞÅÏ¢---Ö÷Òª´¦ÀíÅúÁ¿µ¼ÅÌµÄÎÄ¼ş*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		/*-	ÅúÁ¿ÎÄ¼şµÄÄÚÈİµÚÒ»ĞĞ»ã×Ü¸ñÊ½ÈçÏÂ
			fldstr[0] µ¥Î»±àºÅ
		    fldstr[1] »ã×Ü±ÊÊı
		    fldstr[2] »ã×Ü½ğ¶î
			
		----ÅúÁ¿ÎÄ¼şµÄÆäËûÃ¿Ò»ĞĞ¶ÔÓ¦ÓÚÒ»Ìõ¼ÇÂ¼£¬¸ñÊ½ºÍÊÖ¹¤Â¼ÈëÒ»Ñù£¬ÈçÏÂ
			
		¿¨ºÅ|¿Í»§ºÅ|³Ö¿¨ÈËÖ¤¼şÖÖÀà |³Ö¿¨ÈËÖ¤¼şºÅÂë|³Ö¿¨ÈË¿Í»§Ãû|³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû|³Ö¿¨ÈËĞÔ±ğ|³Ö¿¨ÈË¼ÒÍ¥×¡Ö·|³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à|³Ö¿¨ÈË¼ÒÍ¥µç»°|³Ö¿¨ÈËµ¥Î»Ãû³Æ|³Ö¿¨ÈËµ¥Î»µØÖ· |³Ö¿¨ÈËµ¥Î»ÓÊ±à|³Ö¿¨ÈËµ¥Î»µç»°|³Ö¿¨ÈËÊÖ»úºÅÂë|³Ö¿¨ÈËÓÊÏä|³Ö¿¨ÈËĞÅÓşµÈ¼¶|ÔÂÊÕÈë|³öÉúÈÕÆÚ|»éÒö×´¿ö|µ¥Î»±àºÅ|¸öÈË´úÀí±àºÅ|¿ª»§½ğ¶î|
			
		----------------------------------------------*/
		memset(fldstr,0x0,sizeof(fldstr));
		for(i=0;i<3;i++)/**µÚÒ»ĞĞ¾ÍÁ½¸öÄÚÈİ µ¥Î»±àºÅ|×Ü±ÊÊı|×Ü½ğ¶î|**/
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" fldstr[%d]=[%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/**¿ØÖÆ¿ª»§×Ü½ğ¶îÖ»ÄÜÎªÁã 20130502**/
		if( atof(fldstr[2])!=0){
			sprintf(acErrMsg,"ÅúÁ¿Ô¤¿ª¿¨±ØĞëÊÇ0½ğ¶î!!!" );
			WRITEMSG
			sprintf(acErrMsg,"ÅúÁ¿Ô¤¿ª¿¨±ØĞëÊÇ0½ğ¶î!!!");
			set_zd_data( "0130", acErrMsg );
			strcpy( g_pub_tx.reply, "H019" );
			goto ErrExit;
		}
		/**¿ØÖÆ¿ª»§×Ü½ğ¶îÖ»ÄÜÎªÁã 20130502**/
		long    fd55=0;
		get_zd_long( "0550", &fd55);/**×Ü±ÊÊı****/
		if(fd55 != atoi(fldstr[1]))
		{
			sprintf(acErrMsg,"×Ü±ÊÊıÓëÅÌÄÚ±ÊÊı²»ÏàµÈ!!!" );
			WRITEMSG
			sprintf(acErrMsg,"ÅÌÄÚ»ã×Ü±ÊÊıÎª[%d]", atoi(fldstr[1]) );
			set_zd_data( "0130", acErrMsg );
			strcpy( g_pub_tx.reply, "H019" );
			goto ErrExit;
		}
		lCount_i = atoi(fldstr[1]);	
		dT_amt = atof(fldstr[2]);
		vtcp_log("%s,%d,ÎÄ¼şÖĞµÄ»ã×Ü½ğ¶îfldstr[2]=[%s]",__FILE__,__LINE__,fldstr[2]);
		vtcp_log("%s,%d,ÎÄ¼şÖĞµÄ»ã×Ü½ğ¶îÎª[%lf]",__FILE__,__LINE__,dT_amt);
	}
	
	vtcp_log("%s,%d,Í³¼Æ±ÊÊı[%d]Ç°Ì¨´«±ÊÊı[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	/*------ ÅĞ¶Ï¹ñÔ±ÊÇ·ñÓµÓĞÂú×ãÌõ¼şµÄÆ¾Ö¤ -----*/
	lCount=0;/**¸Õ¿ªÊ¼Îª1**/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			vtcp_log("%s,%d,´¦Àí×îºóÒ»ĞĞ!",__FILE__,__LINE__);
			memset(fldstr,0x0,sizeof(fldstr));
			if ( cHand_flag[0] != '0' )
				{
					for( i=0;i<23;i++ )/*°´ÕÕÉñÂëµÄÎÄ¼ş¸ñÊ½ Ìí¼ÓÁËµ¥Î»±àºÅ ¸öÈË´úÀí±àºÅ ¿ª»§½ğ¶î**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
				}else
				{/**ÊÖ¹¤¿ª¿¨**/
					transfer(tmpstr,fldstr[0]);
				}
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,×îºóÒ»ĞĞÎª·Ç·¨ĞĞ!",__FILE__,__LINE__);
				vtcp_log("%s,%d,Í³¼Æ±ÊÊılCount=[%d]",__FILE__,__LINE__,lCount);	
				break;
			}
			/**ÏÈ½«¸öÈË´úÀí±àºÅÊı×ÖÇ°ÃæµÄ0È¥µô**/
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("[%s][%d]ÏÔÊ¾¸÷¸öÁĞĞÅÏ¢\n",__FILE__,__LINE__);
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
		vtcp_log("%s,%d,Í³¼Æ±ÊÊılCount=[%d]",__FILE__,__LINE__,lCount);	
		break;
		}
		memset(fldstr,0x0,sizeof(fldstr));
		if( cHand_flag[0] != '0' )
			{
				for( i=0;i<23;i++ )/*°´ÕÕÉñÂëµÄÎÄ¼ş¸ñÊ½ Ìí¼ÓÁËµ¥Î»±àºÅ ¸öÈË´úÀí±àºÅ ¿ª»§½ğ¶î**/
					{
						ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
						vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
						pub_base_strpack( fldstr[i] );
					}
			}else
			{			
				vtcp_log("[%s][%d]ÏÔÊ¾¸÷¸öÁĞĞÅÏ¢\n",__FILE__,__LINE__);
				transfer(tmpstr,fldstr[0]);
			}
			
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,´ËĞĞÎª·Ç·¨ĞĞ!",__FILE__,__LINE__);
				continue;
			}
		/*
		¿¨ºÅ|¿Í»§ºÅ|³Ö¿¨ÈËÖ¤¼şÖÖÀà|³Ö¿¨ÈËÖ¤¼şºÅÂë|³Ö¿¨ÈË¿Í»§Ãû|³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû|³Ö¿¨ÈËĞÔ±ğ|³Ö¿¨ÈË¼ÒÍ¥×¡Ö·|³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à|³Ö¿¨ÈË¼ÒÍ¥µç»°|³Ö¿¨ÈËµ¥Î»Ãû³Æ|³Ö¿¨ÈËµ¥Î»µØÖ·|³Ö¿¨ÈËµ¥Î»ÓÊ±à|³Ö¿¨ÈËµ¥Î»µç»°|³Ö¿¨ÈËÊÖ»úºÅÂë|³Ö¿¨ÈËÓÊÏä|³Ö¿¨ÈËĞÅÓşµÈ¼¶|ÔÂÊÕÈë|³öÉúÈÕÆÚ|»éÒö×´¿ö|µ¥Î»±àºÅ|¸öÈË´úÀí±àºÅ|¿ª»§½ğ¶î|
		*/
		/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
		    fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
		    fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
		    fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
		    fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
		    fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
		    fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
		    fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
		    fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
		    fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
		    fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
		    fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
		    fldstr[15]  ³Ö¿¨ÈËÓÊÏä
		    fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
		    fldstr[17]  ÔÂÊÕÈë
		    fldstr[18]  ³öÉúÈÕÆÚ
		    fldstr[19]  »éÒö×´¿ö
		    fldstr[20]  µ¥Î»±àºÅdepid    ok
		    fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
		    fldstr[22]  ¿ª»§½ğ¶îopnamt    ok
		----------------------------------------------*/
		{
			/**ÏÈ½«¸öÈË´úÀí±àºÅÊı×ÖÇ°ÃæµÄ0È¥µô**/
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("[%s][%d]ÏÔÊ¾¸÷¸öÁĞĞÅÏ¢\n",__FILE__,__LINE__);
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
		
		/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
		    fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
		    fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
		    fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
		    fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
		    fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
		    fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
		    fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
		    fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
		    fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
		    fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
		    fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
		    fldstr[15]  ³Ö¿¨ÈËÓÊÏä
		    fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
		    fldstr[17]  ÔÂÊÕÈë
		    fldstr[18]  ³öÉúÈÕÆÚ
		    fldstr[19]  »éÒö×´¿ö
		    fldstr[20]  µ¥Î»±àºÅdepid    ok
		    fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
		    fldstr[22]  ¿ª»§½ğ¶îopnamt    ok
		------------------------------------------*/
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[22] );
		/**¿ØÖÆÃ¿±ÊµÄ¿ª»§½ğ¶îÖ»ÄÜÎªÁã 20130502**/
		if(dTemp_amt != 0.00)
		{
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "µÚ[%d]ĞĞ£¬ÅúÁ¿¿ª¿¨Ã¿±Ê½ğ¶î±ØĞëÎªÁã",lCount);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬ÅúÁ¿¿ª¿¨Ã¿±Ê½ğ¶î±ØĞëÎªÁã",lCount);
			WRITEMSG
			goto TrLog;	
		}
		/**¿ØÖÆÃ¿±ÊµÄ¿ª»§½ğ¶îÖ»ÄÜÎªÁã 20130502**/
		dTmp_amt = dTmp_amt + dTemp_amt;
		lCount = lCount + 1;
		vtcp_log("%s,%d,Í³¼Æ±ÊÊılCount=[%d]",__FILE__,__LINE__,lCount);
	}
	/**È¥µô£¬²»ºÏÀí¡£wangwk-20110117
	if ( cHand_flag[0] == '1' )	---µ¼ÅÌ---
		lCount=lCount-1;
		**/
	vtcp_log("%s,%d,Í³¼Æ±ÊÊı[%d]Ç°Ì¨´«±ÊÊı[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	if ( lCount == 0 )
	{
		sprintf(acErrMsg,"Ã»ÓĞĞèÒªÅúÁ¿¿ª»§µÄ¼ÇÂ¼!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H017" );
		goto ErrExit;
	}
	fclose ( fp );
	vtcp_log("%s,%d,Í³¼Æ±ÊÊı[%d]Ç°Ì¨´«±ÊÊı[%d]",__FILE__,__LINE__,lCount,lCount_i);	
	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"×Ü±ÊÊıÓë·Ö»§Ã÷Ï¸ºÏ¼Æ±ÊÊı²»ÏàµÈ!!!" );
		WRITEMSG
		sprintf(acErrMsg,"Ã÷Ï¸±ÊÊıÎª[%d]", lCount );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"Ã÷Ï¸Óà¶îÎª[%.2lf]dT_amt=[%.2lf]", dTmp_amt,dT_amt);
	WRITEMSG
	ret = pub_base_CompDblVal( dT_amt, dTmp_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"»ã×Ü½ğ¶î[%.2lf]ÓëÃ÷Ï¸×Ü½ğ¶îÎª[%.2lf]",dTmp_amt,dT_amt);
		WRITEMSG
		sprintf(acErrMsg,"»ã×Ü½ğ¶î[%.2lf]ÓëÃ÷Ï¸×Ü½ğ¶îÎª[%.2lf]",dTmp_amt,dT_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	memset(&sMdm_prdt_rel,0,sizeof(struct mdm_prdt_rel_c));
	ret = Mdm_prdt_rel_Sel(g_pub_tx.reply,&sMdm_prdt_rel,"prdt_no='%s' and mdm_code1 ='0'||'%s'",cPrdt_code,cNote_type);
	if(ret)
	{
		vtcp_log("%s,%d,²úÆ·Óë½éÖÊ²»´æÔÚ¹ØÁª¹ØÏµ!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"Z905");
		goto ErrExit;
	}
	if ( strcmp( cNote_type, "020" ) == 0 || strcmp( cNote_type, "024" ) == 0  || strcmp( cNote_type, "027" ) == 0 || strcmp( cNote_type, "028" ) == 0  || strcmp( cNote_type, "029" ) == 0 || strcmp( cNote_type, "031" ) == 0 || strcmp( cNote_type, "032" ) == 0)		/**¿¨½éÖÊĞèÒªÏÂÆ¾Ö¤**/
	{
		ret = sql_sum_long ( "note_mst", "cnt", &lRnum, "tel = '%s' \
					and note_type = '%s' and sts = '0'", \
					g_pub_tx.tel, cNote_type );
		if ( lRnum < lCount )
		{
			sprintf(acErrMsg, "¿ª»§»§Êı[%d]Æ¾Ö¤Êı[%d]", lCount, lRnum );
			WRITEMSG
        	
			sprintf(acErrMsg, "¹ñÔ±¿ÉÓÃÆ¾Ö¤²»×ãÕâ´ÎÅúÁ¿¿ª»§!!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H018" );
			goto ErrExit;
		}
        	
		sprintf(acErrMsg, "¿ª»§»§Êı[%d]Æ¾Ö¤Êı[%d]", lCount, lRnum );
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
	
	if ( cHand_flag[0] != '0' )	/*---·ÇÊÖ¹¤È¥µôµÚÒ»ĞĞ---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
	}
	/****ÕâÖÖÄ£Ê½µÄ»°¾ÍÒªÖØĞÂ¼ÆËã¹ñÔ±¿ÉÓÃÆ¾Ö¤ÁË£¬ÒòÎª²»ÊÇÕæÕıÏÂ¹ñÔ±Æ¾Ö¤**/
	{
		ret=Note_mst_Dec_Sel(g_pub_tx.reply,"tel='%s' and note_type='%s' and sts='0' and cnt>0\
			order by beg_note_no",g_pub_tx.tel,cNote_type);
		if(ret){
			sprintf(acErrMsg,"²éÑ¯¹ñÔ±Æ¾Ö¤Ì¨ÕË±í´íÎó![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		
		ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
		if(ret){
			sprintf(acErrMsg,"²éÑ¯¹ñÔ±Æ¾Ö¤Ì¨ÕË±í´íÎó![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
	}
	iOpcnt=0;
	lCount=0;
	/***ÖğÌõ¼ì²é****/
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
			for( i=0;i<23;i++ )/*¸Ä³É23 ÉñÂëÒªÇó¼ÓÉÏµ¥Î»±àÂë,¸öÈË´úÀí±àºÅ£¬¿ª»§½ğ¶îºó¾ÍÊÇ23¸ö**/
			{
				ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
				pub_base_strpack( fldstr[i] );
			}
		}else
		{
			/**ÊÖ¹¤¿ª¿¨**/
						/**×ª»¯Ò»ÏÂÇ°Ì¨ÊäÈëË³Ğò-wangwenkui-20101226**/
						transfer(tmpstr,fldstr[0]);
		}
		if(memcmp(fldstr[0],CARDHEADCODE,6) != 0 && memcmp(fldstr[0],ICCARDHEADCODE,6) != 0)
			{
				vtcp_log("%s,%d,´ËĞĞÎª·Ç·¨ĞĞ!",__FILE__,__LINE__);
				continue;
			}
		pub_base_ltrim_zero(fldstr[21]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
		    fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
		    fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
		    fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
		    fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
		    fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
		    fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
		    fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
		    fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
		    fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
		    fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
		    fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
		    fldstr[15]  ³Ö¿¨ÈËÓÊÏä
		    fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
		    fldstr[17]  ÔÂÊÕÈë
		    fldstr[18]  ³öÉúÈÕÆÚ
		    fldstr[19]  »éÒö×´¿ö
		    fldstr[20]  µ¥Î»±àºÅdepid    ok
		    fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
		    fldstr[22]  ¿ª»§½ğ¶îopnamt    ok
		----------------------------------------------*/
		if ( strlen(fldstr[0]) == 0  || strlen(fldstr[2]) == 0
		   ||strlen(fldstr[3]) == 0  || strlen(fldstr[4]) == 0
		   ||strlen(fldstr[14]) == 0 || strlen(fldstr[20]) == 0
		   ||strlen(fldstr[21]) == 0 )
		/**Ó¦ĞĞÀïÒªÇó,±ØĞëÎªÁã**/
		{
			if(strlen(fldstr[0]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬¿¨ºÅ²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬¿¨ºÅ²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[2]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬Ö¤¼şÖÖÀà²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬Ö¤¼şÖÖÀà²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[3]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬Ö¤¼şºÅÂë²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬Ö¤¼şºÅÂë²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[4]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬¿Í»§Ãû²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬¿Í»§Ãû²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[14]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬ÊÖ»úºÅÂë²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬ÊÖ»úºÅÂë²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[20]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬µ¥Î»±àºÅ²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬µ¥Î»±àºÅ²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[21]) == 0){
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬¸öÈË´úÀí±àºÅ²»ÄÜÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬¸öÈË´úÀí±àºÅ²»ÄÜÎª¿Õ",lCount);
			}
			if(strlen(fldstr[22]) != 0&& atof(fldstr[22])!= 0.0)
			{
				sprintf( cReturn, "µÚ[%d]ĞĞ£¬¿ª»§½ğ¶î±ØĞëÎª¿Õ",lCount);
				sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬¿ª»§½ğ¶î±ØĞëÎª¿Õ",lCount);
			}
			/*ÆäÖĞÈÎºÎÒ»ÏîÎª¿ÕÈÏÎªÌáÊ¾±¨´í*/
			iErr_flag = 1;
			ret = 1;
			strcpy( g_pub_tx.reply, "H033" );
			WRITEMSG
			goto TrLog;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
			pub_base_strpack( fldstr[20] );
		if(memcmp(cUnit_no,fldstr[20],strlen(fldstr[20]))){
			/**µ¥Î»±àºÅÊäÈë´íÎó*ÌáÊ¾±¨´í**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "µÚ[%d]ĞĞ£¬µ¥Î»±àºÅÊäÈë²»ÕıÈ·",lCount);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬µ¥Î»±àºÅÊäÈë²»ÕıÈ·",lCount);
			WRITEMSG
			goto TrLog;
		}
		
		/**------------ Ã¿´ÎÑ­»·¶¼ÖØĞÂ³õÊ¼»¯È«¾Ö±äÁ¿ -------------**/
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
			/*----Í¬Ò»±àºÅ¶ÔÓ¦ÏÂµÄÉí·İÖ¤ºÅÂëÒÑ¾­´æÔÚÔò²»ÔÊĞí¿ª»§--Õâ¸öµØ·½Òªº«Ï£³¬ºÍĞĞÀï¶¨Ò»ÏÂ£¿¿ÉÒÔ²»Ò»¸öÈË¶à¿ª»§Âğ£¿Èç¹û¶à¿ªµÄ»°£¬Ô­À´µÄ·¢¹¤×ÊÕËºÅÈçºÎ´¦Àí£¿--*/
			/*pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[3] ) == 0 )
			{
				iErr_flag = 1;
				ret = 1;
				pub_base_strpack( sAg_peritemmap.ac_no );
				sprintf( cReturn, "¸öÈË´úÀí±àºÅ%sÒÑÓĞ¶ÔÓ¦ÕËºÅ²»ÄÜÖØĞÂ¿ª»§", fldstr[21]);
				strcpy( g_pub_tx.reply, "H033" );
				sprintf( acErrMsg, "¸öÈË´úÀí±àºÅ%sÒÑÓĞ¶ÔÓ¦ÕËºÅ²»ÄÜÖØĞÂ¿ª»§", fldstr[21]);
				WRITEMSG
				goto TrLog;
			}*/
			/*---------------------------------------------*/
							/*----Í¬Ò»µ¥Î»±àºÅ ²»ÔÊĞí¸öÈË´úÀí±àºÅÖØ¸´----*/

				iErr_flag = 1;
				ret = 1;
				dT_amt = dT_amt - dTx_amt;
				pub_base_strpack( sAg_peritemmap.item_no );
				sprintf( cReturn, " ¸öÈË±àºÅ[%s]ÖØ¸´", sAg_peritemmap.item_no );
				/** 
				strcpy( g_pub_tx.reply, "H033" ); 
				**/
				sprintf( acErrMsg, "¸öÈË±àºÅ[%s]ÖØ¸´", sAg_peritemmap.item_no );				
				WRITEMSG
				
				goto TrLog;
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"²éÑ¯ag_peritemmap³ö´í[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}
		/**¿ªÊ¼²éÑ¯¿Í»§ºÅÊÇ·ñ´æÔÚ£¬²»´æÔÚÔòÖØĞÂ¿ªÁ¢**/
		
	    /*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
		    fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
		    fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
		    fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
		    fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
		    fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
		    fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
		    fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
		    fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
		    fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
		    fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
		    fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
		    fldstr[15]  ³Ö¿¨ÈËÓÊÏä
		    fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
		    fldstr[17]  ÔÂÊÕÈë
		    fldstr[18]  ³öÉúÈÕÆÚ
		    fldstr[19]  »éÒö×´¿ö
		    fldstr[20]  µ¥Î»±àºÅdepid    ok
		    fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
		    fldstr[21]  ¿ª»§½ğ¶î    ok
		----------------------------------------------*/
		
	    
		strcpy(cif_id_code_rel.id_type, fldstr[2]);/* Ö¤¼şÀàĞÍ */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* Ö¤¼şºÅÂë */
		pub_base_strpack(cif_id_code_rel.id_type);
		pub_base_strpack(cif_id_code_rel.id_no);
		vtcp_log("[%s][%d]Ö¤¼şÀàĞÍ[%s]\n",__FILE__,__LINE__,fldstr[2]);
		/**MOD AT 20130528 ÅúÁ¿¿ª»§Ö¤¼şĞÅÏ¢Ğ£Ñé£¬Éí·İÖ¤±ØĞë18Î»£¬ÆäÓàÖ¤¼ş²»ÄÜ³¬¹ı20Î»**/
		if(memcmp(fldstr[2],"1",1) == 0)
				{
					if(strlen(fldstr[3]) != 18 )
						{
							iErr_flag = 1;
							ret = 1;
							sprintf( cReturn, "µÚ[%d]ĞĞ£¬ÅúÁ¿¿ª¿¨Ö¤¼şºÅÂë±ØĞëÎª18Î»",lCount);
							strcpy( g_pub_tx.reply, "H033" );
							sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬ÅúÁ¿¿ª¿¨Ö¤¼şºÅÂë±ØĞëÎª18Î»",lCount);
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
							sprintf( cReturn, "µÚ[%d]ĞĞ£¬Ö¤¼şºÅÂë³¬¹ıÁË20Î»",lCount);
							strcpy( g_pub_tx.reply, "H033" );
							sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬Ö¤¼şºÅÂë³¬¹ıÁË20Î»",lCount);
							WRITEMSG
							goto TrLog;
						}
			}
		/**MOD AT 20130528 ÅúÁ¿¿ª»§Ö¤¼şĞÅÏ¢Ğ£Ñé£¬Éí·İÖ¤±ØĞë18Î»£¬ÆäÓàÖ¤¼ş²»ÄÜ³¬¹ı20Î»**/
		vtcp_log("[%s][%d]Ö¤¼şºÅÂë[%s]\n",__FILE__,__LINE__,fldstr[3]);
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		strcpy(cif_basic_inf.type,"1");		      /* ¿Í»§ÀàĞÍ */
		strcpy(cif_basic_inf.name,fldstr[4]);	  /* ¿Í»§Ãû³Æ */
		strcpy(cif_basic_inf.local,"1");	/* ±¾µØ±êÖ¾ */
		strcpy(cif_basic_inf.poten,"1");	/* Ç±ÔÚ±êÖ¾1-±¾ĞĞ2-Ç±ÔÚ */
		strcpy(cif_basic_inf.shareholder_ind,"1");/*ÊÇ·ñ±¾ĞĞ¹É¶«2-ÊÇ 1-·ñ */
		strcpy(cif_basic_inf.lvl,"4"); /*¿Í»§¼¶±ğ*/

		/*--------¿ªÊ¼½¨Á¢¿Í»§ĞÅÏ¢----------*/
		cif_no = 0;
		/* ¼ì²é¿Í»§Ö¤¼şÀàĞÍ£«Ö¤¼şºÅÂëÊÇ·ñÒÑ¾­´æÔÚ */
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" id_type='%s' and id_no='%s' " , cif_id_code_rel.id_type ,
					  cif_id_code_rel.id_no );
		if( ret==0 )
		{
			sprintf(acErrMsg,"¸Ã¿Í»§ÒÑ¾­ÓµÓĞ¿Í»§ID![%s][%s]",fldstr[5],fldstr[3]);
			WRITEMSG
			/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
			fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
			fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
			fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
			fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
			fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
			fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
			fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
			fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
			fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
			fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
			fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
			fldstr[15]  ³Ö¿¨ÈËÓÊÏä
			fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
			fldstr[17]  ÔÂÊÕÈë
			fldstr[18]  ³öÉúÈÕÆÚ
			fldstr[19]  »éÒö×´¿ö
			fldstr[20]  µ¥Î»±àºÅdepid    ok
			fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
			fldstr[22]  ¿ª»§½ğ¶î    ok
			----------------------------------------------*/
			/****Ôö¼Ó¶Ô»§ÃûµÄÅĞ¶Ï****/
			ret = Cif_basic_inf_Sel(g_pub_tx.reply, &cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(ret == 0)
			{
				pub_base_strpack(fldstr[4]);
				pub_base_strpack(cif_basic_inf.name);				
				if(strcmp(cif_basic_inf.name,fldstr[4]) != 0)
				{
					/**¿Í»§»§Ãû²»·û*ÌáÊ¾±¨´í**/
					iErr_flag = 1;
					ret = 1;
					sprintf( cReturn, "µÚ[%d]ĞĞ£¬»§Ãû²»·ûºÏ£¬Êı¾İ¿âÖĞÎª¡¾%s¡¿",lCount,cif_basic_inf.name);
					strcpy( g_pub_tx.reply, "H033" );
					sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬»§Ãû²»·ûºÏ£¬Êı¾İ¿âÖĞÎª¡¾%s¡¿",lCount,cif_basic_inf.name);
					WRITEMSG
					goto TrLog;
				}
			}
			else if( ret )
			{
				sprintf(acErrMsg,"È¡¿Í»§ĞÅÏ¢Òì³£![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
			cif_no = cif_id_code_rel.cif_no; /*¶ÔÒÑ´æÔÚµÄ¿Í»§ºÅ¸³Öµ*/
			/***½«ĞèÒªµÄĞÅÏ¢´ÓÊı¾İ¿âÖĞ²éÑ¯³öÀ´**/
			/* µÇ¼Ç¿Í»§»ù±¾ĞÅÏ¢±í */
			ret = Cif_addr_inf_Sel(g_pub_tx.reply,&z_cif_addr_inf,"cif_no=%d",cif_no);
			if( ret && ret!=100)
			{
				sprintf(acErrMsg,"µÇ¼Ç¿Í»§µØÖ·ĞÅÏ¢±í´íÎó![%d]",ret);
				WRITEMSG
				/**
				strcpy(g_pub_tx.reply,"C067");
				goto ErrExit;**/
			}
			ret = Cif_per_inf_Sel(g_pub_tx.reply,&z_cif_per_inf,"cif_no=%d",cif_no);
			if( ret )
			{
				sprintf(acErrMsg,"²éÑ¯¸öÈË¿Í»§ĞÅÏ¢±í![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C008");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf,"cif_no=%d and addr_type='5'",cif_no);
			if( ret )/**ÊÖ»ú**/
			{
				sprintf(acErrMsg,"¸öÈË¿Í»§µç×ÓĞÅÏ¢±í![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C062");
				goto ErrExit;**/
			}
			ret = Cif_email_inf_Sel(g_pub_tx.reply,&z_cif_email_inf1,"cif_no=%d and addr_type='6'",cif_no);
			if( ret )/**ÓÊÏä**/
			{
				sprintf(acErrMsg,"¿Í»§µç×ÓµØÖ·ĞÅÏ¢±í![%d]",ret);
				WRITEMSG
				/**del by hxc 100525
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;**/
			}
			goto OK_cif_no;	/*Ö±½Ó¿ª»§*/
		}else if( ret!=100 )
		{
			sprintf(acErrMsg,"È¡¿Í»§Ö¤¼şĞÅÏ¢Òì³£![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
			goto ErrExit;
		}
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"Éú³É¿Í»§idÊ§°Ü!");
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
			sprintf(acErrMsg,"µÇ¼Ç¿Í»§Ö¤¼şÓë¿Í»§ºÅ¶ÔÕÕ±í´íÎó![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C004");
			goto ErrExit;
		}
	
		/* µÇ¼Ç¿Í»§»ù±¾ĞÅÏ¢±í */
		cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* ½¨Á¢ÈÕÆÚ */
		cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ½¨Á¢Ê±¼ä */
		strcpy( cif_basic_inf.sts , "1" ) ;         /* ¿Í»§×´Ì¬£º1Õı³£ 2×¢Ïú */
		cif_basic_inf.cif_seqn = 1 ;                /* ¿Í»§ĞòºÅ */
		ret = Cif_basic_inf_Ins( cif_basic_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"µÇ¼Ç¿Í»§»ù±¾ĞÅÏ¢±í´íÎó![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		/**µÇ¼Ç¿Í»§µØÖ·ĞÅÏ¢±í**/
		z_cif_addr_inf.cif_no=cif_no;
		z_cif_addr_inf.addr_seqn=1;
		z_cif_addr_inf.addr_type[0]='2';
		memcpy(z_cif_addr_inf.addr,fldstr[11],sizeof(z_cif_addr_inf.addr)-1);
		memcpy(z_cif_addr_inf.post_code,fldstr[8],sizeof(z_cif_addr_inf.post_code)-1);
		ret = Cif_addr_inf_Ins( z_cif_addr_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"µÇ¼Ç¿Í»§µØÖ·ĞÅÏ¢±í´íÎó![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
			goto ErrExit;
		}
		/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
			fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
			fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
			fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
			fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
			fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
			fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
			fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
			fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
			fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
			fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
			fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
			fldstr[15]  ³Ö¿¨ÈËÓÊÏä
			fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
			fldstr[17]  ÔÂÊÕÈë
			fldstr[18]  ³öÉúÈÕÆÚ
			fldstr[19]  »éÒö×´¿ö
			fldstr[20]  µ¥Î»±àºÅdepid    ok
			fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
			fldstr[22]  ¿ª»§½ğ¶î opnamt
		----------------------------------------------*/
		/**µÇ¼Ç¸öÈË¿Í»§ĞÅÏ¢±í**/
		z_cif_per_inf.cif_no=cif_no;
		z_cif_per_inf.g_bir=atol(fldstr[18]);
		memcpy(z_cif_per_inf.sex,fldstr[6],1);
		memcpy(z_cif_per_inf.marriage,fldstr[19],1);/**ĞèÒªºÍÉñÂëÔ¼¶¨¾ßÌåÖµ ´ú±íÊ²Ã´ÒâË¼£¬Èç¹û²»Ò»ÖÂĞèÒª×ª»»**/
		memcpy(z_cif_per_inf.name,fldstr[4],sizeof(z_cif_per_inf.name)-1);
		memcpy(z_cif_per_inf.nationality,"01",sizeof(z_cif_per_inf.nationality)-1);
		memcpy(z_cif_per_inf.country,"01",sizeof(z_cif_per_inf.country)-1);
		z_cif_per_inf.mon_income=atof(fldstr[17]);
		ret = Cif_per_inf_Ins( z_cif_per_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"¸öÈË¿Í»§ĞÅÏ¢±í![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C008");
			goto ErrExit;
		}
		sprintf(acErrMsg,"½¨Á¢¿Í»§ĞÅÏ¢³É¹¦,¿Í»§ºÅ[%ld]",cif_no);/*Éú³É¿Í»§ºÅ*/
		WRITEMSG
		/**¿Í»§µç×ÓµØÖ·ĞÅÏ¢±í**/
		z_cif_email_inf.cif_no=cif_no;
		ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
		z_cif_email_inf.addr_seqn=ret+1;
		z_cif_email_inf.addr_type[0]='5';/**ÊÖ»ú**/
		memcpy(z_cif_email_inf.email,fldstr[14],sizeof(z_cif_email_inf.email)-1);
		ret = Cif_email_inf_Ins( z_cif_email_inf ,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"¸öÈË¿Í»§ĞÅÏ¢±í![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C024");
			goto ErrExit;
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		if(strlen(fldstr[15])){/**µç×ÓÓÊ¼ş**/
			z_cif_email_inf1.cif_no=cif_no;
			ret = sql_max_int("cif_email_inf","addr_seqn","cif_no=%d",cif_no);
			z_cif_email_inf1.addr_seqn=ret+1;
			z_cif_email_inf1.addr_type[0]='6';/**µç×ÓÓÊ¼ş**/
			memcpy(z_cif_email_inf1.email,fldstr[15],sizeof(z_cif_email_inf1.email)-1);
			ret = Cif_email_inf_Ins( z_cif_email_inf1 ,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"¸öÈË¿Í»§ĞÅÏ¢±í![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C024");
				goto ErrExit;
			}
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
OK_cif_no:
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		/**°´ÕÕ¿ª¿¨ÕÅÊı½øĞĞ¼ÆËãÆ¾Ö¤ºÅÂë**/
		vtcp_log("%s,%d iOpcnt=[%d],note_mst_z.cnt=[%d]\n",__FILE__,__LINE__,iOpcnt,note_mst_z.cnt);
		if(iOpcnt>note_mst_z.cnt){/***ĞèÒª»»ÁíÒ»±¾Æ¾Ö¤ÁË**/
			ret=Note_mst_Fet_Sel(&note_mst_z,g_pub_tx.reply);
			if(ret){
				sprintf(acErrMsg,"²éÑ¯¹ñÔ±Æ¾Ö¤Ì¨ÕË±í´íÎó![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			if(ret==100){
				vtcp_log("%s,%d Æ¾Ö¤²»¹»ÁË£¬²»Ó¦¸Ã°¡£¡£¡£¡\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"N038");
				goto ErrExit;
			}
			iOpcnt=iOpcnt-note_mst_z.cnt;/***ÖØĞÂ¼ÆËã**/
		}
		vtcp_log("%s,%d sNote_min_no.note_min_no={%s},iOpcnt={%d}\n",__FILE__,__LINE__,sNote_min_no.note_min_no,iOpcnt);
		vtcp_log("%s,%d note_mst_z.beg_note_no={%s}\n",__FILE__,__LINE__,note_mst_z.beg_note_no);
		ret=pub_com_CalNoteNo("020",note_mst_z.beg_note_no,sNote_min_no.note_min_no,iOpcnt);
		if(ret){
			sprintf(acErrMsg,"²éÑ¯¹ñÔ±Æ¾Ö¤Ì¨ÕË±í´íÎó![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N038");
			goto ErrExit;
		}
		/***¿ªÊ¼±È½Ï¿¨ºÅµÄÆ¾Ö¤ºÅÓë¹ñÔ±µÄÆ¾Ö¤ºÅÊÇ·ñÒ»ÖÂ**/
		vtcp_log("%s,%d ¿¨ºÅ¡¾%s¡¿ÓëÆ¾Ö¤ºÅ[%s]\n",__FILE__,__LINE__,fldstr[0],sNote_min_no.note_min_no+4);
		if(memcmp(fldstr[0]+6,sNote_min_no.note_min_no+4,12)){/*¿¨ºÅ7-18Î»ÎªÆ¾Ö¤ºÅÂë**/
			iErr_flag = 1;
			ret = 1;
			sprintf( cReturn, "µÚ[%d]ĞĞ£¬¿¨ºÅ¡¾%s¡¿ÓëÆ¾Ö¤ºÅ²»·û[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			strcpy( g_pub_tx.reply, "H033" );
			sprintf( acErrMsg, "µÚ[%d]ĞĞ£¬¿¨ºÅ[%s]ÓëÆ¾Ö¤ºÅ²»·û[%s]",lCount,fldstr[0],sNote_min_no.note_min_no);
			WRITEMSG
			goto TrLog;
		}
		vtcp_log("%s,%d ¿´¿´½á¹û°É£¡ret=[%d]\n",__FILE__,__LINE__,ret);
TrLog:	
		vtcp_log("%s,%d ¿´¿´½á¹û°É£¡ret=[%d]\n",__FILE__,__LINE__,ret);
		if ( ret == 0 )
		{
			/***ÉñÂëµÄÒªÇóÉú³ÉÎÄ¼şfp1*
			¿¨ºÅ|¿Í»§ºÅ|³Ö¿¨ÈËÖ¤¼şÖÖÀà|³Ö¿¨ÈËÖ¤¼şºÅÂë|³Ö¿¨ÈË¿Í»§Ãû|
			³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû|³Ö¿¨ÈËĞÔ±ğ|³Ö¿¨ÈË¼ÒÍ¥×¡Ö·|³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à|
			³Ö¿¨ÈË¼ÒÍ¥µç»°|³Ö¿¨ÈËµ¥Î»Ãû³Æ|³Ö¿¨ÈËµ¥Î»µØÖ·|³Ö¿¨ÈËµ¥Î»ÓÊ±à|
			³Ö¿¨ÈËµ¥Î»µç»°|³Ö¿¨ÈËÊÖ»úºÅÂë|³Ö¿¨ÈËÓÊÏä|³Ö¿¨ÈËĞÅÓşµÈ¼¶|ÔÂÊÕÈë|³öÉúÈÕÆÚ|»éÒö×´¿ö***/
			/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
			fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
			fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
			fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
			fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
			fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
			fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
			fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
			fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
			fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
			fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
			fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
			fldstr[15]  ³Ö¿¨ÈËÓÊÏä
			fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
			fldstr[17]  ÔÂÊÕÈë
			fldstr[18]  ³öÉúÈÕÆÚ
			fldstr[19]  »éÒö×´¿ö
			fldstr[20]  µ¥Î»±àºÅdepid    ok
			fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
			fldstr[22]  ¿ª»§½ğ¶îopnamt    ok
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
			/**fprintf( fp2, "    ©¦%10s©¦%10s©¦%20s©¦%20s©¦%44s©¦\n", "´úÀí±àºÅ", "¿Í»§ĞÕÃû", "´ı¿ª¿¨ºÅ", "Ö¤¼şºÅÂë", "´íÎóĞÅÏ¢" );**/
			fprintf( fp2, "    ©¦%10s©¦%10s©¦%20s©¦%20s©¦%44s©¦\n", fldstr[21], fldstr[4], \
			fldstr[0], fldstr[3], cReturn );
		}
		vtcp_log("%s,%d lCount=[%d]\n",__FILE__,__LINE__,lCount);
		lCount = lCount + 1;
		iOpcnt++;
	}
	
	fprintf( fp2, "    ©¹©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©½\n" );

	fprintf(fp2,"\n\n");
	fprintf(fp2,"ÅúÁ¿¿ª»§µ¼ÅÌÊ§°Ü!!!Çë¼ì²éÒÔÉÏ¼ÇÂ¼´íÎó!!!!\n");

	fclose( fp );
	fclose( fp1 );
	fclose( fp2 );
	if ( iErr_flag == 1 )/**ÖÁÉÙ±£Ö¤Â¼Èë»òµ¼ÅÌÊ±ËùÓĞµÄ¼ÇÂ¼¶¼ÄÜ¹ıÈ¥,·ñÔòºÍÉñÂëÄÇ±ß¸üÂé·³**/
	{
		/**Ìí¼ÓÓĞ´íÎóÃ÷Ï¸Ê±ÍùÇ°Ì¨·¢ËÍ´íÎóµÄÎÄ¼ş 20130510**/
		memset( execstr, 0x0, sizeof(execstr) );
		sprintf( execstr, "cp %s %s", filename2, filename );
		ret = system( execstr );
		if ( ret )
		{
			sprintf(acErrMsg,"¸ü¸ÄÏÂ´«ÎÄ¼şÃûÊ§°Ü[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;		
		}
		set_zd_data( DC_FILE_SND, "1" );	
		vtcp_log("%s,%d ÍùÇ°Ì¨´«ËÍ´íÎóÃ÷Ï¸filename=¡¾%s¡¿",__FILE__,__LINE__,filename);
		/**Ìí¼ÓÓĞ´íÎóÃ÷Ï¸Ê±ÍùÇ°Ì¨·¢ËÍ´íÎóµÄÎÄ¼ş 20130510**/
		strcpy( g_pub_tx.reply, "H086" );
		goto ErrExit;
	}
	set_zd_data("0260",cFilename);/**ÓÃÓÚ¸øÆ½Ì¨´«ËÍÎÄ¼ş**/
	/**Ìí¼ÓÒ»¸öshellÓÃÓÚ ºËĞÄ»úÆ÷´«ËÍµ½½ğÈÚ½»»»Æ½Ì¨***/
	vtcp_log("%s,%d cFilename=[%s]\n",__FILE__,__LINE__,cFilename);
        /****ĞŞ¸ÄÎªĞÂµÄ¼ÓÃÜ·½Ê½ 20120604
	sprintf(cFtpfile,"%s/bin/opncardtosw.sh %s",getenv("WORKDIR"),cFilename);
	****/
	sprintf(cFtpfile,"%s/bin/opncardtosw.sh %s %s %s",getenv("WORKDIR"),get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		sprintf(acErrMsg,"ftpÎÄ¼ş[%s]µ½Æ½Ì¨³ö´í[%d]",cFilename,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	sprintf(acErrMsg,"ftpÎÄ¼ş[%s]µ½Æ½Ì¨³É¹¦",cFilename);
	WRITEMSG

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ÅúÁ¿¿ª»§³É¹¦£¡[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    fclose( fp );
    fclose( fp1 );
    fclose( fp2 );
    sprintf(acErrMsg,"ÅúÁ¿¿ª»§Ê§°Ü£¡[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
/**
**×ª»¯Ç°Ì¨µÄÎÄ¼şÓòµÄË³Ğò
**/
int transfer(char *buf,char *fdstr)
{
		int i;
		int ret;
		char cFldstr[30][150];/**ÉñÂëÒªÇóÊÇ20¸öÄÚÈİÔİÊ±¶¨ÒåÎª30£¬×î³¤µÄÎª140£¬ËùÒÔÔİÊ±¶¨Îª150***/
		memset(cFldstr, 0, sizeof(cFldstr));
		vtcp_log("[%s][%d] ´¦ÀíÄÚÈİ¡¾%s¡¿ ", __FILE__, __LINE__,buf);
		for( i=0;i<9;i++ )/*¸Ä³É23 ÉñÂëÒªÇó¼ÓÉÏµ¥Î»±àÂë,¸öÈË´úÀí±àºÅ£¬¿ª»§½ğ¶îºó¾ÍÊÇ23¸ö**/
		{
			ret=pub_base_cut_str( buf,cFldstr[i],"|",i+1 );
			vtcp_log("[%s][%d] [%d] [%s] ", __FILE__, __LINE__, i+1, cFldstr[i]);
			pub_base_strpack(cFldstr[i]);
		}
		vtcp_log("[%s][%d] cFldstr[7]->%s ½áÊø ", __FILE__, __LINE__,cFldstr[7]);
		memcpy(fdstr, cFldstr[7],  strlen(cFldstr[7]));/**¿¨ºÅ**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+2*150, cFldstr[6],  strlen(cFldstr[6]));/**³Ö¿¨ÈËÖ¤¼şÖÖÀà**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+3*150, cFldstr[4],  strlen(cFldstr[4]));/**³Ö¿¨ÈËÖ¤¼şºÅÂë**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+4*150, cFldstr[2],  strlen(cFldstr[2]));/**³Ö¿¨ÈË¿Í»§Ãû**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+14*150, cFldstr[8], strlen(cFldstr[8]));/**³Ö¿¨ÈËÊÖ»úºÅÂë**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+20*150, cFldstr[5], strlen(cFldstr[5]));/**µ¥Î»±àºÅ**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+21*150, cFldstr[0], strlen(cFldstr[0]));/**¸öÈË´úÀí±àºÅitemno**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
		memcpy(fdstr+22*150, cFldstr[3], strlen(cFldstr[3]));/**¿ª»§½ğ¶îopnamt**/
		vtcp_log("[%s][%d] transfer ½áÊø ", __FILE__, __LINE__);
			
		return 0;
}
	/*-	fldstr[0]   ¿¨ºÅ ------------ok------------
			fldstr[1]   ¿Í»§ºÅ           ok
			fldstr[2]   ³Ö¿¨ÈËÖ¤¼şÖÖÀà   ok
			fldstr[3]   ³Ö¿¨ÈËÖ¤¼şºÅÂë   ok
			fldstr[4]   ³Ö¿¨ÈË¿Í»§Ãû     ok
			fldstr[5]   ³Ö¿¨ÈË¿Í»§Ó¢ÎÄÃû 
			fldstr[6]   ³Ö¿¨ÈËĞÔ±ğ       
			fldstr[7]   ³Ö¿¨ÈË¼ÒÍ¥×¡Ö·   ok
			fldstr[8]   ³Ö¿¨ÈË¼ÒÍ¥ÓÊ±à   ok
			fldstr[9]   ³Ö¿¨ÈË¼ÒÍ¥µç»°
			fldstr[10]  ³Ö¿¨ÈËµ¥Î»Ãû³Æ   ok
			fldstr[11]  ³Ö¿¨ÈËµ¥Î»µØÖ·   ok
			fldstr[12]  ³Ö¿¨ÈËµ¥Î»ÓÊ±à   ok
			fldstr[13]  ³Ö¿¨ÈËµ¥Î»µç»°
			fldstr[14]  ³Ö¿¨ÈËÊÖ»úºÅÂë   ok
			fldstr[15]  ³Ö¿¨ÈËÓÊÏä
			fldstr[16]  ³Ö¿¨ÈËĞÅÓşµÈ¼¶
			fldstr[17]  ÔÂÊÕÈë
			fldstr[18]  ³öÉúÈÕÆÚ
			fldstr[19]  »éÒö×´¿ö
			fldstr[20]  µ¥Î»±àºÅdepid    ok
			fldstr[21]  ¸öÈË´úÀí±àºÅitemno    ok
			fldstr[22]  ¿ª»§½ğ¶îopnamt    ok
		----------------------------------------------*/
