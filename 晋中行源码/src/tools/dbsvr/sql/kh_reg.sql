drop index kh_reg_1;
drop index kh_reg_2;
drop table kh_reg;
create table KH_REG
(
    Br_no           Varchar2(5)  ,    /**   扣划交易机构            **/
    Out_ac_no       Varchar2(19) ,    /**   账号                    **/
    Out_ac_id       Number(9)    ,    /**   账户id号                **/
    Out_ac_seqn     Number(9)    ,    /**   账户序号                **/
    Tx_date         Number(9)    ,    /**   交易日期                **/
    Nat_date        Number(8)    ,    /**   机器时间                **/
    Out_prdt_no     Varchar2(3)  ,    /**   产品种类                **/
    Out_ac_id_type  Varchar2(1)  ,    /**   转出账户类型            **/
    Kh_amt          Number(16,2) ,    /**   扣划金额                **/
    Intst           Number(16,2) ,    /**   利息                    **/
    Intst_ac_no     Varchar2(19) ,    /**   利息提取账号[焦作add ]  **/
    Intst_ac_seqn   Number(9)    ,    /**   利息提取账户序号        **/
    In_ac_no        Varchar2 (19),    /**   转入账户                **/
    In_ac_id        Number(9)    ,    /**   转入id号                **/
    In_ac_seqn      Number(9)    ,    /**   转入账户序号            **/
    Tel             Varchar2(6)  ,    /**   操作员                  **/
    Auth_tel        Varchar2(6)  ,    /**   授权操作员              **/
    Auth_code       Number(10)   ,    /**   预授权号                **/
    Trace_no        Number(9)    ,    /**   流水号                  **/
    Tx_time         Number(9)    ,    /**   交易时间                **/
    Brf             Varchar2(20) ,    /**   备注                    **/
    Tt_no           Number(9)    ,    /**   柜员流水号              **/
    Low_bo          Varchar2(36) ,    /**   法律文书                **/
    Zf_name1        Varchar2(8)  ,    /**   执法人员1               **/
    Pub_no1         Varchar2(18) ,    /**   执法人员编号            **/
    Zf_name2        Varchar2(8)  ,    /**   执法人员2               **/
    Pub_no2         Varchar2(18) ,    /**   执法人员编号            **/
    Fl_print        Varchar2(1)  ,    /**                           **/
    Flag_intst      Varchar2(1)  ,    /**   利息提取标志[焦作add ]  **/
    Tel_intst       Varchar2(6)  ,    /**   利息提取柜员号          **/
    Trace_no_intst  Number(9)    ,    /**   利息提取流水号          **/
    Tx_date_intst   Number(9)    ,    /**   利息提取日期            **/
    Id_type_intst   Varchar2(1)  ,    /**   利息提取人证件类型      **/
    Id_no_intst     Varchar2 (20),    /**   利息提取人证件号码      **/
    Name_intst      Varchar2(60)      /**利息提取人[焦作加20111014 slzh] **/
)tablespace USERS;
create unique index KH_REG_1 on KH_REG (OUT_AC_ID, OUT_AC_SEQN, TX_DATE, TRACE_NO);
create index KH_REG_2 on KH_REG (OUT_AC_NO, OUT_AC_SEQN, TX_DATE);

