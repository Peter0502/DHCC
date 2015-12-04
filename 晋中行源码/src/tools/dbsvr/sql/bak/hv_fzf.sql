drop index hv_fzf_idx;
drop  table hv_fzf;
create table hv_fzf(
	otype       varchar2(1),   /* 1-��2��С�� */
	or_br_no    varchar2(12),  /* �������к� */
	orderno     varchar2(8),   /* ��֧���������*/
	tx_date     number(9),     /* �������� */
	wt_date     number(9),     /* ί������ */
	br_no       varchar2(7),   /* �������к�*/
	hv_fzf_sts  varchar2(1),   /* ��֧������״̬*/
	cmtno       varchar2(3),   /* ���cmtno,С��pkgno */
	tx_time     number(9),     /* ����ʱ��*/
	pay_qs_no   varchar2(12),  /* �����������к�(�ʺ�) accountno */
	ac_br_no    varchar2(12),  /* �������к� */
	cash_qs_no  varchar2(12),  /* �����������ʺ� */
	o_wt_date   number(9),     /* ԭί������--֧�����׵�ί������ */
	o_tx_date   number(9),     /* ԭ��������--֧�����׵�ί������ */
	o_or_br_no  varchar2(12),  /* ԭ(֧������)�������к�*/
	o_ac_br_no  varchar2(12),  /* ԭ(֧������)�������к�*/
	o_orderno   varchar2(8),   /* ԭ֧���������*/
	o_cur_ind   varchar2(3),   /* ԭ����*/
	o_tx_amt    number(15,2),  /* ԭ���׽�� */
	o_cmtno     varchar2(3),   /* ԭcmtno ���cmtno,С��pkgno */
	o_tx_type   varchar2(1),   /* ԭ��������*/
	sendco      varchar2(4),   /* �������Ĵ��� */
	receco      varchar2(4),   /* �ձ����Ĵ��� */
	req_date    number(9),     /* ��������*/
	req_br_no   varchar2(12),  /* ������*/
	req_content varchar2(255), /* ��������*/
	req_orderno varchar2(8),   /* ������� */
	res_date    number(9),     /* Ӧ������*/
	res_br_no   varchar2(12),  /* Ӧ����*/
	res_content varchar2(255), /* Ӧ������*/
	res_orderno varchar2(8),   /* Ӧ����� */
	tx_sts      varchar2(1),   /* �ý��׵�״̬,¼��,����,����,����*/
	res_sts     varchar2(1),   /* Ӧ����0-���� 1-���ܳ���/0-���˻� 1-�����˻�
				С���ʾΪ1=���� 2=������*/
	addid       varchar2(8),   /* ������*/
	refno       varchar2(20),  /* ���Ĳο��� */
	lw_ind      varchar2(1),   /* ������ʶ */
	hv_prt_ind  varchar2(10),  /* ��ӡ��־��0δ��ӡ��1�Ѵ�ӡ*/
	hv_brf      varchar2(60),  /* ��ע */
	tel         varchar2(6),   /* ����Ա */
	trace_no    number(9),     /* ��ˮ��*/
	send_tel    varchar2(6),   /* ���Ͳ���Ա */
	send_trace_no  number(9) , /* ������ˮ�� */
	reqtype    varchar2(1),    /* �������� 0-���� 1-���� */
	o_pack_date   number(9),   /* ԭ������ */
	o_packid   varchar2(8),   /* ԭ����� */
	resp1      varchar2(8),   /* ccpcӦ���� */
	otxnum     varchar2(5),   /* ԭҵ������ */
	filler     varchar2(50)
)tablespace users;
create	 unique index hv_fzf_idx on hv_fzf( otype,or_br_no,wt_date,orderno,cmtno)tablespace indx;
