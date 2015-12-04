/******************************************************************************/
/*  table name   : mob_acct_type2   (����ǩԼ��)                           */
/*  table index  : mob_acct_type2_idx                                          */
/*  create by    : houyi      20100516                                      */
/******************************************************************************/
drop table mob_acct_type;							
drop index mob_acct_type_idx;
drop index mob_acct_type_idx1;
create table mob_acct_type(
	ac_no					varchar2(20),	/*�˺�(�ƺ����У������˻��ɴ�����ͬ��)*/
	ac_id					number(9),      /*�˻�id������Ϊ0*/
	ac_seqn				number(6),			/**�˻����**/
	ac_type				varchar2(1),   /*�˻�����*/
	server_type		varchar2(4),   /*��������*/
	mobile				varchar2(12),  /*�绰����*/
	tx_date				number(9),      /*��������*/
	lst_fee_date	number(9),      /*�ϴ��շ�����*/
	tel						varchar2(6),   /*���׹�Ա*/
	sfbz					varchar2(1),   /*�շѱ�־0.���գ�1�շ�*/
	mob_sts				varchar2(1),   /*״̬0-��Լ 1-ǩԼ 2-��ͣ*/
	phones				varchar2(60),		/**��������**/
	br_no 			varchar2(5),
	filler			varchar2(50)
)tablespace users; 
create unique index mob_acct_type_idx on mob_acct_type (ac_no,mobile);
create  index mob_acct_type_idx1 on mob_acct_type (ac_no,mob_sts) ; 

