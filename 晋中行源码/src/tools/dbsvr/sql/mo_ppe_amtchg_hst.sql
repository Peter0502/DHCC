drop index mo_ppe_amtchg_hst_idx;
drop table mo_ppe_amtchg_hst;
CREATE TABLE mo_ppe_amtchg_hst(
				ppeno                  varchar2(19)  NOT NULL ,/*固定资产编号*/                                                                                           
				actno                  varchar2(19)           ,/*账号*/                                                                                                        
				actnotype              varchar2(1)            ,/*帐号类型*/                                                                                                    
				lstbal                 number(16,2)           ,/*上笔余额*/                                                                                                   
				txamt                  number(16,2)           ,/*交易金额*/                                                                                                    
				bal		                 number(16,2)           ,/*余额*/                                                                                                        
				dealtype               varchar2(1)            ,/*处理类型*/                                                                                                    
				chgcause               varchar2(200)     		  ,/*变更原因*/                                                                                              
				txdate                 varchar2(8)            ,/*交易日期*/                                                                                                    
				txtime                 varchar2(8)       		  ,/*交易时间*/                                                                                            
				tranceno               number(10)        		  ,/*流水号*/                                                                                              
				trancecnt              number(10)        		  ,/*流水笔次*/                                                                                                    
				txbrno                 varchar2(7)       		  ,/*交易支行号*/                                                                                                  
				tel                    varchar2(6)       		  /*操作员号*/                                                                                                                                  
)tablespace users;                                  
CREATE INDEX mo_ppe_amtchg_hst_idx on mo_ppe_amtchg_hst (ppeno) tablespace indx;
                     
                     
                     
                     
        
        
        
        
        