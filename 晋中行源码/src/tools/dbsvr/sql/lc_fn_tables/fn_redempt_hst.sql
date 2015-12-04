drop table fn_redempt_hst;
create table fn_redempt_hst
(
	tx_date			number(8),	/*交易日期							*/
	trace_no		number(9),	/*主机流水号		                                        */
	tx_br_no		varchar2(5),	/*交易机构		                                        */
	pact_no			varchar2(20),	/*协议编号		                                        */
	prdt_code		varchar2(20),	/*产品代码		                                        */
	ac_no			varchar2(24),	/*账号		                                                */
	ac_id			number(9),	/*账户ID		                                        */
    ac_seqn         number(6),  /*账户序号                                              */
	chnl_type		varchar2(4),	/*渠道类型		                                        */
	redempt_amt		number(16,2),	/*赎回金额		                                        */
	redempt_chrg_amt	number(16,2),	/*赎回费用		                                        */
	return_rate		number(9,6),	/*返还收益利率		                                        */
	ic_date			number(8),	/*起息日期	计算收益的起始日期，截止日期为交易日期。        */
	fct_intst		number(16,2),	/*收益		存期*利率*赎回金额，未扣除各种费用              */
	return_capital		number(16,2),	/*返回本金		                                        */
	profit_amt		number(16,2),	/*分润金额		                                        */
	profit_flag		varchar2(1),	/*分润标志		是否统计进了fn_br_profit                */
						/*			Y:是                                    */
						/*			N:否                                    */
	tel			varchar2(6),	/*柜员		                                                */
	return_date		number(8),	/*归还收益日期		如果本金和收益不是同时归还，需要该字段  */
	return_trace_no		number(9),	/*归还收益的流水		                                */
	sts			varchar2(1),	/*状态		0：登记                                         */
						/*		1：已返回收益                                   */
	buy_date		number(8),	/*认购日期		                                        */
	buy_trace_no		number(9),	/*认购流水号		                                        */
	filler			varchar2(60)	/*备注		                                                */
)tablespace users;
create unique index fn_redempt_hst_1 on fn_redempt_hst(tx_date,trace_no) tablespace indx;
create index fn_redempt_hst_2 on fn_redempt_hst(tx_date,prdt_code)tablespace indx;
