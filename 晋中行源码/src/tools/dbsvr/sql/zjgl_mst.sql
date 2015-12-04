drop  table zjgl_mst;
create table zjgl_mst(
    opn_br_no      varchar2(5),   /* 开户机构号        */
    cur_no         varchar2(2),   /* 币种              */
    acc_no         varchar2(7),   /* 科目号 */
    acc_hrt        varchar(5),    /* 科目控制字        */
    dr_bal         number(16,2),  /* 借方余额          */
    cr_bal         number(16,2),  /* 贷方余额          */
    ys_dr_bal      number(16,2),  /* 昨日借方余额      */
    ys_cr_bal      number(16,2),  /* 昨日贷方余额      */
    sts            varchar2(1),   /* 帐户状态          */
    opn_date       number(8),     /* 开户日期          */
    ic_date        number(8),     /* 起息日期          */
    end_date       number(8),     /* 到期日期          */
    cls_date       number(8),     /* 销户日期          */
    lst_date       number(8),     /* 上笔发生日期      */
    intst_knd      varchar2(10),  /* 计息类型          */
    over_rate_type varchar2(3),   /* 逾期利率编号      */
    rate_type      varchar2(3),   /* 利率编号          */
    rate           number(8,5),   /* 利率              */
    over_rate      number(8,5),   /* 逾期利率          */
    dr_intst_acm   number(19,2),  /* 利息积数          */
    cr_intst_acm   number(19,2),  /* 利息积数          */
    hst_cnt        number(8),     /* 明细帐总笔数      */
    hst_pg         number(5),     /* 明细帐满页页数    */
    od_ind         varchar2(1),   /* 透支标志          */
    name           varchar2(50),  /* 户名              */
    lim_amt        number(16,2),  /* 交易限额          */
    lim_bal        number(16,2),  /* 累计交易额        */
    lim_term       varchar2(2),   /* 限额周期          */
    lim_beg_date   number(8),     /* 限额开始日期      */
    lim_end_date   number(8),     /* 限额结束日期      */
    tmp1           varchar2(50),  /* 备注1             */
    tmp2           varchar2(50),  /* 备注2             */
    tmp_amt1       number(19,2),  /* 备注金额1         */
    tmp_amt2       number(19,2)   /* 备注金额2         */
)tablespace users;
create   unique index zjgl_mst_1 on zjgl_mst( opn_br_no,cur_no,acc_no)tablespace indx;
