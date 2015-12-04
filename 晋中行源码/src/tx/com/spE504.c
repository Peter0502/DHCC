/*************************************************
* 文 件 名:  spE504.c
* 功能描述： 柜员交接交易
* 作    者:  andy
* 完成日期： 2004年3月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "com_tel_connect_c.h"
#include "note_parm_c.h"

int upd_ins_cash(char * ,int );

int spE504()
{
	int	ret;
	int Note_Tag,Cash_Tag;
	char sts[2],tw_sts[2];					/*凭证状态*/ /*对方凭证状态*/
	char mode[2],teller[5];				 	/*操作类型*/ /*对方柜员*/
	char tw_brno[6],tw_tel[5];				/*对方机构*/ /*对方柜员*/
	char cInOut[2];							/*收付标志*/
	char filename[50];
	FILE *fp;
	int  hour,min,sec,num;
	char prtime[9];
  long sumlong=0;
	struct	com_tel_c			g_com_tel;
	struct	com_tel_connect_c	g_com_tel_connect;
	struct	cash_mst_c			g_cash_mst;
	struct  cash_mst_c      t_cash_mst;
	struct	note_mst_c			g_note_mst;
	struct	note_mst_c			f_note_mst;
	struct	note_mst_hst_c		g_note_mst_hst;
	struct	note_mst_c			p_note_mst;
	struct	note_parm_c			p_note_parm;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));
	memset(&g_cash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&g_note_mst,0x00,sizeof(struct note_mst_c));
	memset(&f_note_mst,0x00,sizeof(struct note_mst_c));
	memset(&g_note_mst_hst,0x00,sizeof(struct note_mst_hst_c));
	memset(&p_note_mst,0x00,sizeof(struct note_mst_c));

	/*取前台值*/
	get_zd_data("0670",mode);							/* 交接类型 */
	get_zd_data("0920",teller);							/* 对方柜员 */

	/* 初始化结构 */
	pub_base_sysinit();
	strcpy( g_pub_tx.brf, "柜员交接" );
	
	strcpy(sts,"4");									/* 2表示付出 修改为 4 凭证转移转出 modify by martin 2009/4/23 11:07:09*/

	switch(mode[0])
	{
		case '1':										/*付出*/
				strcpy( g_pub_tx.brf,"交接付出" );
				strcpy( cInOut, "O" );					/*O-代表转出凭证*/
				pub_base_strpack(g_pub_tx.tel);


				/****add by wanglei 20070502 根据晋中要求，交接时，两个柜员的状态必须都是签退状态****/
				ret =Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel = '%s' ",teller);
				if(ret != 0)
				{
					sprintf(acErrMsg,"查询柜员信息表错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O065");
					goto ErrExit;
				}
				
				if(g_com_tel.csts[0] != '2')
				{
					sprintf(acErrMsg,"该业务只能在柜员签退后做!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P094");
					goto ErrExit;
				}
				/****end 20070502****/
				/**********************凭证处理************************/

				/* 对方柜员不能为自己 */
				if( !strcmp(g_pub_tx.tel,teller) )
				{
					sprintf( acErrMsg, "对方柜员必须为其他柜员!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O058" );
					goto ErrExit;
				}

				/* 判断对方柜员和交易柜员是否属同一机构（网点）*/
				ret = pub_base_GetTelInfo( teller, &g_com_tel );
				if( ret )
				{
				 	sprintf( acErrMsg, "查找收入方柜员文件错,ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				if( strcmp( g_pub_tx.tx_br_no, g_com_tel.br_no ) )
				{
					sprintf( acErrMsg, "对方柜员不属于本网点[%s][%s]", \
							 g_pub_tx.tx_br_no,g_com_tel.br_no );
					WRITEMSG
					sprintf( g_pub_tx.reply, "S041" );
					goto ErrExit;
			 	}

				ret = Cash_mst_Sel( g_pub_tx.reply,&g_cash_mst,"tel='%s'", \
									teller);
				if( ret )
				{
					sprintf( acErrMsg, "对方柜员无钱箱，不能交接!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O231" );
					goto ErrExit;
				}/***add by ligl 2006-10-11 13:48**/
        if(g_cash_mst.bal>0.001)
        {
        	sprintf( acErrMsg, "对方柜员钱箱不为零，不能交接!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O121" );
					goto ErrExit;
        }
        ret=sql_sum_long("note_mst", "cnt",&sumlong,"tel='%s' and sts='0'",teller);
        if(ret)
        {
        	sprintf(acErrMsg,"查询柜员凭证台帐错误!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"N038");
        	goto ErrExit;
        }
        if(sumlong>0)
        {
        	sprintf(acErrMsg,"该柜员还有凭证,不允许交接!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"T052");
        	goto ErrExit;
        }
        /**end**/
				strcpy(tw_brno,g_com_tel.br_no);		/*** 对方机构 ***/
				strcpy(tw_tel,teller);					/*** 对方柜员 ***/

				/*判断柜员之前是否存在付出*/
				ret = Com_tel_connect_Sel( g_pub_tx.reply,&g_com_tel_connect,
										   "tel='%s' and ind='0'",g_pub_tx.tel);
				if( ret&&ret!=100 )
				{
					sprintf( acErrMsg, "查询柜员交接登记薄出错!!" );
					WRITEMSG
					goto ErrExit;
				}
				else if( !ret )
				{
					sprintf( acErrMsg, "该柜员已付出,请作收入操作!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O057" );
					goto ErrExit;
				}
				/*判断柜员之前是否存在未收记录,若存在不允许付出  add by wanglei 20070502*/
				ret = Com_tel_connect_Sel( g_pub_tx.reply,&g_com_tel_connect,
										   "tw_tel='%s' and ind='0'",g_pub_tx.tel);
				if( ret==100 )
				{
				}
				else if( ret==0 )
				{
					sprintf( acErrMsg, "柜员交接登记簿中有需要收入的记录,不允许付出!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "P092" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "查询柜员交接登记薄出错!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O056" );
					goto ErrExit;
				}
				/****end by wanglei ****/

				/*查询柜员拥有的可用凭证*/

				sprintf(acErrMsg,"tel='%s' and sts='0'", g_pub_tx.tel);
				WRITEMSG
				ret = Note_mst_Dec_Sel1( g_pub_tx.reply,"tel='%s' and sts='0'",
										g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "查询柜员凭证台帐出错!" );
					WRITEMSG
					goto ErrExit;
				}

				while(1)
				{
					ret = Note_mst_Fet_Sel1(&g_note_mst,g_pub_tx.reply);
					if( ret==100 ) break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetch凭证台帐出错!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

					sprintf( acErrMsg, "/////////////////凭证种类为====[%s]!!",
							g_note_mst.note_type);
					WRITEMSG
TRACE
				 	/* 处理凭证----付出凭证(调出代收)操作 */
				 	ret = pub_com_ChkNote(sts,g_note_mst);
					if( ret )
				 	{
						sprintf( acErrMsg, "付出凭证---处理凭证出错!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

				 	/* 登记转出柜员凭证台帐（分段处理） */
				 	ret = pub_com_NoteApart( sts,g_note_mst );
				 	if( ret )
				 	{
					 	sprintf(acErrMsg,"登记转出柜员凭证台帐（分段处理）错");
					 	WRITEMSG
					 	goto ErrExit;
				 	}

					/* 登记柜员凭证明细 */
					ret = pub_com_RegNoteHst( sts,g_note_mst,tw_brno,tw_tel );
					if( ret==-1 )
					{
						sprintf( acErrMsg, "登记柜员凭证明细错" );
						WRITEMSG
						goto ErrExit;
					}

					/* 登记凭证最小号 */
					ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, \
							 g_note_mst.note_type, g_note_mst.beg_note_no, \
							 g_pub_tx.tel, cInOut );
					if( ret )
					{
						sprintf( acErrMsg, "登记凭证最小号错" );
						WRITEMSG
						goto ErrExit;
					}

					Note_Tag++;

			 	}/*End of while*/

				Note_mst_Clo_Sel1();

				/**********************现金处理************************/
				/*****
				ret = Cash_mst_Dec_Sel(g_pub_tx.reply,"tel='%s' and bal>0.00", \
										g_pub_tx.tel);
				*********/
				ret = Cash_mst_Dec_Sel(g_pub_tx.reply,"tel='%s'and sts='0' ", \
										g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "查询柜员钱箱出错!!" );
					WRITEMSG
					goto ErrExit;
				}

				Cash_Tag = 0;
				while(1)
				{
					ret = Cash_mst_Fet_Sel(&g_cash_mst,g_pub_tx.reply);
					if( ret==100 && Cash_Tag == 0 )
					{
						sprintf( acErrMsg, "无此柜员钱箱信息" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "M035" );
					}else if( ret == 100 && Cash_Tag > 0 )
					{
						break;
					}else if( ret )
					{
						sprintf( acErrMsg, "查询柜员钱箱出错!!" );
						WRITEMSG
						goto ErrExit;
					}
vtcp_log( "zy1   cur_no[%s]", g_cash_mst.cur_no );
					strcpy(g_pub_tx.cur_no,g_cash_mst.cur_no);	/*币种*/
					strcpy(g_pub_tx.note_type,"");				/*凭证种类*/
					strcpy(g_pub_tx.beg_note_no,"");   			/*凭证号码*/
					g_pub_tx.tx_amt1=g_cash_mst.bal;			/*钱箱余额*/
					strcpy(g_pub_tx.cash_code,"");				/*现金分析号*/
					strcpy(g_pub_tx.brf,"");					/*摘要*/
					strcpy(g_pub_tx.add_ind,"0");				/*现金支出*/
vtcp_log( "zy2   cur_no[%s]", g_pub_tx.cur_no );
vtcp_log("%d@%s __________ trace_no = %ld and trace_cnt = %ld and br_no='%s' and tel='%s' ",__LINE__, __FILE__, g_pub_tx.trace_no, g_pub_tx.trace_cnt, g_pub_tx.tx_br_no, g_pub_tx.tel);

					ret = pub_acct_cash( );
					if( ret )
					{
						sprintf(acErrMsg,"调用现金收付主控失败!");
				    	WRITEMSG
						goto ErrExit;
					}
vtcp_log("%d@%s __________ trace_no = %ld and trace_cnt = %ld and br_no='%s' and tel='%s' ",__LINE__, __FILE__, g_pub_tx.trace_no, g_pub_tx.trace_cnt, g_pub_tx.tx_br_no, g_pub_tx.tel);

					ret = upd_ins_cash("1",g_pub_tx.trace_no );
					if( ret )
					{
						sprintf(acErrMsg,"调用券别控制修改失败!");
				    	WRITEMSG
						goto ErrExit;
					}

					strcpy(g_com_tel_connect.cur_no,g_cash_mst.cur_no);/*币种*/
					strcpy(g_com_tel_connect.tel,g_pub_tx.tel);		/*柜员号*/
					strcpy(g_com_tel_connect.tw_tel,teller);		/*对方柜员*/
					g_com_tel_connect.amt=g_cash_mst.bal;			/*交接金额*/
					g_com_tel_connect.tx_date=g_pub_tx.tx_date;		/*交接日期*/
					g_com_tel_connect.trace_no=g_pub_tx.trace_no;	/*流水号*/
					strcpy(g_com_tel_connect.br_no,g_pub_tx.tx_br_no);/*网点号*/
					strcpy(g_com_tel_connect.ind,"0");	  /*获取标志-0表示未取*/

					/*登记柜员交接登记薄*/	
					ret=Com_tel_connect_Ins(g_com_tel_connect,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"登记柜员交接登记薄失败!ret=[%d]",ret);
				    	WRITEMSG
				  		sprintf( g_pub_tx.reply, "O059" );
						goto ErrExit;
					}

					Cash_Tag++;

				}/*End of while*/
				Cash_mst_Clo_Sel();

				if( !Cash_Tag && !Note_Tag )
				{
					sprintf(acErrMsg," 柜员没有可付出的凭证和现金 " );
					WRITEMSG
					strcpy( g_pub_tx.reply,"D228" );
					goto ErrExit;
				}

				/*add by wangjing 打印柜员交接单 begin*/
				pub_base_AllDwnFilName( filename );
				fp = fopen( filename,"w" );
				if( fp==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",filename );
					WRITEMSG
					strcpy( g_pub_tx.reply,"S047" );
					goto ErrExit;
				}
 
				/*打印时间 钱箱金额（人民币）*/
				pub_base_GetTime2( &hour, &min, &sec );
				memset( prtime, 0x00, sizeof( prtime ) );
				sprintf( prtime, "%02d:%02d:%02d", hour, min, sec );    /*打印时间*/
				memset( &g_com_tel_connect, 0x00, sizeof( struct com_tel_connect_c ) );
				ret = Com_tel_connect_Sel( g_pub_tx.reply, &g_com_tel_connect, 
					"ind='0' and br_no='%s' and tel='%s' and tw_tel='%s' and cur_no='01' and tx_date=%ld and trace_no=%ld", 
					g_pub_tx.tx_br_no,g_pub_tx.tel,teller,
					g_pub_tx.tx_date,g_pub_tx.trace_no );
				if( ret )
				{
					vtcp_log( "XXXX ind='0' and br_no='%s' and tel='%s' and tw_tel='%s' and cur_no='01' and tx_date=%ld and trace_no=%ld", 
					g_pub_tx.tx_br_no,g_pub_tx.tel,teller,
					g_pub_tx.tx_date,g_pub_tx.trace_no );
					sprintf( acErrMsg, "打印 取柜员交接登记薄信息错误!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O056" );
					goto ErrExit;
				}
				fprintf( fp, "				       [%s]柜员交接单[金额]	\n",g_pub_tx.tel);
				fprintf( fp, "┌─────┬────────┬────┐\n" );
				fprintf( fp, "│时     间 │    钱箱 金额   │移交柜员│\n" );
				fprintf( fp, "├─────┼────────┼────┤\n" );
				fprintf( fp, "│%10s│%16.2lf│%8s│\n", prtime, g_com_tel_connect.amt,teller);
				fprintf( fp, "└─────┴────────┴────┘ \n\n\n\n\n" );
				/*凭证*/
				ret = Note_mst_Dec_Sel2( g_pub_tx.reply, "tel='%s' and sts='2' order by note_type", g_pub_tx.tel );
				if( ret )
				{
					sprintf( acErrMsg, "查询柜员凭证台帐出错!!" );
					WRITEMSG
					goto ErrExit;
				}
				fprintf( fp, "				       [%s]柜员交接单[凭证]	\n",g_pub_tx.tel);
				fprintf( fp, "┌─────────┬────────┬────────┬────┬────┐\n" );
				fprintf( fp, "│  凭  证  种  类  │    起始号码    │    终止号码    │  数量  │移交柜员│\n" );	
				while(1)
				{
					ret=Note_mst_Fet_Sel2(&p_note_mst,g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetch凭证台帐出错!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

					memset(&p_note_parm,0x00,sizeof(struct note_parm_c));
					ret = Note_parm_Sel( g_pub_tx.reply, &p_note_parm, 
						"note_type='%s'", p_note_mst.note_type );
					if( ret )
					{
						sprintf( acErrMsg, "Fetch凭证参数表出错!!" );
					 	WRITEMSG
					 	goto ErrExit;
					}
					fprintf( fp, "├─────────┼────────┼────────┼────┼────┤\n" );
					fprintf( fp, "│%18s│%16s│%16s│%8d│%8s│\n", p_note_parm.name, p_note_mst.beg_note_no, p_note_mst.end_note_no, p_note_mst.cnt,teller );
				}/*while结束*/
					fprintf( fp, "└─────────┴────────┴────────┴────┴────┘ \n\n\n\n\n" );
				Note_mst_Clo_Sel2();

				fclose(fp);
				set_zd_data( DC_FILE_SND,"1" );
				/*add by wangjing 打印柜员交接单 end*/

				break;
		case '2':										/*收入*/
				strcpy( g_pub_tx.brf,"交接收入" );
				strcpy( cInOut, "I" );					/*I-代表转入凭证*/

				/****add by wanglei 20070502 根据晋中要求，交接时，两个柜员的状态必须都是签退状态****/
				ret =Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel = '%s' ",teller);
				if(ret != 0)
				{
					sprintf(acErrMsg,"查询柜员信息表错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O065");
					goto ErrExit;
				}
				
				if(g_com_tel.csts[0] != '2')
				{
					sprintf(acErrMsg,"该业务只能在柜员签退后做!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P094");
					goto ErrExit;
				}
				/****end 20070502****/

				/*对方柜员不能是自己*/
				if( !strcmp(g_pub_tx.tel,teller) )
				{
					sprintf( acErrMsg, "只能收入其他柜员!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O055" );
					goto ErrExit;
				}
        /**检查收入柜员的钱箱和凭证 add by ligl 2006-10-11 9:21**/
        ret=Cash_mst_Sel(g_pub_tx.reply,&t_cash_mst,"tel='%s' and cur_no='01'",g_pub_tx.tel);
        if(ret)
        {
        	sprintf(acErrMsg,"查询钱箱错误!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"P153");
        	goto ErrExit;
        }
        if(t_cash_mst.bal>0)
        {
        	sprintf(acErrMsg,"接收柜员钱箱还有钱,不允许交接!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"O121");
        	goto ErrExit;
        }
        ret=sql_sum_long("note_mst", "cnt",&sumlong,"tel='%s' and sts='0'",g_pub_tx.tel);
        if(ret)
        {
        	sprintf(acErrMsg,"查询柜员凭证台帐错误!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"N038");
        	goto ErrExit;
        }
        if(sumlong>0)
        {
        	sprintf(acErrMsg,"该柜员还有凭证,不允许交接!!");
        	WRITEMSG
        	sprintf(g_pub_tx.reply,"T052");
        	goto ErrExit;
        }
        /****检查完毕***/
				/*检查交接登记薄是否有对方柜员的付出记录*/
				ret = Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
					"tel='%s' and tw_tel='%s' and ind='0'",teller,g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "对方柜员未做过付出!!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O054" );
					goto ErrExit;
				}

TRACE
				/*查询对方柜员拥有的可用凭证*/
				ret =Note_mst_Dec_Sel1(g_pub_tx.reply,"tel='%s' and sts='2'", \
										teller);
				if( ret )
				{
					sprintf( acErrMsg, "查询柜员凭证台帐出错!!" );
					WRITEMSG
					goto ErrExit;
				}

				while(1)
				{
					ret=Note_mst_Fet_Sel1(&g_note_mst,g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetch凭证台帐出错!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

			 		/* 查找柜员凭证明细 */
			 		/*根据交易日.凭证种类.起始凭证.终止凭证.对方柜员.凭证行为*/
					strcpy(sts,"4"); /* 2表示付出 修改为 4 凭证转移转出 modify by martin 2009/4/23 11:07:09*/
				 	ret = pub_com_GetNoteHst_this( g_com_tel_connect.trace_no, \
						g_pub_tx.tx_date, g_note_mst.note_type, \
						g_note_mst.beg_note_no, g_note_mst.end_note_no, \
						g_pub_tx.tx_br_no, g_pub_tx.tel, sts, &g_note_mst_hst );
				 	if( ret )
				 	{
						sprintf( acErrMsg, "查询柜员凭证明细错 \
								 [%s][%s][%s][%s][%ld]",
								 g_note_mst.note_type, g_note_mst.beg_note_no, \
								 g_note_mst.end_note_no,sts,g_pub_tx.tx_date);
					 	WRITEMSG
					 	goto ErrExit;
				 	}

					strcpy( tw_brno, g_note_mst_hst.br_no );
					strcpy( tw_tel, g_note_mst_hst.tel );

				  	/* 检查柜员是否是本机构的柜员 */
				  	if( strcmp( g_pub_tx.tx_br_no, g_note_mst_hst.br_no ) )
				  	{
					  	sprintf( acErrMsg, "对方柜员不属于本网点[%s][%s]", \
								g_pub_tx.tx_br_no,g_note_mst_hst.br_no );
					  	WRITEMSG
					  	sprintf( g_pub_tx.reply, "S041" );
					  	goto ErrExit;
				 	 }
					 

				  	/* 检查凭证存在，取出状态 */
				 	strcpy(f_note_mst.tel,teller);
				  	strcpy(f_note_mst.br_no,g_note_mst_hst.br_no);
					/*
				 	strcpy(f_note_mst.note_type,g_note_mst_hst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst_hst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst_hst.end_note_no);
					*/
				 	strcpy(f_note_mst.note_type,g_note_mst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst.end_note_no);

					strcpy(sts,"5");  /* 3表示收入 修改为 5 凭证转移转入modify by martin 62009/4/23 11:08:17*/	

				  	/* 检查对方操作员是否拥有这批凭证 */
				  	ret = pub_com_ChkNote( sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "检查柜员凭证错" );
				  	  	WRITEMSG
					  	goto ErrExit;
				 	 }

					/* 登记柜员凭证台帐（并段处理） */
					ret = pub_com_RegTelNote(g_pub_tx.tel, \
									g_pub_tx.tx_br_no,\
									sts, g_note_mst.note_type, \
				  					g_note_mst.beg_note_no,	\
									g_note_mst.end_note_no );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "登记柜员凭证台帐（并段处理）错" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

				 	/* 删除/修改付出柜员的凭证 */
					strcpy(tw_sts,"4"); /* 2表示付出 修改为 4 凭证转移转出 modify by martin 2009/4/23 11:07:09*/
				  	ret = pub_com_NoteDel( tw_sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "删除/修改下发柜员的凭证错" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

					/* 登记柜员凭证明细 */
					ret = pub_com_RegNoteHst( sts,g_note_mst,tw_brno,tw_tel );
					if( ret==-1 )
					{
						sprintf( acErrMsg, "登记柜员凭证明细错" );
						WRITEMSG
						goto ErrExit;
					}

				}/*while结束*/

				Note_mst_Clo_Sel1 ();

				/**********************现金处理************************/
				/*从柜员交接登记薄取出信息*/
				ret = Com_tel_connect_Dec_Upd(g_pub_tx.reply, \
									"tel='%s' and tw_tel='%s' and ind='0'", \
									teller,g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "取柜员交接登记薄信息错误!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O056" );
					goto ErrExit;
				}

				while(1)
				{
					ret=Com_tel_connect_Fet_Upd(&g_com_tel_connect, \
												g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
					{
						sprintf( acErrMsg, "取柜员交接登记薄信息错误!!" );
						WRITEMSG
						sprintf( g_pub_tx.reply, "O056" );
						goto ErrExit;
					}

					strcpy(g_pub_tx.cur_no,g_com_tel_connect.cur_no);/*币种*/
					strcpy(g_pub_tx.note_type,"");				/*凭证种类*/
					strcpy(g_pub_tx.beg_note_no,"");   			/*凭证号码*/
					g_pub_tx.tx_amt1=g_com_tel_connect.amt;		/*收入金额*/
					strcpy(g_pub_tx.cash_code,"");				/*现金分析号*/
					strcpy(g_pub_tx.brf,"");					/*摘要*/
					strcpy(g_pub_tx.add_ind,"1");				/*现金收入*/

					ret = pub_acct_cash();
					if( ret )
					{
						sprintf(acErrMsg,"调用现金收付主控失败!");
				    	WRITEMSG
						goto ErrExit;
					}

					/*修改交接登记薄记录状态为已取*/
					strcpy(g_com_tel_connect.ind,"1");			/*已取状态*/

					ret=Com_tel_connect_Upd_Upd(g_com_tel_connect, \
												g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"更改柜员交接登记薄状态失败!");
				    	WRITEMSG
						goto ErrExit;
					}
				
				}/*End of while*/

				Com_tel_connect_Clo_Upd( );

				break;
		case '3':										/*收回*/
				strcpy( g_pub_tx.brf,"交接收回" );
				strcpy( cInOut, "I" );

				/* 收回时对方柜员*/
				if( !strcmp(g_pub_tx.tel,teller) )
				{
					sprintf( acErrMsg, "收回时柜员号应为当前柜员!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "O053" );
					goto ErrExit;
				}

				/*检查交接登记薄是否有自己付出的记录*/
				ret = Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
										 "tel='%s' and tw_tel='%s' and ind='0'",
										 g_pub_tx.tel,teller);
				if( ret )
				{
					sprintf( acErrMsg, "对方柜员未做过付出!!" );
					WRITEMSG
				  	sprintf( g_pub_tx.reply, "O054" );
					goto ErrExit;
				}

				/*查询柜员拥有的可用凭证*/
				ret =Note_mst_Dec_Sel1(g_pub_tx.reply,"tel='%s' and sts='2'", \
									    g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "查询柜员凭证台帐出错!!" );
					WRITEMSG
					goto ErrExit;
				}

				while(1)
				{
					ret=Note_mst_Fet_Sel1(&g_note_mst,g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
				 	{
						sprintf( acErrMsg, "Fetch凭证台帐出错!!" );
					 	WRITEMSG
					 	goto ErrExit;
			 	 	}

					sts[0] = '4';

			 		/* 查找柜员凭证明细 */
					/*根据交易日.凭证种类.起始凭证.终止凭证.对方柜员.凭证行为*/
				 	ret = pub_com_GetNoteHst_this( g_com_tel_connect.trace_no, \
						g_pub_tx.tx_date, g_note_mst.note_type, \
						g_note_mst.beg_note_no, g_note_mst.end_note_no, \
						g_com_tel_connect.br_no, g_com_tel_connect.tw_tel, \
						sts, &g_note_mst_hst );
				 	if( ret )
				 	{
						sprintf( acErrMsg, "查询柜员凭证明细错" );
					 	WRITEMSG
					 	goto ErrExit;
				 	}

					strcpy( tw_brno, g_note_mst_hst.br_no );
					strcpy( tw_tel, g_note_mst_hst.tel );

				  	/* 检查柜员是否是本机构的柜员 */
				  	if( strcmp( g_pub_tx.tx_br_no, g_note_mst_hst.br_no ) )
				  	{
					  	sprintf( acErrMsg, "对方柜员不属于本网点" );
					  	WRITEMSG
					  	sprintf( g_pub_tx.reply, "S041" );
					  	goto ErrExit;
				 	}

				  	/* 检查凭证存在，取出状态 */
				 	strcpy(f_note_mst.tel,g_pub_tx.tel);
				  	strcpy(f_note_mst.br_no,g_note_mst_hst.br_no);
					/*
				 	strcpy(f_note_mst.note_type,g_note_mst_hst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst_hst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst_hst.end_note_no);
					*/
				 	strcpy(f_note_mst.note_type,g_note_mst.note_type);
				  	strcpy(f_note_mst.beg_note_no,g_note_mst.beg_note_no);
				  	strcpy(f_note_mst.end_note_no,g_note_mst.end_note_no);

					strcpy(sts,"5"); /* 3表示收入 修改为 5 凭证转移转入modify by martin 62009/4/23 11:08:17*/	

				  	/* 检查对方操作员是否拥有这批凭证 */
				  	ret = pub_com_ChkNote( sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "检查柜员凭证错" );
				  	  	WRITEMSG
					  	goto ErrExit;
				 	 }

					/* 登记柜员凭证台帐（并段处理） */
					ret = pub_com_RegTelNote(g_pub_tx.tel, \
									g_pub_tx.tx_br_no,\
									sts, g_note_mst.note_type, \
				  					g_note_mst.beg_note_no,	\
									g_note_mst.end_note_no );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "登记柜员凭证台帐（并段处理）错" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

				 	/* 删除/修改付出柜员的凭证 */
					strcpy(tw_sts,"4"); /* 2表示付出 修改为 4 凭证转移转出 modify by martin 2009/4/23 11:07:09*/
				  	ret = pub_com_NoteDel( tw_sts, f_note_mst );
				  	if( ret )
				  	{
					  	sprintf( acErrMsg, "删除/修改下发柜员的凭证错" );
					  	WRITEMSG
					  	goto ErrExit;
				  	}

					/* 登记柜员凭证明细 */
					ret = pub_com_RegNoteHst( sts,g_note_mst,tw_brno,tw_tel );
					if( ret==-1 )
					{
						sprintf( acErrMsg, "登记柜员凭证明细错" );
						WRITEMSG
						goto ErrExit;
					}

				}/*while结束*/

				Note_mst_Clo_Sel1 ();

				/**********************现金处理************************/
				/* 从柜员交接登记薄取出信息 */
				ret = Com_tel_connect_Dec_Upd(g_pub_tx.reply, \
						"tel='%s' and ind='0'",g_pub_tx.tel);
				if( ret )
				{
					sprintf( acErrMsg, "取柜员交接登记薄信息错误!!" );
					WRITEMSG
					sprintf( g_pub_tx.reply, "S041" );
					goto ErrExit;
				}

				while(1)
				{
					ret=Com_tel_connect_Fet_Upd(&g_com_tel_connect, \
												g_pub_tx.reply);
					if( ret==100 )break;
					else if( ret )
					{
						sprintf( acErrMsg, "取柜员交接登记薄信息错误!!" );
						WRITEMSG
						sprintf( g_pub_tx.reply, "S041" );
						goto ErrExit;
					}

					strcpy(g_pub_tx.cur_no,g_com_tel_connect.cur_no);/*币种*/
					strcpy(g_pub_tx.note_type,"");				/*凭证种类*/
					strcpy(g_pub_tx.beg_note_no,"");   			/*凭证号码*/
					g_pub_tx.tx_amt1=g_com_tel_connect.amt;		/*收入金额*/
					strcpy(g_pub_tx.cash_code,"");				/*现金分析号*/
					strcpy(g_pub_tx.brf,"");					/*摘要*/
					strcpy(g_pub_tx.add_ind,"1");				/*现金收入*/

					ret = pub_acct_cash( );
					if( ret )
					{
						sprintf(acErrMsg,"调用现金收付主控失败!");
				    	WRITEMSG
						goto ErrExit;
					}

					ret = upd_ins_cash("3",g_com_tel_connect.trace_no );
					if( ret )
					{
						sprintf(acErrMsg,"调用券别控制修改失败!");
				    	WRITEMSG
						goto ErrExit;
					}

					/*修改交接登记薄记录状态为已取*/
					strcpy(g_com_tel_connect.ind,"1");			/*已取状态*/

					ret = Com_tel_connect_Upd_Upd( g_com_tel_connect, \
													g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"更改柜员交接登记薄状态失败!");
				    	WRITEMSG
						goto ErrExit;
					}
				
				}/*End of while*/

				Com_tel_connect_Clo_Upd ();
				break;
		default :
				 sprintf( acErrMsg, "凭证操作码不存在,操作码===[%s]",mode );
				 WRITEMSG
				 goto ErrExit;
	}/*switch结束*/

	/* 写交易日志 *
	ret = pub_ins_trace_log();
	if( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
	 	WRITEMSG
		goto ErrExit;
	}
	**/

	strcpy( g_pub_tx.brf, "柜员交接" );
	switch(mode[0])
	{
		case '1':										/*付出*/
			set_zd_data("0680","2");
			set_zd_data("0690","1");
			set_zd_double("0430",g_pub_tx.tx_amt1);
			strcpy(g_pub_tx.add_ind,"1");				/* 减标志 */
			break;
		case '2':										/*收入*/
			set_zd_data("0680","1");
			set_zd_data("0690","2");
			strcpy(g_pub_tx.add_ind,"0");				/* 减标志 */
			set_zd_double("0430",g_pub_tx.tx_amt1);
			break;
		case '3':										/*收回*/
			set_zd_data("0680","1");
			set_zd_data("0690","2");
			strcpy(g_pub_tx.add_ind,"0");				/* 减标志 */
			set_zd_double("0430",g_pub_tx.tx_amt1);
			break;
	}
	strcpy( g_pub_tx.cur_no, "01" );	/* 币种 */
	/*strcpy( g_pub_tx.ac_no, "10104" );	* 在途人民币现金 不走帐了*/
	strcpy( g_pub_tx.ct_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.hst_ind, "1" );	/* 现转 */
	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
int pub_com_GetNoteHst_this(
	long	trace_no,
	long	tx_date,
	char	*note_type,
	char	*beg_note_no,
	char	*end_note_no,
	char	*tw_br_no,
	char	*tw_tel,
	char	*note_act,
	struct	note_mst_hst_c	*note_mst_hst)
{
	int	icount = 0;
	int	iret;

	iret = Note_mst_hst_Dec_Sel( g_pub_tx.reply , 
		"note_type = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld", 
			note_type, tx_date, tw_br_no, 
			tw_tel, note_act, trace_no);
	
	if ( iret )
	{
		sprintf( acErrMsg, "执行	Note_mst_hst_Dec_Sel错，iret=[%d]", iret );
		WRITEMSG
		return(-1);
	}
	while( 1 )
	{
		iret = Note_mst_hst_Fet_Sel( note_mst_hst , g_pub_tx.reply );
		if ( icount > 0 && iret == 100 )
		{
			break;	
		}
		if ( iret == 100 )
		{
			sprintf( acErrMsg, "柜员凭证明细不存在note_type = '%s' and beg_note_no = '%s' and end_note_no = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld", note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act, trace_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "N041" );
			return(-1);
		}else if ( iret )
		{
			sprintf( acErrMsg, "查询柜员凭证明细错!iret=[%d]", iret );
			WRITEMSG
			return(-1);				
		}

		break;

		/*icount++; */
	}
	
		/*
	if ( icount > 1 )
	{
		sprintf( acErrMsg, "柜员凭证明细存在多条记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N042" );
		return(-1);			
	}
		*/
	Note_mst_hst_Clo_Sel ();

	return(0);
}

/*********************************************************************************************
            add      by wanglei      20070502
根据晋中要求，柜员付出时，将Cash_mst中的券别张数清零，Cash_mst_hst中插入一条付出的券别登记记录
                  收回时，做相反操作，也把Cash_mst_hst中加一条收入的券别登记记录
**********************************************************************************************/
int upd_ins_cash(char * mode,int trace_no)
{
	int ret=0;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c; 
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));


	if(mode[0] == '1')/****付出****/
	{
		ret =Cash_mst_Sel1(g_pub_tx.reply,&cash_mst_c,"tel = '%s' ",g_pub_tx.tel);
		if(ret != 0)
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		/*** 首先再登记一条券别登记明细，将所有券别值置为负值，表示付出 ***/
		cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
		vtcp_log("B TRACE [%d]",g_pub_tx.trace_no );
		cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
		strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
		strcpy(cash_mst_hst_c.ac_no,g_pub_tx.ac_no);
		strcpy(cash_mst_hst_c.tw_ac_no,g_pub_tx.name);
		strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
		strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
		strcpy(cash_mst_hst_c.add_ind,"9");    /****登记券别标志****/
		cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
		cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
		cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
		strcpy(cash_mst_hst_c.brf,"柜员交接");
		strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
		cash_mst_hst_c.t10000 = -cash_mst_c.t10000;
		cash_mst_hst_c.t5000  = -cash_mst_c.t5000;
		cash_mst_hst_c.t2000  = -cash_mst_c.t2000;
		cash_mst_hst_c.t1000  = -cash_mst_c.t1000;
		cash_mst_hst_c.t500   = -cash_mst_c.t500;
		cash_mst_hst_c.t200   = -cash_mst_c.t200;
		cash_mst_hst_c.t100   = -cash_mst_c.t100;
		cash_mst_hst_c.t50    = -cash_mst_c.t50;
		cash_mst_hst_c.t20    = -cash_mst_c.t20;
		cash_mst_hst_c.t10    = -cash_mst_c.t10;
		cash_mst_hst_c.t5     = -cash_mst_c.t5;
		cash_mst_hst_c.t2     = -cash_mst_c.t2;
		cash_mst_hst_c.t1     = -cash_mst_c.t1;
		vtcp_log("[%s][%d] hst_t10000 = %ld \n",__FILE__,__LINE__,cash_mst_hst_c.t10000);
		
		ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"插入钱箱明细出错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P155");
			return -1;
		}
	
	
		ret=Cash_mst_Dec_Upd1(g_pub_tx.reply,
			" br_no='%s' and cur_no='%s' and tel='%s' ",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
        	
		ret=Cash_mst_Fet_Upd1(&cash_mst_c,g_pub_tx.reply);
		vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret==100 )
		{
			sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
				g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P154");
			return -1;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		
        	/****将付出方的券别更新为零****/
        	cash_mst_c.t10000 = 0;
		cash_mst_c.t5000  = 0;
		cash_mst_c.t2000  = 0;
		cash_mst_c.t1000  = 0;
		cash_mst_c.t500   = 0;
		cash_mst_c.t200   = 0;
		cash_mst_c.t100   = 0;
		cash_mst_c.t50    = 0;
		cash_mst_c.t20    = 0;
		cash_mst_c.t10    = 0;
		cash_mst_c.t5     = 0;
		cash_mst_c.t2     = 0;
		cash_mst_c.t1     = 0;
        	
		ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		Cash_mst_Clo_Upd1();
	}
	else if(mode[0] == '3')/****收回****/
	{
		ret=Cash_mst_hst_Sel1(g_pub_tx.reply,&cash_mst_hst_c,"trace_no = %ld  and add_ind = '9' and tx_date=%ld ",trace_no,g_pub_tx.tx_date);
		if(ret != 0)
		{
			sprintf(acErrMsg,"查找钱箱明细表错误!");
			WRITEMSG
			return -1;
		}
		ret=Cash_mst_Dec_Upd1(g_pub_tx.reply,
			" br_no='%s' and cur_no='%s' and tel='%s' ",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
        	
		ret=Cash_mst_Fet_Upd1(&cash_mst_c,g_pub_tx.reply);
		vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		if( ret==100 )
		{
			sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
				g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P154");
			return -1;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		
		cash_mst_c.t10000 = -cash_mst_hst_c.t10000; 
		cash_mst_c.t5000  = -cash_mst_hst_c.t5000;
		cash_mst_c.t2000  = -cash_mst_hst_c.t2000;
		cash_mst_c.t1000  = -cash_mst_hst_c.t1000;
		cash_mst_c.t500   = -cash_mst_hst_c.t500; 
		cash_mst_c.t200   = -cash_mst_hst_c.t200; 
		cash_mst_c.t100   = -cash_mst_hst_c.t100; 
		cash_mst_c.t50    = -cash_mst_hst_c.t50;  
		cash_mst_c.t20    = -cash_mst_hst_c.t20;  
		cash_mst_c.t10    = -cash_mst_hst_c.t10;  
		cash_mst_c.t5     = -cash_mst_hst_c.t5;
		cash_mst_c.t2     = -cash_mst_hst_c.t2;
		cash_mst_c.t1     = -cash_mst_hst_c.t1;
		
		ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			return -1;
		}
		Cash_mst_Clo_Upd1();

		/****收回时,将原来的付出的两条明细记录删除，并将刚才插入收入明细也删除,相当于该柜员没有做过交接付出****/
		ret=Cash_mst_hst_Dec_Upd2(g_pub_tx.reply, " (trace_no = %ld  or trace_no = %ld) and tx_date=%ld ", trace_no,g_pub_tx.trace_no,g_pub_tx.tx_date);
		if(ret != 0)
		{
			sprintf(acErrMsg,"查找钱箱明细表错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P095");
			return -1;
		}

		while(1)
		{
			ret=Cash_mst_hst_Fet_Upd2(&cash_mst_hst_c,g_pub_tx.reply);
			if(ret == 100)
			{
				break;
			}
			else if(ret != 0)
			{
				sprintf(acErrMsg,"查找钱箱明细表错误!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P095");
				return -1;
			}
			vtcp_log("ROWID=[%s]",cash_mst_hst_c.rowid);
			ret=Cash_mst_hst_Del_Upd(cash_mst_hst_c,g_pub_tx.reply);
			if(ret != 0)
			{
				vtcp_log("ROWID=[%s]",cash_mst_hst_c.rowid);
				sprintf(acErrMsg,"删除钱箱明细表错误!");
				WRITEMSG
				return -1;
			}
		}
		Cash_mst_hst_Clo_Upd2();
	}

	strcpy( g_pub_tx.dc_code,NULL_DC_CODE );

	return 0;
}
/****end by wanglei 20070502****/
