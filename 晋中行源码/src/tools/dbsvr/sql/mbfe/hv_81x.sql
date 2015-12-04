/**    CMT812/4֧��ҵ���շ�/���÷����嵥���ĵǼǲ� dongxy 20061012 for CZ**/
drop index hv_81x_idx;
drop table hv_81x;
create table hv_81x(
        cmtno           Varchar2(3),    /** 812,�շ��嵥; 814,�����嵥 **/
        br_no           Varchar2(7),    /** �����ţ������кţ�   **/
        in_date         Varchar2(8),    /** ���ף�����ϵͳ������ **/
        txtime          Varchar2(8),    /** ����ʱ�� **/
        orderno         number(3),      /** ˳��� **/
        dtlid           Number(3),      /** ��ϸ���             **/
        chgtype         Varchar(1),     /** �շ����� 1��� 2С�� **/
        receco          Varchar2(4),    /** �ձ����Ĵ��� **/
        cash_qs_no      Varchar2(12),   /** ���������к� **/
        dr_br_no        Varchar2(12),   /** �Ʒ�ֱ�Ӳ����� **/
        totmon          Varchar2(6),    /** �Ʒ��·�     **/
        sdate           Varchar2(8),    /** �Ʒѿ�ʼ���� **/
        edate           Varchar2(8),    /** �Ʒ���ֹ���� **/
        tot_amt         number(15,2),   /** �Ʒ��ܽ��   **/
        txamt           number(15,2),   /** ��/С��Ʒѽ�� **/
        num             Varchar2(5),    /** �����ߴ�/С��Ʒ���Ŀ             **/
        csbrno          Varchar2(12),   /** �����ߴ�/С��Ʒ��嵥֮�������к� **/
        amt             number(15,2),   /** �����ߴ�/С��Ʒ��嵥֮�Ʒ��ܽ�� **/
        zf_amt          number(15,2),   /** �����ߴ�/С��Ʒ��嵥֧֮��ҵ��Ʒѽ�� **/
        xx_amt          number(15,2)    /** �����ߴ�/С��Ʒ��嵥֮��Ϣҵ��Ʒѽ�� **/
)tablespace users;
create unique index hv_81x_idx on hv_81x(cmtno,in_date,dtlid)tablespace indx;

