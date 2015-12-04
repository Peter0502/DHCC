/***���������ʴ�������ˮ***/
drop index  cupserr_idx;
drop table cupserr;
create table cupserr(
	brtype 		varchar2(1),	/* 0-������ 1-���� */
	errflg		varchar2(1),	/* ����ױ�־ */
	filename	varchar2(30),	/* ������ϸ�ļ����� */
	txday		varchar2(8),	/* �������� */
	brno		varchar2(8),    /* ���׻����򿪻����� */
	dlbrno		varchar2(32),	/* ���������ʶ�� 32*/
	fsbrno		varchar2(32),	/* ���ͻ�����ʶ�� 33*/
	trseq 		varchar2(11),	/* ϵͳ���ٺ� 11*/
	txtime		varchar2(10),	/* ���״���ʱ�� 7*/
	cardno		varchar2(19),	/* ���ʺ� 2*/
	txamt		number(16,2),	/* ���׽�� 4*/
	fee		number(16,2),	/* �ֿ��˽��������� 28*/
	msgtype		varchar2(4),	/* �������� */
	
	
	
	txcode		varchar2(6),	/* ���������� 3*/
	mertype		varchar2(4),	/* �̻����� 18*/
	atmno		  varchar2(8),	/* �ܿ����ն˱�ʶ�� 41*/
	wssrno		varchar2(12),	/* �����ο��� 37*/
	inputcd		varchar2(2),	/* ����������� 25*/
	authcd		varchar2(6),	/* ��ȨӦ���� 38*/
	isbkno		varchar2(11),	/* ���ջ�����ʶ�� 100*/
	opnbrno		varchar2(11),	/* ����������ʶ�� 100*/
	otrseq		varchar2(6),	/* ԭʼ���׵�ϵͳ���ٺ� 90.2*/
	respcd		varchar2(2),	/* ���׷����� 39*/
	inputmd		varchar2(3),	/* ��������뷽ʽ 22*/
	insxfee		number(16,2),	/* Ӧ�������� */
	outsxfee	number(16,2),	/* Ӧ�������� */
	zjfee		  number(16,2),	/* ת�ӷ���� */
	infee		  number(16,2),	/* Ӧ�շ��� */
	outfee		number(16,2),	/* Ӧ������ */
	errreasn	varchar2(4),	/* ���ԭ�� */
	cardseq		varchar2(3),	/* ��Ƭ���к� 23*/
	tmreadfg	varchar2(1),	/* �ն˶�ȡ���� 60.2.2*/
	iccardfg	varchar2(1),	/* IC���������� 60.2.3*/
	otxtime		varchar2(10),	/* ԭʼ����ʱ��*/
	oqstime		varchar2(10),	/* ԭʼ������������15*/
	otxday		varchar2(10),	/* ԭʼ�������� 90.3*/
	otxamt		number(16,2),	/* ԭʼ���׽�� 4*/
	eciflag		varchar2(2),	/* ECI��־ */
	outbrno		varchar2(11),	/* ����/ת��������ʶ�� 100*/
	outcardno	varchar2(19),	/* ת������ 102 */
	inbrno		varchar2(11),	/* ת�������ʶ�� 100*/
	incardno	varchar2(19),	/* ת�뿨�� 103*/
	areaflg		varchar2(1),	/* ���׵����־ 
0���ǿ羳һ�㽻�ף�1���羳һ�㽻�ף�2��ͬ��ת�ˡ�3�����ת�ˣ�4���羳ת��**/
	tmp		varchar2(300)	/* ����ʹ�� */
)tablespace users;
create unique index cupserr_idx on cupserr(brtype,trseq,txtime) tablespace indx;
