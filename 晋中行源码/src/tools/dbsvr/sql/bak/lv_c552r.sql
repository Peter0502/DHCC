/**    CMT552С���շ�֪ͨ���ĵǼǲ� **/
drop index lv_c552r_idx;
drop index lv_c552r_idx2;
drop table lv_c552r;
create table lv_c552r(
    cmtno           Varchar2(3)    ,/**  Cmt���*/
    in_date         Number(8)    ,/**  ����ϵͳ����**/
    wt_date         Number(8)    ,/**  ί������    **/
    brno            Varchar2(7)    ,/**  �����к�    **/
    tlrno           Varchar2(6)    ,/**  ����Ա    **/
    sendco          Varchar2(4) ,/**  ����ڵ��    **/
    acbrno          Varchar2(12),/**  ����ֱ�Ӳ�����**/
    iftype          Varchar2(1)    ,/**  ֪ͨ���� 1 ����֪ͨ 2 �տ�֪ͨ 3 �����ִ 4 �տ��ִ **/
    dtlid           Number(3)   ,/**  ��ϸ���    **/
    cashno          Varchar2(12),/**  �տ����к�    **/
    paymon          Varchar2(6), /**  �Ʒ��·�    */
    sdate            Varchar2(8), /**  �Ʒ���ʼ����    **/
    edate            Varchar2(8), /**  �Ʒ���ֹ����    **/
    aorm            Varchar2(1),/**  �Զ�/�ֹ��۷�0 �Զ� 1 �ֹ�**/
    countno            Varchar2(12),/**  ���Ʒ����к�    **/
    totamt            Varchar2(15),/**  ���Ʒ����ܷ���**/
    ferate            Varchar2(6), /**  �б��շѱ���     ��λ�����֮һ**/
    acrate            Varchar2(6), /**  ֱ�Ӳ������շѱ��ʵ�λ�����֮һ**/
    porr            Varchar2(1),/**  ����/�滻    0 ���� 1 �滻 **/
    ccpcrate        Varchar2(6), /**  ccpc�շѱ��� ��λ�����֮һ        **/
    totnomonth        Varchar2(8),/**  ���ۼƷ���ҵ����    **/
    monrate            Varchar2(6), /**  ���ۼ�ҵ�����շѱ��� ��λ�����֮һ **/
    totcrmonth        Varchar2(8),/**  ���ۼƷ���������**/
    crrate            Varchar2(6), /**  ���ۼƳ��������շѱ��ʵ�λ�����֮һ    **/
    num            Varchar2(8),/**  �Ʒ��嵥��    **/
    seqno            Number(8)   ,/**  �Ʒ��嵥���**/
    pkgno            Varchar2(3),/**  �Ʒ��嵥֮�������ͺ�**/
    sorr            Varchar2(1),/**  �Ʒ��嵥֮�շ���־0 ���� 1����**/
    iorw            Varchar2(1),/**  �Ʒ��嵥֮ͬ��/��ر�־0 ͬ�� 1 ���**/
    stime            Varchar2(4) ,/**  �Ʒ��嵥֮ʱ������    **/
    etime            Varchar2(4) ,/**  �Ʒ��嵥֮ʱ����յ�    **/
    totnum            Varchar2(8) ,/**  �Ʒ��嵥֮�Ʒ�����    **/
    brate            Varchar2(6),/**  �Ʒ��嵥֮�������ʵ�λ����/�ʻ��/KB    **/
    iorwrate        Varchar2(6) ,/**  �Ʒ��嵥֮ͬ��/����շѱ���    ��λ�����֮һ **/
    timerate        Varchar2(6) ,/**  �Ʒ��嵥֮ʱ���շѱ��ʵ�λ�����֮һ     **/
    price            Varchar2(6)    ,/**  �Ʒ��嵥֮�Ʒѵ��۵�λ����/�ʻ��/KB     **/
    txamt            Varchar2(15) /**  �Ʒ��嵥֮�Ʒѽ�� ��λ����**/
)tablespace users;
create unique index lv_c552r_idx on lv_c552r(in_date,dtlid,seqno)tablespace indx;
