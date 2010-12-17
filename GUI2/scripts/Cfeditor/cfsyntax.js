var edit_line_bundle_syntax = {
   "delete_lines":
      {
      "delete_select" : 
            {
         "delete_if_startwith_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "delete_if_not_startwith_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "delete_if_match_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "delete_if_not_match_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "delete_if_contains_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "delete_if_not_contains_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            }
            
            },
      "not_matching" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         }
      },
   "insert_lines":
      {
      "expand_scalars" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "insert_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "literal|string|file|preserve_block"
         },
      "insert_select" : 
            {
         "insert_if_startwith_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "insert_if_not_startwith_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "insert_if_match_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "insert_if_not_match_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "insert_if_contains_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "insert_if_not_contains_from_list" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            }
            
            },
      "location" : 
            {
         "before_after" : 
            {
            datatype: "(menu option)",
            pcre_range: "before|after"
            },
         "first_last" : 
            {
            datatype: "(menu option)",
            pcre_range: "first|last"
            },
         "select_line_matching" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "whitespace_policy" : 
         {
         datatype: "(option list)",
         pcre_range: "ignore_leading|ignore_trailing|ignore_embedded|ignore_embedded|exact_match"
         }
      },
   "field_edits":
      {
      "edit_field" : 
            {
         "allow_blank_fields" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "extend_fields" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "field_operation" : 
            {
            datatype: "(menu option)",
            pcre_range: "prepend|append|alphanum|delete|set"
            },
         "field_separator" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "field_value" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "select_field" : 
            {
            datatype: "int",
            pcre_range: "0,99999999"
            },
         "start_fields_from_zero" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "value_separator" : 
            {
            datatype: "string",
            pcre_range: "^.$"
            }
            
            }
      },
   "replace_patterns":
      {
      "replace_with" : 
            {
         "occurrences" : 
            {
            datatype: "(menu option)",
            pcre_range: "all|first"
            },
         "replace_value" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            }
      },
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      ,
      "select_region" : 
            {
         "include_start_delimiter" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "include_end_delimiter" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "select_start" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "select_end" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            }
      }
};


