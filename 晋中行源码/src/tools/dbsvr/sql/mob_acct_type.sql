/******************************************************************************/
/*  table name   : mob_acct_type2   (短信签约表)                           */
/*  table index  : mob_acct_type2_idx                                          */
/*  create by    : houyi      20100516                                      */
/******************************************************************************/
drop table mob_acct_type;							
drop index mob_acct_type_idx;
drop index mob_acct_type_idx1;
create table mob_acct_type(
	ac_no					varchar2(20),	/*账号(黄河银行：贷款账户可存贷款合同号)*/
	ac_id					number(9),      /*账户id，贷款为0*/
	ac_seqn				number(6),			/**账户序号**/
	ac_type				varchar2(1),   /*账户类型*/
	server_type		varchar2(4),   /*服务类型*/
	mobile				varchar2(12),  /*电话号码*/
	tx_date				number(9),      /*交易日期*/
	lst_fee_date	number(9),      /*上次收费日期*/
	tel						varchar2(6),   /*交易柜员*/
	sfbz					varchar2(1),   /*收费标志0.不收，1收费*/
	mob_sts				varchar2(1),   /*状态0-解约 1-签约 2-暂停*/
	phones				varchar2(60),		/**其他号码**/
	br_no 			varchar2(5),
	filler			varchar2(50)
)tablespace users; 
create unique index mob_acct_type_idx on mob_acct_type (ac_no,mobile);
create  index mob_acct_type_idx1 on mob_acct_type (ac_no,mob_sts) ; 

