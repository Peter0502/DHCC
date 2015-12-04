/* ******************************************* */
/* 科目表附加表                                */
/* 科目表一些附加的信息表                      */
/* ******************************************* */
drop  index com_item_add_idx;
drop  table com_item_add;
create table com_item_add(
	acc_no       varchar2(7),  /* 科目号                   */
	acc_hrt      varchar2(5),  /* 科目控制字               */
	add_ind      varchar2(40), /* 附加表控制标志位         */
        /* bit 2 - 扣划方向 0-不能扣划 1-借方 2-贷方 3 双向*/                             
        /* bit 1 - 是否打印交易传票 0- 不需要打印 1-需要打印*/
        /* bit 0 - 0说明不控制限额,  非0控制限额 			*/
	dr_tx_amt    number(16,2), /* 借方科目使用额           */
	cr_tx_amt    number(16,2), /* 贷方科目使用额           */
	dr_tx_cnt    number(6)   , /* 借方使用次数             */
	cr_tx_cnt    number(6)   , /* 贷方使用次数             */
	dr_limit_amt number(16,2), /* 借方限额                 */
	cr_limit_amt number(16,2), /* 贷方限额                 */
	brf          varchar2(60)
)tablespace users;
create unique index com_item_add_idx on com_item_add(acc_no)tablespace indx;
