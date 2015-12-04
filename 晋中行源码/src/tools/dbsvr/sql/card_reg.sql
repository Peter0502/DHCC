/***卡登记簿***/
drop table card_reg;
create table card_reg(
	ac_no                   varchar2(19), /***账卡号***/
	msr1                    varchar2(79), /***磁道1***/
	msr2                    varchar2(40), /***磁道2***/
	msr3                    varchar2(107),/***磁道3***/
	cvn1                    varchar2(3),  /***cvn校验1***/
	cvn2                    varchar2(3),  /***cvn校验2***/
	opn_date                number(9),    /* 开户日期 */
	use_flag                varchar2(1),  /***启用标志 1-开启 0- 未开启***/ 
	l_tx_date               number(9),    /***上笔交易日期***/
	atm_max_dr_cash_tx_amt     number(16,2), /***atm日现金取款交易最大金额***/
	atm_max_dr_cash_tx_cnt     number(9),    /***atm日现金取款交易最大笔数***/
	atm_max_dr_tran_tx_amt     number(16,2), /***atm日转帐取款交易最大金额***/
	atm_max_dr_tran_tx_cnt     number(9),    /***atm日转帐取款交易最大笔数***/
	atm_max_cr_cash_tx_amt     number(16,2), /***atm日现金存款交易最大金额***/
	atm_max_cr_cash_tx_cnt     number(9),    /***atm日现金存款交易最大笔数***/
	atm_max_cr_tran_tx_amt     number(16,2), /***atm日转帐存款交易最大金额***/
	atm_max_cr_tran_tx_cnt     number(9),    /***atm日转帐存款交易最大笔数***/
	atm_td_dr_cash_tx_amt      number(16,2), /***atm本日现金取款交易金额***/
	atm_tm_dr_cash_tx_amt      number(16,2), /***atm本月现金取款交易金额***/
	atm_ts_dr_cash_tx_amt      number(16,2), /***atm本季现金取款交易金额***/
	atm_ty_dr_cash_tx_amt      number(16,2), /***atm本年现金取款交易金额***/
	atm_td_cr_cash_tx_amt      number(16,2), /***atm本日现金存款交易金额***/
	atm_tm_cr_cash_tx_amt      number(16,2), /***atm本月现金存款交易金额***/
	atm_ts_cr_cash_tx_amt      number(16,2), /***atm本季现金存款交易金额***/
	atm_ty_cr_cash_tx_amt      number(16,2), /***atm本年现金存款交易金额***/
	atm_td_dr_cash_tx_cnt      number(9),    /***atm本日现金取款交易笔数***/
	atm_tm_dr_cash_tx_cnt      number(9),    /***atm本月现金取款交易笔数***/
	atm_ts_dr_cash_tx_cnt      number(9),    /***atm本季现金取款交易笔数***/
	atm_ty_dr_cash_tx_cnt      number(9),    /***atm本年现金取款交易笔数***/
	atm_td_cr_cash_tx_cnt      number(9),    /***atm本日现金存款交易笔数***/
	atm_tm_cr_cash_tx_cnt      number(9),    /***atm本月现金存款交易笔数***/
	atm_ts_cr_cash_tx_cnt      number(9),    /***atm本季现金存款交易笔数***/
	atm_ty_cr_cash_tx_cnt      number(9),    /***atm本年现金存款交易笔数***/
	atm_td_dr_tran_tx_amt      number(16,2), /***atm本日转帐取款交易金额***/
	atm_tm_dr_tran_tx_amt      number(16,2), /***atm本月转帐取款交易金额***/
	atm_ts_dr_tran_tx_amt      number(16,2), /***atm本季转帐取款交易金额***/
	atm_ty_dr_tran_tx_amt      number(16,2), /***atm本年转帐取款交易金额***/
	atm_td_cr_tran_tx_amt      number(16,2), /***atm本日转帐存款交易金额***/
	atm_tm_cr_tran_tx_amt      number(16,2), /***atm本月转帐存款交易金额***/
	atm_ts_cr_tran_tx_amt      number(16,2), /***atm本季转帐存款交易金额***/
	atm_ty_cr_tran_tx_amt      number(16,2), /***atm本年转帐存款交易金额***/
	atm_td_dr_tran_tx_cnt      number(9),    /***atm本日转帐取款交易笔数***/
	atm_tm_dr_tran_tx_cnt      number(9),    /***atm本月转帐取款交易笔数***/
	atm_ts_dr_tran_tx_cnt      number(9),    /***atm本季转帐取款交易笔数***/
	atm_ty_dr_tran_tx_cnt      number(9),    /***atm本年转帐取款交易笔数***/
	atm_td_cr_tran_tx_cnt      number(9),    /***atm本日转帐存款交易笔数***/
	atm_tm_cr_tran_tx_cnt      number(9),    /***atm本月转帐存款交易笔数***/
	atm_ts_cr_tran_tx_cnt      number(9),    /***atm本季转帐存款交易笔数***/
	atm_ty_cr_tran_tx_cnt      number(9),    /***atm本年转帐存款交易笔数***/
	atm_td_qy_cnt           number(9),    /***atm本日查询笔数***/
	atm_tm_qy_cnt           number(9),    /***atm本月查询笔数***/
	atm_ts_qy_cnt           number(9),    /***atm本季查询笔数***/
	atm_ty_qy_cnt           number(9),    /***atm本年查询笔数***/
	pos_td_tx_amt           number(16,2), /***pos本日交易金额 **/
	pos_tm_tx_amt           number(16,2), /***pos本月交易金额 **/
	pos_ts_tx_amt           number(16,2), /***pos本季交易金额 **/
	pos_ty_tx_amt           number(16,2), /***pos本年交易金额 **/
	pos_td_tx_cnt           number(9),    /***pos本日交易笔数 **/
	pos_tm_tx_cnt           number(9),    /* pos本月交易笔数 **/
	pos_ts_tx_cnt           number(9),    /* pos本季交易笔数 **/
	pos_ty_tx_cnt           number(9),    /* pos本年交易笔数 **/
	pos_td_qy_cnt           number(9),    /***pos本日查询笔数 **/
	pos_tm_qy_cnt           number(9),    /* pos本月查询笔数 **/
	pos_ts_qy_cnt           number(9),    /* pos本季查询笔数 **/
	pos_ty_qy_cnt           number(9),    /* pos本年查询笔数 **/
	tdcnt                   number(9),    /***当日交易笔数***/
	wh_atm_td_dr_cash_tx_amt   number(16,2), /***当日外行atm现金取款交易金额***/
	wh_atm_tm_dr_cash_tx_amt   number(16,2), /***当月外行atm现金取款交易金额***/
	wh_atm_ts_dr_cash_tx_amt   number(16,2), /***当季外行atm现金取款交易金额***/
	wh_atm_ty_dr_cash_tx_amt   number(16,2), /***当年外行atm现金取款交易金额***/
	wh_atm_td_dr_cash_tx_cnt   number(9),    /***当日外行atm现金取款交易笔数***/
	wh_atm_tm_dr_cash_tx_cnt   number(9),    /***当月外行atm现金取款交易笔数***/
	wh_atm_ts_dr_cash_tx_cnt   number(9),    /***当季外行atm现金取款交易笔数***/
	wh_atm_ty_dr_cash_tx_cnt   number(9),    /***当年外行atm现金取款交易笔数***/
	wh_atm_td_dr_tran_tx_amt   number(16,2), /***当日外行atm转帐取款交易金额***/
	wh_atm_tm_dr_tran_tx_amt   number(16,2), /***当月外行atm转帐取款交易金额***/
	wh_atm_ts_dr_tran_tx_amt   number(16,2), /***当季外行atm转帐取款交易金额***/
	wh_atm_ty_dr_tran_tx_amt   number(16,2), /***当年外行atm转帐取款交易金额***/
	wh_atm_td_dr_tran_tx_cnt   number(9),    /***当日外行atm转帐取款交易笔数***/
	wh_atm_tm_dr_tran_tx_cnt   number(9),    /***当月外行atm转帐取款交易笔数***/
	wh_atm_ts_dr_tran_tx_cnt   number(9),    /***当季外行atm转帐取款交易笔数***/
	wh_atm_ty_dr_tran_tx_cnt   number(9),    /***当年外行atm转帐取款交易笔数***/
	wh_atm_td_cr_cash_tx_amt   number(16,2), /***当日外行atm现金存款交易金额***/
	wh_atm_tm_cr_cash_tx_amt   number(16,2), /***当月外行atm现金存款交易金额***/
	wh_atm_ts_cr_cash_tx_amt   number(16,2), /***当季外行atm现金存款交易金额***/
	wh_atm_ty_cr_cash_tx_amt   number(16,2), /***当年外行atm现金存款交易金额***/
	wh_atm_td_cr_cash_tx_cnt   number(9),    /***当日外行atm现金存款交易笔数***/
	wh_atm_tm_cr_cash_tx_cnt   number(9),    /***当月外行atm现金存款交易笔数***/
	wh_atm_ts_cr_cash_tx_cnt   number(9),    /***当季外行atm现金存款交易笔数***/
	wh_atm_ty_cr_cash_tx_cnt   number(9),    /***当年外行atm现金存款交易笔数***/
	wh_atm_td_cr_tran_tx_amt   number(16,2), /***当日外行atm转帐存款交易金额***/
	wh_atm_tm_cr_tran_tx_amt   number(16,2), /***当月外行atm转帐存款交易金额***/
	wh_atm_ts_cr_tran_tx_amt   number(16,2), /***当季外行atm转帐存款交易金额***/
	wh_atm_ty_cr_tran_tx_amt   number(16,2), /***当年外行atm转帐存款交易金额***/
	wh_atm_td_cr_tran_tx_cnt   number(9),    /***当日外行atm转帐存款交易笔数***/
	wh_atm_tm_cr_tran_tx_cnt   number(9),    /***当月外行atm转帐存款交易笔数***/
	wh_atm_ts_cr_tran_tx_cnt   number(9),    /***当季外行atm转帐存款交易笔数***/
	wh_atm_ty_cr_tran_tx_cnt   number(9),    /***当年外行atm转帐存款交易笔数***/
	wh_atm_tx_free_cnt      number(9),    /***周期内外行交易免费笔数***/
	wh_atm_td_qy_cnt        number(9),    /***当日外行atm查询笔数***/
	wh_atm_tm_qy_cnt        number(9),    /***当月外行atm查询笔数***/
	wh_atm_ts_qy_cnt        number(9),    /***当季外行atm查询笔数***/
	wh_atm_ty_qy_cnt        number(9),    /***当年外行atm查询笔数***/
	wh_atm_qy_free_cnt      number(9),    /***周期内他行atm查询免费笔数***/
	wh_pos_tx_free_cnt      number(9),    /***周期内外行pos交易免费笔数***/
	wh_pos_td_tx_amt        number(16,2), /* 当日外行pos交易金额 **/
	wh_pos_tm_tx_amt        number(16,2), /***当月外行pos交易金额***/
	wh_pos_ts_tx_amt        number(16,2), /***当季外行pos交易金额***/ 
	wh_pos_ty_tx_amt        number(16,2), /***当年外行pos交易金额***/
	wh_pos_td_tx_cnt        number(9),    /***当日外行pos交易笔数***/
	wh_pos_tm_tx_cnt        number(9),    /***当月外行pos交易笔数***/
	wh_pos_ts_tx_cnt        number(9),    /***当季外行pos交易笔数***/ 
	wh_pos_ty_tx_cnt        number(9),    /***当年外行pos交易笔数***/
	wh_pos_td_qy_cnt        number(9),    /***当日外行pos查询笔数***/
	wh_pos_tm_qy_cnt        number(9),    /***当月外行pos查询笔数***/
	wh_pos_ts_qy_cnt        number(9),    /***当季外行pos查询笔数***/
	wh_pos_ty_qy_cnt        number(9),    /***当年外行pos查询笔数***/
	wh_pos_qy_free_cnt      number(9),    /***周期内他行pos查询免费笔数***/
	add_pswd_cnt		number(9),    /***密码错误次数 ***/
	cif_no                  number(9),    /***客户号***/
	card_lx                 varchar(1),   /***卡类型***/
	card_zl                 varchar2(1),  /***卡种类***/
	filler1                 varchar2(30), /***备注***/
	filler2                 varchar2(30), /***备注2***/
	filler3                 varchar2(30), /***备注3***/
	mac                     varchar2(16)  /***密押***/
)tablespace users;
create unique index card_reg_idx on card_reg(ac_no) tablespace indx;


