/***********************************************************/
/** hvpttab:����Ҵ��ת�ʱ�                               */
/** ��������:2005-04-28                                    */
/***********************************************************/
drop index  hvpttab_idx;
drop table  hvpttab;
create table hvpttab
(
	brno varchar2(12), 	   /*���ͻ�������*/
	brname varchar2(60),  /*���ͻ�������*/
	txday varchar2(8),		/*��������*/
	txtime varchar2(8),		/*����ʱ��*/	 
	aptype varchar2(1),		/*ҵ������*/
	txcd varchar2(2),			/*���״���*/
	sendday varchar2(8),		/*��������*/
	apflg varchar2(32),		/*ҵ���ʶ*/
	paybrno varchar2(12),	/*�������к�*/
	paybrtype varchar2(1),	/*�������к�����*/
	payname varchar2(60),	/*����������*/
	payactno varchar2(40),	/*�������˺�*/
	paycode varchar2(20),	/*�����˻�������*/
	cashbrno varchar2(12),	/*�տ����к�*/
	cashbrtype varchar2(1),	/*�տ����к�����*/
	cashname varchar2(60),	/*�տ�������*/
	cashactno varchar2(40),	/*�տ����˺�*/
	cashbrcode varchar2(20),/*�տ��˻�������*/
	txamt varchar2(18),		/*���ҷ��Ž��*/
	paydate varchar2(8),		/*��������*/
	cashdate varchar2(8),	/*�տ�����*/
	dscpt varchar2(20),		/*��;*/
	cashtype varchar2(2),   /*���㷽ʽ*/	
	crdno varchar2(32), 		/*ƾ֤����*/
	txtypeno varchar2(10), 	/*�������ʹ���*/
	expand varchar2(20)   	/*��չ*/
)tablespace users;
create unique index hvpttab_idx on hvpttab(apflg) tablespace indx;
