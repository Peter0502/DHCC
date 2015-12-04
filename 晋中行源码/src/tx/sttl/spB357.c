/*************************************************
* �� �� ��:  spB357.c
* ���������� ��ɵ����ʺźͽ�ݺŹ�ϵ�Ľ����ͽ��
*
* ��    ��:  Dongxy
* ������ڣ� 2006��9��18��
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "cassreg_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"

int spB357()
{
	struct	cassreg_c 	sCassReg;	/* ������ϵ�Ǽǲ� */
	struct	cassreg_c 	sCassReg1;	/* ������ϵ�Ǽǲ� */
	struct	mdm_ac_rel_c 	sMdmAcRel;	/* �����˻���ϵ�� */
	char	cOperCode[2];			/* ������ */
	int		ret;

	memset( &sCassReg, 0x00, sizeof( struct cassreg_c ) );
	memset( &sCassReg1, 0x00, sizeof( struct cassreg_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0670", cOperCode );		/** �������� **/
	get_zd_data( "0320", g_pub_tx.ac_no );		/** �����ʺ� **/
	get_zd_data( "0330", sCassReg.pact_no );	/** ��ݺ� **/
	get_zd_data( "0330", sCassReg1.pact_no );	/** ��ݺ� **/
	get_zd_double("0400",&sCassReg.amt);		/** ������� **/
	get_zd_data("102H",sCassReg.name);		/** �������� **/
	get_zd_double("102I",&sCassReg.bal);		/** ��������ʱ��� **/
	get_zd_data("0070",sCassReg.tel);		/** ����Ա **/
	get_zd_data("0020",sCassReg.br_no);		/** �������� **/
  get_zd_long("0550",&sCassReg.bw_ac_id);/**�ͻ���**/
	vtcp_log("[%s][%d]========��������=[%s]",__FILE__,__LINE__,cOperCode);

	/*ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", g_pub_tx.ac_no );
        if( ret == 100 )
    	{
                sprintf( acErrMsg, "�޴��ʺ�![%s]", g_pub_tx.ac_no );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "A029" );
                goto ErrExit;
        }else if( ret )
        {
        	sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
        }
*/
	/*sCassReg.ac_id = sMdmAcRel.ac_id;*/
	strcpy(sCassReg.db_ac_no, g_pub_tx.ac_no);

	if(cOperCode[0] == '0')	/** ǰ̨ѡ���� **/
	{

		vtcp_log("[%s][%d]========��ʼ����",__FILE__,__LINE__);
		/** ��ݺŲ����ظ� **/
        	ret = Cassreg_Sel( g_pub_tx.reply, &sCassReg1, " pact_no='%s' ", sCassReg1.pact_no);
        	if( ret == 0 )
        	{
                	sprintf( acErrMsg, "�˽�ݺ��Ѵ���!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "L020" );
                	goto ErrExit;
        	}
        	else if( ret != 100 )
        	{
                	sprintf( acErrMsg, "ִ��Cassreg_Sel����![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}

              /*  ret = pub_base_CompDblVal(sCassReg.amt,sCassReg.bal);
                if ( ret ==0 || ret ==1)
                {
                        sprintf( acErrMsg, "���������ڵ����˻����[%d]", ret );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B353" );
                        goto ErrExit;
                }*/
					get_zd_long("0040",&sCassReg.in_trace_no);	/** ������ˮ **/
					get_zd_long("0050",&sCassReg.in_tx_date);	/** �������� **/
					sCassReg.flag[0] = '0';
	        ret = Cassreg_Ins( sCassReg, g_pub_tx.reply );
        	if ( ret )
        	{
        		sprintf( acErrMsg, "ִ��Cassreg_Ins����!ret=[%d]", ret );
        		WRITEMSG
        		goto ErrExit;
        	}
	}
	else if(cOperCode[0] == '1')	/** ǰ̨ѡ��ɾ�� **/
	{
		int cnt; double samt = 0;
		
		cnt=sql_count_sum_double("ln_mst", "bal", &samt, "pact_no = '%s'", sCassReg.pact_no);
		
		vtcp_log("[%s][%d]========��ʼɾ�� cnt %d (samt %.2lf)",__FILE__,__LINE__, cnt, samt);
		if(cnt>0 && samt>0.005){
        		sprintf( acErrMsg, "����δ����");
        		WRITEMSG
                        sprintf( g_pub_tx.reply, "L409" );
        		goto ErrExit;
		}
		vtcp_log("[%s][%d]========��ʼɾ��",__FILE__,__LINE__);
		ret = Cassreg_Dec_Upd(g_pub_tx.reply, " pact_no = '%s' ",sCassReg.pact_no);
	  if ( ret )
    {
          	sprintf( acErrMsg, "ִ��Cassreg_Dec_Upd��[%d]", ret );
          	WRITEMSG
          	goto ErrExit;
    }
    ret = Cassreg_Fet_Upd( &sCassReg, g_pub_tx.reply );
    if( ret &&ret!=100 )
    {
          	sprintf( acErrMsg, "ִ��Cassreg_Fet_Upd��![%d]", ret );
         		WRITEMSG
         		goto ErrExit;
    }
	/*	if(sCassReg.flag[0] == '1')
		{
                	sprintf( acErrMsg, "�˹�ϵ�Ѿ����" );
                        sprintf( g_pub_tx.reply, "H020" );
               		WRITEMSG
               		goto ErrExit;
		}
	*/
		get_zd_long("0040",&sCassReg.out_trace_no);	/** �����ˮ **/
		get_zd_long("0050",&sCassReg.out_tx_date);	/** ������� **/
		sCassReg.flag[0] = '1';
	/***	
	ret = Cassreg_Upd_Upd( sCassReg, g_pub_tx.reply );
	        if ( ret )
        	{
                	sprintf( acErrMsg, "�޸�Cassreg_Upd_Upd��[%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}
	**/
		ret=Cassreg_Del_Upd(sCassReg,g_pub_tx.reply);
		if(ret && ret!=100)
		{
		      sprintf(acErrMsg,"ɾ��Cassreg_Del_Upd��[%d]",ret);
			  WRITEMSG
			  goto ErrExit;
		}
		Cassreg_Clo_Upd();
		
	}
	else
	{
		sprintf( acErrMsg, "�������ʹ���![%s]",cOperCode );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O020" );
		goto ErrExit;
	}


	vtcp_log("[%s][%d]========��ʼд��ˮ",__FILE__,__LINE__);
	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "������������" );	/* ��ע */

	/* ����ˮ��־ */
	strcpy(g_pub_tx.ac_no,sCassReg.pact_no);
	/*strcpy(g_pub_tx.beg_note_no,sCassReg.pact_no);*/
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");	/**/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]========�������",__FILE__,__LINE__);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