var bundle_syntax = {
  edit_line: edit_line_bundle_syntax,
  common:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      }
   },
  agent:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "commands":
      {
      "args" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "contain" : 
            {
         "useshell" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "umask" : 
            {
            datatype: "(menu option)",
            pcre_range: "0|77|22|27|72|077|022|027|072"
            },
         "exec_owner" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "exec_group" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "exec_timeout" : 
            {
            datatype: "int",
            pcre_range: "1,3600"
            },
         "chdir" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "chroot" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "preview" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "no_output" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "module" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         }
      },
   "databases":
      {
      "database_server" : 
            {
         "db_server_owner" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "db_server_password" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "db_server_host" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "db_server_type" : 
            {
            datatype: "(menu option)",
            pcre_range: "postgres|mysql"
            },
         "db_server_connection_db" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "database_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "sql|ms_registry"
         },
      "database_operation" : 
         {
         datatype: "(menu option)",
         pcre_range: "create|delete|drop|cache|verify|restore"
         },
      "database_columns" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "database_rows" : 
         {
         datatype: "slist",
         pcre_range: ".*,.*"
         },
      "registry_exclude" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "environments":
      {
      "environment_host" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "environment_interface" : 
            {
         "env_addresses" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "env_name" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "env_network" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "environment_resources" : 
            {
         "env_cpus" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "env_memory" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "env_disk" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "env_baseline" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "env_spec_file" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            }
            
            },
      "environment_state" : 
         {
         datatype: "(menu option)",
         pcre_range: "create|delete|running|suspended|down"
         },
      "environment_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "xen|kvm|esx|test|xen_net|kvm_net|esx_net|test_net|zone|ec2|eucalyptus"
         }
      },
   "files":
      {
      "acl" : 
            {
         "aces" : 
            {
            datatype: "slist",
            pcre_range: "((user|group):[^:]+:[-=+,rwx()dtTabBpcoD]*(:(allow|deny))?)|((all|mask):[-=+,rwx()]*(:(allow|deny))?)"
            },
         "acl_directory_inherit" : 
            {
            datatype: "(menu option)",
            pcre_range: "nochange|parent|specify|clear"
            },
         "acl_method" : 
            {
            datatype: "(menu option)",
            pcre_range: "append|overwrite"
            },
         "acl_type" : 
            {
            datatype: "(menu option)",
            pcre_range: "generic|posix|ntfs"
            },
         "specify_inherit_aces" : 
            {
            datatype: "slist",
            pcre_range: "((user|group):[^:]+:[-=+,rwx()dtTabBpcoD]*(:(allow|deny))?)|((all|mask):[-=+,rwx()]*(:(allow|deny))?)"
            }
            
            },
      "changes" : 
            {
         "hash" : 
            {
            datatype: "(menu option)",
            pcre_range: "md5|sha1|sha256|sha384|sha512|best"
            },
         "report_changes" : 
            {
            datatype: "(menu option)",
            pcre_range: "all|stats|content|none"
            },
         "update_hashes" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_diffs" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "copy_from" : 
            {
         "source" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "servers" : 
            {
            datatype: "slist",
            pcre_range: "[A-Za-z0-9_.:-]+"
            },
         "collapse_destination_dir" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "compare" : 
            {
            datatype: "(menu option)",
            pcre_range: "atime|mtime|ctime|digest|hash|exists|binary"
            },
         "copy_backup" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|timestamp"
            },
         "encrypt" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "check_root" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "copylink_patterns" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "copy_size" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,inf"
            },
         "findertype" : 
            {
            datatype: "(menu option)",
            pcre_range: "MacOSX"
            },
         "linkcopy_patterns" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "link_type" : 
            {
            datatype: "(menu option)",
            pcre_range: "symlink|hardlink|relative|absolute|none"
            },
         "force_update" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "force_ipv4" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "portnumber" : 
            {
            datatype: "int",
            pcre_range: "1024,99999"
            },
         "preserve" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "purge" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "stealth" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "timeout" : 
            {
            datatype: "int",
            pcre_range: "1,3600"
            },
         "trustkey" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "type_check" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "verify" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "create" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "delete" : 
            {
         "dirlinks" : 
            {
            datatype: "(menu option)",
            pcre_range: "delete|tidy|keep"
            },
         "rmdirs" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "depth_search" : 
            {
         "depth" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "exclude_dirs" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "include_basedir" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "include_dirs" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "rmdeadlinks" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "traverse_links" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "xdev" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "edit_line" : edit_line_bundle_syntax,
      "edit_xml" : edit_line_bundle_syntax,
      "edit_defaults" : 
            {
         "edit_backup" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|timestamp|rotate"
            },
         "empty_file_before_editing" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "max_file_size" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "recognize_join" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "file_select" : 
            {
         "leaf_name" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "path_name" : 
            {
            datatype: "slist",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "search_mode" : 
            {
            datatype: "slist",
            pcre_range: "[0-7augorwxst,+-]+"
            },
         "search_size" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,inf"
            },
         "search_owners" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "search_groups" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "search_bsdflags" : 
            {
            datatype: "slist",
            pcre_range: "[+-]*[(arch|archived|nodump|opaque|sappnd|sappend|schg|schange|simmutable|sunlnk|sunlink|uappnd|uappend|uchg|uchange|uimmutable|uunlnk|uunlink)]+"
            },
         "ctime" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,2147483647"
            },
         "mtime" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,2147483647"
            },
         "atime" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,2147483647"
            },
         "exec_regex" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "exec_program" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "file_types" : 
            {
            datatype: "(option list)",
            pcre_range: "plain|reg|symlink|dir|socket|fifo|door|char|block"
            },
         "issymlinkto" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "file_result" : 
            {
            datatype: "string",
            pcre_range: "[(leaf_name|path_name|file_types|mode|size|owner|group|atime|ctime|mtime|issymlinkto|exec_regex|exec_program|bsdflags)[|&!.]*]*"
            }
            
            },
      "link_from" : 
            {
         "copy_patterns" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "link_children" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "link_type" : 
            {
            datatype: "(menu option)",
            pcre_range: "symlink|hardlink|relative|absolute|none"
            },
         "source" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "when_linking_children" : 
            {
            datatype: "(menu option)",
            pcre_range: "override_file|if_no_such_file"
            },
         "when_no_source" : 
            {
            datatype: "(menu option)",
            pcre_range: "force|delete|nop"
            }
            
            },
      "move_obstructions" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "pathtype" : 
         {
         datatype: "(menu option)",
         pcre_range: "literal|regex|guess"
         },
      "perms" : 
            {
         "bsdflags" : 
            {
            datatype: "slist",
            pcre_range: "[+-]*[(arch|archived|nodump|opaque|sappnd|sappend|schg|schange|simmutable|sunlnk|sunlink|uappnd|uappend|uchg|uchange|uimmutable|uunlnk|uunlink)]+"
            },
         "groups" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$.-]+"
            },
         "mode" : 
            {
            datatype: "string",
            pcre_range: "[0-7augorwxst,+-]+"
            },
         "owners" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$.-]+"
            },
         "rxdirs" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "rename" : 
            {
         "disable" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "disable_mode" : 
            {
            datatype: "string",
            pcre_range: "[0-7augorwxst,+-]+"
            },
         "disable_suffix" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "newname" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "rotate" : 
            {
            datatype: "int",
            pcre_range: "0,99"
            }
            
            },
      "repository" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "touch" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "transformer" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         }
      },
   "interfaces":
      {
      "tcp_ip" : 
            {
         "ipv4_address" : 
            {
            datatype: "string",
            pcre_range: "[0-9.]+/[0-4]+"
            },
         "ipv4_netmask" : 
            {
            datatype: "string",
            pcre_range: "[0-9.]+/[0-4]+"
            },
         "ipv6_address" : 
            {
            datatype: "string",
            pcre_range: "[0-9a-fA-F:]+/[0-9]+"
            }
            
            }
      },
   "methods":
      {
      "usebundle" : edit_line_bundle_syntax
      },
   "outputs":
      {
      "output_level" : 
         {
         datatype: "(menu option)",
         pcre_range: "verbose|debug|inform"
         },
      "promiser_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "promise|bundle"
         }
      },
   "packages":
      {
      "package_architectures" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "package_method" : 
            {
         "package_add_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_arch_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_changes" : 
            {
            datatype: "(menu option)",
            pcre_range: "individual|bulk|bulk_no_names"
            },
         "package_delete_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_delete_convention" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_file_repositories" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "package_installed_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_list_arch_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_list_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_list_name_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_list_update_command" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_list_update_ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "-99999999999,9999999999"
            },
         "package_list_version_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_name_convention" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_name_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_noverify_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_noverify_returncode" : 
            {
            datatype: "int",
            pcre_range: "-99999999999,9999999999"
            },
         "package_patch_arch_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_patch_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_patch_installed_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_patch_list_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_patch_name_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_patch_version_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_update_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_verify_command" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "package_version_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "package_multiline_start" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "package_policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "add|delete|reinstall|update|addupdate|patch|verify"
         },
      "package_select" : 
         {
         datatype: "(menu option)",
         pcre_range: ">|<|==|!=|>=|<="
         },
      "package_version" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "processes":
      {
      "process_count" : 
            {
         "in_range_define" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "match_range" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            },
         "out_of_range_define" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            }
            
            },
      "process_select" : 
            {
         "command" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "pid" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            },
         "pgid" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            },
         "ppid" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            },
         "priority" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "-20,+20"
            },
         "process_owner" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "process_result" : 
            {
            datatype: "string",
            pcre_range: "[(process_owner|pid|ppid||pgid|rsize|vsize|status|command|ttime|stime|tty|priority|threads)[|&!.]*]*"
            },
         "rsize" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            },
         "status" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "stime_range" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,2147483647"
            },
         "ttime_range" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,2147483647"
            },
         "tty" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "threads" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            },
         "vsize" : 
            {
            datatype: "irange [int,int]",
            pcre_range: "0,99999999999"
            }
            
            },
      "process_stop" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "restart_class" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "signals" : 
         {
         datatype: "(option list)",
         pcre_range: "hup|int|trap|kill|pipe|cont|abrt|stop|quit|term|child|usr1|usr2|bus|segv"
         }
      },
   "services":
      {
      "service_policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "start|stop|disable"
         },
      "service_dependencies" : 
         {
         datatype: "slist",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "service_method" : 
            {
         "service_type" : 
            {
            datatype: "(menu option)",
            pcre_range: "windows|init|inetd|xinetd"
            },
         "service_args" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "service_autostart_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "none|boot_time|on_demand"
            },
         "service_dependence_chain" : 
            {
            datatype: "(menu option)",
            pcre_range: "ignore|start_parent_services|stop_child_services|all_related"
            }
            
            }
      },
   "storage":
      {
      "mount" : 
            {
         "edit_fstab" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "mount_type" : 
            {
            datatype: "(menu option)",
            pcre_range: "nfs|nfs2|nfs3|nfs4"
            },
         "mount_source" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "mount_server" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "mount_options" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            },
         "unmount" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            },
      "volume" : 
            {
         "check_foreign" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "freespace" : 
            {
            datatype: "string",
            pcre_range: "[0-9]+[MBkKgGmb%]"
            },
         "sensible_size" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "sensible_count" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "scan_arrivals" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            }
      }
   },
  server:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "access":
      {
      "admit" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "deny" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "maproot" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ifencrypted" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "resource_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "path|literal|context|query"
         }
      },
   "roles":
      {
      "authorize" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      }
   },
  monitor:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "measurements":
      {
      "stream_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "pipe|file"
         },
      "data_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "counter|int|real|string|slist"
         },
      "history_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "weekly|scalar|static|log"
         },
      "units" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "match_value" : 
            {
         "select_line_matching" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "select_line_number" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "extraction_regex" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "track_growing_file" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            }
            
            }
      }
   },
  runagent:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      }
   },
  executor:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      }
   },
  knowledge:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "topics":
      {
      "association" : 
            {
         "forward_relationship" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "backward_relationship" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "associates" : 
            {
            datatype: "slist",
            pcre_range: ".*"
            }
            
            },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "occurrences":
      {
      "represents" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "representation" : 
         {
         datatype: "(menu option)",
         pcre_range: "literal|url|db|file|web|image|portal"
         },
      "web_root" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "path_root" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      },
   "inferences":
      {
      "follow_topics" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "infer" : 
            {
         "pre_assoc_pattern" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "post_assoc_pattern" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "inference" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            }
      }
   },
  reporter:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      }
   },
  hub:
   {
   "vars":
      {
      "string" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "int" : 
         {
         datatype: "int",
         pcre_range: "-99999999999,9999999999"
         },
      "real" : 
         {
         datatype: "real",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "slist" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ilist" : 
         {
         datatype: "ilist",
         pcre_range: "-99999999999,9999999999"
         },
      "rlist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "policy" : 
         {
         datatype: "(menu option)",
         pcre_range: "free|overridable|constant|ifdefined"
         }
      },
   "classes":
      {
      "or" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "and" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "xor" : 
         {
         datatype: "clist",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "dist" : 
         {
         datatype: "rlist",
         pcre_range: "-9.99999E100,9.99999E100"
         },
      "expression" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         },
      "not" : 
         {
         datatype: "class",
         pcre_range: "[a-zA-Z0-9_!&@@$|.()]+"
         }
      },
   "reports":
      {
      "friend_pattern" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "intermittency" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "lastseen" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "printfile" : 
            {
         "file_to_print" : 
            {
            datatype: "string",
            pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
            },
         "number_of_lines" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            }
            
            },
      "report_to_file" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "showstate" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
      },
   "*":
      {
      "action" : 
            {
         "action_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "fix|warn|nop"
            },
         "ifelapsed" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "expireafter" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "log_string" : 
            {
            datatype: "string",
            pcre_range: ".*"
            },
         "log_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "log_kept" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_priority" : 
            {
            datatype: "(menu option)",
            pcre_range: "emergency|alert|critical|error|warning|notice|info|debug"
            },
         "log_repaired" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "log_failed" : 
            {
            datatype: "string",
            pcre_range: "stdout|udp_syslog|(\"?[a-zA-Z]:\\\\.*)|(/.*)"
            },
         "value_kept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_repaired" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "value_notkept" : 
            {
            datatype: "real",
            pcre_range: ".*"
            },
         "audit" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "background" : 
            {
            datatype: "(menu option)",
            pcre_range: "true|false|yes|no|on|off"
            },
         "report_level" : 
            {
            datatype: "(menu option)",
            pcre_range: "inform|verbose|error|log"
            },
         "measurement_class" : 
            {
            datatype: "string",
            pcre_range: ".*"
            }
            
            },
      "classes" : 
            {
         "promise_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_failed" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_denied" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "repair_timeout" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "promise_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_kept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_repaired" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "cancel_notkept" : 
            {
            datatype: "slist",
            pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
            },
         "persist_time" : 
            {
            datatype: "int",
            pcre_range: "0,99999999999"
            },
         "timer_policy" : 
            {
            datatype: "(menu option)",
            pcre_range: "absolute|reset"
            }
            
            },
      "ifvarclass" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "handle" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$()\\[\\].]+"
         },
      "depends_on" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "comment" : 
         {
         datatype: "string",
         pcre_range: ".*"
         }
      }
   }
};


