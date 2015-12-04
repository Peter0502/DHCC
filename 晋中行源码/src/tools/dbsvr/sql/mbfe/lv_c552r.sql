/**    CMT552С���շ�֪ͨ���ĵǼǲ� **/
drop index lv_c552r_idx;
drop index lv_c552r_idx2;
drop table lv_c552r;
create table lv_c552r(
    cmtno           varchar2(3)    ,/**  Cmt���*/
    in_date         number(8)    ,/**  ����ϵͳ����**/
    wt_date         number(8)    ,/**  ί������    **/
    brno            varchar2(7)    ,/**  �����к�    **/
    tlrno           varchar2(6)    ,/**  ����Ա    **/
    sendco          varchar2(4) ,/**  ����ڵ��    **/
    acbrno          varchar2(12),/**  ����ֱ�Ӳ�����**/
    iftype          varchar2(1)    ,/**  ֪ͨ���� 1 ����֪ͨ 2 �տ�֪ͨ 3 �����ִ 4 �տ��ִ **/
    dtlid           number(3)   ,/**  ��ϸ���    **/
    cashno          varchar2(12),/**  �տ����к�    **/
    paymon          varchar2(6), /**  �Ʒ��·�    */
    sdate           number(8), /**  �Ʒ���ʼ����    **/
    edate           number(8), /**  �Ʒ���ֹ����    **/
    aorm            varchar2(1),/**  �Զ�/�ֹ��۷�0 �Զ� 1 �ֹ�**/
    countno         varchar2(12),/**  ���Ʒ����к�    **/
    totamt          varchar2(15),/**  ���Ʒ����ܷ���**/
    ferate          varchar2(6), /**  �б��շѱ���     ��λ�����֮һ**/
    acrate          varchar2(6), /**  ֱ�Ӳ������շѱ��ʵ�λ�����֮һ**/
    porr            varchar2(1),/**  ����/�滻    0 ���� 1 �滻 **/
    ccpcrate        varchar2(6), /**  ccpc�շѱ��� ��λ�����֮һ        **/
    totnomonth      varchar2(8),/**  ���ۼƷ���ҵ����    **/
    monrate         varchar2(6), /**  ���ۼ�ҵ�����շѱ��� ��λ�����֮һ **/
    totcrmonth      varchar2(8),/**  ���ۼƷ���������**/
    crrate          varchar2(6), /**  ���ۼƳ��������շѱ��ʵ�λ�����֮һ    **/
    num             varchar2(8),/**  �Ʒ��嵥��    **/
    seqno           number(8)   ,/**  �Ʒ��嵥���**/
    pkgno           varchar2(3),/**  �Ʒ��嵥֮�������ͺ�**/
    sorr            varchar2(1),/**  �Ʒ��嵥֮�շ���־0 ���� 1����**/
    iorw            varchar2(1),/**  �Ʒ��嵥֮ͬ��/��ر�־0 ͬ�� 1 ���**/
    stime           varchar2(4) ,/**  �Ʒ��嵥֮ʱ������    **/
    etime           varchar2(4) ,/**  �Ʒ��嵥֮ʱ����յ�    **/
    totnum          varchar2(8) ,/**  �Ʒ��嵥֮�Ʒ�����    **/
    brate           varchar2(6),/**  �Ʒ��嵥֮�������ʵ�λ����/�ʻ��/KB    **/
    iorwrate        varchar2(6) ,/**  �Ʒ��嵥֮ͬ��/����շѱ���    ��λ�����֮һ **/
    timerate        varchar2(6) ,/**  �Ʒ��嵥֮ʱ���շѱ��ʵ�λ�����֮һ     **/
    price           varchar2(6)    ,/**  �Ʒ��嵥֮�Ʒѵ��۵�λ����/�ʻ��/KB     **/
    txamt           varchar2(15) /**  �Ʒ��嵥֮�Ʒѽ�� ��λ����**/
)tablespace users;
create unique index lv_c552r_idx on lv_c552r(in_date,dtlid,seqno)tablespace indx;
