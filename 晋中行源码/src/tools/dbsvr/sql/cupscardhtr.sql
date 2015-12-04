/***������������ˮ***/
drop index  cupscardhtr_idx;
drop table cupscardhtr;
create table cupscardhtr(
	brtype 		varchar2(1),	/* 0-������(ת�뷽) 1-����(ת����) */
	filename	varchar2(30),	/* ������ϸ�ļ����� */
	txday		varchar2(8),	/* ��������*/
	brno		varchar2(8),	/* ���׻����򿪻����� */
	dlbrno		varchar2(32),	/* ���������ʶ�� 32*/
	fsbrno		varchar2(32),	/* ���ͻ�����ʶ�� 33*/
	trseq 		varchar2(11),	/* ϵͳ���ٺ� 11*/
	txtime		varchar2(10),	/* ���״���ʱ�� 7*/
	cardno		varchar2(19),	/* ���ʺ� 2*/
	txamt		number(16,2),	/* ���׽�� 4*/
	dsamt		number(16,2),	/* ���ִ���ʱ�ĳжҽ�� 95*/
	fee		number(16,2),	/* �ֿ��˽��������� 28*/
	msgtype		varchar2(4),	/* �������� */
	txcode		varchar2(6),	/* ���������� 3*/
	mertype		varchar2(4),	/* �̻����� 18*/
	atmno		varchar2(8),	/* �ܿ����ն˱�ʶ�� 41*/
	acbkflg		varchar2(15),	/* �ܿ�����ʶ�� 42*/
	acbkaddr	varchar2(40),	/* �ܿ������Ƶ�ַ 43*/
	wssrno		varchar2(12),	/* �����ο��� 37*/
	inputcd		varchar2(2),	/* ����������� 25*/
	authcd		varchar2(6),	/* ��ȨӦ���� 38*/
	isbkno		varchar2(11),	/* ���ջ�����ʶ�� 100*/
	otrseq		varchar2(6),	/* ԭʼ���׵�ϵͳ���ٺ� 90.2*/
	respcd		varchar2(2),	/* ���׷����� 39*/
	inputmd		varchar2(3),	/* ��������뷽ʽ 22*/
	infee		number(16,2),	/* Ӧ�������� */
	outfee		number(16,2),	/* Ӧ�������� */
	zjfee		number(16,2),	/* ת�ӷ���� */
	dsflag		varchar2(1),	/* ��˫ת����־ */
	cardseq		varchar2(3),	/* ��Ƭ���к� 23*/
	tmreadfg	varchar2(1),	/* �ն˶�ȡ���� 60.2.2*/
	iccardfg	varchar2(1),	/* IC���������� 60.2.3*/
	otxtime		varchar2(10),	/* ԭʼ��������ʱ�� 90.3*/
	opnbrno		varchar2(11),	/* ����������ʶ�� */
	outtxfg		varchar2(1),	/* ���⽻�ױ�־ */
	jyqd		varchar2(2),	/* �������� 60.2.5*/
	eciflag		varchar2(2),	/* ECI��־ */
	curcd 		varchar2(3),	/* ���׻��Ҵ��� 49*/
	dltxday		varchar2(4),	/* �ܿ������ڵ����� 13*/
	dltxtime	varchar2(6),	/* �ܿ������ڵ�ʱ�� 12*/
	qsday		varchar2(4),	/* �������� 15*/
	sqresn		varchar2(3),     /* ����Ȩԭ�� 000:��������Ȩ 001:�쳣����Ȩ */
	certtype	varchar2(2),	/* ֤������ 61.1*/
	certnum		varchar2(20),	/* ֤����� 61.2*/
	dldsflg		varchar2(1),	/* �ܿ�����˫��־ 0-����Ϣ 1-˫��Ϣ */
	clzxseq		varchar2(9),	/* ����������ˮ�� */
	opncdbk		varchar2(11),	/* �������д��� */
	feeflg		varchar2(1),	/* �շѱ�־  0-������ȡ����Ȩ���� 1-��Ҫ��ȡ����Ȩ���� */
	tzxeflg		varchar2(1),	/* �Ƿ����֪ͨ�޶��־ 0-�����ڴ���Ȩ֪ͨ�޶� 1-���ڴ���Ȩ֪ͨ�޶� */
	djyobj		varchar2(30),	/* ��У����Ŀ ����֪ͨ���������������Ĵ�У����Ŀ�� ÿһλ��ȡֵ����һ������Ȩ��Ŀ����1λ����12λ���α�ʾ��
			 ����ȨУ��PVV
			 ����ȨУ��CVV
			 ����ȨУ��CVV2
			 ����ȨУ��ARQC
			 ����ȨУ�鿨�ų���
			 ����ȨУ�鿨��У��λ
			 ����ȨУ�鿨��Ч��
			 ����ȨУ��pin���Դ���
			 ����ȨУ��ARPC
			 ֹ���������
			 VIP�ļ����
			 �����޶���
			 ÿһλ��ȡֵ�����֣�
			 Y-�����Ѱ���Э���Ҫ�����˸������Ȩ
			 N-Э��δҪ�����������Ȩ
			 ����λ����ʹ�ã���ȱʡֵ��䡣 */
	dsqfee		number(16,2),	/* ����Ȩ���� */
	outbrno		varchar2(11),	/* ת��������ʶ�� 100*/
	outcardno	varchar2(19),	/* ת������ 102*/
	inbrno		varchar2(11),	/* ת�������ʶ�� 100*/
	incardno	varchar2(19),	/* ת�뿨�� 103*/
	cityfg		varchar2(1),	/* ͬ����ر�־ */
	tmp		varchar2(300)	/* ����ʹ�� */
)tablespace users;
create unique index cupscardhtr_idx on cupscardhtr(brtype,trseq,txtime,txcode) tablespace indx;
