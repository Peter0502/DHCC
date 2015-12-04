drop index hv_zf_idx;
drop  table hv_zf;
create table hv_zf(
	br_no          varchar2(7),   /*�������к�*/
	tx_date        number(9),     /*��������*/
	wt_date        number(9),     /*ί������*/
	tx_time        number(9),     /*����ʱ��*/
	orderno        varchar2(8),   /*���еĽ������*/
	cur_no         varchar2(3),   /*���ҷ���*/ 
	note_type      varchar2(3),   /*ƾ֤���� */
	note_no        varchar2(16),  /*ƾ֤�� */  
	tx_amt         number(15,2),  /*���׽�� */
	tx_chrg_ind    varchar2(1),   /*�����ѱ�־ 0.�ֽ� 1.�ֽ�Ӽ� 2.ת�� 3.ת�ʼӼ� 4.����*/
	chrg_amt       number(15,2),  /*������ */
	pay_qs_no      varchar2(12),  /*�����������к�(�ʺ�) accountno */
	or_br_no       varchar2(12),  /*�������к�--�˻㽻�׵�ԭ������ */
	pay_opn_br_no  varchar2(12),  /*�����˿������к� */
	pay_ac_no      varchar2(32),  /*�������ʺ� */      
	pay_name       varchar2(60),  /*����������--�˻㽻�׵�ԭ�տ��˻��� */
	pay_addr       varchar2(60),  /*�����˵�ַ */    
	cash_qs_no     varchar2(12),  /*�����������к� */
	ac_br_no       varchar2(12),  /*�������к�--�˻㽻�׵�ԭ������ */
	cash_opn_br_no varchar2(12),  /*�տ��˿������к� */
	cash_ac_no     varchar2(32),  /*�տ����ʺ� */      
	cash_name      varchar2(60),  /*�տ�������--�˻㽻�׵�ԭ�����˻��� */
	cash_addr      varchar2(60),  /*�տ��˵�ַ */  
	yw_type        varchar2(2),   /*ҵ������ */    
	sendco         varchar2(4),   /*�������Ĵ��� */
	receco         varchar2(4),   /*�ձ����Ĵ��� */
	cmtno          varchar2(3),   /*CMT��� */     
	beg_sts        varchar2(1),   /*���statԭʼ״̬����ʱ��ල�� */
	operlevel      varchar2(1),   /*���ȼ���0-һ��1-����2-�ؼ�(����ս�����֧��)*/
	hv_sts         varchar2(1),   /*״̬ �����hv_define.h���� */
	tel            varchar2(6),   /*����ԱԱ */
	trace_no       number(9),     /*��ˮ��*/   
	hv_osts        varchar2(1),   /*�˻㽻���õ���ԭ֧�����ʽ���״̬ */
	checkflag      varchar2(1),   /*���ʼ���־*/
	hv_prt_ind     varchar2(10),  /*��ӡ��־��0δ��ӡ��1�Ѵ�ӡ*/
	resend_ind     varchar2(1),   /*�ܾ��ط���־0δ���·��ͣ�1��ʾ�Ѿ����·��;�����,2����ʾ�ֹ��˻�֧�У�M��ʾ���·��͵������� */
	resend_date    number(9),     /*�ܾ����·������� */
	o_orderno      varchar2(8),   /*ԭ֧���������--�˻㽻���õ� */
	o_tx_date      number(9),     /*ԭ֧����������--�˻㽻���õ� */
	o_wt_date      number(9),     /*ԭ֧������ί������--�˻㽻���õ� */ 
	o_cmtno        varchar2(3),   /*ԭcmtno--�˻㽻���õ� */
	lw_ind         varchar2(1),   /*������ʶ 1-�� 2-�� */
	addid          varchar2(8),   /*������ id --addit��*/
	refno          varchar2(20),  /*���Ĳο��� */
	auth_tel       varchar2(6),   /*��Ȩ����Ա*/
	auth_trace_no  number(9),     /*��Ȩ����Ա��ˮ��*/      
	chk            varchar2(6),   /*���˲���Ա*/
	chk_trace_no   number(9),     /*���˲���Ա��ˮ��*/      
	send_tel       varchar2(6),   /*���Ͳ���Ա*/
	send_tel_trace_no number(9),  /*���Ͳ���Ա��ˮ��*/
	dealerr_date      number(9),  /*���ʴ�������*/
	dealerr_tel       varchar2(6),/*���ʴ����Ա*/
	resp1             varchar2(8),/*Ӧ����*/
	hv_brf            varchar2(60)/*��ע */ 
)tablespace users;
create	 unique index hv_zf_idx on hv_zf( or_br_no,wt_date,orderno,cmtno)tablespace indx;
