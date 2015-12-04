/* ATM�ֻ���atmrecord
table atmrecord
table index�� atmrecord_idx  unique  atmid varchar2(20);*/

drop index  atmrecord_idx;
drop table atmrecord;      
create table atmrecord(    
	Atmno	       Varchar2(10),    /*  ����ATM���� */
	Atmid	       Varchar2(20),    /*  ATM���к�*/
	Atmmode	       Varchar2(1),     /*  ATM�ͺ�*/
	Opnbr	       Varchar2(7),     /*  ��������*/
	Depflag	       Varchar2(1),     /*  ���ܱ�־*/
	Chaflag	       Varchar2(1),     /*  ת�˹��ܱ�־*/
	Userflag       Varchar2(1),     /*  ���ñ� 1����2δ����*/         
	Atmstat	       Varchar2(1),     /*  ATM״̬	 1����2����3δǩ��*/
	Paycnt	       Number  (3),     /*  ÿ��ȡ�����*/
	Payamt	       Number(16,2),  /*  ÿ�������*/
	Paymamt	       Number(16,2),  /*  ÿ�������*/
	Feerate	       Number(7,5),   /*  ������*/
	Opndate	       Varchar2(8),     /*  ע������*/
	Startdte       Varchar2(8),     /*  ��������*/
	Enddate	       Varchar2(8),     /*  ͣ������*/
	Place	       Varchar2(40),     /*  ���ŵ�ַ*/
	tlrno	       Varchar2(6),     /*ATM  ����Ա��*/
	filler	       Varchar2(60)     /*  ����Ա��*/
)tablespace users;
create unique index atmrecord_idx on atmrecord (atmno) tablespace indx;
