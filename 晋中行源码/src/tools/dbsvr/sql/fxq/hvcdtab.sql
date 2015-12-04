/***********************************************************/
/** hvcdtab:����Ҵ���ֽ�֧ȡҵ���                     */
/** ��������:2005-04-28                                    */
/***********************************************************/
drop index  hvcdtab_idx;
drop table  hvcdtab;
create table hvcdtab
(
	brno varchar2(12), 	      /*���ͻ�������*/
	brname varchar2(60),     /*���ͻ�������*/
	txday varchar2(8),			/*��������*/
	txtime varchar2(8),		   /*����ʱ��*/	 
	aptype varchar2(1),		   /*ҵ������*/
	txcd varchar2(2),			   /*���״���*/
	sendday varchar2(8),		   /*��������*/
	apflg varchar2(32),		   /*ҵ���ʶ*/
	paybrno	varchar2(12),     /*�������к�*/		
   paybrtype	varchar2(1),   /*�������к�����*/	
   payname	varchar2(60),     /*����������*/			
   payactno	varchar2(40),     /*�������˺�*/		
   paycode	varchar2(20),     /*�����˻�������	��ѡ*/
   payertype	varchar2(2),   /*���������� 01����λ��02������*/
   paycertif	varchar2(22),  /*������֤�����ͼ����� ��ѡ 2λ֤�����͡�20λ����*/
   cashname	varchar2(60),     /*�տ�������*/
   cashertype	varchar2(2),      /*�տ�������	01����λ��02������*/
   cashcertif	varchar2(22),  /*�տ���֤�����ͼ�����	��ѡ 2λ֤�����͡�20λ����*/
   txamt	varchar2(18),        /*���ҷ��ż����	3λ���֡�15λ���*/
   paydate	varchar2(8),      /*��������*/
   dscpt	varchar2(20),        /*��; ��ѡ*/
	cashtype varchar2(2), 		   /*���㷽ʽ*/
	txtypeno varchar2(10), 	   /*�������ʹ���*/
	expand varchar2(20)		   /*��չ*/
)tablespace users;
create unique index hvcdtab_idx on hvcdtab(apflg) tablespace indx;
