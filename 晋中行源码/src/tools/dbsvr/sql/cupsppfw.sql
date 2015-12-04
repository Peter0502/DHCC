/***������Ʒ�Ʒ���ѽ�����ˮ***/
drop index  cupsppfw_idx;
drop table cupsppfw;
create table cupsppfw(
	brtype 		varchar2(1),	/* 0-������ 1-���� */
	filename	varchar2(30),	/* ������ϸ�ļ����� */
	txday		  varchar2(8),	/* �������� */
	brno		  varchar2(8),  /* ���׻����򿪻����� */
	firstbr		varchar2(11),	/* ����/������һ�����д��� */
	cardtype	varchar2(1),	/* �Ƿ�������׼�� */
	cdtype		varchar2(1),	/* ����ǿ���־ */
	jyqd		  varchar2(2),	/* �������� 60.2.5*/
	cardjz		varchar2(1),	/* ������ */
	outtxfg		varchar2(1),	/* �羳���ױ�־ */
	mertype		varchar2(4),	/* �̻����� 18*/
	secbr		  varchar2(11),	/* ����/�������������д��� */
	dlbrno		varchar2(32),	/* ���������ʶ�� 32*/
	fsbrno		varchar2(32),	/* ���ͻ�����ʶ�� 33*/
	opnbrno		varchar2(11),	/* ����������ʶ�� */
	isbkno		varchar2(11),	/* ���ջ�����ʶ�� 100*/
	sdbrno		varchar2(11),	/* �յ��������� */
	msgtype		varchar2(4),	/* �������� */
	txcode		varchar2(6),	/* ���������� 3*/
	inputcd		varchar2(2),	/* ����������� 25*/
	trseq 		varchar2(11),	/* ϵͳ���ٺ� 11*/
	txtime		varchar2(10),	/* ���״���ʱ�� 7*/
	cardno		varchar2(19),	/* ���ʺ� 2*/
	outcardno	varchar2(19),	/* ת������ 102*/
	incardno	varchar2(19),	/* ת�뿨�� 103*/
	otxmsg		varchar2(42),	/* ԭʼ������Ϣ 90*/
	atmno		  varchar2(8),	/* �ܿ����ն˱�ʶ�� 41*/
	acbkflg		varchar2(15),	/* �ܿ�����ʶ�� 42*/
	wssrno		varchar2(12),	/* �����ο��� 37*/
	txamt		  number(16,2),	/* ���׽�� 4*/
	slfee		  number(16,2),	/* ���������� */
	pinpfee		number(16,2),	/* Ʒ�Ʒ���� */
	jamt		  number(16,2),	/* �����*/
	tmp1		  varchar2(12),	/* ����ʹ�� */
	tmp2		  varchar2(100)	/* ����ʹ�� */
)tablespace users;
create unique index cupsppfw_idx on cupsppfw(brtype,trseq,txtime) tablespace indx;
