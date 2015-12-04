/*************************************************
* �� �� ��: spPLKH.c
* ��������: һ����,����spWNJZ.cд
*
* ��    ��: XJ
* �������: 20101007
*
* �޸�����:
* �� �� ��:
* �޸�����:
*
insert into tx_def values ('2508','һ��������','10000000000000000000000010000100000000111110000000000000000000000100000000000000000000000000000000001000000000000000000000000000','1','1','2','3','');
insert into tx_flow_def  values ('2508','0','PLKH','#$');
insert into tx_sub_def  values ('PLKH','һ��������','spPLKH','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include <stdio.h>
#include "prdt_ac_id_c.h"
#include "sub_dd_mst_c.h"
#include "dc_acc_rel_c.h"

spPLKH()
{
	char cInput_acno1[20],cInput_acno2[20];
	int ret=0;
	int iflag1=0,iflag2=0;
	long sub_ac_seqn=0;
	char tmpstr[1024];
	char fldstr[10][100];
	char tmp_sub_tx_code[5];
	char cNote_type_bk[4],cBeg_note_no_bk[17];
	double dTemp_amt = 0.00;
	double dTmp_amt = 0.00;
	double tot_amt = 0.00;
	double tot_amt1 = 0.00;
	long   tmp_svc_ind=0;
	int i=0;
	FILE *fp=NULL;
	char filename[248];
	struct  sub_dd_mst_c     sSub_dd_mst;
	struct  sub_dd_mst_c     gSub_dd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dc_acc_rel_c sDc_acc_rel;
	struct  prdt_ac_id_c    sPrdt_ac_id;

	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&sDc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset(cInput_acno1,0x00,sizeof(cInput_acno1));
	memset(cInput_acno2,0x00,sizeof(cInput_acno2));
	memset(tmpstr, 0x0, sizeof(tmpstr) );
	memset(fldstr,0x0,sizeof(fldstr));
	memset(filename,0x0,sizeof(filename));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_long("0440",&sub_ac_seqn);
	get_zd_double("0400",&g_pub_tx.tx_amt1);
	tot_amt1=g_pub_tx.tx_amt1;
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0580",g_pub_tx.beg_note_no);
	get_zd_data("0580",g_pub_tx.end_note_no);
	get_zd_data("0630",g_pub_tx.brf);
	pub_base_old_acno( g_pub_tx.ac_no );
	strcpy(g_pub_tx.cur_no,"01");
	pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
	/* by Wang Yongwei 20101201 ==13 */
	if(g_pub_tx.ac_no[0]=='9' && strlen(g_pub_tx.ac_no)==13)/* �ڲ��� */
		g_pub_tx.ac_seqn=0;

	set_zd_data("0670","2");
	set_zd_data("0210",g_pub_tx.cur_no);
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/

	memset(cNote_type_bk,0x00,sizeof(cNote_type_bk));
	memset(cBeg_note_no_bk,0x00,sizeof(cBeg_note_no_bk));
	memcpy(cNote_type_bk,g_pub_tx.note_type,sizeof(cNote_type_bk)-1);
	memcpy(cBeg_note_no_bk,g_pub_tx.beg_note_no,sizeof(cBeg_note_no_bk)-1);

	/**�跽����**/

	/* �����������л����⴦�� */
	vtcp_log("sub_ac_seqn is [%ld]",sub_ac_seqn);
	if(sub_ac_seqn>0)
	{
		ret = Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst,"ac_no='%s' and sub_ac_seqn=%ld and ac_sts='1'",g_pub_tx.ac_no,sub_ac_seqn);
		if(ret==100){
			vtcp_log("[%s][%d] ��������ѯ����  ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN08");
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"��ѯsub_ac_no����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}else{
	        vtcp_log("[%s][%d]��Ӧ�����˻���[%s]\n",__FILE__,__LINE__,sSub_dd_mst.ac_no);
		vtcp_log("[%s][%d]��Ӧ�����˻���[%s]\n",__FILE__,__LINE__,sSub_dd_mst.sub_ac_no);
			strncpy(g_pub_tx.ac_no1,sSub_dd_mst.sub_ac_no,strlen(sSub_dd_mst.sub_ac_no));
			tmp_svc_ind=g_pub_tx.svc_ind;
			memset(tmp_sub_tx_code,0,sizeof(tmp_sub_tx_code));
			memcpy(tmp_sub_tx_code,g_pub_tx.sub_tx_code,4);
			memcpy(g_pub_tx.sub_tx_code,"D311",4);

			g_pub_tx.svc_ind=1201;
			strcpy( g_pub_tx.hst_ind,"1" );
			strcpy(g_pub_tx.add_ind,"0");
			memset(g_pub_tx.note_type,0x00,sizeof(g_pub_tx.note_type));
			memset(g_pub_tx.beg_note_no,0x00,sizeof(g_pub_tx.beg_note_no));
			ret=pub_base_sub_ac_trance(sub_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
				WRITEMSG
				goto ErrExit;
			}
			g_pub_tx.svc_ind=tmp_svc_ind;
			vtcp_log("���˻��������[%s][%d],�ָ�ԭ����\n",__FILE__,__LINE__);
			memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			memcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no,sizeof(sSub_dd_mst.ac_no)-1);/* Ϊ���˻�����׼������ */
			g_pub_tx.ac_id=0;
			memcpy(g_pub_tx.note_type,cNote_type_bk,sizeof(cNote_type_bk));
			memcpy(g_pub_tx.beg_note_no,cBeg_note_no_bk,sizeof(cBeg_note_no_bk)-1);
			g_pub_tx.svc_ind=tmp_svc_ind;
			memcpy(g_pub_tx.sub_tx_code,tmp_sub_tx_code,4);
		}
	}
	/* �������л����˻�������� */

		/*** ȡ��Ʒ���룬���ʺ���� ***/
		ret=pub_base_ac_prdt(&sPrdt_ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"ȡ��Ʒ�������!");
			WRITEMSG
		    goto ErrExit;
		}
		strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
		if( sPrdt_ac_id.ac_id_type[0]=='2' )
		{
			sprintf(acErrMsg,"�����ʺŲ������˽���!");
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;
		}

		if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** �Թ����� ***/
		{
			strcpy(g_pub_tx.add_ind,"0"); /*** �跽���� ***/
			g_pub_tx.svc_ind=1001;  				/*���ڴ�ȡ*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** �ڲ��� ***/
		{
			strcpy(g_pub_tx.add_ind,"0");
			g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else
		{
			sprintf(acErrMsg,"�ʺ����ʹ���![%s]",g_pub_tx.ac_id_type);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P410");
			goto ErrExit;
		}
		memcpy(g_pub_tx.prdt_code, sPrdt_ac_id.prdt_no, sizeof(sPrdt_ac_id.prdt_no)-1);

		strcpy(g_pub_tx.sub_tx_code,"PLKH");
		strcpy( g_pub_tx.hst_ind,"1" );
		/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */
		vtcp_log("[%s][%d] ����һ���� �跽���˺ż��� ac_no=%s sub_ac_seqn=%d",__FILE__,__LINE__,g_pub_tx.ac_no,sub_ac_seqn);
		if( pub_acct_trance() )
		{
				sprintf(acErrMsg,"���׼��ʴ������!");
				WRITEMSG
				goto ErrExit;
		}
		set_zd_data("0660","1");
		set_zd_double("1002",0.00);
		set_zd_double("1003",0.00);
		set_zd_double("1004",0.00);
		set_zd_double("100E",0.00);

		strcpy(g_pub_tx.sub_tx_code,"WNJZ"); /* Ϊ�˼�dc_log,��ΪspPLKHû��dc_entry */
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
	/**�������˿�ʼ������ϸ�����ļ��д�����̨**/
	/* ��ʼ�����ļ����� */
	set_zd_data("0660","2");	/* �ļ��ڶ����������� */
	memset(filename,0x0,sizeof(filename));
	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]",filename);
	WRITEMSG
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	strcpy(g_pub_tx.cur_no,"01");
	pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
	while (1)
	{
		/***********ÿ��ѭ�������³�ʼ��ȫ�ֱ���*************/
		pub_base_sysinit();
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
			break;
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/*-------˵������
		fldstr[0] ת���˺�
		fldstr[1] ת���ӻ����
		fldstr[2] ת����
		fldstr[3] ƾ֤����
		fldstr[4] ƾ֤����
		fldstr[5] ժҪ
		--------------------------------------------------*/
		sub_ac_seqn=0;
		pub_base_strpack(fldstr[0]);
		strcpy(g_pub_tx.ac_no,fldstr[0]);
		g_pub_tx.tx_amt1=atof( fldstr[2] );
		strcpy(g_pub_tx.brf,fldstr[5]);
		sub_ac_seqn=atoi(fldstr[1]);
		/* g_pub_tx.sub_ac_seqn=sub_ac_seqn; */
		tot_amt += g_pub_tx.tx_amt1; /* �ۼ��ļ�����Ƿ�����͹����Ľ��һ�� */

		pub_base_strpack(fldstr[3]);
		if(memcmp(fldstr[3],"ת��֧Ʊ",8)==0)
		{
			strcpy(g_pub_tx.note_type,"002");
		}
		else if(memcmp(fldstr[3],"����ƾ֤",8)==0)
		{
			strcpy(g_pub_tx.note_type,"299");
		}
		else
			g_pub_tx.note_type[0]='\0';

		pub_base_strpack(fldstr[4]);
		strcpy(g_pub_tx.beg_note_no,fldstr[4]);
		/* �����������л����⴦�� */
		strcpy(g_pub_tx.cur_no,"01");
		strcpy( g_pub_tx.ct_ind,"2" );
		strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
		pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);

		/*** ȡ��Ʒ���룬���ʺ���� ***/
		pub_base_old_acno(g_pub_tx.ac_no);
		memset(&g_mdm_ac_rel,0,sizeof(g_mdm_ac_rel));
		memset(&sPrdt_ac_id,0,sizeof(sPrdt_ac_id));
		if(g_pub_tx.ac_no[0]=='9')/* �ڲ��� */
			g_pub_tx.ac_seqn=0;
		ret=pub_base_ac_prdt(&sPrdt_ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"ȡ��Ʒ�������!");
			WRITEMSG
		    goto ErrExit;
		}
		/* if(g_mdm_ac_rel.kt_flag[0] =='1' && sub_ac_seqn==0) */
		if(!(g_mdm_ac_rel.draw_pwd_chg,"1") && sub_ac_seqn==0)
		{
			vtcp_log("%s,%d,�ö����˻����Ϊ��,����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN11");
		    goto ErrExit;

		}
		/* if(g_mdm_ac_rel.kt_flag[0] =='0' && sub_ac_seqn!=0) */
		if(!strcmp(g_mdm_ac_rel.draw_pwd_chg,"0") && sub_ac_seqn!=0)
		{
			vtcp_log("%s,%d,�ö����˻����Ϊ��,����!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SN09");
		    goto ErrExit;

		}
		/* if(g_mdm_ac_rel.kt_flag[0] =='1') */
		if(!strcmp(g_mdm_ac_rel.draw_pwd_chg,"1"))
		{
			/* �����ж����˻���ȥ��ѯsub_dd_mst */
				memset(&gSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
				ret = Sub_dd_mst_Sel(g_pub_tx.reply,&gSub_dd_mst,"ac_no='%s' and sub_ac_seqn=%ld and ac_sts='1'",g_pub_tx.ac_no,sub_ac_seqn);
				if(ret==100){
				}else if(ret){
					sprintf(acErrMsg,"��ѯsub_ac_no����!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}else{
					vtcp_log("[%s][%d]��Ӧ�����˻���[%s]\n",__FILE__,__LINE__,gSub_dd_mst.ac_no);
					tmp_svc_ind=g_pub_tx.svc_ind;
					memset(tmp_sub_tx_code,0,sizeof(tmp_sub_tx_code));
					memcpy(tmp_sub_tx_code,g_pub_tx.sub_tx_code,4);
					memcpy(g_pub_tx.sub_tx_code,"D312",4);

					strcpy( g_pub_tx.hst_ind,"1" );
					strcpy(g_pub_tx.add_ind,"1");
					g_pub_tx.svc_ind=1201;
					memset(g_pub_tx.note_type,0x00,sizeof(g_pub_tx.note_type));
					memset(g_pub_tx.beg_note_no,0x00,sizeof(g_pub_tx.beg_note_no));
					ret=pub_base_sub_ac_trance(sub_ac_seqn);
					if( ret )
					{
						sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
						WRITEMSG
						goto ErrExit;
					}
					vtcp_log("���˻��������[%s][%d],�ָ�ԭ����\n",__FILE__,__LINE__);
					memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
					memcpy(g_pub_tx.ac_no,gSub_dd_mst.ac_no,sizeof(gSub_dd_mst.ac_no)-1);/* Ϊ���˻�����׼������ */
					g_pub_tx.ac_id=0;
					g_pub_tx.svc_ind=tmp_svc_ind;
					memcpy(g_pub_tx.sub_tx_code,tmp_sub_tx_code,4);
				}
		}
			/* ��������֧�������˻�������� */

			strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
			if( sPrdt_ac_id.ac_id_type[0]=='2' )
			{
				sprintf(acErrMsg,"�����ʺŲ������˽���!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P403");
				goto ErrExit;
			}

		if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** �Թ����� ***/
		{
			strcpy(g_pub_tx.add_ind,"1"); /*** �������� ***/
			g_pub_tx.svc_ind=1001;  				/*���ڴ�ȡ*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** �ڲ��� ***/
		{
			strcpy(g_pub_tx.add_ind,"1");
			g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else
		{
			sprintf(acErrMsg,"�ʺ����ʹ���![%s]",g_pub_tx.ac_id_type);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P410");
			goto ErrExit;
		}
		memcpy(g_pub_tx.prdt_code, sPrdt_ac_id.prdt_no, sizeof(sPrdt_ac_id.prdt_no)-1);
		vtcp_log("[%s][%d]  yjdd   g_pub_tx.tx_amt1=%.2f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		strcpy(g_pub_tx.sub_tx_code,"PLKH");
		strcpy( g_pub_tx.hst_ind,"1" );

		vtcp_log("[%s][%d] ����һ���� �������˺ż��� ac_no=%s sub_ac_seqn=%d",__FILE__,__LINE__,g_pub_tx.ac_no,sub_ac_seqn);
		if( pub_acct_trance() )
		{
				sprintf(acErrMsg,"���׼��ʴ������!");
				WRITEMSG
				goto ErrExit;
		}
		strcpy(g_pub_tx.sub_tx_code,"WNJZ"); /* Ϊ�˼�dc_log,��ΪspPLKHû��dc_entry */
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
	}
	if(pub_base_CompDblVal(tot_amt,tot_amt1))
	{
		vtcp_log("%s,%d,�ļ��ۼƽ���봫�ͽ�һ��![%.2f][%.2lf]",__FILE__,__LINE__,tot_amt,tot_amt1);
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
	}
	fclose(fp);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"һ�������ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
       if(fp != NULL)    /*add by chengbo 20100903 SN*/
       {
            fclose(fp);
            fp = NULL;
       }
	sprintf(acErrMsg,"һ��������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0 || strlen(g_pub_tx.reply)==0)
	{
		strcpy(g_pub_tx.reply,"H034");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
