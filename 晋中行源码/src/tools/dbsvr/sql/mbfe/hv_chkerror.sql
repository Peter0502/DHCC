drop index hv_chkerror_idx;
drop  table hv_chkerror;
create table hv_chkerror(
	br_no varchar2(7),         /*������*/                                   
	cmtno varchar2(3),         /*���ı��*/                                 
	orderno varchar2(8),       /*�������(dhcc build)*/                     
	pay_qs_no varchar2(12),    /*�����������к�*/                           
	or_br_no varchar2(12),     /*���������к�*/                             
	tx_date number(9),         /*��������*/                                 
	tx_time number(9),         /*����ʱ��*/                                 
	tx_amt number(15,2),       /* ����Ľ�� */                             
	tx_amt_ccb number(15,2),   /*������ҵ���н��׽��City Credit Bank(CCB)*/
	tx_amt_ccpc number(15,2),  /*ccpc���׽��*/                             
	o_sts varchar2(1),         /* ԭʼ״̬ */                               
	sts varchar2(1),           /* ״̬*/                                    
	chk_flg varchar2(1),       /* ����״̬ */                               
	title varchar2(30),        /* ���� */                                   
	prt_flag varchar2(1),      /*��ӡ��־��0δ��ӡ��1�Ѵ�ӡ*/               
	brf varchar2(60)           /*�����ֶ�*/                                 
)tablespace users;           
create	 unique index hv_chkerror_idx on hv_chkerror( orderno,or_br_no,tx_date)tablespace indx;
