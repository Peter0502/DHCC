drop index mo_ppe_amtchg_hst_idx;
drop table mo_ppe_amtchg_hst;
CREATE TABLE mo_ppe_amtchg_hst(
				ppeno                  varchar2(19)  NOT NULL ,/*�̶��ʲ����*/                                                                                           
				actno                  varchar2(19)           ,/*�˺�*/                                                                                                        
				actnotype              varchar2(1)            ,/*�ʺ�����*/                                                                                                    
				lstbal                 number(16,2)           ,/*�ϱ����*/                                                                                                   
				txamt                  number(16,2)           ,/*���׽��*/                                                                                                    
				bal		                 number(16,2)           ,/*���*/                                                                                                        
				dealtype               varchar2(1)            ,/*��������*/                                                                                                    
				chgcause               varchar2(200)     		  ,/*���ԭ��*/                                                                                              
				txdate                 varchar2(8)            ,/*��������*/                                                                                                    
				txtime                 varchar2(8)       		  ,/*����ʱ��*/                                                                                            
				tranceno               number(10)        		  ,/*��ˮ��*/                                                                                              
				trancecnt              number(10)        		  ,/*��ˮ�ʴ�*/                                                                                                    
				txbrno                 varchar2(7)       		  ,/*����֧�к�*/                                                                                                  
				tel                    varchar2(6)       		  /*����Ա��*/                                                                                                                                  
)tablespace users;                                  
CREATE INDEX mo_ppe_amtchg_hst_idx on mo_ppe_amtchg_hst (ppeno) tablespace indx;
                     
                     
                     
                     
        
        
        
        
        