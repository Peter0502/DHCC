/***********************************************************/
/** patotab:�漰������������н����˻���ת��ҵ���         */
/** ��������:2005-04-28                                    */
/***********************************************************/
drop index  patotab_idx;
drop table  patotab;
create table patotab
(
	brno varchar2(12), 	      /*���ͻ�������*/
	brname varchar2(60),     /*���ͻ�������*/
	txday varchar2(8),		   /*��������*/
	txtime varchar2(8),		   /*����ʱ��*/	 
	aptype varchar2(1),		   /*ҵ������*/
	txcd varchar2(2),			   /*���״���*/
	sendday varchar2(8),		   /*��������*/
	apflg varchar2(32),		   /*ҵ���ʶ*/	
   paybrno	varchar2(12),     /*�������к�*/	
   paybrtype	varchar2(1),   /*�������к�����*/	
   payname	varchar2(60),     /*����������*/	
   payactno	varchar2(40),     /*�������˺�*/	
   paycode	varchar2(20),     /*�����˻������� ��ѡ*/
   payertype	varchar2(2),      /*���������� 01����λ��02������*/
   paycertif	varchar2(22),  /*������֤�����ͼ����� 2λ֤�����͡�20λ���� ��ѡ*/
   cashbrno	varchar2(12),     /*�տ����к�*/
   cashbrtype	varchar2(1),   /*�տ����к�����*/	
   cashname	varchar2(60),     /*�տ�������*/	
   cashactno	varchar2(40),  /*�տ����˺�*/	
   cashcode	varchar2(20),     /*�տ��˻������� ��ѡ*/
   cashertype	varchar2(2),      /*�տ������� 01����λ��02������*/
   cashcertif	varchar2(22),  /*�տ���֤�����ͼ�����	��ǣ�2λ֤�����͡�20λ���� ��ѡ*/
   txamt	varchar2(18),        /*���ҷ��ż���� 3λ���֡�15λ���*/
   paydate	varchar2(8),      /*��������*/	
   cashdate	varchar2(8),      /*�տ�����*/	
   dscpt	varchar2(20),        /*��;	��ѡ*/
   paytype	varchar2(2),      /*���㷽ʽ*/	
	crdno varchar2(32), 		   /*ƾ֤����*/
	txtypeno varchar2(10), 	   /*�������ʹ���*/
	expand varchar2(20)		   /*��չ*/
)tablespace users;
create unique index patotab_idx on patotab(apflg) tablespace indx;
