create table pact_gaga_rel (
    pact_no varchar2(20),        /**��ͬ��      **/
    gaga_id varchar2(20),        /**����ѺƷ���**/
    name varchar(60),  		/** ��Ʒ���� **/
    gaga_bal decimal(17,2),      /**����Ѻ���  **/
    gaga_val decimal(17,2),      /**����Ѻ��ֵ  **/
    stsvar varchar2(1),          /**״̬  X�Ŵ��������Y-�Ŵ�������ʾ����1-����������2-����ȡ��      **/
    pact_type varchar2(1),       /**D:���� C:�ж� **/
    io_ind varchar2(4),          /** 120Ϊ���ж��ڴ浥 ����Ϊ���д浥    **/
    bill_no varchar2(20),         /** ������Ѻ�浥�˺�  **/
    filler varchar2(30)          /**            **/
)tablespace users;
create index  pact_gaga_rel_idx on pact_gaga_rel (pact_no,gaga_id) tablespace indx;
