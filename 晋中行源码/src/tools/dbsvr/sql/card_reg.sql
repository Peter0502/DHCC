/***���Ǽǲ�***/
drop table card_reg;
create table card_reg(
	ac_no                   varchar2(19), /***�˿���***/
	msr1                    varchar2(79), /***�ŵ�1***/
	msr2                    varchar2(40), /***�ŵ�2***/
	msr3                    varchar2(107),/***�ŵ�3***/
	cvn1                    varchar2(3),  /***cvnУ��1***/
	cvn2                    varchar2(3),  /***cvnУ��2***/
	opn_date                number(9),    /* �������� */
	use_flag                varchar2(1),  /***���ñ�־ 1-���� 0- δ����***/ 
	l_tx_date               number(9),    /***�ϱʽ�������***/
	atm_max_dr_cash_tx_amt     number(16,2), /***atm���ֽ�ȡ��������***/
	atm_max_dr_cash_tx_cnt     number(9),    /***atm���ֽ�ȡ���������***/
	atm_max_dr_tran_tx_amt     number(16,2), /***atm��ת��ȡ��������***/
	atm_max_dr_tran_tx_cnt     number(9),    /***atm��ת��ȡ���������***/
	atm_max_cr_cash_tx_amt     number(16,2), /***atm���ֽ���������***/
	atm_max_cr_cash_tx_cnt     number(9),    /***atm���ֽ����������***/
	atm_max_cr_tran_tx_amt     number(16,2), /***atm��ת�ʴ��������***/
	atm_max_cr_tran_tx_cnt     number(9),    /***atm��ת�ʴ���������***/
	atm_td_dr_cash_tx_amt      number(16,2), /***atm�����ֽ�ȡ��׽��***/
	atm_tm_dr_cash_tx_amt      number(16,2), /***atm�����ֽ�ȡ��׽��***/
	atm_ts_dr_cash_tx_amt      number(16,2), /***atm�����ֽ�ȡ��׽��***/
	atm_ty_dr_cash_tx_amt      number(16,2), /***atm�����ֽ�ȡ��׽��***/
	atm_td_cr_cash_tx_amt      number(16,2), /***atm�����ֽ���׽��***/
	atm_tm_cr_cash_tx_amt      number(16,2), /***atm�����ֽ���׽��***/
	atm_ts_cr_cash_tx_amt      number(16,2), /***atm�����ֽ���׽��***/
	atm_ty_cr_cash_tx_amt      number(16,2), /***atm�����ֽ���׽��***/
	atm_td_dr_cash_tx_cnt      number(9),    /***atm�����ֽ�ȡ��ױ���***/
	atm_tm_dr_cash_tx_cnt      number(9),    /***atm�����ֽ�ȡ��ױ���***/
	atm_ts_dr_cash_tx_cnt      number(9),    /***atm�����ֽ�ȡ��ױ���***/
	atm_ty_dr_cash_tx_cnt      number(9),    /***atm�����ֽ�ȡ��ױ���***/
	atm_td_cr_cash_tx_cnt      number(9),    /***atm�����ֽ���ױ���***/
	atm_tm_cr_cash_tx_cnt      number(9),    /***atm�����ֽ���ױ���***/
	atm_ts_cr_cash_tx_cnt      number(9),    /***atm�����ֽ���ױ���***/
	atm_ty_cr_cash_tx_cnt      number(9),    /***atm�����ֽ���ױ���***/
	atm_td_dr_tran_tx_amt      number(16,2), /***atm����ת��ȡ��׽��***/
	atm_tm_dr_tran_tx_amt      number(16,2), /***atm����ת��ȡ��׽��***/
	atm_ts_dr_tran_tx_amt      number(16,2), /***atm����ת��ȡ��׽��***/
	atm_ty_dr_tran_tx_amt      number(16,2), /***atm����ת��ȡ��׽��***/
	atm_td_cr_tran_tx_amt      number(16,2), /***atm����ת�ʴ��׽��***/
	atm_tm_cr_tran_tx_amt      number(16,2), /***atm����ת�ʴ��׽��***/
	atm_ts_cr_tran_tx_amt      number(16,2), /***atm����ת�ʴ��׽��***/
	atm_ty_cr_tran_tx_amt      number(16,2), /***atm����ת�ʴ��׽��***/
	atm_td_dr_tran_tx_cnt      number(9),    /***atm����ת��ȡ��ױ���***/
	atm_tm_dr_tran_tx_cnt      number(9),    /***atm����ת��ȡ��ױ���***/
	atm_ts_dr_tran_tx_cnt      number(9),    /***atm����ת��ȡ��ױ���***/
	atm_ty_dr_tran_tx_cnt      number(9),    /***atm����ת��ȡ��ױ���***/
	atm_td_cr_tran_tx_cnt      number(9),    /***atm����ת�ʴ��ױ���***/
	atm_tm_cr_tran_tx_cnt      number(9),    /***atm����ת�ʴ��ױ���***/
	atm_ts_cr_tran_tx_cnt      number(9),    /***atm����ת�ʴ��ױ���***/
	atm_ty_cr_tran_tx_cnt      number(9),    /***atm����ת�ʴ��ױ���***/
	atm_td_qy_cnt           number(9),    /***atm���ղ�ѯ����***/
	atm_tm_qy_cnt           number(9),    /***atm���²�ѯ����***/
	atm_ts_qy_cnt           number(9),    /***atm������ѯ����***/
	atm_ty_qy_cnt           number(9),    /***atm�����ѯ����***/
	pos_td_tx_amt           number(16,2), /***pos���ս��׽�� **/
	pos_tm_tx_amt           number(16,2), /***pos���½��׽�� **/
	pos_ts_tx_amt           number(16,2), /***pos�������׽�� **/
	pos_ty_tx_amt           number(16,2), /***pos���꽻�׽�� **/
	pos_td_tx_cnt           number(9),    /***pos���ս��ױ��� **/
	pos_tm_tx_cnt           number(9),    /* pos���½��ױ��� **/
	pos_ts_tx_cnt           number(9),    /* pos�������ױ��� **/
	pos_ty_tx_cnt           number(9),    /* pos���꽻�ױ��� **/
	pos_td_qy_cnt           number(9),    /***pos���ղ�ѯ���� **/
	pos_tm_qy_cnt           number(9),    /* pos���²�ѯ���� **/
	pos_ts_qy_cnt           number(9),    /* pos������ѯ���� **/
	pos_ty_qy_cnt           number(9),    /* pos�����ѯ���� **/
	tdcnt                   number(9),    /***���ս��ױ���***/
	wh_atm_td_dr_cash_tx_amt   number(16,2), /***��������atm�ֽ�ȡ��׽��***/
	wh_atm_tm_dr_cash_tx_amt   number(16,2), /***��������atm�ֽ�ȡ��׽��***/
	wh_atm_ts_dr_cash_tx_amt   number(16,2), /***��������atm�ֽ�ȡ��׽��***/
	wh_atm_ty_dr_cash_tx_amt   number(16,2), /***��������atm�ֽ�ȡ��׽��***/
	wh_atm_td_dr_cash_tx_cnt   number(9),    /***��������atm�ֽ�ȡ��ױ���***/
	wh_atm_tm_dr_cash_tx_cnt   number(9),    /***��������atm�ֽ�ȡ��ױ���***/
	wh_atm_ts_dr_cash_tx_cnt   number(9),    /***��������atm�ֽ�ȡ��ױ���***/
	wh_atm_ty_dr_cash_tx_cnt   number(9),    /***��������atm�ֽ�ȡ��ױ���***/
	wh_atm_td_dr_tran_tx_amt   number(16,2), /***��������atmת��ȡ��׽��***/
	wh_atm_tm_dr_tran_tx_amt   number(16,2), /***��������atmת��ȡ��׽��***/
	wh_atm_ts_dr_tran_tx_amt   number(16,2), /***��������atmת��ȡ��׽��***/
	wh_atm_ty_dr_tran_tx_amt   number(16,2), /***��������atmת��ȡ��׽��***/
	wh_atm_td_dr_tran_tx_cnt   number(9),    /***��������atmת��ȡ��ױ���***/
	wh_atm_tm_dr_tran_tx_cnt   number(9),    /***��������atmת��ȡ��ױ���***/
	wh_atm_ts_dr_tran_tx_cnt   number(9),    /***��������atmת��ȡ��ױ���***/
	wh_atm_ty_dr_tran_tx_cnt   number(9),    /***��������atmת��ȡ��ױ���***/
	wh_atm_td_cr_cash_tx_amt   number(16,2), /***��������atm�ֽ���׽��***/
	wh_atm_tm_cr_cash_tx_amt   number(16,2), /***��������atm�ֽ���׽��***/
	wh_atm_ts_cr_cash_tx_amt   number(16,2), /***��������atm�ֽ���׽��***/
	wh_atm_ty_cr_cash_tx_amt   number(16,2), /***��������atm�ֽ���׽��***/
	wh_atm_td_cr_cash_tx_cnt   number(9),    /***��������atm�ֽ���ױ���***/
	wh_atm_tm_cr_cash_tx_cnt   number(9),    /***��������atm�ֽ���ױ���***/
	wh_atm_ts_cr_cash_tx_cnt   number(9),    /***��������atm�ֽ���ױ���***/
	wh_atm_ty_cr_cash_tx_cnt   number(9),    /***��������atm�ֽ���ױ���***/
	wh_atm_td_cr_tran_tx_amt   number(16,2), /***��������atmת�ʴ��׽��***/
	wh_atm_tm_cr_tran_tx_amt   number(16,2), /***��������atmת�ʴ��׽��***/
	wh_atm_ts_cr_tran_tx_amt   number(16,2), /***��������atmת�ʴ��׽��***/
	wh_atm_ty_cr_tran_tx_amt   number(16,2), /***��������atmת�ʴ��׽��***/
	wh_atm_td_cr_tran_tx_cnt   number(9),    /***��������atmת�ʴ��ױ���***/
	wh_atm_tm_cr_tran_tx_cnt   number(9),    /***��������atmת�ʴ��ױ���***/
	wh_atm_ts_cr_tran_tx_cnt   number(9),    /***��������atmת�ʴ��ױ���***/
	wh_atm_ty_cr_tran_tx_cnt   number(9),    /***��������atmת�ʴ��ױ���***/
	wh_atm_tx_free_cnt      number(9),    /***���������н�����ѱ���***/
	wh_atm_td_qy_cnt        number(9),    /***��������atm��ѯ����***/
	wh_atm_tm_qy_cnt        number(9),    /***��������atm��ѯ����***/
	wh_atm_ts_qy_cnt        number(9),    /***��������atm��ѯ����***/
	wh_atm_ty_qy_cnt        number(9),    /***��������atm��ѯ����***/
	wh_atm_qy_free_cnt      number(9),    /***����������atm��ѯ��ѱ���***/
	wh_pos_tx_free_cnt      number(9),    /***����������pos������ѱ���***/
	wh_pos_td_tx_amt        number(16,2), /* ��������pos���׽�� **/
	wh_pos_tm_tx_amt        number(16,2), /***��������pos���׽��***/
	wh_pos_ts_tx_amt        number(16,2), /***��������pos���׽��***/ 
	wh_pos_ty_tx_amt        number(16,2), /***��������pos���׽��***/
	wh_pos_td_tx_cnt        number(9),    /***��������pos���ױ���***/
	wh_pos_tm_tx_cnt        number(9),    /***��������pos���ױ���***/
	wh_pos_ts_tx_cnt        number(9),    /***��������pos���ױ���***/ 
	wh_pos_ty_tx_cnt        number(9),    /***��������pos���ױ���***/
	wh_pos_td_qy_cnt        number(9),    /***��������pos��ѯ����***/
	wh_pos_tm_qy_cnt        number(9),    /***��������pos��ѯ����***/
	wh_pos_ts_qy_cnt        number(9),    /***��������pos��ѯ����***/
	wh_pos_ty_qy_cnt        number(9),    /***��������pos��ѯ����***/
	wh_pos_qy_free_cnt      number(9),    /***����������pos��ѯ��ѱ���***/
	add_pswd_cnt		number(9),    /***���������� ***/
	cif_no                  number(9),    /***�ͻ���***/
	card_lx                 varchar(1),   /***������***/
	card_zl                 varchar2(1),  /***������***/
	filler1                 varchar2(30), /***��ע***/
	filler2                 varchar2(30), /***��ע2***/
	filler3                 varchar2(30), /***��ע3***/
	mac                     varchar2(16)  /***��Ѻ***/
)tablespace users;
create unique index card_reg_idx on card_reg(ac_no) tablespace indx;


