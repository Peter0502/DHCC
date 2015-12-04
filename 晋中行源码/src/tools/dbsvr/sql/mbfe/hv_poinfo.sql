drop index hv_poinfo_idx;
drop  table hv_poinfo;
create table hv_poinfo(
	po_no      varchar2(20),   /* ��Ʊ���� */  
	br_no      varchar2(7),    /* �������� */ 
	bank_type  varchar2(1),    /* �б��־:0-���л�Ʊ 1-���л�Ʊ*/  
	or_br_no   varchar2(12),   /* �������к� */ 
	pay_qs_no  varchar2(12),   /* ֧�����������к�*/ 
	l_tx_date  number(9),      /* �ϴν������� */
	td_cnt     number(9),      /* ���ս��ױ��� */
	lost_cnt   number(9),      /* ��ʧ����ÿ��ʧ�����ʧ����һ */
	note_type  varchar2(3),    /* ƾ֤���� */
	note_no    varchar2(16),   /* ƾ֤�� */ 
	o_note_type varchar2(3),  /* ԭʼƾ֤����*/
	o_note_no varchar2(16),   /* ԭʼƾ֤��*/
	po_bal number(15,2),      /* ���(��Ʊ��Ʊ����)*/
	cash_amt number(15,2),    /* ʵ�ʽ�����/�Ҹ���� */
	fee_amt number(15,2),     /* �����ѽ�� */
	rem_amt number(15,2),     /* ������/�˻ؽ�� */
	cur_id varchar2(3),       /* ���� */
	pay_br_no varchar2(12),   /* ǩ�����к�/�����˿����к� */
	pay_ac_no varchar2(32),   /* �������ʺ�*/
	pay_name varchar2(60),    /* �����˻���*/
	pay_br_name varchar2(60), /* ǩ��������*/
	pay_addr varchar2(60),    /* �����˵�ַ*/
	cash_ac_no varchar2(32),  /* �տ����ʺ�*/
	cash_br_no varchar2(12),  /* �Ҹ����к�*/
	cash_br_name varchar2(60),/* �Ҹ�������*/
	cash_addr varchar2(60),   /* �Ҹ��е�ַ*/
	cash_name varchar2(60),   /* �տ��˻���*/
	hold_ac_no varchar2(32),  /* ��ǰ��Ʊ���ʺ� */
	hold_name varchar2(60),   /* ��ǰ��Ʊ������ */
	l_po_sts varchar2(1),     /* �ϴ��ʻ�״̬ */
	po_sts varchar2(1),       /* ��Ʊ��ǰ״̬��־ 0-�Ǽ� 1-ǩ�� 9-ǩ��ȡ�� 2-��Ʊ�ƴ� 3-����Ҹ� 4-��Ʊ�Ҹ� 5-ȫ����� 6-�����˻� 7-δ���˻� 8-��Ʊ�⸶ A-�⸶ȡ�� B-���� C-����Ҹ�ȡ�� D-��Ʊ�Ǽ�ȡ�� E-��ʧ F-���ʧ G-����δ���˻� H-δ�ý�??I-δ�ó��ʳ��� */ 
	reg_date number(9),       /* �Ǽ�����*/
	sign_date number(9),      /* ǩ������*/
	pay_date number(9),       /* ��ʾ��󸶿�����*/
	pwd varchar2(10),         /* ��Ѻ    */
	po_ind varchar2(1),       /* ��Ʊ���0-��ת�û�Ʊ1-����ת�û�Ʊ2-�ֽ��Ʊ */
	deal_mode varchar2(1),    /* ת���ֽ���ʽ 0-�ֽ� 1 ת��(�ʺ�) 2��Ŀ��ת*/
	drsubject varchar2(7),    /* �����Ŀ */
	crsubject varchar2(7),    /* �ʽ�ȥ�� */
	content varchar2(60),     /* ��ע*/
	tx_chrg_type varchar2(1), /* �����ѱ�־ 0.�ֽ� 1.�ֽ�Ӽ� 2.ת�� 3.ת�ʼӼ� 4.����*/
	checkflag varchar2(1),    /* ���ʼ���־*/
	op_tel varchar2(6),       /* ��������Ա */
	chk_tel varchar2(6),      /* ���ϲ���Ա */
	pass_tel varchar2(6),     /* ��Ѻ����Ա */
	add_pwd_ind varchar2(1),  /* �Ƿ��/��Ѻ���� 1-��Ѻ/��Ѻ 0-δ���� */
	brf varchar2(60)
)tablespace users;
create	 unique index hv_poinfo_idx on hv_poinfo( po_no,pay_br_no)tablespace indx;