var body_control_syntax = {
  common:
   {
      "bundlesequence" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "ignore_missing_bundles" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "ignore_missing_inputs" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "inputs" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "version" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "lastseenexpireafter" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "output_prefix" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "domain" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "require_comments" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "host_licenses_paid" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "syslog_host" : 
         {
         datatype: "string",
         pcre_range: "[a-zA-Z0-9_$.:-]+"
         },
      "syslog_port" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "fips_mode" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         }
   },
  agent:
   {
      "abortclasses" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "abortbundleclasses" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "addclasses" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "agentaccess" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "agentfacility" : 
         {
         datatype: "(menu option)",
         pcre_range: "LOG_USER|LOG_DAEMON|LOG_LOCAL0|LOG_LOCAL1|LOG_LOCAL2|LOG_LOCAL3|LOG_LOCAL4|LOG_LOCAL5|LOG_LOCAL6|LOG_LOCAL7"
         },
      "auditing" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "binarypaddingchar" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "bindtointerface" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "hashupdates" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "childlibpath" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "checksum_alert_time" : 
         {
         datatype: "int",
         pcre_range: "0,60"
         },
      "defaultcopytype" : 
         {
         datatype: "(menu option)",
         pcre_range: "mtime|atime|ctime|digest|hash|binary"
         },
      "dryrun" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "editbinaryfilesize" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "editfilesize" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "environment" : 
         {
         datatype: "slist",
         pcre_range: "[A-Za-z0-9_]+=.*"
         },
      "exclamation" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "expireafter" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "files_single_copy" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "files_auto_define" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "hostnamekeys" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "ifelapsed" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "inform" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "intermittency" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "max_children" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "maxconnections" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "mountfilesystems" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "nonalphanumfiles" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "repchar" : 
         {
         datatype: "string",
         pcre_range: "."
         },
      "default_repository" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "secureinput" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "sensiblecount" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "sensiblesize" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "skipidentify" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "suspiciousnames" : 
         {
         datatype: "slist",
         pcre_range: "List of names to warn about if found during any file search"
         },
      "syslog" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "track_value" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "timezone" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "default_timeout" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "verbose" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         }
   },
  server:
   {
      "cfruncommand" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "maxconnections" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "denybadclocks" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "allowconnects" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "denyconnects" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "allowallconnects" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "trustkeysfrom" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "allowusers" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "dynamicaddresses" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "skipverify" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "logallconnections" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "logencryptedtransfers" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "hostnamekeys" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "auditing" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "bindtointerface" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "serverfacility" : 
         {
         datatype: "(menu option)",
         pcre_range: "LOG_USER|LOG_DAEMON|LOG_LOCAL0|LOG_LOCAL1|LOG_LOCAL2|LOG_LOCAL3|LOG_LOCAL4|LOG_LOCAL5|LOG_LOCAL6|LOG_LOCAL7"
         },
      "port" : 
         {
         datatype: "int",
         pcre_range: "1024,99999"
         },
      "keycacheTTL" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         }
   },
  monitor:
   {
      "forgetrate" : 
         {
         datatype: "real",
         pcre_range: "0,1"
         },
      "monitorfacility" : 
         {
         datatype: "(menu option)",
         pcre_range: "LOG_USER|LOG_DAEMON|LOG_LOCAL0|LOG_LOCAL1|LOG_LOCAL2|LOG_LOCAL3|LOG_LOCAL4|LOG_LOCAL5|LOG_LOCAL6|LOG_LOCAL7"
         },
      "histograms" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "tcpdump" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "tcpdumpcommand" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         }
   },
  runagent:
   {
      "hosts" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "port" : 
         {
         datatype: "int",
         pcre_range: "1024,99999"
         },
      "force_ipv4" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "trustkey" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "encrypt" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "background_children" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "max_children" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "output_to_file" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "timeout" : 
         {
         datatype: "int",
         pcre_range: "1,9999"
         }
   },
  executor:
   {
      "splaytime" : 
         {
         datatype: "int",
         pcre_range: "0,99999999999"
         },
      "mailfrom" : 
         {
         datatype: "string",
         pcre_range: ".*@.*"
         },
      "mailto" : 
         {
         datatype: "string",
         pcre_range: ".*@.*"
         },
      "smtpserver" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "mailmaxlines" : 
         {
         datatype: "int",
         pcre_range: "0,1000"
         },
      "schedule" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "executorfacility" : 
         {
         datatype: "(menu option)",
         pcre_range: "LOG_USER|LOG_DAEMON|LOG_LOCAL0|LOG_LOCAL1|LOG_LOCAL2|LOG_LOCAL3|LOG_LOCAL4|LOG_LOCAL5|LOG_LOCAL6|LOG_LOCAL7"
         },
      "exec_command" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         }
   },
  knowledge:
   {
      "build_directory" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "document_root" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "generate_manual" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "graph_directory" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "graph_output" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "html_banner" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "html_footer" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "id_prefix" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "manual_source_directory" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "query_engine" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "query_output" : 
         {
         datatype: "(menu option)",
         pcre_range: "html|text"
         },
      "sql_type" : 
         {
         datatype: "(menu option)",
         pcre_range: "mysql|postgres"
         },
      "sql_database" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "sql_owner" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "sql_passwd" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "sql_server" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "sql_connection_db" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "style_sheet" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "view_projections" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         }
   },
  reporter:
   {
      "aggregation_point" : 
         {
         datatype: "string",
         pcre_range: "\"?(([a-zA-Z]:\\\\.*)|(/.*))"
         },
      "auto_scaling" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "build_directory" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "csv2xml" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         },
      "error_bars" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "html_banner" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "html_embed" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "html_footer" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "query_engine" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "reports" : 
         {
         datatype: "(option list)",
         pcre_range: "all|audit|performance|all_locks|active_locks|hashes|classes|last_seen|monitor_now|monitor_history|monitor_summary|compliance|setuid|file_changes|installed_software|software_patches|value|variables"
         },
      "report_output" : 
         {
         datatype: "(menu option)",
         pcre_range: "csv|html|text|xml"
         },
      "style_sheet" : 
         {
         datatype: "string",
         pcre_range: ".*"
         },
      "time_stamps" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         }
   },
  hub:
   {
      "export_zenoss" : 
         {
         datatype: "(menu option)",
         pcre_range: "true|false|yes|no|on|off"
         },
      "hub_schedule" : 
         {
         datatype: "slist",
         pcre_range: ".*"
         }
   }
};

