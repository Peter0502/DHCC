/**************************************************/
/* TABLE NAME:POINFO ��Ʊ������Ϣ��                */
/* Description  : Postal Order Saving Master File  */
/* Table index  : pono,signday,                    */
/* create by shenlei: 2006-11-9                    */
/***************************************************/
drop index cashier_idx;
drop table cashier;
create table cashier(
	pono         varchar2(20),/* ��Ʊ����*/
	brno         varchar2(7), /* ��������*/
	orbrno       varchar2(12),/* �������к�*/
	payqsactno   varchar2(12),/* ֧���������ʺ�*/
	txday        number(8),   /* �������� */
	lostcnt      number(6),   /* ��ʧ����ÿ��ʧ�����ʧ����һ */
	obrno        varchar2(12),/* �Է������� no user*/
	voctype      varchar2(3), /* ƾ֤����  */
	vocnum       varchar2(16),/* ƾ֤��   */
	ovoctype     varchar2(3), /* ����������*/
	ovocnum      varchar2(16),/* �������*/
	avbal        number(15,2),/* ���(��Ʊ��Ʊ����)*/
	useamt       number(15,2),/* ʵ�ʽ����� */
	feeamt       number(15,2),/* ������ */
	leavebal     number(15,2),/* ������ */
	cour         varchar2(3), /* ���� */
	paybrno      varchar2(12),/* �����˿����к� */
	payactno     varchar2(32),/* �������ʺ�*/
	payname      varchar2(60),/* �����˻���*/
	paybrname    varchar2(60),/* ǩ��������*/ 
	payaddress   varchar2(60),/* �����˵�ַ*/ 
	cashactno    varchar2(32),/* �տ����ʺ�*/ 
	cashbrno     varchar2(12),/* �Ҹ����к�*/
	cashbrname   varchar2(60),/* �Ҹ�������*/
	cashaddress  varchar2(60),/* �Ҹ��е�ַ*/
	cashname     varchar2(60),/* �տ��˻���*/
	holdbrno     varchar2(12),/* ���Ҹ��к�*/
    	holdbrname   varchar2(60),/* ���Ҹ�����*/
	holdactno    varchar2(32),/* ���Ҹ��ʺ�*/
	holdname     varchar2(60),/* ���Ҹ�����*/
	lactcls      varchar2(1), /* �ϴ��ʻ�״̬*/
	stat         varchar2(1), /* ��Ʊ��ǰ״̬��־ 0-�Ǽ� 1-ǩ�� 
		2-��Ʊ�Ҹ� 3-��Ʊ�⸶ 4-��Ʊ��ʧ 5-���ʧ
		6-��Ʊ��Ʊ 7-ǩ��ȡ�� 8-�Ǽ�ȡ�� */
	cashstat     varchar2(1), /*�⸶״̬��0��ϵͳ�ڱ��н⸶���У�1��ϵͳ�ڱ��н⸶���У� 3��ϵͳ��⸶����,2.ϵͳ�ڽ⸶ϵͳ��*/
	signday      number(8),   /* ǩ������*/
	regday       number(8),   /* ��������*/
	cashday      number(8),   /* �Ҹ�����*/
	jqday        number(8),   /* ��������*/
	endday       number(8),   /* ��Ʊ��������*/
	passwd       varchar2(20),/* ��Ѻ*/
        avbaltype    varchar2(1), /* �����־:0.���1.������*/
	bptype       varchar2(1), /* ��Ʊ���0.ת�ʱ�Ʊ1.�ֽ�Ʊ*/
	ccflag       varchar2(1), /* ת���ֽ��־0-�ֽ�1 ת��(�ʺ�) 2��Ŀ��ת*/
	cashflag     varchar2(1), /* �տ��ֽ��־0-�ֽ�1 ת��(�ʺ�) 2��Ŀ��ת*/
	drsubject    varchar2(7), /* �����Ŀ*/
	crsubject    varchar2(7), /* �ʽ�ȥ��*/
	reason       varchar2(60),/* ժҪ*/
	feeflag      varchar2(1), /* �����ѱ�־  0.�ֽ� 1.�ֽ�Ӽ� 2.ת�� 3.ת�ʼӼ� 4.����*/
	checkflag    varchar2(1), /* ���ʱ�־ */
	inflag       varchar2(1), /* ���ڱ�־��0.���ڣ�1.����*/
	optlrno      varchar2(4) ,/* ��������Ա */
	chktlrno     varchar2(4) ,/* ǩ������Ա */
	passtlrno    varchar2(4) ,/* ��Ѻ����Ա */
	cashtlrno    varchar2(4) ,/* �Ҹ�����Ա */
	jqtlrno      varchar2(4) ,/* �������Ա */
	addpassflg   varchar2(1) ,/* �Ƿ��/��Ѻ���� 1-��Ѻ/��Ѻ 0-δ���� */
	filler       varchar2(60) /* ��ע*/
)tablespace users;
create unique index cashier_idx on cashier(pono,signday) tablespace indx;
