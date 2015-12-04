/******************************************************************************/
/*  Table Name   : hvCheckMr (���ҵ��˶�����)                               */
/*  Description  : �����ʷ����Ҫ��ccpc�´��ú˶���ϸ                         */
/*  Location     : On each region                                             */
/*  Table Index  : no                                                         */
/******************************************************************************/
drop  table hv_chkmrhis;
create table hv_chkmrhis(
	tx_date number(9),          /*30Aί������ ǿ���� 8n      */                                           
	ccpc_no varchar2(12),       /*CC6 CCPC�ţ��������кš���������ߺţ����д������Ĵ���������к�12n  */ 
	send_cnt number(9),         /*CDJ �����ܱ��� �����ܱ��� ǿ���� 10n */                                 
	send_amt number(22,2),      /*CMJ �����ܽ�� �����ܽ�� ǿ���� 22n */                                 
	send_item number(9),        /*CD1 ���˷�����Ŀ�� 2n      */                                           
	send_dtl varchar2(352),                                                                               
	recevice_cnt number(9),     /*CDI �����ܱ���10n  */                                                   
	recevice_amt number(22,2),  /*CMI �����ܽ��22n  */                                                   
	recevice_item number(9),    /*CD2 ���˷�����Ŀ��2n  */                                                
	recevice_dtl varchar2(352),                                                                           
	online_cnt number(9),       /*CDQ��ʱת�˱��� 10n   */                                                
	online_amt number(22,2),    /*CDQ��ʱת���ܽ�� 22n */                                                
	online_item number(9),      /*CDQ ��ʱת�˷�����Ŀ�� ǿ���� 2n */                                     
	online_dtl varchar2(192) 
)tablespace users;
