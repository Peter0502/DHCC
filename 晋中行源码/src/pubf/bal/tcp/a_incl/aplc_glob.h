/**************************************************************************/
/*   Filename : /CStalk/a_incl/aplc_glob.h                                */
/*   Usage    : APCTL Client Library  apreqlib.a                          */
/**************************************************************************/
   ClientConnectId          t_conn_id;
   RCBP                     t_client_rcb;
   char                     t_user[30];
   char                     t_password[30];
   char                     t_client_name[30];
   char                     t_request_name[30];
   char                     t_xact_obj[30];
   char                     t_xact_name[128];
   char                    *t_request_buffer;
   char                    *t_source_path;
   char                    *t_target_path;
   short                    t_request_length;
   short                    t_timeout;
   short                    t_protocol;
   short                    t_ap_errcode;
   short                    t_source_len;
   short                    t_target_len;
   short                    t_prep_xname;
   long                     t_rc_status;
   long                     t_xact_id;
   long                     t_request_id;
   void                     (*t_errors_handler)();
   void                     (*t_normal_handler)();

   Boolean          requestXactWait();
   Boolean          requestBLOBWait();
   long             requestXactNoWait();
   long             requestBLOBNoWait();
   Boolean          (*Request)(RCBP t_client_rcb)       = requestXactWait;
   Boolean          (*RequestBLOB)(RCBP t_client_rcb)   = requestBLOBWait;
   ClientRequestId  (*RequestNo)(RCBP t_client_rcb)     = requestXactNoWait;
   ClientRequestId  (*RequestBLOBNo)(RCBP t_client_rcb) = requestBLOBNoWait;
