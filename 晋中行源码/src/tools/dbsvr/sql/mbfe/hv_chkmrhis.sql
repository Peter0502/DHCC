/******************************************************************************/
/*  Table Name   : hvCheckMr (大额业务核对主表)                               */
/*  Description  : 存放历史中需要从ccpc下传得核对明细                         */
/*  Location     : On each region                                             */
/*  Table Index  : no                                                         */
/******************************************************************************/
drop  table hv_chkmrhis;
create table hv_chkmrhis(
	tx_date number(9),          /*30A委托日期 强制项 8n      */                                           
	ccpc_no varchar2(12),       /*CC6 CCPC号（或银行行号、特许参与者号）城市处理中心代码或银行行号12n  */ 
	send_cnt number(9),         /*CDJ 往账总笔数 往账总笔数 强制项 10n */                                 
	send_amt number(22,2),      /*CMJ 往账总金额 往账总金额 强制项 22n */                                 
	send_item number(9),        /*CD1 往账分类项目数 2n      */                                           
	send_dtl varchar2(352),                                                                               
	recevice_cnt number(9),     /*CDI 来账总笔数10n  */                                                   
	recevice_amt number(22,2),  /*CMI 来账总金额22n  */                                                   
	recevice_item number(9),    /*CD2 来账分类项目数2n  */                                                
	recevice_dtl varchar2(352),                                                                           
	online_cnt number(9),       /*CDQ即时转账笔数 10n   */                                                
	online_amt number(22,2),    /*CDQ即时转账总金额 22n */                                                
	online_item number(9),      /*CDQ 即时转账分类项目数 强制项 2n */                                     
	online_dtl varchar2(192) 
)tablespace users;
