#ifndef _EXTERN_H
#define _EXTERN_H

extern void 	_balmain(TPSVCINFO*);
extern void    	bal_callback(TPSVCINFO*);
extern void    	bal_pollcall(TPSVCINFO *);

extern T_infGlobal  g_global;              /* global information */
extern T_infDate    g_date;                /* date information  */
extern T_BalInfo    g_balinfo;             /* bal information   */
extern int          balerrno;              /* bal errno         */

#ifdef FOR_APCTL
extern char                   service_name[30],client_name[30];
extern void                   (*func)();
extern void                   (*db_service_init)();
extern void                   (*user_poll_call)(); 
extern void                   (*common_table_update)();
extern void                   (*user_call_back)();
extern char                   tp_name[];
extern short                  special_server;
extern void                   (*user_call_back)();
extern void                   (*Pre_Server_Call)();
extern char**                 __argv;
extern int                    __argc;
#endif

#ifdef FOR_APCTL
extern short   time83_mvmacta();
extern short   time81_mvtota();
extern int    time18_putmsg( char *, int *, char*);
extern void    msg_decode( char *string );
extern void    SetNowait_Release();
extern void    ResetTimeout();
extern void    _balDumpTITA();
extern void    client_get_time(char*);
#endif

extern void    	bal_init_txcom();
extern int     	bal_begin_tx(T_BalInfo*);
extern int     	bal_commit_tx(T_BalInfo*);
extern int     	bal_abort_tx(T_BalInfo*);
extern int     	bal_get_cbsname(char*, unsigned long);
extern void    	response_nowait(char**, short *, long*);
extern int 		bal_conv_in(T_BTR*);
extern int 		bal_conv_out( T_BTR*, int*, T_BTR*, unsigned char*);
extern int 		bal_set_lockwaittime();
extern void            bal_gettime( char*);
extern void            bal_gen_audit(T_BTR*, int);
extern int             bal_call_auditsvr(T_BTR*, int);
extern unsigned long   bal_get_hostip();
extern unsigned long   bal_get_svcdigit();
extern long            bal_get_responseid(char*);
extern int             bal_callback_rpl_ind(T_BTR*, int);
extern int             bal_is_callback_rpl_ind();
extern void            bal_dumpmesg(char*, char*, int);
extern int 		bal_initinfo(T_BalInfo*, int, char**);
extern int     	bal_get_pcsname(char*, unsigned long);
extern int     	bal_register_pollcall(unsigned long, unsigned long);
extern int     	bal_call_auditsvr(T_BTR*, int);
extern int     	bal_unregister_pollcall(unsigned long);
extern int     	bal_notify_pollserver(T_BalInfo*);
extern int 		bal_process_tx(T_BTR*);
extern int 		bal_req_globalinfo();
extern char*    balstrerror(int);
extern char* 	balchstrerror(int);
extern void 	bal_dump_mallocinfo (char *);
extern short 	request_nowait(char*, char*, short, long*);
extern short  	request_wait(char*, char*, short, short, char**, short*);
extern void     bal_return_error(TPSVCINFO*);

#endif
