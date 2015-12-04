/*****************************************************************************/
/*     Filename : /CStalk/a_incl/ap_req_incl.h                               */
/*     Usage    : request_wait new function     reqlib.a                     */
/*****************************************************************************/
extern  ClientConnectId        t_conn_id;
extern  RCBP                   t_client_rcb;
extern  char                   t_user[30];
extern  char                   t_password[30];
extern  char                   t_client_name[30];
extern  char                   t_request_name[30];
extern  char                   t_xact_obj[30];
extern  char                  *t_request_buffer;
extern  char                  *t_source_path;
extern  char                  *t_target_path;
extern  short                  t_request_length;
extern  short                  t_timeout;
extern  short                  t_protocol;
extern  short                  t_ap_errcode;
extern  long                   t_rc_status;
extern  long                   t_xact_id;
extern  long                   t_request_id;
extern  void                   (*t_errors_handler)();
extern  void                   (*t_normal_handler)();

extern  Boolean                (*request)();
extern  ClientRequestId        (*requestno)();
extern  Boolean                (*requestBLOB)();
extern  ClientRequestId        (*requestBLOBno)();
