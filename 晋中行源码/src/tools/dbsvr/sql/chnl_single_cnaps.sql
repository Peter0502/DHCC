/*二代支付单笔记账登记薄 Added by YangMeng 2014/7/20 11:09:07*/
drop index chnl_single_cnaps_1;
drop index chnl_single_cnaps_2;
drop table chnl_single_cnaps;
create table chnl_single_cnaps(
	tx_time       			varchar(6),
	tx_code       			varchar(4),
	chnl_type	  			varchar(1)   		not null, --渠道类型：1：银联；2：农信银；3：人行支付4:二代代理平台
	bbmk          			varchar(4)   		not null,
	pt_date       			number(9)    		not null,
	pt_trace_no   			varchar(12)  		not null,
	tx_date       			number(9)    		not null,
	trace_no      			number(9)			not null,
	tx_br_no      			varchar(5),
	opn_br_no     			varchar(5),
	ct_ind        			varchar(1),
	ac_no         			varchar(32),
	ac_no1        			varchar(32),
	ac_no2        			varchar(32),
	cur_no        			varchar(2),
	tx_amt        			number(17,2),
	th_amt        			number(17,2),
	fee_amt       			number(17,2),
	sts           			varchar(1),
	ttyn          			varchar(20),
	tel           			varchar(6),
	o_tx_date     			number(9),
	o_trace_no    			varchar(12),
	o_pt_date     			number(9),
	o_pt_trace_no 			varchar(12),
	new_ac_no2    			varchar(32),
	new_ac_name2  			varchar(60),
	g_ac_no2      			varchar(32),
	dz_ind        			varchar(1),
	dz_ref        			varchar(32),
	pt_time       			varchar(6),
	brf           			varchar(30),
	fd15_cups     			number(9),  --增加银联的清算日期
	fd11_cups     			varchar(6), --增加银联的系统跟踪号
	fd7_cups      			varchar(10), --增加银联的交易传输时间
	fd32_cups     			varchar(11), --增加银联的受理机构标识码
	fd33_cups     			varchar(11), --增加银联的发送机构标识码
	fill1					varchar(32),
	fill2					varchar(32),
	fill3					number(17,2),
	fill4					number(17,2),
	fill5 	    			number(9),
	fill6					number(9)
);
create unique index chnl_single_cnaps_1 on chnl_single_cnaps (chnl_type, bbmk, pt_date, pt_trace_no);
create index chnl_single_cnaps_2 on chnl_single_cnaps (bbmk,pt_date,pt_trace_no,Tx_date,trace_no);

