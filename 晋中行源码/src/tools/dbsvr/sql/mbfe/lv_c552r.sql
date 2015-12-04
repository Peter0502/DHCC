/**    CMT552小额收费通知报文登记簿 **/
drop index lv_c552r_idx;
drop index lv_c552r_idx2;
drop table lv_c552r;
create table lv_c552r(
    cmtno           varchar2(3)    ,/**  Cmt编号*/
    in_date         number(8)    ,/**  进入系统日期**/
    wt_date         number(8)    ,/**  委托日期    **/
    brno            varchar2(7)    ,/**  接收行号    **/
    tlrno           varchar2(6)    ,/**  操作员    **/
    sendco          varchar2(4) ,/**  发起节点号    **/
    acbrno          varchar2(12),/**  接收直接参与者**/
    iftype          varchar2(1)    ,/**  通知类型 1 付款通知 2 收款通知 3 付款回执 4 收款回执 **/
    dtlid           number(3)   ,/**  明细序号    **/
    cashno          varchar2(12),/**  收款行行号    **/
    paymon          varchar2(6), /**  计费月份    */
    sdate           number(8), /**  计费起始日期    **/
    edate           number(8), /**  计费中止日期    **/
    aorm            varchar2(1),/**  自动/手工扣费0 自动 1 手工**/
    countno         varchar2(12),/**  被计费行行号    **/
    totamt          varchar2(15),/**  被计费行总费用**/
    ferate          varchar2(6), /**  行别收费比率     单位：万分之一**/
    acrate          varchar2(6), /**  直接参与者收费比率单位：万分之一**/
    porr            varchar2(1),/**  并存/替换    0 并存 1 替换 **/
    ccpcrate        varchar2(6), /**  ccpc收费比率 单位：万分之一        **/
    totnomonth      varchar2(8),/**  月累计发起业务量    **/
    monrate         varchar2(6), /**  月累计业务量收费比率 单位：万分之一 **/
    totcrmonth      varchar2(8),/**  月累计发起超容流量**/
    crrate          varchar2(6), /**  月累计超容流量收费比率单位：万分之一    **/
    num             varchar2(8),/**  计费清单数    **/
    seqno           number(8)   ,/**  计费清单序号**/
    pkgno           varchar2(3),/**  计费清单之报文类型号**/
    sorr            varchar2(1),/**  计费清单之收发标志0 发送 1接收**/
    iorw            varchar2(1),/**  计费清单之同城/异地标志0 同城 1 异地**/
    stime           varchar2(4) ,/**  计费清单之时间段起点    **/
    etime           varchar2(4) ,/**  计费清单之时间段终点    **/
    totnum          varchar2(8) ,/**  计费清单之计费总量    **/
    brate           varchar2(6),/**  计费清单之基础费率单位：分/笔或分/KB    **/
    iorwrate        varchar2(6) ,/**  计费清单之同城/异地收费比率    单位：万分之一 **/
    timerate        varchar2(6) ,/**  计费清单之时段收费比率单位：万分之一     **/
    price           varchar2(6)    ,/**  计费清单之计费单价单位：分/笔或分/KB     **/
    txamt           varchar2(15) /**  计费清单之计费金额 单位：分**/
)tablespace users;
create unique index lv_c552r_idx on lv_c552r(in_date,dtlid,seqno)tablespace indx;
