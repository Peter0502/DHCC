/*大小额交易流水登记表（渠道交易增加相应的业务信息）*/
drop index chnl_tx_detl_1;
drop index chnl_tx_detl_2;
drop  table chnl_tx_detl;
create table chnl_tx_detl(
  tx_code		 varchar(6)    not null,
  syscode  		 varchar(6)    not null,
  chnl_type      varchar(1)    not null,  /*渠道类型：1:银联卡 2:农信银 3:人行支付 4:二代代理平台 5:二代农信银*/
  tx_date        number(9)       not null,  /*核心日期*/                                     
  tx_time        varchar(26)   not null,  /*核心时间*/                                     
  trace_no       number(9)       not null,  /*核心流水*/                                     
  trace_cnt      number(9)       not null,  /*交易流水笔次*/                                 
  pt_date        number(9)       not null,  /*平台日期*/                                     
  pt_trace_no    number(9)       not null,  /*平台流水*/                                     
  wt_date        number(9)       not null,	/*委托日期*/                                   
  orderno		 varchar(12)     not null,	/*支付序号*/                                     
  or_br_no	   	 varchar(14)   	not null,	/*发起行行号*/                                     
  sndrcvflag	 varchar(4)      not null,	/*往来标示*/                                   
  ac_no          varchar(32)           ,  /*交易账号 行内账户*/
  ac_name        varchar(60)           ,  /*行内账户户名 add by xk*/                      
  add_ind        varchar(1)            ,  /*主账号增减标志 0减 1增*/                     
  ct_ind         varchar(1)            ,  /*现转标志 1现 2转 */                          
  tx_amt         number(18,2)          ,  /*交易金额*/                                   
  df_br_no       varchar(12)           ,  /*对方行行号*/                                 
  df_ac_no       varchar(32)           ,  /*对方账号*/                                   
  df_name        varchar(60)           ,  /*对方名称*/                                     
  df_id_type     varchar(3)            ,  /*对方id*/                                       
  df_id_no       varchar(20)           ,  /*对方证件号码*/                                 
  txnum          varchar(4)            ,  /*业务类型编号*/                                 
  yw_type        varchar(5)            ,  /*业务种类*/                                   
  dz_ind         varchar(2)      not null,	/*对账状态*/                                   
  filler1        varchar(60)           ,  /*备注1*/                                      
  filler2        varchar(60)           ,  /*备注2*/                                      
  filler3        varchar(60)           ,  /*备注3*/                                      
  filler4        varchar(60)              /*备注4*/
);
create  index chnl_tx_detl_1 on chnl_tx_detl(tx_date,trace_no,wt_date,orderno,or_br_no,sndrcvflag);
create  unique index chnl_tx_detl_2 on chnl_tx_detl(chnl_type,pt_date,pt_trace_no,sndrcvflag);
create  index chnl_tx_detl_3 on chnl_tx_detl(ac_no);

