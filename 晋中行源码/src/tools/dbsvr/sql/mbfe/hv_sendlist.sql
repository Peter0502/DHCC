drop index hv_sendlist_idx;
drop  table hv_sendlist;
create table hv_sendlist(
	br_no varchar2(7),       /*      ������ */                      
	tel varchar2(6),         /*  ����Ա��*/                         
	trace_no number(9),      /*      ��ˮ�� */                      
	ref_no varchar2(20),     /*  ���Ĳο���*/                       
	cmtno varchar2(3),       /*cmt��*/                              
	cmt_title varchar2(30),  /*��������*/                           
	orderno varchar2(8),     /*�������*/                           
	or_br_no varchar2(12),   /*�������к�*/                    
	ac_br_no varchar2(12),   /*�������к�*/                    
	pay_ac_no varchar2(32),  /*�������˺�*/                    
	pay_name varchar2(60),   /*����������*/                    
	cash_ac_no varchar2(32), /*�տ����˺�*/                    
	cash_name varchar2(60),  /*�տ�������*/                    
	tx_amt number(15,2),     /*���׽��*/                      
	resp_flg varchar2(1),    /*�Ƿ��յ�CMT910����0-û��,1-��*/ 
	resp_no varchar2(8),     /*Ӧ����*/                        
	resp_title varchar2(60), /*��Ӧ����*/                      
	brf varchar2(30)         
)tablespace users;                                       
create	 unique index hv_sendlist_idx on hv_sendlist( ref_no)tablespace indx;
