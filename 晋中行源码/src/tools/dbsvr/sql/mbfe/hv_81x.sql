/**    CMT812/4支付业务收费/费用返还清单报文登记簿 dongxy 20061012 for CZ**/
drop index hv_81x_idx;
drop table hv_81x;
create table hv_81x(
        cmtno           Varchar2(3),    /** 812,收费清单; 814,返还清单 **/
        br_no           Varchar2(7),    /** 机构号（接收行号）   **/
        in_date         Varchar2(8),    /** 交易（进入系统）日期 **/
        txtime          Varchar2(8),    /** 交易时间 **/
        orderno         number(3),      /** 顺序号 **/
        dtlid           Number(3),      /** 明细序号             **/
        chgtype         Varchar(1),     /** 收费类型 1大额 2小额 **/
        receco          Varchar2(4),    /** 收报中心代码 **/
        cash_qs_no      Varchar2(12),   /** 接收清算行号 **/
        dr_br_no        Varchar2(12),   /** 计费直接参与者 **/
        totmon          Varchar2(6),    /** 计费月份     **/
        sdate           Varchar2(8),    /** 计费开始日期 **/
        edate           Varchar2(8),    /** 计费终止日期 **/
        tot_amt         number(15,2),   /** 计费总金额   **/
        txamt           number(15,2),   /** 大/小额计费金额 **/
        num             Varchar2(5),    /** 参与者大/小额计费数目             **/
        csbrno          Varchar2(12),   /** 参与者大/小额计费清单之参与者行号 **/
        amt             number(15,2),   /** 参与者大/小额计费清单之计费总金额 **/
        zf_amt          number(15,2),   /** 参与者大/小额计费清单之支付业务计费金额 **/
        xx_amt          number(15,2)    /** 参与者大/小额计费清单之信息业务计费金额 **/
)tablespace users;
create unique index hv_81x_idx on hv_81x(cmtno,in_date,dtlid)tablespace indx;

