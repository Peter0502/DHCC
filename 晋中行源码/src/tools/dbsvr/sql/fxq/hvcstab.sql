/***********************************************************/
/** hvcstab:����Ҵ���ֽ����ҵ���                               */
/** ��������:2005-04-28                                    */
/***********************************************************/
drop index  hvcstab_idx;
drop table  hvcstab;
create table hvcstab
(
	brno varchar2(12), 	      /*���ͻ�������*/
	brname varchar2(60),     /*���ͻ�������*/
	txday varchar2(8),		   /*��������*/
	txtime varchar2(8),		   /*����ʱ��*/	 
	aptype varchar2(1),		   /*ҵ������*/
	txcd varchar2(2),			   /*���״���*/
	sendday varchar2(8),		   /*��������*/
	apflg varchar2(32),		   /*ҵ���ʶ*/
	Payname	varchar2(60),     /*�ɿ�������*/	
   paycertif	varchar2(22),  /*�ɿ���֤�����ͼ����� 2λ֤�����͡�20λ����*/
   cashbrno	varchar2(12),     /*�տ����к�*/	
   cashbrtype	varchar2(1),   /*�տ����к�����*/
   cashname	varchar2(60),     /*�տ�������*/	
   cashactno	varchar2(40),  /*�տ����˺�*/	
   cashertype	varchar2(2),      /*�տ������� 01����λ��02������*/
   cashcode	varchar2(20),     /*�տ��˻�������	��ѡ*/
   cashcertif	varchar2(22),  /*�տ���֤�����ͼ�����	2λ֤�����͡�20λ�����ѡ*/
   txamt	varchar2(18),        /*���ҷ��ż����	3λ���֡�15λ���*/
   cashdate	varchar2(8),      /*�տ�����*/	
   cfrom	varchar2(20),        /*�ֽ���Դ	��ѡ*/
	cashtype varchar2(2), 		   /*���㷽ʽ*/
	txtypeno varchar2(10), 	   /*�������ʹ���*/
	expand varchar2(20)		   /*��չ*/
)tablespace users;
create unique index hvcstab_idx on hvcstab(apflg) tablespace indx;
