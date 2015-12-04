drop  table cif_tranq_stat;
create table cif_tranq_stat(
	cif_no number(9),
	cur_no varchar2(2),
	"date" number(9),
	t_cash_cnt number(9),
	t_cash_amt number(16,2),
	t_turn_cnt number(9),
	t_turn_amt number(16,2),
	t_atm_cnt number(9),
	t_atm_amt number(16,2),
	t_pos_cnt number(9),
	t_pos_amt number(16,2),
	x_cash_cnt number(9),
	x_cash_amt number(16,2),
	x_turn_cnt number(9),
	x_turn_amt number(16,2),
	x_atm_cnt number(9),
	x_atm_amt number(16,2),
	x_pos_cnt number(9),
	x_pos_amt number(16,2),
	m_cash_cnt number(9),
	m_cash_amt number(16,2),
	m_turn_cnt number(9),
	m_turn_amt number(16,2),
	m_atm_cnt number(9),
	m_atm_amt number(16,2),
	m_pos_cnt number(9),
	m_pos_amt number(16,2),
	q_cash_cnt number(9),
	q_cash_amt number(16,2),
	q_turn_cnt number(9),
	q_turn_amt number(16,2),
	q_atm_cnt number(9),
	q_atm_amt number(16,2),
	q_pos_cnt number(9),
	q_pos_amt number(16,2),
	hy_cash_cnt number(9),
	hy_cash_amt number(16,2),
	hy_turn_cnt number(9),
	hy_turn_amt number(16,2),
	hy_atm_cnt number(9),
	hy_atm_amt number(16,2),
	hy_pos_cnt number(9),
	hy_pos_amt number(16,2),
	y_cash_cnt number(9),
	y_cash_amt number(16,2),
	y_turn_cnt number(9),
	y_turn_amt number(16,2),
	y_atm_cnt number(9),
	y_atm_amt number(16,2),
	y_pos_cnt number(9),
	y_pos_amt number(16,2) 
)tablespace users;
