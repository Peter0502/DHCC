/*****************************************************************************/
/*     Filename : /CStalk/a_incl/ap_req_glob.h                               */
/*     Usage    : request_wait new function     reqlib.a                     */
/*****************************************************************************/
ClientConnectId          t_conn_id;
RCBP                     t_client_rcb;
char                     t_user[30];
char                     t_password[30];
char                     t_client_name[30];
char                     t_request_name[30];
char                     t_xact_obj[30];
char                    *t_request_buffer;
char                    *t_source_path;
char                    *t_target_path;
short                    t_request_length;
short                    t_timeout;
short                    t_protocol;
short                    t_ap_errcode;
long                     t_rc_status;
long                     t_xact_id;
long                     t_request_id;
void                     (*t_errors_handler)();
void                     (*t_normal_handler)();

Boolean                  requestXactWait();
ClientRequestId          requestXactNoWait();
Boolean                  requestBLOBWait();
ClientRequestId          requestBLOBNoWait();
Boolean                  (*request)(RCBP t_client_rcb) = requestXactWait;
ClientRequestId          (*requestno)(RCBP t_client_rcb) = requestXactNoWait;
Boolean                  (*requestBLOB)(RCBP client_rcb) = requestBLOBWait;
ClientRequestId          (*requestBLOBno)(RCBP client_rcb) = requestBLOBNoWait;
