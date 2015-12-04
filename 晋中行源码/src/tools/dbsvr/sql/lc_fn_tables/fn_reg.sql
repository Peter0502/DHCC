drop table fn_reg;
create table fn_reg
(
	tx_date		number(8),	/*交易日期								*/
	trace_no	number(9),	/*主机流水号		                                                */
	tx_br_no	varchar2(5),	/*交易机构		                                                */
	pact_no		varchar2(20),	/*协议编号		                                                */
	prdt_code	varchar2(20),	/*产品代码		                                                */
	ac_no		varchar2(24),	/*账号		                                                        */
	ac_id		number(9),	/*账户ID		                                                */
	ac_seqn		number(6),	/*账户序号		                                                */
	cif_type	varchar2(1),	/*客户类型		1.个人；2.对公                                  */
	ct_ind		varchar2(1),	/*现转标志		                                                */
	chnl_type	Varchar2(4),	/*渠道类型		                                                */
	count		number(9),	/*认购笔数		                                                */
	tx_amt		number(16,2),	/*认购金额		                                                */
	redempt_amt	number(16,2),	/*赎回金额		在投资期间客户赎回的金额，返还收益时，          */
					/*			本金=认购金额-赎回金额                          */
	return_capital	number(16,2),	/*返回本金		                                                */
	flt_radio	Number(9,6),	/*利率浮动比例		                                                */
	return_rate	number(9,6),	/*返还收益利率		实际收益率*（1+利率浮动比例）                   */
	intst		number(16,2),	/*预期收益		                                                */
	ic_date		number(8),	/*起息日期		                                                */
	end_date	number(8),	/*到期日期		                                                */
	fct_intst	number(16,2),	/*收益			未扣除费用                                      */
	profit_amt	number(16,2),	/*预期分润金额		认购机构的分润金额                              */
	fct_profit_amt	number(16,2),	/*实际分润金额		认购机构的分润金额                              */
	profit_flag	varchar2(1),	/*分润标志		是否统计进了fn_br_profit                        */
					/*			Y:是                                            */
					/*			N:否                                            */
	brf		varchar2(20),	/*摘要		                                                        */
	sts		varchar2(1),	/*状态		0：认购                                                 */
					/*		1：已扣款                                               */
					/*		2：已分配收益                                           */
					/*		3：募集失败                                             */
					/*		4：申购                                                 */
					/*		5：申购未通过                                           */
					/*		*：撤销                                                 */
    amt1    number(16,2),/*备用金额1*/
    amt2    number(16,2),/*备用金额2*/
    date1    number(8),/*备用日期1*/
    date2    number(8),/*备用日期1*/
	pay_date	number(8),	/*扣款日期		                                                */
	pay_trace_no	number(9),	/*扣款流水号		                                                */
	return_date	number(8),	/*返还收益日期		                                                */
	return_trace_no	number(9),	/*返还收益流水号		                                        */
	tel		varchar2(6),	/*签约柜员		                                                */
	mang		varchar2(6),	/*客户经理号		                                                */
	agent_id_type	varchar2(1),	/*代理人证件类型	                                            */
	agent_id	varchar2(20),	/*代理人证件		                                                */
	agent_name	varchar2(60),	/*代理人姓名		                                                */
	filler		varchar2(60)	/*备注		                                                        */
)tablespace users;
create unique index fn_reg_1 on fn_reg(tx_date,trace_no) tablespace indx;
create index fn_reg_2 on fn_reg(prdt_code,ac_id,ac_seqn,sts) tablespace indx;
create index fn_reg_3 on fn_reg(tx_br_no,prdt_code,sts) tablespace indx;
