drop index hv_chkagemr_idx;
drop  table hv_chkagemr;
create table hv_chkagemr(
	orderno varchar2(8),        /**֧���������(���Ͳ����������Ѿ�����) *key*/               
	pay_qs_no varchar2(12),     /**�����������к�(�ʺ�) accountno       *key*/               
	br_no varchar2(7),          /* �������к� */                                             
	tx_date number(9),          /* ��������   */                                             
	cmtno varchar2(3),          /* CMT���    */                                             
	o_cmtno varchar2(3),        /* �˻�֧��ԭ����CMT���    */                               
	tx_time number(9),          /* ����ʱ��   */                                             
	cur_no varchar2(3),         /* ���ҷ���   */                                             
	tx_amt number(15,2),        /* ���׽��   */                                             
	other_amt number(15,2),     /* ������ */                                               
	jf_amt number(15,2),        /* �ܸ����/ʵ�ʽ����� */                                  
	or_br_no varchar2(12),      /* �������к� */                                             
	pay_no varchar2(12),        /* �����˿������к� */                                       
	pay_ac_no varchar2(32),     /* �������ʺ� */                                             
	payname varchar2(60),       /* ���������� */                                             
	payaddress varchar2(60),    /* �����˵�ַ */                                             
	cashqsactno varchar2(12),   /* �����������ʺ� */                                         
	acbrno varchar2(12),        /* �������к� */                                             
	cashno varchar2(12),        /* �տ��˿������к� */                                       
	cash_ac_no varchar2(32),    /* �տ����ʺ� */                                             
	cash_name varchar2(60),     /* �տ������� */                                             
	cash_addr varchar2(60),     /* �տ��˵�ַ */                                             
	yw_type varchar2(2),        /* ҵ������   */                                             
	sendco varchar2(4),         /* �������Ĵ��� */                                           
	receco varchar2(4),         /* �ձ����Ĵ��� */                                           
	operlevel varchar2(1),      /* ���ȼ�0-һ��1-����2-�ؼ�(����ս�����֧��)*/              
	obrno varchar2(12),          /* �Է������� û��ʵ������ */                               
	voctype varchar2(2),                                                                     
	vocnum varchar2(12),                                                                     
	p_date number(9),           /* Ʊ������ */                                               
	p_num varchar2(8),          /* Ʊ�ݺ� */                                                 
	hp_type varchar2(1),        /* ��Ʊ��� */                                               
	pass_wd varchar2(10),       /* ��Ʊ��Ѻ */                                               
	inst_rate number(8,5),      /* ������� */                                               
	term number(9),             /* ������� */                                               
	o_tx_date varchar2(8),      /* �˻�ר��ԭί������ */                                     
	ou_tx_date varchar2(8),     /* �˻�ר��ԭ�˻��������� */                                 
	o_orderno_hv varchar2(8),   /* �˻�ר��ԭ֧��������� */                                 
	o_tx_type varchar2(1),      /* ԭ�������� */                                             
	p_orderno_hv varchar2(8),   /* ����ר��--������֧��������� */                           
	dc varchar2(1),             /* �����־  */                                              
	pay_date number(9),         /* ��ʾ�������� */                                           
	intst number(15,2),         /* ������Ϣ  */                                              
	reason varchar2(60),        /* reason */                                                 
	repson varchar2(1),         /* repson */                                                 
	chk_flg varchar2(1),        /* �Ƿ�˶Ա�־ 0-δ�˶ԣ�1-�˶� */                          
	brf1 varchar2(30),          /* ��ע�ֶ�1 */                                              
	brf2 varchar2(30)           /* ��ע�ֶ�2 */                                              
)tablespace users;        
create	 unique index hv_chkagemr_idx on hv_chkagemr( orderno,or_br_no)tablespace indx;
