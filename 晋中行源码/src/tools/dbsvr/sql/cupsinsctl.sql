/***������������ˮ���Ʊ�***/
drop index  cupsinsctl_idx;
drop table cupsinsctl;
create table cupsinsctl(
	trseq 		number(6),	/*�������*/
	brtype		varchar2(1),	/*0-������ 1-���� */
	lstinsday	varchar2(8),	/*���һ�β����� */
	name1		varchar2(11),	/*�ļ���1-11λ */
	name2		varchar2(1),	/*�ļ���12λ */
	name3		varchar2(10),	/*�ļ���12λ���ȡֵ */
	tabname		varchar2(20)	/*���ļ���Ӧ�����ĸ��� */
)tablespace users;
create unique index cupsinsctl_idx on cupsinsctl(trseq) tablespace indx;
