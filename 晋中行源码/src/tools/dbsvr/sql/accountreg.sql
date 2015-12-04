/*  ͬ������Ǽǲ� */
drop index  accountreg_idx;
drop table  accountreg;
create table accountreg
(
	br_no  		varchar2(5),      /*  ����� */
	trace_no 	number(9),      /*  ��ˮ��      */
	seqno  		number(2),     /*  ���        */
	change_no    varchar2(4),        /*  ������      */
	br_name      varchar2(60),       /*  ��������    */
	note_type  varchar2(3),             /*  ƾ֤����    */
	note_no     varchar2(16),       /*  ƾ֤��      */ 
	pay_ac_no    varchar2(32),      /*   �����ʺ�    */
	pay_name     varchar2(60),       /*  ��������    */
	amt         number(16,2),       /*  ���        */
	cash_ac_no   varchar2(32),       /*  �տ��ʺ�    */
	cash_name    varchar2(60),       /*  �տ�����    */
	cash_opn_br_no  varchar2(12),       /*  �Է��к�    */
	cash_chg_no   varchar2(4),        /*  �Է�������  */
	cash_opn_name  varchar2(60),       /*  �Է�����    */
	put_flag     varchar2(1),        /*  ������� 1--���2--���� */
	tx_date     number(9),        /*  ��������    */
	tx_time     number(9),        /*  ����ʱ��    */
	stat        varchar2(1),        /*  ״̬��־ 1--�Ǽ�2--���� 8-�Ѵ�ӡ 9-ȡ��*/
	tel         varchar2(4),        /*  ����Ա      */
	chk         varchar2(4),        /*  ����Ա      */
	pwd         varchar2(6)         /*  ��Ѻ        */
);
create unique index accountreg_idx on accountreg (br_no,trace_no,tx_date);
