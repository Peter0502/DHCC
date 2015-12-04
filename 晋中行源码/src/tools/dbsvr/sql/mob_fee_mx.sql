/******************************************************************************/
/*  table name   : mob_fee_mx   (���ſ۷���ϸ)                           */
/*  table index  : mob_fee_mx_idx                                          */
/*  create by    : ����                                      */
/*  modified by wangxd@20101225					*/
/*  ���˵��:�����۷ѹ�Ա,������,�۷�˵��,�ϴο�����,�����룬ǩԼ�˺ż���������Ԥ���ֶ�					*/
/******************************************************************************/

drop table mob_fee_mx;
drop index mob_fee_mx_idx;
create table mob_fee_mx(
	tx_date       number,          /*��������*/
	tel	      varchar2(6),       /*���׹�Ա*/
	trace_no      number,          /*ϵͳ��ˮ*/
	tx_code	      varchar2(4),	/*������*/
	ct_ind	      varchar2(2),	/*��ת��־ 1--�ֽ� 2--ת��*/
	br_no         varchar2(5),       /*�����*/
	ac_no         varchar2(24),      /*�˺�*/
	name          varchar2(60),      /*����*/
	sign_ac_no    varchar2(30),	/*ǩԼ�ʺ�*/
	server_type   varchar2(4),       /*��������*/ 
	amt_should    number(16,2),    /*����Ӧ��*/
	amt_fact      number(16,2),    /*ʵ��*/
	cnt           number,          /*Ƿ������*/
	lst_fee_date  number,		/*�ϴο�����*/
	print_cnt     number,		/*��ӡ����*/
	memo	      varchar2(100),	/*�۷�˵��*/
	sts	      varchar2(2),	/*��ϸ״̬*/
	filler1	      varchar2(20),	/*�����ֶ�1   ������ˮ��*/
	filler2	      varchar2(30),	/*�����ֶ�2*/
	filler3	      varchar2(50),	/*�����ֶ�3*/
	filler4	      varchar2(100)	/*�����ֶ�4*/
)tablespace users;
create  index mob_fee_mx_idx on mob_fee_mx ( tx_date,ac_no,trace_no ) ;


