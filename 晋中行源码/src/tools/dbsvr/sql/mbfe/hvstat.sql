/***֧��ϵͳ״̬��***/
drop table hvstat;      
create table hvstat(    
	tx_date		number(8),		/*	�������� */
	stat		varchar2(1),		/*  ״̬*/
	chflag		varchar2(1),		/*	���һ�θ��±�־:0-�ɹ�,1-ʧ�� */
	message		varchar2(60),		/*  ��������*/
	filler1		varchar2(30),			
	filler2		varchar2(30)			
)tablespace users;
