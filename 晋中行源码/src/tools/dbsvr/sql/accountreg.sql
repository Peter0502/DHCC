/*  同城清算登记簿 */
drop index  accountreg_idx;
drop table  accountreg;
create table accountreg
(
	br_no  		varchar2(5),      /*  网点号 */
	trace_no 	number(9),      /*  流水号      */
	seqno  		number(2),     /*  序号        */
	change_no    varchar2(4),        /*  交换号      */
	br_name      varchar2(60),       /*  网点名称    */
	note_type  varchar2(3),             /*  凭证类型    */
	note_no     varchar2(16),       /*  凭证号      */ 
	pay_ac_no    varchar2(32),      /*   付款帐号    */
	pay_name     varchar2(60),       /*  付款名称    */
	amt         number(16,2),       /*  金额        */
	cash_ac_no   varchar2(32),       /*  收款帐号    */
	cash_name    varchar2(60),       /*  收款名称    */
	cash_opn_br_no  varchar2(12),       /*  对方行号    */
	cash_chg_no   varchar2(4),        /*  对方交换号  */
	cash_opn_name  varchar2(60),       /*  对方名称    */
	put_flag     varchar2(1),        /*  提入提出 1--提出2--提入 */
	tx_date     number(9),        /*  交易日期    */
	tx_time     number(9),        /*  交易时间    */
	stat        varchar2(1),        /*  状态标志 1--登记2--记账 8-已打印 9-取消*/
	tel         varchar2(4),        /*  操作员      */
	chk         varchar2(4),        /*  复核员      */
	pwd         varchar2(6)         /*  密押        */
);
create unique index accountreg_idx on accountreg (br_no,trace_no,tx_date);
