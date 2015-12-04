drop index kh_reg_1;
drop index kh_reg_2;
drop table kh_reg;
create table KH_REG
(
    Br_no           Varchar2(5)  ,    /**   �ۻ����׻���            **/
    Out_ac_no       Varchar2(19) ,    /**   �˺�                    **/
    Out_ac_id       Number(9)    ,    /**   �˻�id��                **/
    Out_ac_seqn     Number(9)    ,    /**   �˻����                **/
    Tx_date         Number(9)    ,    /**   ��������                **/
    Nat_date        Number(8)    ,    /**   ����ʱ��                **/
    Out_prdt_no     Varchar2(3)  ,    /**   ��Ʒ����                **/
    Out_ac_id_type  Varchar2(1)  ,    /**   ת���˻�����            **/
    Kh_amt          Number(16,2) ,    /**   �ۻ����                **/
    Intst           Number(16,2) ,    /**   ��Ϣ                    **/
    Intst_ac_no     Varchar2(19) ,    /**   ��Ϣ��ȡ�˺�[����add ]  **/
    Intst_ac_seqn   Number(9)    ,    /**   ��Ϣ��ȡ�˻����        **/
    In_ac_no        Varchar2 (19),    /**   ת���˻�                **/
    In_ac_id        Number(9)    ,    /**   ת��id��                **/
    In_ac_seqn      Number(9)    ,    /**   ת���˻����            **/
    Tel             Varchar2(6)  ,    /**   ����Ա                  **/
    Auth_tel        Varchar2(6)  ,    /**   ��Ȩ����Ա              **/
    Auth_code       Number(10)   ,    /**   Ԥ��Ȩ��                **/
    Trace_no        Number(9)    ,    /**   ��ˮ��                  **/
    Tx_time         Number(9)    ,    /**   ����ʱ��                **/
    Brf             Varchar2(20) ,    /**   ��ע                    **/
    Tt_no           Number(9)    ,    /**   ��Ա��ˮ��              **/
    Low_bo          Varchar2(36) ,    /**   ��������                **/
    Zf_name1        Varchar2(8)  ,    /**   ִ����Ա1               **/
    Pub_no1         Varchar2(18) ,    /**   ִ����Ա���            **/
    Zf_name2        Varchar2(8)  ,    /**   ִ����Ա2               **/
    Pub_no2         Varchar2(18) ,    /**   ִ����Ա���            **/
    Fl_print        Varchar2(1)  ,    /**                           **/
    Flag_intst      Varchar2(1)  ,    /**   ��Ϣ��ȡ��־[����add ]  **/
    Tel_intst       Varchar2(6)  ,    /**   ��Ϣ��ȡ��Ա��          **/
    Trace_no_intst  Number(9)    ,    /**   ��Ϣ��ȡ��ˮ��          **/
    Tx_date_intst   Number(9)    ,    /**   ��Ϣ��ȡ����            **/
    Id_type_intst   Varchar2(1)  ,    /**   ��Ϣ��ȡ��֤������      **/
    Id_no_intst     Varchar2 (20),    /**   ��Ϣ��ȡ��֤������      **/
    Name_intst      Varchar2(60)      /**��Ϣ��ȡ��[������20111014 slzh] **/
)tablespace USERS;
create unique index KH_REG_1 on KH_REG (OUT_AC_ID, OUT_AC_SEQN, TX_DATE, TRACE_NO);
create index KH_REG_2 on KH_REG (OUT_AC_NO, OUT_AC_SEQN, TX_DATE);

