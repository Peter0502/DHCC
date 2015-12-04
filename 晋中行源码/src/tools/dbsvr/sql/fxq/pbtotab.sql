/***********************************************************/
/** pbtotab:�漰���˵������ת��Ʊ��ҵ���                 */
/** ��������:2005-04-28                                    */
/***********************************************************/
drop index  pbtotab_idx;
drop table  pbtotab;
create table pbtotab
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
	payactno  varchar2(40),		/*�������˺�,��ѡ*/
	paycode	varchar2(20),		/*�����˻�������,��ѡ*/
	cashbro	varchar2(12),		/*�տ����к�*/
	cashbrtype  varchar2(1), 	/*�տ����к�����*/
	cashname  varchar2(60),		/*�տ�������*/
	cashactno  varchar2(40),	/*�տ����˺�,��ѡ*/
	cashcode  varchar2(20),		/*�տ��˻�������,��ѡ*/
	cashertype  varchar2(2),	/*�տ�������,01����λ��02������*/
	cashcertif  varchar2(22),	/*����֤�����ͼ�����,2λ֤�����͡�20λ����*/
	txamt  varchar2(48),		/*���ҷ��ż����,3λ���֡�15λ���*/
	paydate	 varchar2(8),		/*��������*/
	cashdate  varchar2(8),		/*�տ�����*/
	dscpt	 varchar2(20),		/*��;	,��ѡ*/
	paytype  varchar2(2),		/*���㷽ʽ*/
	crdno	varchar2(32),		/*ƾ֤����	,��ѡ*/
	txtypeno  varchar2(10),		/*�������ʹ���*/
	expand	 varchar2(20)		/*��չ,�ݲ�ʹ��*/
)tablespace users;
create unique index pbtotab_idx on pbtotab(apflg) tablespace indx;
