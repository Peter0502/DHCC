/******************************************************************************/
/*  table name   : mob_acct_type_hst   (短信签约表)                           */
/*  table index  : mob_acct_type_hst_idx                                          */
/*  create by    : wangxudong@2010/01/27                                      */
/*  memo         : 由于表结构与代码不符,故修改表结构                          */
/*  memo         : 考虑签约交易的历史查询问题，及账号曾经做过的操作，将该表字段进行扩充,wangxd 20101224
                          */
/******************************************************************************/
drop table mob_acct_type_hst;
drop index mob_acct_type_hst_idx;
create table mob_acct_type_hst(
        ac_no           varchar2(20),  /*账号(黄河银行：贷款账户可存贷款合同号)*/
        ac_id           number(9),      /*账户id，贷款为0*/
        ac_seqn         number(6),      /**账户序号**/
        ac_type         varchar2(1),   /*账户类型*/
        server_type     varchar2(4),   /*服务类型*/
        mobile		varchar2(12),  /*电话号码*/
        ori_tx_date     number(9),      /*原交易日期*/
        lst_fee_date    number(9),      /*上次收费日期*/
        lst_tel         varchar2(4),   /*交易柜员*/
        sfbz            varchar2(1),   /*收费标志0.不收，1收费*/
        mob_sts         varchar2(1),   /*状态0-解约 1-签约*/
        phones          varchar2(60),  /**其他号码**/
        lst_br_no       varchar2(5),   /*原签约交易机构*/
        filler          varchar2(50),         
				tx_date		number(9),	/*交易日期*/
				tx_code		varchar2(4),	/*交易码*/
				trace_no	number(9),	/*交易流水号*/
				opr_sts		varchar2(1),	/*操作标识 0-签约 1--解约 2--修改 3--暂停 4--解暂停*/
				br_no		varchar2(5),	/*交易机构*/
				tx_tel		varchar2(6),	/*交易柜员*/
				new_ac_no	varchar2(30),	/*账号或协议号*/
				new_mobile	varchar2(50),	/*相关手机号*/
				fee		number(16,2),  /*手续费*/
				auth_tel	varchar2(6),	/*授权柜员*/
				filler1		varchar2(15),	/*备用字段1*/
				filler2		varchar2(20),	/*备用字段2*/
				filler3		varchar2(30),	/*备用字段3*/
				filler4		varchar2(50),	/*备用字段4*/
				filler5		varchar2(50),	/*备用字段5*/
				filler6		varchar2(50),	/*备用字段6*/
				memo		varchar2(100),	/*修改说明*/
				sts		varchar2(1)	/*状态*/
)tablespace users;
create  index mob_acct_type_hst_idx on mob_acct_type_hst ( ac_no,mobile,tx_date,opr_sts ) ;
