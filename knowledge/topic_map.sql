# CREATE DATABASE cf_topic_map
# USE _topic_map
CREATE TABLE topics(topic_name varchar(256),topic_comment varchar(1024),CREATE TABLE associations(from_name varchar(256),from_type varchar(256),from_assoc varchar(256),to_assoc varchar(256),to_type varchar(256),to_name varchar(256));
CREATE TABLE occurrences(topic_name varchar(256),locator varchar(1024),locator_type varchar(256),subtype varchar(256));
delete from topics
delete from associations
delete from occurrences
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_67','promise_knowledge_cf_67','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-repo..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin/cf-report','_var_cfengine_bin_cf_report','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('mon_update','mon_update','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_63','promise_knowledge_cf_63','promises','Generate knowledge associative graphs')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/enterprise.cf -g','_usr_local_sbin_cf_know__f__srv_www_htdocs_enterprise_cf__g','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('web_server','web_server','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_56','promise_knowledge_cf_56','promises','Generate config reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/local/sbin/cf-promises -r','_usr_local_sbin_cf_promises__r','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_43','promise_knowledge_cf_43','promises','Update magnified view files for monitoring knowledge')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/srv/www/htdocs/reports/192.168.1.100/mean_field.nov','_srv_www_htdocs_reports_192_168_1_100_mean_field_nov','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_35','promise_knowledge_cf_35','promises','Update long view files for monitoring knowledge')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/srv/www/htdocs/reports/192.168.1.100/fluctuations.nov','_srv_www_htdocs_reports_192_168_1_100_fluctuations_nov','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_28','promise_knowledge_cf_28','promises','Make sure knowledge files are accessible to webserver')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/srv/www/htdocs','_srv_www_htdocs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_15','promise_knowledge_cf_15','promises','(Uncommented promise of type <i>vars</i> made by: all_hosts..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('all_hosts','all_hosts','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_12','promise_knowledge_cf_12','promises','(Uncommented promise of type <i>vars</i> made by: doc_root..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('doc_root','doc_root','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('setup_knowledge','setup_knowledge','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_137','promise_webserver_cf_137','promises','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_130','promise_webserver_cf_130','promises','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('APACHE_MODULES=.*','APACHE_MODULES___','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_117','promise_webserver_cf_117','promises','(Uncommented promise of type <i>insert_lines</i> made by: APACHE_CONF_INCLUDE_FILES..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('APACHE_CONF_INCLUDE_FILES=\"/home/mark/cfengine-inputs/httpd.conf\"','APACHE_CONF_INCLUDE_FILES___home_mark_cfengine_inputs_httpd_conf_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_108','promise_webserver_cf_108','promises','(Uncommented promise of type <i>vars</i> made by: del_modules..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('del_modules','del_modules','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_104','promise_webserver_cf_104','promises','(Uncommented promise of type <i>vars</i> made by: add_modules..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('add_modules','add_modules','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('fixapache','fixapache','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_85','promise_webserver_cf_85','promises','(Uncommented promise of type <i>reports</i> made by: The web server software c..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('The web server software could not be installed','The_web_server_software_could_not_be_installed','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!software_ok.on','_software_ok_on','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_76','promise_webserver_cf_76','promises','(Uncommented promise of type <i>files</i> made by: /etc/sysconfig/apache2..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/sysconfig/apache2','_etc_sysconfig_apache2','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('software_ok','software_ok','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('php5','php5','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('apache2-prefork','apache2_prefork','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('apache2-mod_php5','apache2_mod_php5','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_64','promise_webserver_cf_64','promises','(Uncommented promise of type <i>packages</i> made by: apache2..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('on','on','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_53','promise_webserver_cf_53','promises','(Uncommented promise of type <i>commands</i> made by: /etc/init.d/apache2 resta..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/init.d/apache2 restart','_etc_init_d_apache2_restart','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('start_apache','start_apache','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_44','promise_webserver_cf_44','promises','(Uncommented promise of type <i>processes</i> made by: apache2..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('off','off','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_38','promise_webserver_cf_38','promises','(Uncommented promise of type <i>processes</i> made by: apache2..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('apache2','apache2','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('web_ok.on','web_ok_on','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_23','promise_webserver_cf_23','promises','(Uncommented promise of type <i>vars</i> made by: match_package..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('match_package','match_package','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_17','promise_webserver_cf_17','promises','(Uncommented promise of type <i>vars</i> made by: site_http_conf..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('site_http_conf','site_http_conf','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_11','promise_webserver_cf_11','promises','(Uncommented promise of type <i>vars</i> made by: document_root..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('document_root','document_root','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('web_server','web_server','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_laptop_cf_31','promise_laptop_cf_31','promises','(Uncommented promise of type <i>files</i> made by: /home/backup..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/home/backup','_home_backup','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!usb_backup_device_loaded','_usb_backup_device_loaded','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_laptop_cf_22','promise_laptop_cf_22','promises','(Uncommented promise of type <i>files</i> made by: /media/disk-1/CfengineBac..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/media/disk-1/CfengineBackupDevice','_media_disk_1_CfengineBackupDevice','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('usb_backup_device_loaded','usb_backup_device_loaded','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_laptop_cf_6','promise_laptop_cf_6','promises','(Uncommented promise of type <i>vars</i> made by: mount_point..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('mount_point','mount_point','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('laptop','laptop','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_206','promise_library_cf_206','promises','(Uncommented promise of type <i>reports</i> made by: Added a 5 minute schedule..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Added a 5 minute schedule to crontabs','Added_a_5_minute_schedule_to_crontabs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_200','promise_library_cf_200','promises','(Uncommented promise of type <i>replace_patterns</i> made by: cfexecd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfexecd','cfexecd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_196','promise_library_cf_196','promises','(Uncommented promise of type <i>insert_lines</i> made by: 0,5,10,15,20,25,30,35,40,..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','0_5_10_15_20_25_30_35_40_45_50_55__________var_cfengine_bin_cf_execd__F','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('upgrade_cfexecd','upgrade_cfexecd','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_171','promise_library_cf_171','promises','(Uncommented promise of type <i>delete_lines</i> made by: $(regex)..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('$(regex)','__regex_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('DeleteLinesMatching','DeleteLinesMatching','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_119','promise_library_cf_119','promises','(Uncommented promise of type <i>field_edits</i> made by: NETCONFIG_DNS_STATIC_SEAR..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','NETCONFIG_DNS_STATIC_SEARCHLIST__','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_111','promise_library_cf_111','promises','(Uncommented promise of type <i>vars</i> made by: search..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('search','search','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('resolver','resolver','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_422','promise_site_cf_422','promises','(Uncommented promise of type <i>roles</i> made by: .*..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('roles','roles','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('.*','__','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_418','promise_site_cf_418','promises','(Uncommented promise of type <i>access</i> made by: /home/mark/.cfagent/bin/c..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/home/mark/.cfagent/bin/cf-agent','_home_mark__cfagent_bin_cf_agent','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_414','promise_site_cf_414','promises','(Uncommented promise of type <i>access</i> made by: /home/mark/LapTop..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/home/mark/LapTop','_home_mark_LapTop','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('access_rules','access_rules','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_401','promise_site_cf_401','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous low entropy (3d..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_icmp_in) av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','Anomalous_low_entropy__3dev__incoming_ICMP_traffic_on___mon_host__at___mon_env_time_________measured_value___mon_value_icmp_in__av___mon_average_icmp_in__pm___mon_stddev_icmp_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('anomaly_hosts.icmp_in_high_anomaly.!entropy_icmp_in_high','anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_394','promise_site_cf_394','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous (2dev) incoming..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_udp_in) av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','Anomalous__2dev__incoming__non_DNS__UDP_traffic_on___mon_host__at___mon_env_time_________measured_value___mon_value_udp_in__av___mon_average_udp_in__pm___mon_stddev_udp_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_dns_in_low.anomaly_hosts.udp_in_high_dev2','entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_387','promise_site_cf_387','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous (3dev) incoming..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_dns_in) av $(average_dns_in) pm $(mon.stddev_dns_in)','Anomalous__3dev__incoming_DNS_packets_on___mon_host__at___mon_env_time_________measured_value___mon_value_dns_in__av___average_dns_in__pm___mon_stddev_dns_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_dns_in_low.anomaly_hosts.dns_in_high_anomaly','entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_380','promise_site_cf_380','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous number of new T..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous number of new TCP connections on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_tcpsyn_in) av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','Anomalous_number_of_new_TCP_connections_on___mon_host__at___mon_env_time_________measured_value___mon_value_tcpsyn_in__av___mon_average_tcpsyn_in__pm___mon_stddev_tcpsyn_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_tcpsyn_in_low.anomaly_hosts.tcpsyn_in_high_dev2','entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_373','promise_site_cf_373','promises','(Uncommented promise of type <i>reports</i> made by: LOW ENTROPY Incoming www ..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time)\n     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','LOW_ENTROPY_Incoming_www_anomaly_high_anomaly_dev___on___mon_host__at___mon_env_time_________measured_value___svalue_www_in__av___average_www_in__pm___stddev_www_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_www_in_low.anomaly_hosts.www_in_high_anomaly','entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_366','promise_site_cf_366','promises','(Uncommented promise of type <i>reports</i> made by: HIGH ENTROPY Incoming www..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_www_in) av $(mon.average_www_in) pm $(mon.stddev_www_in)','HIGH_ENTROPY_Incoming_www_anomaly_high_anomaly_dev___on___mon_host__at___mon_env_time_________measured_value___mon_value_www_in__av___mon_average_www_in__pm___mon_stddev_www_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_www_in_high&anomaly_hosts.www_in_high_anomaly','entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_359','promise_site_cf_359','promises','(Uncommented promise of type <i>reports</i> made by: RootProc anomaly high 2 d..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('RootProc anomaly high 2 dev on $(mon.host) at $(mon.env_time) \n    measured value $(mon.value_rootprocs) av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','RootProc_anomaly_high_2_dev_on___mon_host__at___mon_env_time_______measured_value___mon_value_rootprocs__av___mon_average_rootprocs__pm___mon_stddev_rootprocs_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('rootprocs_high_dev2','rootprocs_high_dev2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/tmp/shadow','_tmp_shadow','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_349','promise_site_cf_349','promises','(Uncommented promise of type <i>files</i> made by: /tmp/passwd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/tmp/passwd','_tmp_passwd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs/shadow','_var_cfengine_inputs_shadow','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_343','promise_site_cf_343','promises','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs/pass..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs/passwd','_var_cfengine_inputs_passwd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_331','promise_site_cf_331','promises','(Uncommented promise of type <i>vars</i> made by: files..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_330','promise_site_cf_330','promises','(Uncommented promise of type <i>vars</i> made by: sysdir..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('sysdir','sysdir','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('anomalies','anomalies','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_321','promise_site_cf_321','promises','(Uncommented promise of type <i>reports</i> made by: I am alive..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('I am alive','I_am_alive','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('free_disk_watch','free_disk_watch','promises','(Uncommented promise of type <i>measurements</i> made by: /bin/df..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/bin/df','_bin_df','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('apache_watch_memory','apache_watch_memory','promises','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_pr..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('apache_watch_cpu','apache_watch_cpu','promises','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_pr..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/state/cf_procs','_var_cfengine_state_cf_procs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('free_memory_watch','free_memory_watch','promises','(Uncommented promise of type <i>measurements</i> made by: /proc/meminfo..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/proc/meminfo','_proc_meminfo','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('linux','linux','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('monitor_self_watch','monitor_self_watch','promises','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_ro..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/state/cf_rootprocs','_var_cfengine_state_cf_rootprocs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('atlas','atlas','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('watch','watch','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_210','promise_site_cf_210','promises','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf\\..*..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/resolv.conf\\..*','_etc_resolv_conf____','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_204','promise_site_cf_204','promises','(Uncommented promise of type <i>files</i> made by: /var/cfengine/outputs..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/outputs','_var_cfengine_outputs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('garbage_collection','garbage_collection','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_177','promise_site_cf_177','promises','Update the locate database')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/bin/updatedb --prunepaths=/windows','_usr_bin_updatedb___prunepaths__windows','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Hr12','Hr12','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_171','promise_site_cf_171','promises','(Uncommented promise of type <i>commands</i> made by: /etc/init.d/mysql start..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/init.d/mysql start','_etc_init_d_mysql_start','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('start_mysqld','start_mysqld','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_163','promise_site_cf_163','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_155','promise_site_cf_155','promises','(Uncommented promise of type <i>processes</i> made by: mysql..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('mysql','mysql','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_152','promise_site_cf_152','promises','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_138','promise_site_cf_138','promises','Lay a tripwire for change management')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/tmp/testfile2','_tmp_testfile2','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_131','promise_site_cf_131','promises','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/resolv.conf','_etc_resolv_conf','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_123','promise_site_cf_123','promises','(Uncommented promise of type <i>reports</i> made by: Laptop wakes up..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Laptop wakes up','Laptop_wakes_up','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!up_and_running','_up_and_running','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_114','promise_site_cf_114','promises','(Uncommented promise of type <i>vars</i> made by: component..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('component','component','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('main','main','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_101','promise_site_cf_101','promises','(Uncommented promise of type <i>processes</i> made by: cron..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cron','cron','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('exec_fix','exec_fix','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_93','promise_site_cf_93','promises','This is inaccessible in new suse after cd opened/closed')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/dev/sr0','_dev_sr0','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_81','promise_site_cf_81','promises','Check if there are still promises about cfengine 2 that need removing')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs/.*','_var_cfengine_inputs___','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_74','promise_site_cf_74','promises','Test change detection')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/bin','_usr_bin','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_64','promise_site_cf_64','promises','(Uncommented promise of type <i>commands</i> made by: /usr/sbin/ntpdate no.pool..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/sbin/ntpdate no.pool.ntp.org','_usr_sbin_ntpdate_no_pool_ntp_org','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('run_agent','run_agent','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cfagent..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin/cfagent','_var_cfengine_bin_cfagent','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('integrate_cfengine2','integrate_cfengine2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_42','promise_site_cf_42','promises','(Uncommented promise of type <i>vars</i> made by: cf2bits..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf2bits','cf2bits','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfengine','cfengine','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_19','promise_site_cf_19','promises','(Uncommented promise of type <i>vars</i> made by: crontab..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!SuSE','_SuSE','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_15','promise_site_cf_15','promises','(Uncommented promise of type <i>vars</i> made by: acl..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('acl','acl','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_13','promise_site_cf_13','promises','(Uncommented promise of type <i>vars</i> made by: crontab..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('crontab','crontab','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('SuSE','SuSE','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('g','g','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_45','promise_update_cf_45','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-exec..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin/cf-execd','_var_cfengine_bin_cf_execd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('reload.start_exec','reload_start_exec','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_41','promise_update_cf_41','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin/cf-monitord','_var_cfengine_bin_cf_monitord','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('reload.start_monitor','reload_start_monitor','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_37','promise_update_cf_37','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-serv..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin/cf-serverd','_var_cfengine_bin_cf_serverd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('reload.start_server','reload_start_server','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_31','promise_update_cf_31','promises','(Uncommented promise of type <i>processes</i> made by: cf-execd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-execd','cf_execd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_30','promise_update_cf_30','promises','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-monitord','cf_monitord','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_29','promise_update_cf_29','promises','(Uncommented promise of type <i>processes</i> made by: cf-serverd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-serverd','cf_serverd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_21','promise_update_cf_21','promises','(Uncommented promise of type <i>files</i> made by: /var/cfengine/bin..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin','_var_cfengine_bin','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_13','promise_update_cf_13','promises','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs','_var_cfengine_inputs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_7','promise_update_cf_7','promises','(Uncommented promise of type <i>vars</i> made by: master_location..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('master_location','master_location','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('any','any','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('update','update','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system outputs','system_outputs','mission_status','Messages sent by cfengine to the operators as a matter of policy.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('host portal','host_portal','mission_status','Portal to access mission status, monitored host data')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system reports','system_reports','mission_status','Reports from around the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system policy','system_policy','mission_status','Local policy description with comments and dependencies')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('suppliers','suppliers','manager_view','Contact information and web pages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('current projects','current_projects','manager_view','Team projects and service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('work shifts','work_shifts','manager_view','Staff rotas')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service level agreements','service_level_agreements','manager_view','Promises made to clients about services')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('software licenses','software_licenses','manager_view','Terms of agreements, and renewal status')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('request for change','request_for_change','manager_view','(RFC) The initial proposal for an intended change of system policy.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('security routines and procedures','security_routines_and_procedures','security_view','Non-automated, human workbook')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('security policy','security_policy','security_view','Local risk assessment summary')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('quality assurance','quality_assurance','architect_view','Procedures for review')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mission statement','mission_statement','architect_view','Business objectives')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('security view','security_view','views')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('operator view','operator_view','views')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('manager view','manager_view','views')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('architect view','architect_view','views')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mission status','mission_status','system_knowledge','Overview of IT operations')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('views','views','system_knowledge','Roles for IT stakeholders')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('management issues','management_issues','system_knowledge','The main areas of management')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ontology','ontology','system_knowledge','Main index')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copernicus','copernicus','system_knowledge','Cfengine\'s online support knowledge base')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('machine resources','machine_resources','system_knowledge','An inventory of physical and virtual resources')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('server lifecycle','server_lifecycle','system_knowledge','From creation to update and eventual reinstallation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service catalogue','service_catalogue','system_knowledge','The basic service promises of the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system trends','system_trends','system_knowledge','Long term performance data about the system.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system reports','system_reports','system_knowledge','Reports generated by cfengine')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hosts','hosts','machine_resources','A general term for computers that run applications of any kind')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system knowledge','system_knowledge','Frequently_Asked_Topics','Cfengine knowledge console')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('security','security','Frequently_Asked_Topics','Defending an acceptable level of risk')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('management issues','management_issues','Frequently_Asked_Topics','Decision making support')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('operating systems','operating_systems','Frequently_Asked_Topics','System software')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Frequently Asked Topics','Frequently_Asked_Topics','any')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('linear topic map notation','linear_topic_map_notation','topic_maps')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('the tao','the_tao','topic_maps','topics, associations and occurrences')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('lvalue','lvalue','body_constraint')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('rvalue','rvalue','body_constraint')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('voluntary cooperation','voluntary_cooperation','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('roles','roles','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('versions','versions','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('dependencies','dependencies','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('workflows','workflows','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('stories','stories','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('services','services','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('communicated information','communicated_information','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('subjects','subjects','promise_theory','An abstraction for any things that can be discussed')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('occurrences','occurrences','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('occurrence-types','occurrence_types','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('topic-instances','topic_instances','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('type-instances','type_instances','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('associations','associations','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('structural relationships','structural_relationships','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('location relationships','location_relationships','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('causative relationships','causative_relationships','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('conceptual relationships','conceptual_relationships','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('actions','actions','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('behaviours','behaviours','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('facts','facts','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('topics','topics','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('topic-types','topic_types','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('body-constraint','body_constraint','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('body-type','body_type','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promise body','promise_body','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promise-type','promise_type','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promisee','promisee','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promiser','promiser','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('announcements','announcements','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('conditional promises','conditional_promises','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promises','promises','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promise proposals','promise_proposals','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('policy','policy','promise_theory','A declared set of intentions describing how systems should behave')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('bundles','bundles','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('intentions','intentions','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('agents','agents','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('convergence','convergence','convergent_operators','The idea that we always move closer to the desired state of a system on every promise verification. Once the desired state has been reached, the state hsa converged and no further changes take place. Normally convergence to the desired state happens in a single repair, unless there are dependencies that cannot be resolved. Convergence is a stronger constraint on system behaviour than idempotence.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('idempotence','idempotence','convergent_operators','Idempotence is a mathematical property of operations or changes. An operator is idempotent if applying it twice is the same as applying it once. This is not the same as convergence, because the result need not be anchored to anything. e.g. <pre>touch myfile</pre> is idempotent but it is not a fixed point because its effect depends on which directory the operation is performed in. <pre>touch /tmp/myfile</pre> is convergent.')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('overlapping sets','overlapping_sets','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cascading','cascading','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('object paradigm','object_paradigm','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('aspect paradigm','aspect_paradigm','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('service orientation','service_orientation','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('hierarchical structure','hierarchical_structure','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('peer to peer network','peer_to_peer_network','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('heterogeneous system','heterogeneous_system','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('homogeneous system','homogeneous_system','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pattern matching','pattern_matching','models_of_knowledge','Classifying items by the patterns they form')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('topic maps','topic_maps','models_of_knowledge','ISO standard electronic index with semantic annotations')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('self-healing','self_healing','models_of_system_administration','System exhibiting automated repair and maintenance')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('autonomics','autonomics','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('computer immunology','computer_immunology','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('convergent operators','convergent_operators','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promise theory','promise_theory','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('BDMA','BDMA','models_of_system_administration','Build, Deploy, Manage, Audit, system lifecycle')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('audit','audit','BDMA','Traditionally servers needed to be audited to determine their actual state. Under cfengine, auditing simply means generating summary reports about recent compliance or repair operations.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('manage','manage','BDMA','Traditionally servers needed to be managed by responding to incidents. Under cfengine indicdents are repaired automatically and management is chiefly about observing and diving the success of the current system policy.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('deploy','deploy','BDMA','Traditionally servers were deployed by pushing out a cloned image. Under cfengine the deployment of policy just means publishing a new version of policy on a policy server.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('build','build','BDMA','Also called \"provisioning\" systems, i.e. the process of specifying and assembling the resources to fulfill a set of system promises. Traditionally servers were built as a \"golden image\" for cloning. Under cfengine, the build phase of the server lifecycle involves planning policy for the desired-state. Installation may also be considered a part of the build process.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('inventory','inventory','management_issues','List of items within a datacenter, e.g. servers, applications, services. Expressed in terms of a static database that is being updated every now on then, depending on how often you scan the network. Cfengine holds accurate inventory lists and reports can easily be created on the fly. Cfengine dosen’t hold any static database that requires frequent scan (that takes a lot of resources) to describe the inventory.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system discovery','system_discovery','management_issues','The ability to detect new items (see CIs) in a datacenter. Cfengine recognizes new items on the fly and you can have them reported automatically. No need for data mining or guesses.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('configuration management','configuration_management','management_issues','The ability to configure a Datacenter through configuration settings. This is the core of Cfengine. Cfengine comes with many advanced tools to ease the configuration of Datacenters and have it self-repair according the settings you define. See also Configuration Items (CIs), Change Management.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('change management','change_management','management_issues','The ability to trace, log and manipulate change in files. Whenever a change occurs in a file, this probably affects some other parts of the system. The ability to track, log and potentially restore changes are important for many purposes.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('reporting','reporting','management_issues','Extracting information in human readable form')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('application management','application_management','management_issues','Deploying and patching applications, inclding the ability to configure specific applications, like Apache-servers, Microsoft Exchange, etc.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('release management','release_management','management_issues','The process of preparing for major changes to policy or enterprise activity.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('incident management','incident_management','management_issues','Repairing policy violations')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('management terminology','management_terminology','management_issues')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('knowledge management','knowledge_management','management_issues')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('identity management','identity_management','management_issues','Managing user credentials')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('storage management','storage_management','management_issues','Disk volumes, primary, redundant and secondary storage (backup)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('capacity planning','capacity_planning','management_issues','Sometimes called dimensioning or service provisioning. Determining whether sufficient resources will be available to deliver planned services')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('compliance','compliance','management_issues','Frameworks for standardizing and regulating systems')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('team work','team_work','management_issues','Focused group in a flat organization for efficient communication and decision-making')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('maintenance paradigms','maintenance_paradigms','management_issues','Approaches to keeping systems in an operational state')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('business value','business_value','management_issues')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('anomaly detection','anomaly_detection','monitoring')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('models of organization','models_of_organization','research')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('models of knowledge','models_of_knowledge','research')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('models of system administration','models_of_system_administration','research')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('phone number mobile','phone_number_mobile','Contact_Information')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('phone number office','phone_number_office','Contact_Information')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('website','website','Contact_Information')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('home page','home_page','Contact_Information')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('email address','email_address','Contact_Information')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('gentoo','gentoo','operating_systems','Linux distro gentoo')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('slackware','slackware','operating_systems','Linux distro slackware')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('debian','debian','operating_systems','Linux distro debian')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('redhat','redhat','operating_systems','Linux distro redhat')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('fedora','fedora','operating_systems','Linux distro fedora')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ubuntu','ubuntu','operating_systems','Linux distro ubuntu')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('suse','suse','operating_systems','Linux distro suse')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('darwin','darwin','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('windows','windows','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('distro','distro','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('crayos','crayos','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('netbsd','netbsd','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('openbsd','openbsd','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('freebsd','freebsd','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('irix','irix','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('ultrix','ultrix','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('aix','aix','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('macos X','macos_X','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('sunos','sunos','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('solaris','solaris','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('gnu/linux','gnu_linux','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('linux','linux','operating_systems')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cfengine 2 control settings','cfengine_2_control_settings','cfengine_2','Parameters that affect the behaviour of the agent rather than affect the system configuration.')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('$(cf2_actions)','__cf2_actions_','cfengine_2')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('$(nova_actions)','__nova_actions_','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('$(cf3_actions)','__cf3_actions_','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('rlist','rlist','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('real','real','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('ilist','ilist','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('int','int','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('slist','slist','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('string','string','cfengine_3')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('enterprise level cfengine','enterprise_level_cfengine','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cfengine 3.0.x','cfengine_3_0_x','cfengine_components','cfengine version')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cfengine 2.2.x','cfengine_2_2_x','cfengine_components','cfengine version')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cfengine nova','cfengine_nova','cfengine_components','cfengine version')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfengine 3','cfengine_3','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfengine 2','cfengine_2','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfengine 1','cfengine_1','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-report','cf_report','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-know','cf_know','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-runagent','cf_runagent','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-monitord','cf_monitord','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-execd','cf_execd','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-serverd','cf_serverd','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-agent','cf_agent','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf-promises','cf_promises','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfd','cfd','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfenvgraph','cfenvgraph','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfshow','cfshow','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfrun','cfrun','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfenvd','cfenvd','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfexecd','cfexecd','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfservd','cfservd','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfagent','cfagent','cfengine_components')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfengine components','cfengine_components','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('data types','data_types','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('versions','versions','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('global variables','global_variables','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('lists','lists','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('functions','functions','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('arrays','arrays','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('variables','variables','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('template','template','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('modules','modules','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('distributed system','distributed_system','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('research','research','technical_concepts','Cfengine research')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('files','files','technical_concepts','Notable or special files and types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('entropy','entropy','technical_concepts','A measure of the variation in an observation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('intermittency','intermittency','technical_concepts','The entropy of availability of a service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('faults','faults','technical_concepts','Error messages and conditions')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('software','software','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('crises and fire fighting','crises_and_fire_fighting','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('unusual operational behaviour','unusual_operational_behaviour','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('normal operational behaviour','normal_operational_behaviour','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('budget','budget','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('availability','availability','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('reliability','reliability','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rollback','rollback','technical_concepts','Refers to the fictitious ability to perform an undo operation in change management')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('redundancy','redundancy','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('resources','resources','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('symptoms','symptoms','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('automation','automation','technical_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('tamperproof system','tamperproof_system','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('lastseen','lastseen','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('instrumentation','instrumentation','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('friend status','friend_status','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('role based access control','role_based_access_control','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('checksums','checksums','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('hash','hash','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('hashes','hashes','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('digest','digest','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('digests','digests','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('key exchange','key_exchange','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('trusted third party','trusted_third_party','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('firewalls','firewalls','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('trust in cfengine','trust_in_cfengine','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('DMZ','DMZ','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('publickeys','publickeys','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('private keys','private_keys','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('public private keys','public_private_keys','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('encryption','encryption','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('privacy','privacy','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('permission','permission','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('authentication','authentication','security')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('access control','access_control','security')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('monitoring','monitoring','environment','Ability to monitor the current state of your datacenter. Cfengine can sample and trace behaviour. Unlike many other monitoring solutions, Cfengine scales easily to thousands of machines. Cfengine monitors your datacenter intelligently, using historic data to decide whether a behaviour should call for action or not.')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('persistent classes','persistent_classes','environment')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('soft classes','soft_classes','environment')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('hard classes','hard_classes','environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('provisioning','provisioning','management_terminology','The ability to install and adopt new services, applications, OS and servers, e.g. using PXE-boot Cfengine can turn any server into anything kind of server with any kind of service running on it. Cfengine can easily deploy a service or new application throughout the datacenter, regardless of the number of servers.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pre-screening','pre_screening','management_terminology','The ability to foresee consequences of change. If you make a change one place, what’s the impact of the rest of the datacenter? Cfengine includes a dry-run mode which simulates the consequences of a change.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('CI','CI','management_terminology','Configuration Items. Computer related items that can be configured. Items typically are servers, routers, switches, applications and storage devices. Cfengine can discover different types of CIs automatically, trace dependencies and make them available in reports through an online knowledge portal.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('baseline','baseline','management_terminology','Definition of what your system looks like or what it must look like. Many traditional vendors in the industry require a baseline to be able to roll out their system and get it running. In Cfengine, there is no need for a special baseline. Cfengine focuses on the opposite – the end state.')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('enterprise','enterprise','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('agreement','agreement','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('contract','contract','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('SLA','SLA','management_terminology','Service Level Agreement')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('operations','operations','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('business process','business_process','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('FCAPS','FCAPS','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('budget','budget','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('CM','CM','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('gold server','gold_server','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('version Control','version_Control','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('teams','teams','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('teamwork','teamwork','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('departments','departments','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('authoring','authoring','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('adoption of cfengine','adoption_of_cfengine','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('road map','road_map','management_terminology')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('BDIM','BDIM','management_terminology','Business Driven IT Management')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('auditing','auditing','compliance','A process to verify that the datacenter operate according to the guidelines and legislations. A rapid growth in datacenter size and complexity, combined with government regulations, have called for a drastic increase in audits.  Cfengine comes with reporting and logging capabilties that make auditing straightforward.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('CMDB','CMDB','compliance','Configuration Management Database. A form of extended inventort system, championed by ITIL.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('SAS-70','SAS_70','compliance','Stands for Statement of Auditing Standards no. 70. SAS-70 is an auditing standard. SAS 70 defines the professional standards used by a service auditor to assess the internal controls of a service organization and issue a service auditor’s report. Cfengine can be used to ensure SAS-70 compliance in the same way you can achieve other standardization compliance.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('EUROSOX','EUROSOX','compliance','The 8th EU data directive')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('SOX','SOX','compliance','The Sarbanes-Oxley Act')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('eTOM','eTOM','compliance','The Extended Telecom Operations Map')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ISO/IEC 20000','ISO_IEC_20000','compliance','A standards document based on a subset of ITIL')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ITIL','ITIL','compliance','Stands for Information Technology Infrastructure Library. ITIL is a set of self-proclaimed best-practises originally developed by the British government. It contains a set of common sense rules. Cfengine can be used as a tool for organizations looking to becoming ITIL-compliant. By translating many of your best practises into policy rules, you can have Cfengine ensure parts of ITIL-compliance.')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('access to file not granted','access_to_file_not_granted','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('trustKeysFrom not set','trustKeysFrom_not_set','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('allowMultipleConnectionsFrom not set','allowMultipleConnectionsFrom_not_set','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('allowConnectionsFrom not set','allowConnectionsFrom_not_set','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('bad memory reference','bad_memory_reference','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('print server not running','print_server_not_running','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('web server not running','web_server_not_running','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('remote connection problem','remote_connection_problem','faults')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('segmentation fault','segmentation_fault','faults')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('samba','samba','storage','The Unix service for integrating with MS Windows')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('NFS','NFS','storage','Network File System (storage)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('oracle','oracle','database','Commercial relational SQL database')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('postgres','postgres','database','Open source relational SQL database engine for transaction processing')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mysql','mysql','database','Open source SQL database engine optimized for web')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('DHCP','DHCP','directory_services','An IP configuration service.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('NIS','NIS','directory_services','Sun\'s Network Information Service (previously called Yellow Pages).')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('LDAP','LDAP','directory_services','A well-known directory service. MS Active Directory purports to support this standard.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('DNS','DNS','directory_services','Domain Name Service for translating Internet addresses into hostnames and vice versa.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('BOOTP','BOOTP','boot_services','A simple broadcast protocol for booting a system directly from the network, e.g. for installation purposes.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('PXE','PXE','boot_services','A composite network service based on the PXE protocol, allows systems to boot directly from the network.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('connectivity','connectivity','network_services','Cables and hardware')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('routing','routing','network_services','Router and switch configuration')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('login','login','application_services','The ability to log onto systems with a username and password')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('database','database','application_services','Relational database services')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('storage','storage','application_services','Remotely accessible disk volumes.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('directory services','directory_services','application_services','Directory services are look-up services, typically with identity or host information.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('web','web','application_services','The WWW web service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('configuration','configuration','application_services','Cfengine desired-state automation and self-repair service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mail','mail','application_services','Electronic mail service, as made famous by the Unix operating system.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('monitoring','monitoring','service_catalogue','Pseudo-real-time status information about system components')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('network services','network_services','service_catalogue','Network connectivity and availability')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('application services','application_services','service_catalogue','Software running on servers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('boot services','boot_services','service_catalogue','Boot services are a way of booting a computer directly from the network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('installation','installation','service_catalogue','Installing computers from bare metal')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('userexists','userexists','functions','True if user name or numerical id exists on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('usemodule','usemodule','functions','Execute cfengine module script and set class if successful')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('translatepath','translatepath','functions','Translate path separators from Unix style to the host\'s native')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('strcmp','strcmp','functions','True if the two strings match exactly')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('splitstring','splitstring','functions','Convert a string in arg1 into a list of max arg3 strings by splitting on a regular expression in arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('splayclass','splayclass','functions','True if the first argument\'s time-slot has arrived, according to a policy in arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('selectservers','selectservers','functions','Select tcp servers which respond correctly to a query and return their number, set array of names')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rrange','rrange','functions','Define a range of real numbers for cfengine internal use')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('returnszero','returnszero','functions','True if named shell command has exit status zero')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('remoteclassesmatching','remoteclassesmatching','functions','Read persistent classes matching a regular expression from a remote cfengine server and add them into local context with prefix')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('remotescalar','remotescalar','functions','Read a scalar value from a remote cfengine server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regldap','regldap','functions','True if the regular expression in arg6 matches a value item in an ldap search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('reglist','reglist','functions','True if the regular expression in arg2 matches any item in the list whose id is arg1')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regline','regline','functions','True if the regular expression in arg1 matches a line in file arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('registryvalue','registryvalue','functions','Returns a value for an MS-Win registry key,value pair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regextract','regextract','functions','True if the regular expression in arg 1 matches the string in arg2 and sets a non-empty array of backreferences named arg3')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regcmp','regcmp','functions','True if arg1 is a regular expression matching that matches string arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regarray','regarray','functions','True if arg1 matches any item in the associative array with id=arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readtcp','readtcp','functions','Connect to tcp port, send string and assign result to variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readstringlist','readstringlist','functions','Read and assign a list variable from a file of separated strings')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readstringarray','readstringarray','functions','Read an array of strings from a file and assign the dimension to a variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readreallist','readreallist','functions','Read and assign a list variable from a file of separated real numbers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readrealarray','readrealarray','functions','Read an array of real numbers from a file and assign the dimension to a variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readintlist','readintlist','functions','Read and assign a list variable from a file of separated ints')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readintarray','readintarray','functions','Read an array of integers from a file and assign the dimension to a variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('readfile','readfile','functions','Read max number of bytes from named file and assign to variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('randomint','randomint','functions','Generate a random integer between the given limits')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('peerleaders','peerleaders','functions','Get a list of peer leaders from the named partitioning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('peerleader','peerleader','functions','Get the assigned peer-leader of the partition to which we belong')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('peers','peers','functions','Get a list of peers (not including ourself) from the partition to which we belong')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('on','on','functions','Convert an exact date/time to an integer system representation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('now','now','functions','Convert the current time into system representation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ldapvalue','ldapvalue','functions','Extract the first matching named value from ldap')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ldaplist','ldaplist','functions','Extract all named values from multiple ldap records')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ldaparray','ldaparray','functions','Extract all values from an ldap record')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('lastnode','lastnode','functions','Extract the last of a separated string, e.g. filename from a path')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('join','join','functions','Join the items of arg2 into a string, using the conjunction in arg1')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isvariable','isvariable','functions','True if the named variable is defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isplain','isplain','functions','True if the named object is a plain/regular file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isnewerthan','isnewerthan','functions','True if arg1 is newer (modified later) than arg2 (mtime)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('islink','islink','functions','True if the named object is a symbolic link')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('islessthan','islessthan','functions','True if arg1 is numerically less than arg2, else compare strings like NOT strcmp')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isgreaterthan','isgreaterthan','functions','True if arg1 is numerically greater than arg2, else compare strings like strcmp')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isdir','isdir','functions','True if the named object is a directory')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('irange','irange','functions','Define a range of integer values for cfengine internal use')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('iprange','iprange','functions','True if the current host lies in the range of IP addresses specified')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hostsseen','hostsseen','functions','Extract the list of hosts last seen/not seen within the last arg1 hours')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hostrange','hostrange','functions','True if the current host lies in the range of enumerated hostnames specified')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hostinnetgroup','hostinnetgroup','functions','True if the current host is in the named netgroup')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('host2ip','host2ip','functions','Returns the primary name-service IP address for the named host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hashmatch','hashmatch','functions','Compute the hash of arg1, of type arg2 and test if it matches the value in arg 3')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hash','hash','functions','Return the hash of arg1, type arg2 and assign to a variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('groupexists','groupexists','functions','True if group or numerical id exists on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('grep','grep','functions','Extract the sub-list if items matching the regular expression in arg1 of the list named in arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getuid','getuid','functions','Return the integer user id of the named user on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getgid','getgid','functions','Return the integer group id of the named group on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getenv','getenv','functions','Return the environment variable named arg1, truncated at arg2 characters')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getindices','getindices','functions','Get a list of keys to the array whose id is the argument and assign to variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getfields','getfields','functions','Get an array of fields in the lines matching regex arg1 in file arg2, split on regex arg3 as array name arg4')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('filesexist','filesexist','functions','True if the named list of files can ALL be accessed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('fileexists','fileexists','functions','True if the named file can be accessed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('execresult','execresult','functions','Execute named command and assign output to variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('escape','escape','functions','Escape regular expression characters in a string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('diskfree','diskfree','functions','Return the free space (in KB) available on the directory\'s current partition (0 if not found)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('countlinesmatching','countlinesmatching','functions','Count the number of lines matching regex arg1 in file arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('countclassesmatching','countclassesmatching','functions','Count the number of defined classes matching regex arg1')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classmatch','classmatch','functions','True if the regular expression matches any currently defined class')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classify','classify','functions','True if the canonicalization of the argument is a currently defined class')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('changedbefore','changedbefore','functions','True if arg1 was changed before arg2 (ctime)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('canonify','canonify','functions','Convert an abitrary string into a legal class name')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ago','ago','functions','Convert a time relative to now to an integer system representation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('accumulated','accumulated','functions','Convert an accumulated amount of time into a system representation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('accessedbefore','accessedbefore','functions','True if arg1 was accessed before arg2 (atime)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('class','class','data_types','A boolean returned by certain functions in classes promises')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('policy','policy','data_types','The policy for (dis)allowing (re)definition of variables matching free,overridable,constant,ifdefined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rlist','rlist','data_types','A list of real numbers matching -9.99999E100,9.99999E100')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ilist','ilist','data_types','A list of integers matching -99999999999,9999999999')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('slist','slist','data_types','A list of scalar strings matching ')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('real','real','data_types','A scalar real number matching -9.99999E100,9.99999E100')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('int','int','data_types','A scalar integer matching -99999999999,9999999999')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('string','string','data_types','A scalar string matching ')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('data types','data_types','miscellaneous_concepts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('select_region','select_region','values')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('comment','comment','values')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('depends_on','depends_on','values')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('handle','handle','values')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('ifvarclass','ifvarclass','values')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('classes','classes','values')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('action','action','values')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('track_growing_file','track_growing_file','body_constraints','If true, cfengine remembers the position to which is last read when opening the file, and resets to the start if the file has since been truncated')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('track_growing_file','track_growing_file','values','If true, cfengine remembers the position to which is last read when opening the file, and resets to the start if the file has since been truncated')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('extraction_regex','extraction_regex','body_constraints','Regular expression that should contain a single backreference for extracting a value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('extraction_regex','extraction_regex','values','Regular expression that should contain a single backreference for extracting a value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_line_number','select_line_number','body_constraints','Read from the n-th line of the output (fixed format)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_line_number','select_line_number','values','Read from the n-th line of the output (fixed format)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('match_value','match_value','body_constraints','Criteria for extracting the measurement from a datastream')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('units','units','body_constraints','The engineering dimensions of this value or a note about its intent used in plots')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('history_type','history_type','body_constraints','Whether the data can be seen as a time-series or just an isolated value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('data_type','data_type','body_constraints','The datatype being collected.')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stream_type','stream_type','body_constraints','The datatype being collected.')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('measurements','measurements','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('inference','inference','body_constraints','Result of the syllogism')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('inference','inference','values','Result of the syllogism')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('post_assoc_pattern','post_assoc_pattern','body_constraints','Name of backward/inverse association from associates to promiser topic')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('post_assoc_pattern','post_assoc_pattern','values','Name of backward/inverse association from associates to promiser topic')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pre_assoc_pattern','pre_assoc_pattern','body_constraints','Name of forward association between promiser topic and associates')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('infer','infer','body_constraints','Specify the promise-inference triangle from this topic')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('follow_topics','follow_topics','body_constraints','Use the knowledge promised by topics matching this pattern')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('inferences','inferences','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('path_root','path_root','body_constraints','Base path of the occurrence when locating file (replaced by web_root)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('web_root','web_root','body_constraints','Base URL of the occurrence when rendered as a web-URL (replaces path_root)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('representation','representation','body_constraints','How to interpret the promiser string e.g. actual data or reference to data')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('represents','represents','body_constraints','List of subtopics that explains the type(s) of information represented by the occurrence')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('occurrences','occurrences','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('associates','associates','body_constraints','List of associated topics by this forward relationship')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('associates','associates','values','List of associated topics by this forward relationship')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('backward_relationship','backward_relationship','body_constraints','Name of backward/inverse association from associates to promiser topic')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('backward_relationship','backward_relationship','values','Name of backward/inverse association from associates to promiser topic')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('forward_relationship','forward_relationship','body_constraints','Name of forward association between promiser topic and associates')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('association','association','body_constraints','Declare associated topics')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('topics','topics','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('authorize','authorize','body_constraints','List of public-key user names that are allowed to activate the promised class during remote agent activation')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('roles','roles','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('resource_type','resource_type','body_constraints','The type of object being granted access (the default grants access to files)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ifencrypted','ifencrypted','body_constraints','true/false whether the current file access promise is conditional on the connection from the client being encrypted')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('maproot','maproot','body_constraints','List of host names or IP addresses to grant full read-privilege on the server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('deny','deny','body_constraints','List of host names or IP addresses to deny access to file objects')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('admit','admit','body_constraints','List of host names or IP addresses to grant access to file objects')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('access','access','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('scan_arrivals','scan_arrivals','body_constraints','true/false generate pseudo-periodic disk change arrival distribution')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('scan_arrivals','scan_arrivals','values','true/false generate pseudo-periodic disk change arrival distribution')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('sensible_count','sensible_count','body_constraints','Minimum number of files that should be defined on a sensible-looking storage device')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('sensible_count','sensible_count','values','Minimum number of files that should be defined on a sensible-looking storage device')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('sensible_size','sensible_size','body_constraints','Minimum size in bytes that should be used on a sensible-looking storage device')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('freespace','freespace','body_constraints','Absolute or percentage minimum disk space that should be available before warning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('freespace','freespace','values','Absolute or percentage minimum disk space that should be available before warning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('check_foreign','check_foreign','body_constraints','true/false verify storage that is mounted from a foreign system on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('volume','volume','body_constraints','Criteria for monitoring/probing mounted volumes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('unmount','unmount','body_constraints','true/false unmount a previously mounted filesystem')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('unmount','unmount','values','true/false unmount a previously mounted filesystem')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_options','mount_options','body_constraints','List of option strings to add to the file system table (\"fstab\")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_options','mount_options','values','List of option strings to add to the file system table (\"fstab\")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_server','mount_server','body_constraints','Hostname or IP or remote file system server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_source','mount_source','body_constraints','Path of remote file system to mount')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_type','mount_type','body_constraints','Protocol type of remote file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_type','mount_type','values','Protocol type of remote file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_fstab','edit_fstab','body_constraints','true/false add or remove entries to the file system table (\"fstab\")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount','mount','body_constraints','Criteria for mounting foreign file systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('storage','storage','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_dependence_chain','service_dependence_chain','body_constraints','How to handle dependencies and dependent services')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_autostart_policy','service_autostart_policy','body_constraints','Should the service be started automatically by the OS')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_autostart_policy','service_autostart_policy','values','Should the service be started automatically by the OS')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_args','service_args','body_constraints','Parameters for starting the service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_type','service_type','body_constraints','Service abstraction type')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_method','service_method','body_constraints','Details of the service specification')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_dependencies','service_dependencies','body_constraints','A list of services on which this service depends')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('service_policy','service_policy','body_constraints','Policy for service status to maintain')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('services','services','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('signals','signals','body_constraints','A list of menu options representing signals to be sent to a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('restart_class','restart_class','body_constraints','A class to be defined globally if the process is not running, so that a command: rule can be referred to restart the process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_stop','process_stop','body_constraints','A command used to stop a running process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('vsize','vsize','body_constraints','Range of integers matching the virtual memory size of a process, in kilobytes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('vsize','vsize','values','Range of integers matching the virtual memory size of a process, in kilobytes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('threads','threads','body_constraints','Range of integers matching the threads (NLWP) field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('threads','threads','values','Range of integers matching the threads (NLWP) field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('tty','tty','body_constraints','Regular expression matching the tty field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ttime_range','ttime_range','body_constraints','Range of integers matching the total elapsed time of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stime_range','stime_range','body_constraints','Range of integers matching the start time of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stime_range','stime_range','values','Range of integers matching the start time of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('status','status','body_constraints','Regular expression matching the status field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('status','status','values','Regular expression matching the status field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rsize','rsize','body_constraints','Range of integers matching the resident memory size of a process, in kilobytes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_result','process_result','body_constraints','Boolean class expression returning the logical combination of classes set by a process selection test')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_result','process_result','values','Boolean class expression returning the logical combination of classes set by a process selection test')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_owner','process_owner','body_constraints','List of regexes matching the user of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('priority','priority','body_constraints','Range of integers matching the priority field (PRI/NI) of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('priority','priority','values','Range of integers matching the priority field (PRI/NI) of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ppid','ppid','body_constraints','Range of integers matching the parent process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ppid','ppid','values','Range of integers matching the parent process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pgid','pgid','body_constraints','Range of integers matching the parent group id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pgid','pgid','values','Range of integers matching the parent group id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pid','pid','body_constraints','Range of integers matching the process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pid','pid','values','Range of integers matching the process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('command','command','body_constraints','Regular expression matching the command/cmd field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_select','process_select','body_constraints','Criteria for matching processes in the system process table')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('out_of_range_define','out_of_range_define','body_constraints','List of classes to define if the matches are out of range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('out_of_range_define','out_of_range_define','values','List of classes to define if the matches are out of range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('match_range','match_range','body_constraints','Integer range for acceptable number of matches for this process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('match_range','match_range','values','Integer range for acceptable number of matches for this process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('in_range_define','in_range_define','body_constraints','List of classes to define if the matches are in range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_count','process_count','body_constraints','Criteria for constraining the number of processes matching other criteria')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('processes','processes','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_version','package_version','body_constraints','Version reference point for determining promised version')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_select','package_select','body_constraints','A criterion for first acceptable match relative to \"package_version\"')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_policy','package_policy','body_constraints','Criteria for package installation/upgrade on the current system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_multiline_start','package_multiline_start','body_constraints','Regular expression which matches the start of a new package in multiline output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_multiline_start','package_multiline_start','values','Regular expression which matches the start of a new package in multiline output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_version_regex','package_version_regex','body_constraints','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_verify_command','package_verify_command','body_constraints','Command to verify the correctness of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_update_command','package_update_command','body_constraints','Command to update to the latest version a currently installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_update_command','package_update_command','values','Command to update to the latest version a currently installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_version_regex','package_patch_version_regex','body_constraints','Regular expression with one backreference to extract update version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_version_regex','package_patch_version_regex','values','Regular expression with one backreference to extract update version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_name_regex','package_patch_name_regex','body_constraints','Regular expression with one backreference to extract update name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_list_command','package_patch_list_command','body_constraints','Command to obtain a list of available patches or updates')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_list_command','package_patch_list_command','values','Command to obtain a list of available patches or updates')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_installed_regex','package_patch_installed_regex','body_constraints','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_command','package_patch_command','body_constraints','Command to update to the latest patch release of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_command','package_patch_command','values','Command to update to the latest patch release of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_arch_regex','package_patch_arch_regex','body_constraints','Regular expression with one backreference to extract update architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_arch_regex','package_patch_arch_regex','values','Regular expression with one backreference to extract update architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_returncode','package_noverify_returncode','body_constraints','Integer return code indicating package verification failure')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_returncode','package_noverify_returncode','values','Integer return code indicating package verification failure')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_regex','package_noverify_regex','body_constraints','Regular expression to match verification failure output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_regex','package_noverify_regex','values','Regular expression to match verification failure output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_regex','package_name_regex','body_constraints','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_regex','package_name_regex','values','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_convention','package_name_convention','body_constraints','This is how the package manager expects the package to be referred to, e.g. $(name).$(arch)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_convention','package_name_convention','values','This is how the package manager expects the package to be referred to, e.g. $(name).$(arch)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_version_regex','package_list_version_regex','body_constraints','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_version_regex','package_list_version_regex','values','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_update_ifelapsed','package_list_update_ifelapsed','body_constraints','The ifelapsed locking time in between updates of the package list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_update_ifelapsed','package_list_update_ifelapsed','values','The ifelapsed locking time in between updates of the package list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_update_command','package_list_update_command','body_constraints','Command to update the list of available packages (if any)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_update_command','package_list_update_command','values','Command to update the list of available packages (if any)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_name_regex','package_list_name_regex','body_constraints','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_command','package_list_command','body_constraints','Command to obtain a list of available packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_command','package_list_command','values','Command to obtain a list of available packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_arch_regex','package_list_arch_regex','body_constraints','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_arch_regex','package_list_arch_regex','values','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_installed_regex','package_installed_regex','body_constraints','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_installed_regex','package_installed_regex','values','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_file_repositories','package_file_repositories','body_constraints','A list of machine-local directories to search for packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_file_repositories','package_file_repositories','values','A list of machine-local directories to search for packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_delete_convention','package_delete_convention','body_constraints','This is how the package manager expects the package to be referred to in the deletion part of a package update, e.g. $(name)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_delete_command','package_delete_command','body_constraints','Command to remove a package from the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_changes','package_changes','body_constraints','Menu option - whether to group packages into a single aggregate command')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_changes','package_changes','values','Menu option - whether to group packages into a single aggregate command')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_arch_regex','package_arch_regex','body_constraints','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_add_command','package_add_command','body_constraints','Command to install a package to the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_method','package_method','body_constraints','Criteria for installation and verification')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_architectures','package_architectures','body_constraints','Select the architecture for package selection')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('packages','packages','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promiser_type','promiser_type','body_constraints','Output level to observe for the named promise or bundle (meta-promise)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('output_level','output_level','body_constraints','Output level to observe for the named promise or bundle (meta-promise)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('outputs','outputs','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('usebundle','usebundle','body_constraints','Specify the name of a bundle to run as a parameterized method')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('methods','methods','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ipv6_address','ipv6_address','body_constraints','IPv6 address for the interface')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ipv4_netmask','ipv4_netmask','body_constraints','Netmask for the interface')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ipv4_address','ipv4_address','body_constraints','IPv4 address for the interface')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('tcp_ip','tcp_ip','body_constraints','Interface tcp/ip properties')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('interfaces','interfaces','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('replace_value','replace_value','body_constraints','Value used to replace regular expression matches in search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('occurrences','occurrences','body_constraints','Menu option to replace all occurrences or just first (NB the latter is non-convergent)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('replace_with','replace_with','body_constraints','Search-replace pattern')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('replace_patterns','replace_patterns','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_separator','value_separator','body_constraints','Character separator for subfields inside the selected field')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_field','select_field','body_constraints','Integer index of the field required 1..n')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_field','select_field','values','Integer index of the field required 1..n')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_value','field_value','body_constraints','Set field value to a fixed value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_value','field_value','values','Set field value to a fixed value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_separator','field_separator','body_constraints','The regular expression used to separate fields in a line')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_operation','field_operation','body_constraints','Menu option policy for editing subfields')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_operation','field_operation','values','Menu option policy for editing subfields')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('extend_fields','extend_fields','body_constraints','true/false add new fields at end of line if necessary to complete edit')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('extend_fields','extend_fields','values','true/false add new fields at end of line if necessary to complete edit')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('allow_blank_fields','allow_blank_fields','body_constraints','true/false allow blank fields in a line (do not purge)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_field','edit_field','body_constraints','Edit line-based file as matrix of fields')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('field_edits','field_edits','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('whitespace_policy','whitespace_policy','body_constraints','Criteria for matching and recognizing existing lines')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_line_matching','select_line_matching','body_constraints','Regular expression for matching file line location')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('first_last','first_last','body_constraints','Menu option, choose first or last occurrence of match in file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('before_after','before_after','body_constraints','Menu option, point cursor before of after matched line')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('location','location','body_constraints','Specify where in a file an insertion will be made')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_not_contains_from_list','insert_if_not_contains_from_list','body_constraints','Insert line if a regex in the list DOES NOT match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_not_contains_from_list','insert_if_not_contains_from_list','values','Insert line if a regex in the list DOES NOT match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_contains_from_list','insert_if_contains_from_list','body_constraints','Insert line if a regex in the list match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_contains_from_list','insert_if_contains_from_list','values','Insert line if a regex in the list match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_not_match_from_list','insert_if_not_match_from_list','body_constraints','Insert line if it DOES NOT fully match a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_not_match_from_list','insert_if_not_match_from_list','values','Insert line if it DOES NOT fully match a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_match_from_list','insert_if_match_from_list','body_constraints','Insert line if it fully matches a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_match_from_list','insert_if_match_from_list','values','Insert line if it fully matches a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_not_startwith_from_list','insert_if_not_startwith_from_list','body_constraints','Insert line if it DOES NOT start with a string in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_not_startwith_from_list','insert_if_not_startwith_from_list','values','Insert line if it DOES NOT start with a string in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_if_startwith_from_list','insert_if_startwith_from_list','body_constraints','Insert line if it starts with a string in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_select','insert_select','body_constraints','Insert only if lines pass filter criteria')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_type','insert_type','body_constraints','Type of object the promiser string refers to')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('expand_scalars','expand_scalars','body_constraints','Expand any unexpanded variables')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('insert_lines','insert_lines','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('not_matching','not_matching','body_constraints','true/false negate match criterion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_not_contains_from_list','delete_if_not_contains_from_list','body_constraints','Delete line if a regex in the list DOES NOT match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_not_contains_from_list','delete_if_not_contains_from_list','values','Delete line if a regex in the list DOES NOT match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_contains_from_list','delete_if_contains_from_list','body_constraints','Delete line if a regex in the list match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_contains_from_list','delete_if_contains_from_list','values','Delete line if a regex in the list match a line fragment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_not_match_from_list','delete_if_not_match_from_list','body_constraints','Delete line if it DOES NOT fully match a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_not_match_from_list','delete_if_not_match_from_list','values','Delete line if it DOES NOT fully match a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_match_from_list','delete_if_match_from_list','body_constraints','Delete line if it fully matches a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_match_from_list','delete_if_match_from_list','values','Delete line if it fully matches a regex in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_not_startwith_from_list','delete_if_not_startwith_from_list','body_constraints','Delete line if it DOES NOT start with a string in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_not_startwith_from_list','delete_if_not_startwith_from_list','values','Delete line if it DOES NOT start with a string in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_if_startwith_from_list','delete_if_startwith_from_list','body_constraints','Delete line if it starts with a string in the list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete_select','delete_select','body_constraints','Delete only if lines pass filter criteria')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('delete_lines','delete_lines','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_end','select_end','body_constraints','Regular expression matches end of edit region from start')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_end','select_end','values','Regular expression matches end of edit region from start')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_start','select_start','body_constraints','Regular expression matching start of edit region')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_start','select_start','values','Regular expression matching start of edit region')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_start_delimiter','include_start_delimiter','body_constraints','Whether to include the section delimiter')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_region','select_region','body_constraints','Limit edits to a demarked region of the file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('transformer','transformer','body_constraints','Command (with full path) used to transform current file (no shell wrapper used)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('touch','touch','body_constraints','true/false whether to touch time stamps on file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repository','repository','body_constraints','Name of a repository for versioning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rotate','rotate','body_constraints','Maximum number of file rotations to keep')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rotate','rotate','values','Maximum number of file rotations to keep')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('newname','newname','body_constraints','The desired name for the current file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('newname','newname','values','The desired name for the current file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_suffix','disable_suffix','body_constraints','The suffix to add to files when disabling (.cfdisabled)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_suffix','disable_suffix','values','The suffix to add to files when disabling (.cfdisabled)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_mode','disable_mode','body_constraints','The permissions to set when a file is disabled')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_mode','disable_mode','values','The permissions to set when a file is disabled')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable','disable','body_constraints','true/false automatically rename and remove permissions')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rename','rename','body_constraints','Criteria for renaming files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rxdirs','rxdirs','body_constraints','true/false add execute flag for directories if read flag is set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rxdirs','rxdirs','values','true/false add execute flag for directories if read flag is set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('owners','owners','body_constraints','List of acceptable owners or user ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('owners','owners','values','List of acceptable owners or user ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mode','mode','body_constraints','File permissions (like posix chmod)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mode','mode','values','File permissions (like posix chmod)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('groups','groups','body_constraints','List of acceptable groups of group ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('bsdflags','bsdflags','body_constraints','List of menu options for bsd file system flags to set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('perms','perms','body_constraints','Criteria for setting permissions on a file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pathtype','pathtype','body_constraints','Menu option for interpreting promiser file object')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('move_obstructions','move_obstructions','body_constraints','true/false whether to move obstructions to file-object creation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_no_source','when_no_source','body_constraints','Behaviour when the source file to link to does not exist')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_linking_children','when_linking_children','body_constraints','Policy for overriding existing files when linking directories of children')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_linking_children','when_linking_children','values','Policy for overriding existing files when linking directories of children')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('source','source','body_constraints','The source file to which the link should point')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('source','source','values','The source file to which the link should point')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','body_constraints','The type of link used to alias the file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','values','The type of link used to alias the file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_children','link_children','body_constraints','true/false whether to link all directory\'s children to source originals')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_children','link_children','values','true/false whether to link all directory\'s children to source originals')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_patterns','copy_patterns','body_constraints','A set of patterns that should be copied ansd synchronized instead of linked')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_from','link_from','body_constraints','Criteria for linking file from a source')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_result','file_result','body_constraints','Logical expression combining classes defined by file search criteria')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_result','file_result','values','Logical expression combining classes defined by file search criteria')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('issymlinkto','issymlinkto','body_constraints','List of regular expressions to match file objects')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_types','file_types','body_constraints','List of acceptable file types from menu choices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_program','exec_program','body_constraints','Execute this command on each file and match if the exit status is zero')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_regex','exec_regex','body_constraints','Matches file if this regular expression matches any full line returned by the command')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('atime','atime','body_constraints','Range of access times (atime) for acceptable files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mtime','mtime','body_constraints','Range of modification times (mtime) for acceptable files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ctime','ctime','body_constraints','Range of change times (ctime) for acceptable files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_bsdflags','search_bsdflags','body_constraints','String of flags for bsd file system flags expected set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_bsdflags','search_bsdflags','values','String of flags for bsd file system flags expected set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_groups','search_groups','body_constraints','List of acceptable group names or ids for the file, or regexes to match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_groups','search_groups','values','List of acceptable group names or ids for the file, or regexes to match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_owners','search_owners','body_constraints','List of acceptable user names or ids for the file, or regexes to match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_size','search_size','body_constraints','Integer range of file sizes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_size','search_size','values','Integer range of file sizes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_mode','search_mode','body_constraints','A list of mode masks for acceptable file permissions')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('path_name','path_name','body_constraints','List of pathnames to match acceptable target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('path_name','path_name','values','List of pathnames to match acceptable target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('leaf_name','leaf_name','body_constraints','List of regexes that match an acceptable name')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_select','file_select','body_constraints','Choose which files select in a search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('recognize_join','recognize_join','body_constraints','Join together lines that end with a backslash, up to 4kB limit')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('recognize_join','recognize_join','values','Join together lines that end with a backslash, up to 4kB limit')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('max_file_size','max_file_size','body_constraints','Do not edit files bigger than this number of bytes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('max_file_size','max_file_size','values','Do not edit files bigger than this number of bytes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('empty_file_before_editing','empty_file_before_editing','body_constraints','Baseline memory model of file to zero/empty before commencing promised edits')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_backup','edit_backup','body_constraints','Menu option for backup policy on edit changes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_defaults','edit_defaults','body_constraints','Default promise details for file edits')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_xml','edit_xml','body_constraints','XML editing model for file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_line','edit_line','body_constraints','Line editing model for file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('xdev','xdev','body_constraints','true/false exclude directories that are on different devices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('xdev','xdev','values','true/false exclude directories that are on different devices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('traverse_links','traverse_links','body_constraints','true/false traverse symbolic links to directories (false)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('traverse_links','traverse_links','values','true/false traverse symbolic links to directories (false)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rmdeadlinks','rmdeadlinks','body_constraints','true/false remove links that point to nowhere')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rmdeadlinks','rmdeadlinks','values','true/false remove links that point to nowhere')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_dirs','include_dirs','body_constraints','List of regexes of directory names to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_dirs','include_dirs','values','List of regexes of directory names to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_basedir','include_basedir','body_constraints','true/false include the start/root dir of the search results')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_basedir','include_basedir','values','true/false include the start/root dir of the search results')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exclude_dirs','exclude_dirs','body_constraints','List of regexes of directory names NOT to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exclude_dirs','exclude_dirs','values','List of regexes of directory names NOT to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depth','depth','body_constraints','Maximum depth level for search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depth_search','depth_search','body_constraints','Criteria for file depth searches')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rmdirs','rmdirs','body_constraints','true/false whether to delete empty directories during recursive deletion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('dirlinks','dirlinks','body_constraints','Menu option policy for dealing with symbolic links to directories during deletion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete','delete','body_constraints','Criteria for deleting files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('create','create','body_constraints','true/false whether to create non-existing file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('verify','verify','body_constraints','true/false verify transferred file by hashing after copy (resource penalty)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('verify','verify','values','true/false verify transferred file by hashing after copy (resource penalty)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('type_check','type_check','body_constraints','true/false compare file types before copying and require match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('type_check','type_check','values','true/false compare file types before copying and require match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('trustkey','trustkey','body_constraints','true/false trust public keys from remote server if previously unknown')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('trustkey','trustkey','values','true/false trust public keys from remote server if previously unknown')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stealth','stealth','body_constraints','true/false whether to preserve time stamps on copied file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stealth','stealth','values','true/false whether to preserve time stamps on copied file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('purge','purge','body_constraints','true/false purge files on client that do not match files on server when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('purge','purge','values','true/false purge files on client that do not match files on server when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('preserve','preserve','body_constraints','true/false whether to preserve file permissions on copied file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('portnumber','portnumber','body_constraints','Port number to connect to on server host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('portnumber','portnumber','values','Port number to connect to on server host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_ipv4','force_ipv4','body_constraints','true/false force use of ipv4 on ipv6 enabled network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_ipv4','force_ipv4','values','true/false force use of ipv4 on ipv6 enabled network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_update','force_update','body_constraints','true/false force copy update always')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_update','force_update','values','true/false force copy update always')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','body_constraints','Menu option for type of links to use when copying')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','values','Menu option for type of links to use when copying')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('linkcopy_patterns','linkcopy_patterns','body_constraints','List of patterns matching files that should be replaced with symbolic links')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('findertype','findertype','body_constraints','Menu option for default finder type on MacOSX')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_size','copy_size','body_constraints','Integer range of file sizes that may be copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_size','copy_size','values','Integer range of file sizes that may be copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copylink_patterns','copylink_patterns','body_constraints','List of patterns matching files that should be linked instead of copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copylink_patterns','copylink_patterns','values','List of patterns matching files that should be linked instead of copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('check_root','check_root','body_constraints','true/false check permissions on the root directory when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('check_root','check_root','values','true/false check permissions on the root directory when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('encrypt','encrypt','body_constraints','true/false use encrypted data stream to connect to remote host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_backup','copy_backup','body_constraints','Menu option policy for file backup/version control')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('compare','compare','body_constraints','Menu option policy for comparing source and image file attributes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('compare','compare','values','Menu option policy for comparing source and image file attributes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('collapse_destination_dir','collapse_destination_dir','body_constraints','true/false Place files in subdirectories into the root destination directory during copy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('servers','servers','body_constraints','List of servers in order of preference from which to copy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('source','source','body_constraints','Reference source file from which to copy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_from','copy_from','body_constraints','Criteria for copying file from a source')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_diffs','report_diffs','body_constraints','Generate reports summarizing the major differences between individual text files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_diffs','report_diffs','values','Generate reports summarizing the major differences between individual text files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('update_hashes','update_hashes','body_constraints','Update hash values immediately after change warning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_changes','report_changes','body_constraints','Specify criteria for change warnings')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hash','hash','body_constraints','Hash files for change detection')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('changes','changes','body_constraints','Criteria for change management')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('specify_inherit_aces','specify_inherit_aces','body_constraints','Native settings for access control entry')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl_type','acl_type','body_constraints','Access control list type for the affected file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl_method','acl_method','body_constraints','Editing method for access control list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl_directory_inherit','acl_directory_inherit','body_constraints','Access control list type for the affected file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('aces','aces','body_constraints','Native settings for access control entry')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl','acl','body_constraints','Criteria for access control lists on file')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('environment_type','environment_type','body_constraints','Virtual environment type')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('environment_state','environment_state','body_constraints','The desired dynamical state of the specified environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_spec_file','env_spec_file','body_constraints','The path to a file containing a technology specific set of promises for the virtual instance')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_baseline','env_baseline','body_constraints','The path to an image with which to baseline the virtual environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_baseline','env_baseline','values','The path to an image with which to baseline the virtual environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_disk','env_disk','body_constraints','Amount of secondary storage (DISK) in the virtual environment (MB)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_disk','env_disk','values','Amount of secondary storage (DISK) in the virtual environment (MB)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_memory','env_memory','body_constraints','Amount of primary storage (RAM) in the virtual environment (KB)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_memory','env_memory','values','Amount of primary storage (RAM) in the virtual environment (KB)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_cpus','env_cpus','body_constraints','Number of virtual CPUs in the environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('environment_resources','environment_resources','body_constraints','Virtual environment resource description')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_network','env_network','body_constraints','The hostname of the virtual network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_network','env_network','values','The hostname of the virtual network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_name','env_name','body_constraints','The hostname of the virtual environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_name','env_name','values','The hostname of the virtual environment')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('env_addresses','env_addresses','body_constraints','The IP addresses of the environment\'s network interfaces')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('environment_interface','environment_interface','body_constraints','Virtual environment outward identity and location')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('environment_host','environment_host','body_constraints','The name of the virtual environment host (this must be promised uniquely)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('environments','environments','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('registry_exclude','registry_exclude','body_constraints','A list of regular expressions to ignore in key/value verification')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('database_rows','database_rows','body_constraints','An ordered list of row values to be promised by SQL databases')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('database_columns','database_columns','body_constraints','A list of column definitions to be promised by SQL databases')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('database_operation','database_operation','body_constraints','The nature of the promise - to be or not to be')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('database_type','database_type','body_constraints','The type of database that is to be manipulated')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_connection_db','db_server_connection_db','body_constraints','The name of an existing database to connect to in order to create/manage other databases')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_type','db_server_type','body_constraints','The dialect of the database server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_type','db_server_type','values','The dialect of the database server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_host','db_server_host','body_constraints','Hostname or address for connection to database, blank means localhost')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_host','db_server_host','values','Hostname or address for connection to database, blank means localhost')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_password','db_server_password','body_constraints','Clear text password for database connection')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_password','db_server_password','values','Clear text password for database connection')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('db_server_owner','db_server_owner','body_constraints','User name for database connection')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('database_server','database_server','body_constraints','Credentials for connecting to a local/remote database server')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('databases','databases','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('module','module','body_constraints','true/false whether to expect the cfengine module protocol')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('no_output','no_output','body_constraints','true/false discard all output from the command')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('no_output','no_output','values','true/false discard all output from the command')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('preview','preview','body_constraints','true/false preview command when running in dry-run mode (with -n)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('chroot','chroot','body_constraints','Directory of root sandbox for process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('chdir','chdir','body_constraints','Directory for setting current/base directory for the process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_timeout','exec_timeout','body_constraints','Timeout in seconds for command completion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_timeout','exec_timeout','values','Timeout in seconds for command completion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_group','exec_group','body_constraints','The group name or id under which to run the process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_group','exec_group','values','The group name or id under which to run the process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_owner','exec_owner','body_constraints','The user name or id under which to run the process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('umask','umask','body_constraints','The umask value for the child process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('umask','umask','values','The umask value for the child process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('useshell','useshell','body_constraints','true/false embed the command in a shell environment (true)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('contain','contain','body_constraints','Containment options for the execution process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('args','args','body_constraints','Alternative string of arguments for the command (concatenated with promiser string)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('commands','commands','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('comment','comment','body_constraints','A comment about this promise\'s real intention that follows through the program')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depends_on','depends_on','body_constraints','A list of promise handles that this promise builds on or depends on somehow (for knowledge management)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('handle','handle','body_constraints','A unique id-tag string for referring to this as a promisee elsewhere')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ifvarclass','ifvarclass','body_constraints','Extended classes ANDed with context')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('timer_policy','timer_policy','body_constraints','Whether a persistent class restarts its counter when rediscovered')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('timer_policy','timer_policy','values','Whether a persistent class restarts its counter when rediscovered')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('persist_time','persist_time','body_constraints','A number of minutes the specified classes should remain active')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cancel_notkept','cancel_notkept','body_constraints','A list of classes to be cancelled if the promise is not kept for any reason')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cancel_repaired','cancel_repaired','body_constraints','A list of classes to be cancelled if the promise is repaired')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cancel_kept','cancel_kept','body_constraints','A list of classes to be cancelled if the promise is kept')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_kept','promise_kept','body_constraints','A list of classes to be defined globally')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_timeout','repair_timeout','body_constraints','A list of classes to be defined globally')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_denied','repair_denied','body_constraints','A list of classes to be defined globally')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_failed','repair_failed','body_constraints','A list of classes to be defined globally')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_repaired','promise_repaired','body_constraints','A list of classes to be defined globally')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classes','classes','body_constraints','Signalling behaviour')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('measurement_class','measurement_class','body_constraints','If set performance will be measured and recorded under this identifier')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_level','report_level','body_constraints','The reporting level for standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_level','report_level','values','The reporting level for standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('background','background','body_constraints','true/false switch for parallelizing the promise repair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('background','background','values','true/false switch for parallelizing the promise repair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('audit','audit','body_constraints','true/false switch for detailed audit records of this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('audit','audit','values','true/false switch for detailed audit records of this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_notkept','value_notkept','body_constraints','A real number value (possibly negative) attributed to not keeping this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_notkept','value_notkept','values','A real number value (possibly negative) attributed to not keeping this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_repaired','value_repaired','body_constraints','A real number value attributed to reparing this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_repaired','value_repaired','values','A real number value attributed to reparing this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_kept','value_kept','body_constraints','A real number value attributed to keeping this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_failed','log_failed','body_constraints','This should be filename of a file to which log_string will be saved, if undefined it goes to the system logger')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_repaired','log_repaired','body_constraints','This should be filename of a file to which log_string will be saved, if undefined it goes to the system logger')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_priority','log_priority','body_constraints','The priority level of the log message, as interpreted by a syslog server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_kept','log_kept','body_constraints','This should be filename of a file to which log_string will be saved, if undefined it goes to the system logger')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_level','log_level','body_constraints','The reporting level sent to syslog')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_level','log_level','values','The reporting level sent to syslog')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_string','log_string','body_constraints','A message to be written to the log when a promise verification leads to a repair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_string','log_string','values','A message to be written to the log when a promise verification leads to a repair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('expireafter','expireafter','body_constraints','Number of minutes before a repair action is interrupted and retried')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('expireafter','expireafter','values','Number of minutes before a repair action is interrupted and retried')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ifelapsed','ifelapsed','body_constraints','Number of minutes before next allowed assessment of promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('action_policy','action_policy','body_constraints','Whether to repair or report about non-kept promises')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('action','action','body_constraints','Output behaviour')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('*','_','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('showstate','showstate','body_constraints','List of services about which status reports should be reported to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_to_file','report_to_file','body_constraints','The path and filename to which output should be appended')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('number_of_lines','number_of_lines','body_constraints','Integer maximum number of lines to print from selected file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_to_print','file_to_print','body_constraints','Path name to the file that is to be sent to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('printfile','printfile','body_constraints','Quote part of a file to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('lastseen','lastseen','body_constraints','Integer time threshold in hours since current peers were last seen, report absence')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('intermittency','intermittency','body_constraints','Real number threshold [0,1] of intermittency about current peers, report above')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('friend_pattern','friend_pattern','body_constraints','Regular expression to keep selected hosts from the friends report list')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('reports','reports','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('not','not','body_constraints','Evaluate the negation of string expression in normal form')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('expression','expression','body_constraints','Evaluate string expression of classes in normal form')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('dist','dist','body_constraints','Generate a probabilistic class distribution (strategy in cfengine 2)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('xor','xor','body_constraints','Combine class sources with XOR')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('and','and','body_constraints','Combine class sources with AND')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('or','or','body_constraints','Combine class sources with inclusive OR')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('classes','classes','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('policy','policy','body_constraints','The policy for (dis)allowing (re)definition of variables')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rlist','rlist','body_constraints','A list of real numbers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ilist','ilist','body_constraints','A list of integers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('slist','slist','body_constraints','A list of scalar strings')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('real','real','body_constraints','A scalar real number')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('int','int','body_constraints','A scalar integer')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('string','string','body_constraints','A scalar string')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('vars','vars','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('sys','sys','bundles','cfengine\'s internal bundle of system specific values')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('an arbitrary string','an_arbitrary_string','values','Should match the generic pattern , i.e. unspecified characters')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a syslog level','a_syslog_level','values','Should match the generic pattern LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7, i.e. a syslog constant')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a file path','a_file_path','values','Should match the generic pattern [cC]:\\.*|/.*, i.e. a system file path suitable for the target system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a user/group id','a_user_group_id','values','Should match the generic pattern [a-zA-Z0-9_$.-]+, i.e. an alphanumeric string with option underscores and hyphens')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a cfengine identifier','a_cfengine_identifier','values','Should match the generic pattern [a-zA-Z0-9_$.]+, i.e. an alphanumeric string with option underscores')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a cfengine class expression','a_cfengine_class_expression','values','Should match the generic pattern [a-zA-Z0-9_!&|.()]+, i.e. an alphanumeric string with option underscores and logical operators')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('posix file mode or permission','posix_file_mode_or_permission','values','Should match the generic pattern [0-7augorwxst,+-]+, i.e. something that you would give as an argument to chmod')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a single character','a_single_character','values','Should match the generic pattern ^.$, i.e. one symbol')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('real number','real_number','values','Should match the generic pattern -9.99999E100,9.99999E100, i.e. a number between the minus and positive maximum values')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('integer','integer','values','Should match the generic pattern -99999999999,9999999999, i.e. a number between the minus and positive maximum values')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a positive integer','a_positive_integer','values','Should match the generic pattern 0,99999999999, i.e. a number between zero and the maximum value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a time range','a_time_range','values','Should match the generic pattern 0,2147483648, i.e. a value from zero to a maximum system time -- but you should use time functions to convert this')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link type','link_type','values','Should match the generic pattern symlink,hardlink,relative,absolute,none, i.e. a support link type')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('boolean','boolean','values','Should match the generic pattern true,false,yes,no,on,off, i.e. a positive or a negative')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system signals','system_signals','values','Should match the generic pattern hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv, i.e. a unix signal name')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('values','values','system_policy','Formal rvalues in constraint assignments and their legal ranges')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('functions','functions','system_policy','In-built functions that may be used to set variables or classes')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('comments','comments','system_policy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('body constraints','body_constraints','system_policy','The attributes that cfengine enables you to promise about the promiser')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise types','promise_types','system_policy','The types of promise that cfengine can keep')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promises','promises','system_policy','Complete occurrences of promiser + promisee + promise-body')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promisers','promisers','system_policy','The objects affected by a promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promisees','promisees','system_policy','Recipients of a promise, i.e. promise handles, or persons')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('contexts','contexts','system_policy','Class expressions that say where or when a promise applies')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('bodies','bodies','system_policy','A modular collection of body constraints for re-use')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('bundles','bundles','system_policy','A modular collection of promises of different types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('available patches report','available_patches_report','system_reports','Patches already installed on system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('installed patches report','installed_patches_report','system_reports','Patches not yet installed, but published by vendor')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('variables report','variables_report','system_reports','Current variable values expanded on different hosts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value report','value_report','system_reports','Value estimate / ROI of cfengine configuration')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('setuid report','setuid_report','system_reports','Known setuid programs found on system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promises not kept report','promises_not_kept_report','system_reports','Promises that were recently unkept')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promises repaired report','promises_repaired_report','system_reports','Promises that were recently repaired')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise report','promise_report','system_reports','Per-promise average compliance report')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('performance report','performance_report','system_reports','Time cost of verifying system promises')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('monitor summary report','monitor_summary_report','system_reports','Pseudo-real-time measurement')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('license report','license_report','system_reports','License utilization statistics')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('lastseen report','lastseen_report','system_reports','Time and frequency of communications with peers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('installed software report','installed_software_report','system_reports','Software already installed on system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hashes report','hashes_report','system_reports','File hash values measured (change detection)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_diffs report','file_diffs_report','system_reports','Latest observed differences to system files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_changes report','file_changes_report','system_reports','Latest observed changes to system files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('compliance report','compliance_report','system_reports','Total summary of host compliance')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classes report','classes_report','system_reports','User defined classes observed on the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('micro-audit report','micro_audit_report','system_reports','Generated by cfengine self-auditing')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('is a promise made by','is_a_promise_made_by','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('makes promises','makes_promises','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promises have been made by','promises_have_been_made_by','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('makes promise of type','makes_promise_of_type','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('bundle contains promiser','bundle_contains_promiser','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('occurs in bundle','occurs_in_bundle','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('is a promise of type','is_a_promise_of_type','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('has current exemplars','has_current_exemplars','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('used in promise','used_in_promise','references')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('bundle reference','bundle_reference','references')
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_67','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_67','mon_update','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-report','promise_knowledge_cf_67','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-report','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-report','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_63','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_63','web_server','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/enterprise.cf -g','promise_knowledge_cf_63','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/enterprise.cf -g','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/enterprise.cf -g','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_56','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_56','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-promises -r','promise_knowledge_cf_56','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-promises -r','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-promises -r','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_43','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_43','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/mean_field.nov','promise_knowledge_cf_43','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/mean_field.nov','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/mean_field.nov','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_35','mon_update','has direct influence on','is directly influenced by','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_35','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_35','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/fluctuations.nov','mon_update','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/fluctuations.nov','promise_knowledge_cf_35','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/fluctuations.nov','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/reports/192.168.1.100/fluctuations.nov','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_28','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_28','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs','promise_knowledge_cf_28','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_15','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_15','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('all_hosts','promise_knowledge_cf_15','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('all_hosts','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('all_hosts','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_12','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_12','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('doc_root','promise_knowledge_cf_12','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('doc_root','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('doc_root','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_137','field_edits','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_137','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_130','field_edits','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_130','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','promise_webserver_cf_137','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','promise_webserver_cf_130','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','field_edits','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_117','insert_lines','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_117','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_CONF_INCLUDE_FILES=\"/home/mark/cfengine-inputs/httpd.conf\"','promise_webserver_cf_117','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_CONF_INCLUDE_FILES=\"/home/mark/cfengine-inputs/httpd.conf\"','insert_lines','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_CONF_INCLUDE_FILES=\"/home/mark/cfengine-inputs/httpd.conf\"','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_108','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_108','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('del_modules','promise_webserver_cf_108','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('del_modules','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('del_modules','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_104','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_104','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('add_modules','promise_webserver_cf_104','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('add_modules','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('add_modules','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_85','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_85','!software_ok.on','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('The web server software could not be installed','promise_webserver_cf_85','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('The web server software could not be installed','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('The web server software could not be installed','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_76','web_ok','has direct influence on','is directly influenced by','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_76','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_76','software_ok','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','web_ok','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','promise_webserver_cf_76','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('software_ok','web_ok','is activated by class context','activates promises','contexts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','software_ok','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','promise_webserver_cf_64','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','software_ok','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','promise_webserver_cf_64','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','software_ok','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','promise_webserver_cf_64','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_64','software_ok','has direct influence on','is directly influenced by','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_64','packages','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_64','on','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('on','software_ok','is activated by class context','activates promises','contexts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('on','int','returns data-type','is returned by function','contexts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_53','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_53','start_apache','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/apache2 restart','promise_webserver_cf_53','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/apache2 restart','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/apache2 restart','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_44','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_44','off','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_38','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_38','web_ok.on','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','software_ok','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','promise_webserver_cf_64','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','promise_webserver_cf_44','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','promise_webserver_cf_38','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_23','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_23','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_package','promise_webserver_cf_23','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_package','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_package','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_17','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_17','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('site_http_conf','promise_webserver_cf_17','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('site_http_conf','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('site_http_conf','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_11','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_11','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('document_root','promise_webserver_cf_11','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('document_root','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('document_root','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_31','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_31','!usb_backup_device_loaded','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/backup','promise_laptop_cf_31','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/backup','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/backup','laptop','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_22','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_22','usb_backup_device_loaded','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/media/disk-1/CfengineBackupDevice','promise_laptop_cf_22','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/media/disk-1/CfengineBackupDevice','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/media/disk-1/CfengineBackupDevice','laptop','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_6','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_6','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_point','promise_laptop_cf_6','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_point','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_point','laptop','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_206','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_206','exec_fix','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Added a 5 minute schedule to crontabs','promise_library_cf_206','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Added a 5 minute schedule to crontabs','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Added a 5 minute schedule to crontabs','upgrade_cfexecd','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_200','replace_patterns','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_200','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','promise_library_cf_200','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','replace_patterns','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','upgrade_cfexecd','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','cf-execd','is a variant of','is a variant of','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','cfengine 2','is a component of','has component','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_196','insert_lines','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_196','exec_fix','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','promise_library_cf_196','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','insert_lines','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','upgrade_cfexecd','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_171','delete_lines','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_171','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(regex)','promise_library_cf_171','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(regex)','delete_lines','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(regex)','DeleteLinesMatching','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_119','field_edits','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_119','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','promise_library_cf_119','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','field_edits','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','resolver','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_111','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_111','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search','promise_library_cf_111','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search','resolver','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_422','roles','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_422','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('roles','access_rules','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('roles','agents','are identified in','play different','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('roles','promises','are recurring patterns of','define','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('.*','promise_site_cf_422','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('.*','roles','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('.*','access_rules','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_418','access','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_418','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/.cfagent/bin/cf-agent','promise_site_cf_418','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/.cfagent/bin/cf-agent','access','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/.cfagent/bin/cf-agent','access_rules','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_414','access','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_414','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/LapTop','promise_site_cf_414','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/LapTop','access','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/LapTop','access_rules','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_401','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_401','anomaly_hosts.icmp_in_high_anomaly.!entropy_icmp_in_high','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_icmp_in) av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','promise_site_cf_401','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_icmp_in) av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_icmp_in) av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_394','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_394','entropy_dns_in_low.anomaly_hosts.udp_in_high_dev2','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_udp_in) av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','promise_site_cf_394','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_udp_in) av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_udp_in) av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_387','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_387','entropy_dns_in_low.anomaly_hosts.dns_in_high_anomaly','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_dns_in) av $(average_dns_in) pm $(mon.stddev_dns_in)','promise_site_cf_387','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_dns_in) av $(average_dns_in) pm $(mon.stddev_dns_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_dns_in) av $(average_dns_in) pm $(mon.stddev_dns_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_380','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_380','entropy_tcpsyn_in_low.anomaly_hosts.tcpsyn_in_high_dev2','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous number of new TCP connections on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_tcpsyn_in) av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','promise_site_cf_380','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous number of new TCP connections on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_tcpsyn_in) av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous number of new TCP connections on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_tcpsyn_in) av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_373','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_373','entropy_www_in_low.anomaly_hosts.www_in_high_anomaly','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time)\n     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','promise_site_cf_373','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time)\n     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time)\n     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_366','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_366','entropy_www_in_high&anomaly_hosts.www_in_high_anomaly','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_www_in) av $(mon.average_www_in) pm $(mon.stddev_www_in)','promise_site_cf_366','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_www_in) av $(mon.average_www_in) pm $(mon.stddev_www_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time) \n    - measured value $(mon.value_www_in) av $(mon.average_www_in) pm $(mon.stddev_www_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_359','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_359','rootprocs_high_dev2','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('RootProc anomaly high 2 dev on $(mon.host) at $(mon.env_time) \n    measured value $(mon.value_rootprocs) av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','promise_site_cf_359','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('RootProc anomaly high 2 dev on $(mon.host) at $(mon.env_time) \n    measured value $(mon.value_rootprocs) av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('RootProc anomaly high 2 dev on $(mon.host) at $(mon.env_time) \n    measured value $(mon.value_rootprocs) av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/shadow','promise_site_cf_349','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/shadow','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/shadow','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_349','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_349','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/passwd','promise_site_cf_349','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/passwd','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/passwd','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/shadow','promise_site_cf_343','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/shadow','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/shadow','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_343','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_343','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','promise_site_cf_331','makes promises','is a promise made by','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','vars','makes promise of type','promises have been made by','promise_types','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','anomalies','occurs in bundle','bundle contains promiser','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','setup_knowledge','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','web_server','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','laptop','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','anomalies','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','garbage_collection','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','main','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','cfengine','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','update','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/passwd','promise_site_cf_343','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/passwd','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/passwd','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_331','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_331','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_330','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_330','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sysdir','promise_site_cf_330','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sysdir','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sysdir','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_321','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_321','linux','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('I am alive','promise_site_cf_321','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('I am alive','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('I am alive','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_disk_watch','measurements','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_disk_watch','linux','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/bin/df','free_disk_watch','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/bin/df','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/bin/df','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache_watch_memory','measurements','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache_watch_memory','linux','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache_watch_cpu','measurements','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache_watch_cpu','linux','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_procs','apache_watch_memory','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_procs','apache_watch_cpu','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_procs','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_procs','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_memory_watch','measurements','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_memory_watch','linux','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/proc/meminfo','free_memory_watch','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/proc/meminfo','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/proc/meminfo','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('monitor_self_watch','measurements','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('monitor_self_watch','atlas','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_rootprocs','monitor_self_watch','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_rootprocs','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_rootprocs','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_210','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_210','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf\\..*','promise_site_cf_210','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf\\..*','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf\\..*','garbage_collection','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_204','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_204','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/outputs','promise_site_cf_204','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/outputs','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/outputs','garbage_collection','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_177','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_177','Hr12','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/updatedb --prunepaths=/windows','promise_site_cf_177','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/updatedb --prunepaths=/windows','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/updatedb --prunepaths=/windows','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_171','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_171','start_mysqld','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/mysql start','promise_site_cf_171','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/mysql start','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/mysql start','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_163','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_163','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_155','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_155','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mysql','promise_site_cf_155','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mysql','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mysql','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mysql','database','is managed by','may be used to manage','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_152','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_152','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_138','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_138','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/testfile2','promise_site_cf_138','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/testfile2','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/testfile2','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_131','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_131','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf','promise_site_cf_131','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_123','up_and_running','has direct influence on','is directly influenced by','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_123','reports','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_123','!up_and_running','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','up_and_running','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','promise_site_cf_123','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('!up_and_running','up_and_running','is activated by class context','activates promises','contexts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_114','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_114','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('component','promise_site_cf_114','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('component','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('component','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_101','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_101','exec_fix','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cron','promise_site_cf_101','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cron','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cron','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_93','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_93','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/dev/sr0','promise_site_cf_93','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/dev/sr0','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/dev/sr0','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_81','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_81','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/.*','promise_site_cf_81','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/.*','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/.*','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_74','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_74','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin','promise_site_cf_74','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_64','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_64','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/sbin/ntpdate no.pool.ntp.org','promise_site_cf_64','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/sbin/ntpdate no.pool.ntp.org','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/sbin/ntpdate no.pool.ntp.org','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('run_agent','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('run_agent','integrate_cfengine2','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cfagent','run_agent','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cfagent','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cfagent','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_42','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_42','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf2bits','promise_site_cf_42','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf2bits','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf2bits','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_19','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_19','!SuSE','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_15','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_15','SuSE','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl','promise_site_cf_15','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl','g','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl','files','is a possible body constraint for','can have body constraints','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_13','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_13','SuSE','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','promise_site_cf_19','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','promise_site_cf_13','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','g','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_45','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_45','reload.start_exec','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-execd','promise_update_cf_45','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-execd','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-execd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_41','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_41','reload.start_monitor','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','promise_site_cf_163','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','promise_update_cf_41','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_37','commands','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_37','reload.start_server','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','promise_site_cf_163','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','promise_update_cf_37','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_31','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_31','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','promise_update_cf_31','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','cfengine 3','is a component of','has component','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_30','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_30','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','promise_site_cf_152','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','promise_update_cf_30','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','cfengine 3','is a component of','has component','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_29','processes','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_29','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','promise_site_cf_152','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','promise_update_cf_29','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','cfengine 3','is a component of','has component','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_21','reload','has direct influence on','is directly influenced by','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_21','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_21','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','reload','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','promise_update_cf_21','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_13','reload','has direct influence on','is directly influenced by','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_13','files','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_13','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','reload','has direct influence on','is directly influenced by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','promise_update_cf_13','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_7','vars','is a promise of type','has current exemplars','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_7','any','is activated by class context','activates promises','promises','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('master_location','promise_update_cf_7','makes promises','is a promise made by','promisers','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('master_location','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('master_location','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('software licenses','license report','are reported in','reports on','manager_view','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('request for change','promise proposals','might be an example of','might be a','manager_view','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('request for change','intentions','might be an example of','might be a','manager_view','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('request for change','change management','are a basis for','uses','manager_view','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('security view','software patches report','contains a current summary of','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('security view','setuid report','contains a current summary of','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','copernicus','finds documentation in','provides documentation for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','system outputs','see policy-defined events in','are an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','installed software report','see software state','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','performance report','see promise cost in','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','promises not kept report','recent compliance at','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','promises repaired report','recent compliance at','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','promise report','recent compliance at','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','lastseen report','displays communications summary in','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','system policy','displays operational goals in','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','faults','has open tickets under','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('operator view','host portal','displays operational state in','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manager view','','needs an appreciation of','is an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manager view','server lifecycle','needs an appreciation of','is an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manager view','compliance report','contains a current summary of','is an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manager view','management issues','sees the catalogue of','are an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manager view','service catalogue','is responsible for','are an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','system policy','designs','is designed by','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','copernicus','has reference support in','is a resource for','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','contexts','depends on environmental','are an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','system policy','is about plotting','is an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','service catalogue','is informed by','are an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','system trends','is informed by','are an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('architect view','BDMA','planning the system lifecycle','is an aspect of','views','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mission status','security view','may be seen from','is a viewpoint on','system_knowledge','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mission status','operator view','may be seen from','is a viewpoint on','system_knowledge','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mission status','manager view','may be seen from','is a viewpoint on','system_knowledge','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mission status','architect view','may be seen from','is a viewpoint on','system_knowledge','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('server lifecycle','BDMA','is modelled by','is a model for','system_knowledge','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service catalogue','services','is a list of','are described in','system_knowledge','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service catalogue','bundles','promises are implemented as','implement','system_knowledge','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hosts','machine resources','form the bulk of','are composed from','machine_resources','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('versions','occurrences','label variations in','vary according to','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dependencies','conditional promises','result in','require the use of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('workflows','actions','are formed by associating','combine into','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stories','workflows','can talk about','can be the subject of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stories','topics','are formed by associating','can be combined into','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('services','promises','are formed by associating','combine into','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('subjects','topics','are represented as','are knowledge-map instances of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrences','occurrence types','are classified into disjoint','are disjoint classes of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrences','(menu option)','is a body constraint of type','has possible body constraints','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrences','replace_with','is a possible sub-body constraint for','may have sub-body constraints','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrence-types','topic map sub-types','are also called','might be called','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topic-instances','occurrences','are simply called','might be called','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topic-instances','topics','are realizations of','are discussed in','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('type-instances','topics','is another term for','may be described as','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','topics','form networks of','are inter-related through','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','structural relationships','represent','can be represented as','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','causative relationships','represent','can be represented as','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','location relationships','represent','can be represented as','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','conceptual relationships','represent','can be represented as','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('actions','intentions','can be the subject of','can talk about','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('behaviours','intentions','can be the subject of','can talk about','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('facts','intentions','can be the subject of','can talk about','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topics','topic types','can be classified into disjoint','are disjoint classes of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topics','stories','can be combined into','can be broken down into','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topics','promises','can talk about','can be represented as','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('body-constraint','promise','describes the extent (or quantity) of a','is quantified by its','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('body-type','body-constraint','describes the subject (nature or quality) of a','is qualified by its','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise body','body-constraint','has attribute','is an attribute of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise body','body-type','has attribute','is an attribute of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise-type','@(all_actions)','has instances','is an instance of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise-type','promise','describes the nature (or quality) of a','is qualified by its','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promisee','promise','is the recipient of','is made to a','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promiser','promise','makes a','is made by a','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('announcements','communicated information','are a form of','includes','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('conditional promises','promise of necessary conditions','are completed by','complete','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promises','communicated information','are a form of','includes','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promises','topics','can be made about','can be the subject of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promises','intentions','reveal','are revealed in the form of','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise proposals','promises','are a basis for','may be based on','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('policy','promises','is a collection of','combine into','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('policy','(menu option)','is a body constraint of type','has possible body constraints','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('policy','vars','is a possible body constraint for','can have body constraints','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bundles','promises','are collections of','combine to form','promise_theory','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('convergence','Convergence','is related to','is related to','convergent_operators','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cascading','hierarchical structure','is related to','is related to','models_of_organization','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('object paradigm','object orientation','is related to','is related to','models_of_organization','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aspect paradigm','aspect orientation','is related to','is related to','models_of_organization','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service orientation','peer to peer network','is related to','is related to','models_of_organization','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hierarchical structure','object orientation','is related to','is related to','models_of_organization','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','crises and fire fighting','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','unusual operational behaviour','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','normal operational behaviour','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','budget','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','availability','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','reliability','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','crises and fire fighting','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','unusual operational behaviour','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','normal operational behaviour','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','budget','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','availability','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','reliability','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','crises and fire fighting','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','unusual operational behaviour','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','normal operational behaviour','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','budget','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','availability','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','reliability','improves','is improved by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('convergent operators','convergence','implement','is implemented by','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise theory','cfengine','is implemented in','is the reference implementation of','models_of_system_administration','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('audit','system reports','consists of','are an aspect of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('audit','boolean','is a body constraint of type','has possible body constraints','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('audit','action','is a possible sub-body constraint for','may have sub-body constraints','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manage','repair','consists of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manage','system outputs','consists of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('manage','monitoring','consists of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deploy','packages','makes use of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deploy','update','makes use of','can be used as part of','BDMA','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('build','promises','consists of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('build','system services','consists of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('build','system policy','consists of','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('build','PXE','bare metal installation with','can be used as part of','BDMA','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('change management','change detection scans','may be implemented by','implements','management_issues','procedures');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('incident management','Incident Management vs Maintenance','see also','see also','management_issues','manual');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('knowledge management','cf-know','may be implemented by','implements','management_issues','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('identity management','Changing a password','may be implemented by','implements','management_issues','manual');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compliance','Reporting','is documented by','documents','management_issues','manual');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('business value','self-healing','is improved by','improves','management_issues','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('business value','autonomics','is improved by','improves','management_issues','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('business value','computer immunology','is improved by','improves','management_issues','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('gentoo','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slackware','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('debian','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('redhat','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('fedora','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ubuntu','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('suse','linux','is distro of','has distro','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('darwin','macos X','see also','see also','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('distro','linux','is a packaging of','is packaged as a','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('solaris','sunos','is a current name for','is recently called','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('gnu/linux','linux','is related to','is related to','operating_systems','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(cf2_actions)','cfengine 2','is a promise type in','has promise types','cfengine_2','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(nova_actions)','cfengine nova','is a promise type in','has promise types','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(cf3_actions)','cfengine 3','is a promise type in','has promise types','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','real','is a list of','has list type','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','real number','is a list of type','is used in','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','real number','is a body constraint of type','has possible body constraints','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','vars','is a possible body constraint for','can have body constraints','cfengine_3','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real','real number','is a body constraint of type','has possible body constraints','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real','vars','is a possible body constraint for','can have body constraints','cfengine_3','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','int','is a list of','has list type','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','integer','is a list of type','is used in','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','integer','is a body constraint of type','has possible body constraints','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','vars','is a possible body constraint for','can have body constraints','cfengine_3','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('int','integer','is a body constraint of type','has possible body constraints','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('int','vars','is a possible body constraint for','can have body constraints','cfengine_3','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','string','is a list of','has list type','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','an arbitrary string','is a list of type','is used in','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','an arbitrary string','is a body constraint of type','has possible body constraints','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','vars','is a possible body constraint for','can have body constraints','cfengine_3','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('string','an arbitrary string','is a body constraint of type','has possible body constraints','cfengine_3','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('string','vars','is a possible body constraint for','can have body constraints','cfengine_3','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine 3.0.x','cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine 2.2.x','cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine nova','cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine nova','enterprise level cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine nova','cfengine 3','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine 3','cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine 2','cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfengine 1','cfengine','is a version of','has version','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-report','cfengine 3','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-know','cfengine 3','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-runagent','cfengine 3','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-agent','cfengine 3','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-promises','cfengine 3','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfd','cf-serverd','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfd','cfservd','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfd','cfengine 1','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvgraph','cf-report','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvgraph','cfengine 2','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfshow','cf-report','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfshow','cfengine 2','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfrun','cf-runagent','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfrun','cfengine 2','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvd','cf-monitord','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvd','cfengine 2','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfservd','cf-serverd','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfservd','cfengine 2','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfagent','cf-agent','is a variant of','is a variant of','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfagent','cfengine 2','is a component of','has component','cfengine_components','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('global variables','List variable substitution and expansion','explained in','comments on','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lists','List variable substitution and expansion','explained in','comments on','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('arrays','variables','are a special case of','see also','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('variables','setting variables with functions','may be implemented by','implements','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('variables','vars','are promised by','are promises about','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('variables','Special variables','is related to','is related to','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('intermittency','real','is a body constraint of type','has possible body constraints','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('intermittency','reports','is a possible body constraint for','can have body constraints','technical_concepts','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('budget','BDIM','is an aspect of','in ITIL is called','technical_concepts','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen','a positive integer','is a body constraint of type','has possible body constraints','security','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen','reports','is a possible body constraint for','can have body constraints','security','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hash','string','returns data-type','is returned by function','security','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hash','(menu option)','is a body constraint of type','has possible body constraints','security','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hash','changes','is a possible sub-body constraint for','may have sub-body constraints','security','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('access control','Role based access control','implements','may be implemented by','security','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('monitoring','environment','detects','is detected by','environment','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('monitoring','mission status','contributes to','draws on data from','environment','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('CI','configuration management','is an object for','is about the management of','management_terminology','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('contract','promise suggestions','is a bilateral bundle of','can be combined into a','management_terminology','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('SLA','Agreement','is an example of','has exemplar','management_terminology','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('CM','Configuration Management','is the ITIL name for','in ITIL is called','management_terminology','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('SAS-70','SOX','is a framework for implementing','falls within the bounds of','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('EUROSOX','change management','involves','is a principal component of','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('EUROSOX','identity management','involves','is a principal component of','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('SOX','EUROSOX','corresponds to','corresponds to','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('SOX','change management','involves','is a principal component of','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('SOX','identity management','involves','is a principal component of','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ISO/IEC 20000','ITIL','standardizes a part of','expands on standard','compliance','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('segmentation fault','Bad memory reference','possibly caused by','possibly caused by','faults','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NFS','storage','is managed by','may be used to manage','storage','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('postgres','database','is managed by','may be used to manage','database','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('userexists','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('usemodule','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('translatepath','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('strcmp','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('splitstring','slist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('splayclass','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('selectservers','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rrange','rrange [real,real]','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('returnszero','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('remoteclassesmatching','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('remotescalar','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regldap','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reglist','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regline','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registryvalue','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regextract','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regcmp','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regarray','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readtcp','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readstringlist','slist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readstringarray','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readreallist','rlist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readrealarray','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readintlist','ilist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readintarray','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readfile','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('randomint','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('peerleaders','slist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('peerleader','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('peers','slist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('now','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ldapvalue','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ldaplist','slist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ldaparray','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastnode','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('join','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isvariable','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isplain','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isnewerthan','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('islink','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('islessthan','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isgreaterthan','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isdir','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('irange','irange [int,int]','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('iprange','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hostsseen','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hostrange','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hostinnetgroup','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('host2ip','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hashmatch','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groupexists','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('grep','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getuid','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getgid','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getenv','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getindices','slist','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getfields','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('filesexist','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('fileexists','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('execresult','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('escape','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('diskfree','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('countlinesmatching','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('countclassesmatching','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classmatch','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classify','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('changedbefore','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('canonify','string','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ago','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('accumulated','int','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('accessedbefore','class','returns data-type','is returned by function','functions','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_region','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comment','an arbitrary string','is a body constraint of type','has possible body constraints','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comment','topics','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comment','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depends_on','an arbitrary string','is a list of type','is used in','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depends_on','an arbitrary string','is a body constraint of type','has possible body constraints','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depends_on','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('handle','string','is a body constraint of type','has possible body constraints','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('handle','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifvarclass','an arbitrary string','is a body constraint of type','has possible body constraints','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifvarclass','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classes','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('action','*','is a possible body constraint for','can have body constraints','values','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('track_growing_file','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('track_growing_file','match_value','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extraction_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extraction_regex','match_value','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_number','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_number','match_value','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_value','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('units','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('units','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('history_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('history_type','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('data_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('data_type','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stream_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stream_type','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('measurements','watch','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('inference','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('inference','infer','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('post_assoc_pattern','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('post_assoc_pattern','infer','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pre_assoc_pattern','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pre_assoc_pattern','infer','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('infer','inferences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('follow_topics','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('follow_topics','inferences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_root','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_root','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('web_root','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('web_root','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('representation','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('representation','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('represents','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('represents','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('represents','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associates','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associates','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associates','association','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('backward_relationship','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('backward_relationship','association','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('forward_relationship','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('forward_relationship','association','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('association','topics','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('authorize','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('authorize','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('authorize','roles','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('resource_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('resource_type','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifencrypted','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifencrypted','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('maproot','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('maproot','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('maproot','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deny','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deny','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deny','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('admit','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('admit','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('admit','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('access','access_rules','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('scan_arrivals','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('scan_arrivals','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_count','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_count','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_size','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_size','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('freespace','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('freespace','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_foreign','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_foreign','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('volume','storage','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('unmount','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('unmount','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_options','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_options','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_options','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_server','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_server','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_source','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_source','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_type','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_fstab','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_fstab','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount','storage','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_dependence_chain','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_dependence_chain','service_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_autostart_policy','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_autostart_policy','service_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_args','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_args','service_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_type','service_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_method','services','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_dependencies','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_dependencies','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_dependencies','services','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_policy','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service_policy','services','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('signals','system signals','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('signals','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('restart_class','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('restart_class','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_stop','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_stop','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vsize','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vsize','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('threads','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('threads','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('tty','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('tty','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ttime_range','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ttime_range','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stime_range','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stime_range','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('status','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('status','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rsize','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rsize','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_result','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_result','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_owner','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_owner','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_owner','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('priority','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('priority','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ppid','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ppid','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pgid','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pgid','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pid','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pid','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('command','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('command','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_select','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('out_of_range_define','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('out_of_range_define','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('out_of_range_define','process_count','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_range','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_range','process_count','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('in_range_define','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('in_range_define','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('in_range_define','process_count','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_count','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','web_server','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','main','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','cfengine','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','update','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_select','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_select','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_policy','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_policy','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_multiline_start','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_multiline_start','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_verify_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_verify_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_update_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_update_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_version_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_version_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_name_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_name_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_list_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_list_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_installed_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_installed_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_arch_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_arch_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_returncode','integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_returncode','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_convention','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_convention','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_version_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_version_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_update_ifelapsed','integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_update_ifelapsed','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_update_command','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_update_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_name_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_name_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_arch_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_arch_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_installed_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_installed_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_file_repositories','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_file_repositories','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_file_repositories','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_delete_convention','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_delete_convention','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_delete_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_delete_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_changes','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_changes','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_arch_regex','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_arch_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_add_command','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_add_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_method','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_architectures','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_architectures','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_architectures','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('packages','web_server','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promiser_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promiser_type','outputs','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('output_level','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('output_level','outputs','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('usebundle','(ext bundle)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('usebundle','methods','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv6_address','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv6_address','tcp_ip','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_netmask','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_netmask','tcp_ip','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_address','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_address','tcp_ip','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('tcp_ip','interfaces','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_value','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_value','replace_with','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_with','replace_patterns','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_patterns','upgrade_cfexecd','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_separator','a single character','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_separator','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_field','int','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_field','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_value','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_value','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_separator','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_separator','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_operation','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_operation','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extend_fields','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extend_fields','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('allow_blank_fields','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('allow_blank_fields','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_field','field_edits','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_edits','fixapache','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_edits','resolver','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('whitespace_policy','(option list)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('whitespace_policy','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_matching','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_matching','match_value','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_matching','location','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('first_last','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('first_last','location','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('before_after','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('before_after','location','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('location','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_contains_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_contains_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_contains_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_contains_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_contains_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_contains_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_match_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_match_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_match_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_match_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_match_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_match_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_startwith_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_startwith_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_startwith_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_startwith_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_startwith_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_startwith_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_select','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_type','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expand_scalars','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expand_scalars','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_lines','fixapache','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_lines','upgrade_cfexecd','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not_matching','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not_matching','delete_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_contains_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_contains_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_contains_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_contains_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_contains_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_contains_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_match_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_match_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_match_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_match_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_match_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_match_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_startwith_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_startwith_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_startwith_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_startwith_from_list','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_startwith_from_list','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_startwith_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_select','delete_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_lines','DeleteLinesMatching','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_end','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_end','select_region','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_start','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_start','select_region','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_start_delimiter','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_start_delimiter','select_region','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('transformer','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('transformer','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('touch','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('touch','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repository','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repository','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rotate','int','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rotate','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('newname','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('newname','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_suffix','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_suffix','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_mode','posix file mode or permission','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_mode','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rename','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rxdirs','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rxdirs','perms','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('owners','a user/group id','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('owners','a user/group id','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('owners','perms','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mode','posix file mode or permission','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mode','perms','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groups','a user/group id','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groups','a user/group id','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groups','perms','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bsdflags','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bsdflags','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bsdflags','perms','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('perms','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pathtype','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pathtype','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('move_obstructions','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('move_obstructions','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_no_source','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_no_source','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_linking_children','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_linking_children','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','link type','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_children','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_children','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_patterns','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_patterns','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_patterns','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_from','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_result','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_result','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('issymlinkto','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('issymlinkto','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('issymlinkto','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_types','(option list)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_types','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_program','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_program','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_regex','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_regex','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('atime','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('atime','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mtime','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mtime','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ctime','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ctime','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_bsdflags','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_bsdflags','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_bsdflags','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_groups','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_groups','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_groups','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_owners','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_owners','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_owners','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_size','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_size','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_mode','posix file mode or permission','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_mode','posix file mode or permission','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_mode','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_name','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_name','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_name','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('leaf_name','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('leaf_name','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('leaf_name','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_select','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('recognize_join','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('recognize_join','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('max_file_size','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('max_file_size','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('empty_file_before_editing','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('empty_file_before_editing','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_backup','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_backup','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_defaults','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_xml','(ext bundle)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_xml','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_line','(ext bundle)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_line','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xdev','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xdev','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('traverse_links','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('traverse_links','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdeadlinks','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdeadlinks','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_dirs','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_dirs','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_dirs','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_basedir','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_basedir','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exclude_dirs','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exclude_dirs','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exclude_dirs','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depth','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depth','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depth_search','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdirs','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdirs','delete','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dirlinks','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dirlinks','delete','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('create','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('create','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('verify','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('verify','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('type_check','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('type_check','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('trustkey','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('trustkey','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stealth','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stealth','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('purge','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('purge','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preserve','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preserve','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('portnumber','int','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('portnumber','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_ipv4','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_ipv4','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_update','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_update','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('linkcopy_patterns','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('linkcopy_patterns','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('linkcopy_patterns','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('findertype','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('findertype','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_size','irange [int,int]','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_size','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copylink_patterns','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copylink_patterns','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copylink_patterns','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_root','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_root','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('encrypt','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('encrypt','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_backup','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_backup','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compare','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compare','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('collapse_destination_dir','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('collapse_destination_dir','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('servers','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('servers','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('servers','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_from','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_diffs','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_diffs','changes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('update_hashes','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('update_hashes','changes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_changes','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_changes','changes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('changes','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('specify_inherit_aces','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('specify_inherit_aces','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('specify_inherit_aces','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_type','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_method','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_method','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_directory_inherit','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_directory_inherit','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aces','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aces','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aces','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_type','environments','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_state','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_state','environments','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_spec_file','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_spec_file','environment_resources','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_baseline','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_baseline','environment_resources','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_disk','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_disk','environment_resources','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_memory','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_memory','environment_resources','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_cpus','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_cpus','environment_resources','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_resources','environments','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_network','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_network','environment_interface','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_name','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_name','environment_interface','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_addresses','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_addresses','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('env_addresses','environment_interface','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_interface','environments','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_host','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('environment_host','environments','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registry_exclude','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registry_exclude','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registry_exclude','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_rows','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_rows','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_rows','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_columns','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_columns','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_columns','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_operation','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_operation','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_type','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_connection_db','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_connection_db','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_type','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_type','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_host','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_host','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_password','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_password','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_owner','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_owner','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_server','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('module','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('module','commands','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no_output','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no_output','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preview','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preview','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chroot','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chroot','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chdir','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chdir','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_timeout','int','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_timeout','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_group','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_group','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_owner','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_owner','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('umask','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('umask','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('useshell','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('useshell','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('contain','commands','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('args','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('args','commands','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','setup_knowledge','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','web_server','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','main','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','cfengine','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','update','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('timer_policy','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('timer_policy','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('persist_time','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('persist_time','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_notkept','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_notkept','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_notkept','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_repaired','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_repaired','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_repaired','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_kept','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_kept','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cancel_kept','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_kept','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_kept','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_kept','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_timeout','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_timeout','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_timeout','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_denied','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_denied','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_denied','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_failed','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_failed','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_failed','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_repaired','slist','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_repaired','slist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_repaired','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('measurement_class','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('measurement_class','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_level','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_level','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('background','boolean','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('background','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_notkept','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_notkept','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_repaired','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_repaired','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_kept','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_kept','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_failed','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_failed','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_repaired','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_repaired','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_priority','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_priority','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_kept','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_kept','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_level','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_level','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_string','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_string','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expireafter','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expireafter','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifelapsed','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifelapsed','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('action_policy','(menu option)','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('action_policy','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('showstate','an arbitrary string','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('showstate','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('showstate','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_to_file','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_to_file','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('number_of_lines','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('number_of_lines','printfile','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_to_print','string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_to_print','printfile','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('printfile','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('friend_pattern','an arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('friend_pattern','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','web_server','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','upgrade_cfexecd','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','anomalies','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','watch','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','main','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not','class','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expression','class','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expression','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dist','real number','is a list of type','is used in','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dist','real number','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dist','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xor','clist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xor','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('and','clist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('and','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('or','clist','is a body constraint of type','has possible body constraints','body_constraints','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('or','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','setup_knowledge','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','fixapache','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','web_server','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','laptop','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','resolver','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','anomalies','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','main','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','cfengine','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','g','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','update','is promised in bundle','is a bundle with promises of type','promise_types','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','extraction_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','units','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','inference','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','post_assoc_pattern','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','pre_assoc_pattern','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','follow_topics','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','path_root','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','web_root','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','represents','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','associates','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','backward_relationship','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','forward_relationship','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','authorize','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','maproot','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','deny','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','admit','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','mount_options','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','mount_server','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','service_args','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','tty','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','status','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','process_owner','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','command','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','out_of_range_define','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','in_range_define','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_version','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_multiline_start','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_version_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_patch_version_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_patch_name_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_patch_installed_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_patch_arch_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_noverify_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_name_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_name_convention','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_list_version_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_list_update_command','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_list_name_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_list_arch_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_installed_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_file_repositories','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_delete_convention','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_arch_regex','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','package_architectures','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','replace_value','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','field_value','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','field_separator','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','select_line_matching','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','insert_if_not_contains_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','insert_if_contains_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','insert_if_not_match_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','insert_if_match_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','insert_if_not_startwith_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','insert_if_startwith_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','delete_if_not_contains_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','delete_if_contains_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','delete_if_not_match_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','delete_if_match_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','delete_if_not_startwith_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','delete_if_startwith_from_list','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','select_end','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','select_start','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','newname','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','disable_suffix','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','source','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','copy_patterns','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','issymlinkto','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','search_groups','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','search_owners','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','leaf_name','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','include_dirs','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','exclude_dirs','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','linkcopy_patterns','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','copylink_patterns','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','env_network','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','env_name','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','env_addresses','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','registry_exclude','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','database_columns','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','db_server_connection_db','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','db_server_host','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','db_server_password','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','db_server_owner','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','exec_group','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','exec_owner','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','args','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','comment','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','depends_on','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','ifvarclass','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','measurement_class','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','value_notkept','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','value_repaired','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','value_kept','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','log_string','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','showstate','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','friend_pattern','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','slist','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','string','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('an arbitrary string','string','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a syslog level','string','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','string','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a user/group id','owners','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a user/group id','groups','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a user/group id','string','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','string','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine class expression','class','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('posix file mode or permission','disable_mode','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('posix file mode or permission','mode','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('posix file mode or permission','search_mode','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a single character','value_separator','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','dist','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','rlist','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','real','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','real','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','package_noverify_returncode','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','package_list_update_ifelapsed','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','ilist','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','int','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','int','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','select_line_number','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','sensible_count','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','sensible_size','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','vsize','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','threads','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','rsize','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','ppid','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','pgid','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','pid','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','match_range','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','max_file_size','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','depth','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','env_disk','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','env_memory','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','env_cpus','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','persist_time','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','expireafter','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','ifelapsed','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','number_of_lines','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','lastseen','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','int','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a time range','int','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link type','link_type','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','track_growing_file','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','ifencrypted','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','scan_arrivals','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','check_foreign','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','unmount','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','edit_fstab','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','extend_fields','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','allow_blank_fields','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','expand_scalars','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','not_matching','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','include_start_delimiter','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','touch','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','disable','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','rxdirs','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','move_obstructions','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','link_children','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','recognize_join','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','empty_file_before_editing','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','xdev','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','traverse_links','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','rmdeadlinks','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','include_basedir','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','rmdirs','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','create','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','verify','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','type_check','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','trustkey','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','stealth','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','purge','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','preserve','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','force_ipv4','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','force_update','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','check_root','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','encrypt','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','collapse_destination_dir','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','report_diffs','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','update_hashes','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','module','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','no_output','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','preview','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','useshell','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','background','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','audit','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','int','is a special case of','is the generic type for','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('system signals','signals','are used in constraint parameters','takes value','values','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comments','comment','see instances of','is one of a number of','system_policy','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('available patches report','packages','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('installed patches report','packages','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('setuid report','files','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('performance report','promises','is based on','reported in','system_reports','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen report','access','is affected by','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen report','copy_from','is affected by','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('installed software report','packages','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hashes report','changes','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hashes report','files','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_diffs report','changes','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_diffs report','files','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_changes report','changes','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_changes report','files','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compliance report','promise report','see also','see also','system_reports','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compliance report','promises','is based on','reported in','system_reports','');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classes report','classes','is generated with','reported in','system_reports','promise_types');
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_67','promises.cf.html#promise_knowledge_cf_67','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_67','promises.cf.html#promise_knowledge_cf_67','0','mon_update')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_67','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-repo..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mon_update','promises.cf.html#promise_knowledge_cf_67','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mon_update','promises.cf.html#promise_knowledge_cf_67','0','/var/cfengine/bin/cf-report')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mon_update','promises.cf.html#promise_knowledge_cf_67','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_63','promises.cf.html#promise_knowledge_cf_63','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_63','promises.cf.html#promise_knowledge_cf_63','0','web_server')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_63','Generate knowledge associative graphs','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_63','Generate knowledge associative graphs','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#promise_knowledge_cf_63','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#promise_knowledge_cf_63','0','/usr/local/sbin/cf-know -f /srv/www/htdocs/enterprise.cf -g')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#promise_knowledge_cf_63','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#bundle_web_server','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_56','promises.cf.html#promise_knowledge_cf_56','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_56','promises.cf.html#promise_knowledge_cf_56','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_56','Generate config reports','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_56','Generate config reports','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_43','promises.cf.html#promise_knowledge_cf_43','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_43','promises.cf.html#promise_knowledge_cf_43','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_43','Update magnified view files for monitoring knowledge','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_43','Update magnified view files for monitoring knowledge','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_35','promises.cf.html#promise_knowledge_cf_35','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_35','promises.cf.html#promise_knowledge_cf_35','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_35','Update long view files for monitoring knowledge','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_35','Update long view files for monitoring knowledge','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_28','promises.cf.html#promise_knowledge_cf_28','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_28','promises.cf.html#promise_knowledge_cf_28','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_28','Make sure knowledge files are accessible to webserver','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_28','Make sure knowledge files are accessible to webserver','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_15','promises.cf.html#promise_knowledge_cf_15','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_15','promises.cf.html#promise_knowledge_cf_15','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_15','(Uncommented promise of type <i>vars</i> made by: all_hosts..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_12','promises.cf.html#promise_knowledge_cf_12','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_12','promises.cf.html#promise_knowledge_cf_12','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_12','(Uncommented promise of type <i>vars</i> made by: doc_root..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('setup_knowledge','promises.cf.html#bundle_setup_knowledge','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_137','promises.cf.html#promise_webserver_cf_137','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_137','promises.cf.html#promise_webserver_cf_137','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_137','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_130','promises.cf.html#promise_webserver_cf_130','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_130','promises.cf.html#promise_webserver_cf_130','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_130','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_117','promises.cf.html#promise_webserver_cf_117','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_117','promises.cf.html#promise_webserver_cf_117','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_117','(Uncommented promise of type <i>insert_lines</i> made by: APACHE_CONF_INCLUDE_FILES..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_108','promises.cf.html#promise_webserver_cf_108','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_108','promises.cf.html#promise_webserver_cf_108','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_108','(Uncommented promise of type <i>vars</i> made by: del_modules..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_104','promises.cf.html#promise_webserver_cf_104','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_104','promises.cf.html#promise_webserver_cf_104','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_104','(Uncommented promise of type <i>vars</i> made by: add_modules..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('fixapache','promises.cf.html#bundle_fixapache','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_85','promises.cf.html#promise_webserver_cf_85','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_85','promises.cf.html#promise_webserver_cf_85','0','!software_ok.on')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_85','(Uncommented promise of type <i>reports</i> made by: The web server software c..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_software_ok_on','promises.cf.html#promise_webserver_cf_85','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_software_ok_on','promises.cf.html#promise_webserver_cf_85','0','The web server software could not be installed')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_software_ok_on','promises.cf.html#promise_webserver_cf_85','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_76','promises.cf.html#promise_webserver_cf_76','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_76','promises.cf.html#promise_webserver_cf_76','0','software_ok')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_76','(Uncommented promise of type <i>files</i> made by: /etc/sysconfig/apache2..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_ok','promises.cf.html#promise_webserver_cf_76','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_ok','promises.cf.html#promise_webserver_cf_76','0','/etc/sysconfig/apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_ok','promises.cf.html#promise_webserver_cf_76','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_64','promises.cf.html#promise_webserver_cf_64','0','packages')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_64','promises.cf.html#promise_webserver_cf_64','0','on')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_64','(Uncommented promise of type <i>packages</i> made by: php5..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_64','(Uncommented promise of type <i>packages</i> made by: apache2-prefork..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_64','(Uncommented promise of type <i>packages</i> made by: apache2-mod_php5..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_64','(Uncommented promise of type <i>packages</i> made by: apache2..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','promises.cf.html#promise_webserver_cf_64','0','php5')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','promises.cf.html#promise_webserver_cf_64','0','apache2-prefork')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','promises.cf.html#promise_webserver_cf_64','0','apache2-mod_php5')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','promises.cf.html#promise_webserver_cf_64','0','packages')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','promises.cf.html#promise_webserver_cf_64','0','apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','promises.cf.html#promise_webserver_cf_64','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_53','promises.cf.html#promise_webserver_cf_53','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_53','promises.cf.html#promise_webserver_cf_53','0','start_apache')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_53','(Uncommented promise of type <i>commands</i> made by: /etc/init.d/apache2 resta..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_apache','promises.cf.html#promise_webserver_cf_53','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_apache','promises.cf.html#promise_webserver_cf_53','0','/etc/init.d/apache2 restart')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_apache','promises.cf.html#promise_webserver_cf_53','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_44','promises.cf.html#promise_webserver_cf_44','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_44','promises.cf.html#promise_webserver_cf_44','0','off')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_44','(Uncommented promise of type <i>processes</i> made by: apache2..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('off','promises.cf.html#promise_webserver_cf_44','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('off','promises.cf.html#promise_webserver_cf_44','0','apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('off','promises.cf.html#promise_webserver_cf_44','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_38','promises.cf.html#promise_webserver_cf_38','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_38','promises.cf.html#promise_webserver_cf_38','0','web_ok.on')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_38','(Uncommented promise of type <i>processes</i> made by: apache2..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_ok_on','promises.cf.html#promise_webserver_cf_38','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_ok_on','promises.cf.html#promise_webserver_cf_38','0','apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_ok_on','promises.cf.html#promise_webserver_cf_38','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_23','promises.cf.html#promise_webserver_cf_23','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_23','promises.cf.html#promise_webserver_cf_23','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_23','(Uncommented promise of type <i>vars</i> made by: match_package..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_17','promises.cf.html#promise_webserver_cf_17','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_17','promises.cf.html#promise_webserver_cf_17','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_17','(Uncommented promise of type <i>vars</i> made by: site_http_conf..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_11','promises.cf.html#promise_webserver_cf_11','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_11','promises.cf.html#promise_webserver_cf_11','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_11','(Uncommented promise of type <i>vars</i> made by: document_root..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_31','promises.cf.html#promise_laptop_cf_31','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_31','promises.cf.html#promise_laptop_cf_31','0','!usb_backup_device_loaded')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_31','(Uncommented promise of type <i>files</i> made by: /home/backup..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_31','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_31','0','/home/backup')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_31','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_22','promises.cf.html#promise_laptop_cf_22','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_22','promises.cf.html#promise_laptop_cf_22','0','usb_backup_device_loaded')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_22','(Uncommented promise of type <i>files</i> made by: /media/disk-1/CfengineBac..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_22','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_22','0','/media/disk-1/CfengineBackupDevice')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_22','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_6','promises.cf.html#promise_laptop_cf_6','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_6','promises.cf.html#promise_laptop_cf_6','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_6','(Uncommented promise of type <i>vars</i> made by: mount_point..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('laptop','promises.cf.html#bundle_laptop','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_206','promises.cf.html#promise_library_cf_206','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_206','promises.cf.html#promise_library_cf_206','0','exec_fix')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_206','(Uncommented promise of type <i>reports</i> made by: Added a 5 minute schedule..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_200','promises.cf.html#promise_library_cf_200','0','replace_patterns')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_200','promises.cf.html#promise_library_cf_200','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_200','(Uncommented promise of type <i>replace_patterns</i> made by: cfexecd..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_196','promises.cf.html#promise_library_cf_196','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_196','promises.cf.html#promise_library_cf_196','0','exec_fix')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_196','(Uncommented promise of type <i>insert_lines</i> made by: 0,5,10,15,20,25,30,35,40,..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('upgrade_cfexecd','promises.cf.html#bundle_upgrade_cfexecd','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_171','promises.cf.html#promise_library_cf_171','0','delete_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_171','promises.cf.html#promise_library_cf_171','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_171','(Uncommented promise of type <i>delete_lines</i> made by: $(regex)..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('DeleteLinesMatching','promises.cf.html#bundle_DeleteLinesMatching','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_119','promises.cf.html#promise_library_cf_119','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_119','promises.cf.html#promise_library_cf_119','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_119','(Uncommented promise of type <i>field_edits</i> made by: NETCONFIG_DNS_STATIC_SEAR..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_111','promises.cf.html#promise_library_cf_111','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_111','promises.cf.html#promise_library_cf_111','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_111','(Uncommented promise of type <i>vars</i> made by: search..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('resolver','promises.cf.html#bundle_resolver','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_422','promises.cf.html#promise_site_cf_422','0','roles')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_422','promises.cf.html#promise_site_cf_422','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_422','(Uncommented promise of type <i>roles</i> made by: .*..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_418','promises.cf.html#promise_site_cf_418','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_418','promises.cf.html#promise_site_cf_418','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_418','(Uncommented promise of type <i>access</i> made by: /home/mark/.cfagent/bin/c..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_414','promises.cf.html#promise_site_cf_414','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_414','promises.cf.html#promise_site_cf_414','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_414','(Uncommented promise of type <i>access</i> made by: /home/mark/LapTop..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('access_rules','promises.cf.html#bundle_access_rules','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_401','promises.cf.html#promise_site_cf_401','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_401','promises.cf.html#promise_site_cf_401','0','anomaly_hosts.icmp_in_high_anomaly.!entropy_icmp_in_high')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_401','(Uncommented promise of type <i>reports</i> made by: Anomalous low entropy (3d..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','promises.cf.html#promise_site_cf_401','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','promises.cf.html#promise_site_cf_401','0','Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at $(mon.env_time) 
    - measured value $(mon.value_icmp_in) av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','promises.cf.html#promise_site_cf_401','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_394','promises.cf.html#promise_site_cf_394','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_394','promises.cf.html#promise_site_cf_394','0','entropy_dns_in_low.anomaly_hosts.udp_in_high_dev2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_394','(Uncommented promise of type <i>reports</i> made by: Anomalous (2dev) incoming..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','promises.cf.html#promise_site_cf_394','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','promises.cf.html#promise_site_cf_394','0','Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at $(mon.env_time) 
    - measured value $(mon.value_udp_in) av $(mon.average_udp_in) pm $(mon.stddev_udp_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','promises.cf.html#promise_site_cf_394','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_387','promises.cf.html#promise_site_cf_387','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_387','promises.cf.html#promise_site_cf_387','0','entropy_dns_in_low.anomaly_hosts.dns_in_high_anomaly')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_387','(Uncommented promise of type <i>reports</i> made by: Anomalous (3dev) incoming..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','promises.cf.html#promise_site_cf_387','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','promises.cf.html#promise_site_cf_387','0','Anomalous (3dev) incoming DNS packets on $(mon.host) at $(mon.env_time) 
    - measured value $(mon.value_dns_in) av $(average_dns_in) pm $(mon.stddev_dns_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','promises.cf.html#promise_site_cf_387','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_380','promises.cf.html#promise_site_cf_380','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_380','promises.cf.html#promise_site_cf_380','0','entropy_tcpsyn_in_low.anomaly_hosts.tcpsyn_in_high_dev2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_380','(Uncommented promise of type <i>reports</i> made by: Anomalous number of new T..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','promises.cf.html#promise_site_cf_380','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','promises.cf.html#promise_site_cf_380','0','Anomalous number of new TCP connections on $(mon.host) at $(mon.env_time) 
    - measured value $(mon.value_tcpsyn_in) av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','promises.cf.html#promise_site_cf_380','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_373','promises.cf.html#promise_site_cf_373','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_373','promises.cf.html#promise_site_cf_373','0','entropy_www_in_low.anomaly_hosts.www_in_high_anomaly')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_373','(Uncommented promise of type <i>reports</i> made by: LOW ENTROPY Incoming www ..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_373','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_373','0','LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time)
     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_373','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_366','promises.cf.html#promise_site_cf_366','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_366','promises.cf.html#promise_site_cf_366','0','entropy_www_in_high&anomaly_hosts.www_in_high_anomaly')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_366','(Uncommented promise of type <i>reports</i> made by: HIGH ENTROPY Incoming www..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_366','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_366','0','HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at $(mon.env_time) 
    - measured value $(mon.value_www_in) av $(mon.average_www_in) pm $(mon.stddev_www_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_366','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_359','promises.cf.html#promise_site_cf_359','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_359','promises.cf.html#promise_site_cf_359','0','rootprocs_high_dev2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_359','(Uncommented promise of type <i>reports</i> made by: RootProc anomaly high 2 d..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rootprocs_high_dev2','promises.cf.html#promise_site_cf_359','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rootprocs_high_dev2','promises.cf.html#promise_site_cf_359','0','RootProc anomaly high 2 dev on $(mon.host) at $(mon.env_time) 
    measured value $(mon.value_rootprocs) av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rootprocs_high_dev2','promises.cf.html#promise_site_cf_359','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_349','promises.cf.html#promise_site_cf_349','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_349','promises.cf.html#promise_site_cf_349','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_349','(Uncommented promise of type <i>files</i> made by: /tmp/shadow..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_349','(Uncommented promise of type <i>files</i> made by: /tmp/passwd..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_343','promises.cf.html#promise_site_cf_343','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_343','promises.cf.html#promise_site_cf_343','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_343','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs/shad..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_343','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs/pass..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_331','promises.cf.html#promise_site_cf_331','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_331','promises.cf.html#promise_site_cf_331','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_331','(Uncommented promise of type <i>vars</i> made by: files..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_330','promises.cf.html#promise_site_cf_330','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_330','promises.cf.html#promise_site_cf_330','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_330','(Uncommented promise of type <i>vars</i> made by: sysdir..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomalies','promises.cf.html#bundle_anomalies','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_321','promises.cf.html#promise_site_cf_321','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_321','promises.cf.html#promise_site_cf_321','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_321','(Uncommented promise of type <i>reports</i> made by: I am alive..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_disk_watch','promises.cf.html#free_disk_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_disk_watch','promises.cf.html#free_disk_watch','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_disk_watch','(Uncommented promise of type <i>measurements</i> made by: /bin/df..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('apache_watch_memory','promises.cf.html#apache_watch_memory','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('apache_watch_memory','promises.cf.html#apache_watch_memory','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('apache_watch_memory','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_pr..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('apache_watch_cpu','promises.cf.html#apache_watch_cpu','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('apache_watch_cpu','promises.cf.html#apache_watch_cpu','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('apache_watch_cpu','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_pr..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_memory_watch','promises.cf.html#free_memory_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_memory_watch','promises.cf.html#free_memory_watch','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_memory_watch','(Uncommented promise of type <i>measurements</i> made by: /proc/meminfo..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#promise_site_cf_321','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#promise_site_cf_321','0','I am alive')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#promise_site_cf_321','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_disk_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_disk_watch','0','/bin/df')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_disk_watch','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#apache_watch_memory','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#apache_watch_memory','0','/var/cfengine/state/cf_procs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#apache_watch_memory','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#apache_watch_cpu','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#apache_watch_cpu','0','/var/cfengine/state/cf_procs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#apache_watch_cpu','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_memory_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_memory_watch','0','/proc/meminfo')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_memory_watch','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_self_watch','promises.cf.html#monitor_self_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_self_watch','promises.cf.html#monitor_self_watch','0','atlas')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_self_watch','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_ro..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atlas','promises.cf.html#monitor_self_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atlas','promises.cf.html#monitor_self_watch','0','/var/cfengine/state/cf_rootprocs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atlas','promises.cf.html#monitor_self_watch','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('watch','promises.cf.html#bundle_watch','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_210','promises.cf.html#promise_site_cf_210','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_210','promises.cf.html#promise_site_cf_210','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_210','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf\\..*..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_204','promises.cf.html#promise_site_cf_204','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_204','promises.cf.html#promise_site_cf_204','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_204','(Uncommented promise of type <i>files</i> made by: /var/cfengine/outputs..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('garbage_collection','promises.cf.html#bundle_garbage_collection','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_177','promises.cf.html#promise_site_cf_177','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_177','promises.cf.html#promise_site_cf_177','0','Hr12')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_177','Update the locate database','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_177','Update the locate database','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Hr12','promises.cf.html#promise_site_cf_177','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Hr12','promises.cf.html#promise_site_cf_177','0','/usr/bin/updatedb --prunepaths=/windows')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Hr12','promises.cf.html#promise_site_cf_177','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_171','promises.cf.html#promise_site_cf_171','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_171','promises.cf.html#promise_site_cf_171','0','start_mysqld')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_171','(Uncommented promise of type <i>commands</i> made by: /etc/init.d/mysql start..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_mysqld','promises.cf.html#promise_site_cf_171','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_mysqld','promises.cf.html#promise_site_cf_171','0','/etc/init.d/mysql start')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_mysqld','promises.cf.html#promise_site_cf_171','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_163','promises.cf.html#promise_site_cf_163','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_163','promises.cf.html#promise_site_cf_163','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_163','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-serv..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_163','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_155','promises.cf.html#promise_site_cf_155','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_155','promises.cf.html#promise_site_cf_155','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_155','(Uncommented promise of type <i>processes</i> made by: mysql..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_152','promises.cf.html#promise_site_cf_152','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_152','promises.cf.html#promise_site_cf_152','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_152','(Uncommented promise of type <i>processes</i> made by: cf-serverd..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_152','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_138','promises.cf.html#promise_site_cf_138','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_138','promises.cf.html#promise_site_cf_138','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_138','Lay a tripwire for change management','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_138','Lay a tripwire for change management','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','promises.cf.html#promise_site_cf_131','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','promises.cf.html#promise_site_cf_131','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_123','promises.cf.html#promise_site_cf_123','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_123','promises.cf.html#promise_site_cf_123','0','!up_and_running')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_123','(Uncommented promise of type <i>reports</i> made by: Laptop wakes up..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_up_and_running','promises.cf.html#promise_site_cf_123','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_up_and_running','promises.cf.html#promise_site_cf_123','0','Laptop wakes up')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_up_and_running','promises.cf.html#promise_site_cf_123','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_114','promises.cf.html#promise_site_cf_114','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_114','promises.cf.html#promise_site_cf_114','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_114','(Uncommented promise of type <i>vars</i> made by: component..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('main','promises.cf.html#bundle_main','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_101','promises.cf.html#promise_site_cf_101','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_101','promises.cf.html#promise_site_cf_101','0','exec_fix')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_101','(Uncommented promise of type <i>processes</i> made by: cron..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_206','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_206','0','Added a 5 minute schedule to crontabs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_206','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_196','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_196','0','0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_196','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_site_cf_101','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_site_cf_101','0','cron')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_site_cf_101','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_93','promises.cf.html#promise_site_cf_93','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_93','promises.cf.html#promise_site_cf_93','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_93','This is inaccessible in new suse after cd opened/closed','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_93','This is inaccessible in new suse after cd opened/closed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_81','promises.cf.html#promise_site_cf_81','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_81','promises.cf.html#promise_site_cf_81','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_81','Check if there are still promises about cfengine 2 that need removing','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_81','Check if there are still promises about cfengine 2 that need removing','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_74','promises.cf.html#promise_site_cf_74','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_74','promises.cf.html#promise_site_cf_74','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_74','Test change detection','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_74','Test change detection','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_64','promises.cf.html#promise_site_cf_64','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_64','promises.cf.html#promise_site_cf_64','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_64','(Uncommented promise of type <i>commands</i> made by: /usr/sbin/ntpdate no.pool..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('run_agent','promises.cf.html#run_agent','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('run_agent','promises.cf.html#run_agent','0','integrate_cfengine2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('run_agent','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cfagent..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integrate_cfengine2','promises.cf.html#run_agent','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integrate_cfengine2','promises.cf.html#run_agent','0','/var/cfengine/bin/cfagent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integrate_cfengine2','promises.cf.html#run_agent','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_42','promises.cf.html#promise_site_cf_42','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_42','promises.cf.html#promise_site_cf_42','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_42','(Uncommented promise of type <i>vars</i> made by: cf2bits..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine','promises.cf.html#bundle_cfengine','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_19','promises.cf.html#promise_site_cf_19','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_19','promises.cf.html#promise_site_cf_19','0','!SuSE')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_19','(Uncommented promise of type <i>vars</i> made by: crontab..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_SuSE','promises.cf.html#promise_site_cf_19','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_SuSE','promises.cf.html#promise_site_cf_19','0','crontab')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_SuSE','promises.cf.html#promise_site_cf_19','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_15','promises.cf.html#promise_site_cf_15','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_15','promises.cf.html#promise_site_cf_15','0','SuSE')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_15','(Uncommented promise of type <i>vars</i> made by: acl..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_13','promises.cf.html#promise_site_cf_13','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_13','promises.cf.html#promise_site_cf_13','0','SuSE')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_13','(Uncommented promise of type <i>vars</i> made by: crontab..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_15','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_15','0','acl')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_15','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_13','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_13','0','crontab')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_13','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('g','promises.cf.html#bundle_g','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_45','promises.cf.html#promise_update_cf_45','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_45','promises.cf.html#promise_update_cf_45','0','reload.start_exec')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_45','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-exec..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_exec','promises.cf.html#promise_update_cf_45','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_exec','promises.cf.html#promise_update_cf_45','0','/var/cfengine/bin/cf-execd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_exec','promises.cf.html#promise_update_cf_45','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_41','promises.cf.html#promise_update_cf_41','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_41','promises.cf.html#promise_update_cf_41','0','reload.start_monitor')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_41','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_monitor','promises.cf.html#promise_update_cf_41','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_monitor','promises.cf.html#promise_update_cf_41','0','/var/cfengine/bin/cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_monitor','promises.cf.html#promise_update_cf_41','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_37','promises.cf.html#promise_update_cf_37','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_37','promises.cf.html#promise_update_cf_37','0','reload.start_server')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_37','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-serv..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_server','promises.cf.html#promise_update_cf_37','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_server','promises.cf.html#promise_update_cf_37','0','/var/cfengine/bin/cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_server','promises.cf.html#promise_update_cf_37','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_31','promises.cf.html#promise_update_cf_31','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_31','promises.cf.html#promise_update_cf_31','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_31','(Uncommented promise of type <i>processes</i> made by: cf-execd..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_30','promises.cf.html#promise_update_cf_30','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_30','promises.cf.html#promise_update_cf_30','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_30','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_29','promises.cf.html#promise_update_cf_29','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_29','promises.cf.html#promise_update_cf_29','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_29','(Uncommented promise of type <i>processes</i> made by: cf-serverd..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_21','promises.cf.html#promise_update_cf_21','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_21','promises.cf.html#promise_update_cf_21','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_21','(Uncommented promise of type <i>files</i> made by: /var/cfengine/bin..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_13','promises.cf.html#promise_update_cf_13','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_13','promises.cf.html#promise_update_cf_13','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_13','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_7','promises.cf.html#promise_update_cf_7','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_7','promises.cf.html#promise_update_cf_7','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_7','(Uncommented promise of type <i>vars</i> made by: master_location..)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_56','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_56','0','/usr/local/sbin/cf-promises -r')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_56','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_43','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_43','0','/srv/www/htdocs/reports/192.168.1.100/mean_field.nov')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_43','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_35','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_35','0','/srv/www/htdocs/reports/192.168.1.100/fluctuations.nov')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_35','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_28','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_28','0','/srv/www/htdocs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_28','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_15','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_15','0','all_hosts')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_15','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_12','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_12','0','doc_root')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_12','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_137','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_137','0','APACHE_MODULES=.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_137','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_130','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_130','0','APACHE_MODULES=.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_130','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_117','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_117','0','APACHE_CONF_INCLUDE_FILES="/home/mark/cfengine-inputs/httpd.conf"')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_117','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_108','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_108','0','del_modules')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_108','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_104','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_104','0','add_modules')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_104','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_23','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_23','0','match_package')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_23','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_17','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_17','0','site_http_conf')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_17','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_11','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_11','0','document_root')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_11','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_laptop_cf_6','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_laptop_cf_6','0','mount_point')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_laptop_cf_6','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_200','0','replace_patterns')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_200','0','cfexecd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_200','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_171','0','delete_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_171','0','$(regex)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_171','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_119','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_119','0','NETCONFIG_DNS_STATIC_SEARCHLIST.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_119','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_111','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_111','0','search')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_111','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_422','0','roles')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_422','0','.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_422','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_418','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_418','0','/home/mark/.cfagent/bin/cf-agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_418','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_414','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_414','0','/home/mark/LapTop')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_414','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_349','0','/tmp/shadow')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_349','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_349','0','/tmp/passwd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_349','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_343','0','/var/cfengine/inputs/shadow')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_343','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_343','0','/var/cfengine/inputs/passwd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_343','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_331','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_331','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_331','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_330','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_330','0','sysdir')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_330','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_210','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_210','0','/etc/resolv.conf\..*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_210','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_204','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_204','0','/var/cfengine/outputs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_204','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_163','0','/var/cfengine/bin/cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_163','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_163','0','/var/cfengine/bin/cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_163','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_155','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_155','0','mysql')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_155','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_152','0','cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_152','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_152','0','cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_152','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_138','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_138','0','/tmp/testfile2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_138','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','/etc/resolv.conf')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_114','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_114','0','component')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_114','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_93','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_93','0','/dev/sr0')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_93','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_81','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_81','0','/var/cfengine/inputs/.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_81','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_74','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_74','0','/usr/bin')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_74','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_64','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_64','0','/usr/sbin/ntpdate no.pool.ntp.org')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_64','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_42','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_42','0','cf2bits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_42','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_31','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_31','0','cf-execd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_31','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_30','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_30','0','cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_30','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_29','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_29','0','cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_29','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_21','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_21','0','/var/cfengine/bin')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_21','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_13','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_13','0','/var/cfengine/inputs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_13','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_7','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_7','0','master_location')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_update_cf_7','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update','promises.cf.html#bundle_update','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_outputs','Messages sent by cfengine to the operators as a matter of policy.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('host_portal','reports/host_portal.html','0','portal')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('host_portal','Portal to access mission status, monitored host data','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_reports','Reports from around the system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_policy','Local policy description with comments and dependencies','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('suppliers','Contact information and web pages','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('current_projects','http://www.example.com/scope_of_work.html','0','SOW')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('current_projects','Team projects and service','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('work_shifts','http://www.example.com/shifts_and_rotas.ods','0','Spreadsheet')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('work_shifts','Staff rotas','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_level_agreements','Promises made to clients about services','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_licenses','This cfengine software license expires <b>31 December 2012</b>','4','Cfengine Nova')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_licenses','reports/license_report.html','0','site usage')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_licenses','Terms of agreements, and renewal status','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('request_for_change','(RFC) The initial proposal for an intended change of system policy.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('security_routines_and_procedures','Non-automated, human workbook','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('security_policy','Local risk assessment summary','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('quality_assurance','Procedures for review','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mission_statement','Business objectives','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mission_status','/managed_hosts.png','5','/reports/allhosts.html')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mission_status','/managed_hosts.png','5','see all managed hosts')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mission_status','/reports/meters.png','5','/reports/host_portal.html')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mission_status','/reports/meters.png','5','network host status')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mission_status','Overview of IT operations','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('views','Roles for IT stakeholders','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('management_issues','Aspects of IT management typically covered by ITIL','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('management_issues','The main areas of management','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ontology','Main index','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copernicus','https://cfengine.com/copernicus/','0','Support knowledge portal')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copernicus','Cfengine\'s online support knowledge base','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('machine_resources','An inventory of physical and virtual resources','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('server_lifecycle','From creation to update and eventual reinstallation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_catalogue','The basic service promises of the system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_trends','reports/192.168.1.100/mainpage.html','0','weekly 192.168.1.100')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_trends','Long term performance data about the system.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_reports','Reports generated by cfengine','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hosts','reports/192.168.1.100/mainpage.html','0','status 192.168.1.100')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hosts','A general term for computers that run applications of any kind','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_knowledge','Cfengine knowledge console','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('security','https://cfengine.com/inside/security','0','advisories')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('security','Defending an acceptable level of risk','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('management_issues','Decision making support','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('operating_systems','The software installed on a computer that provides its basic software infrastructure','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('operating_systems','System software','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linear_topic_map_notation','http://www.ontopia.net/download/ltm.html','0','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('the_tao','topics, associations and occurrences','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lvalue','/inside/manuals/cf3-reference#Types-in-cfengine-3','0','Cfengine reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lvalue','Refers to the left hand side of an assignment \"lvalue => rvalue\"','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lvalue','A variable identifier, i.e. the left hand side of an \'=\' association. The promiser in a variable promise.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rvalue','/inside/manuals/cf3-reference#Types-in-cfengine-3','0','Cfengine reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rvalue','Refers to the right hand side of an assignment \"lvalue => rvalue\"','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rvalue','A variable value, i.e. the right hand side of an \'=\' association. The value promised in a variable promise.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('voluntary_cooperation','Describes the approach to modelling in which one does not assume absolute power to control, but rather assumes best effort of the system parts. A promise is an expression of voluntary cooperation - the opposite of an obligation.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('versions','An indentity string that distinguishes altered states of an agent or occurrence of a topic.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dependencies','Promises or promisees that provide the pre-requisites for conditional promises may be called dependencies of the conditional promise.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('workflows','A representation or story about actions or services. ','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stories','A sequence of topics that relates a start and an end point through a topic map. Different\naths through a topic map can relate different stories. An entire story promises to have some intention.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('services','An abstraction for actions or processes that are associated with a specific set of promises. Services are economically motivated black-boxes for promises.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('communicated_information','Any information that is transmitted falls into this category. It is a container class for more specific messages, e.g. promises.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('subjects','A subject is an abstract concept -- it is any thing that can be talked about or discussed.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('subjects','An abstraction for any things that can be discussed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','promises.cf.html#promise_library_cf_200','0','promise_library_cf_200')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','promises.cf.html#promise_library_cf_200','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','An information resource that represents a topic. Typically a document, database entry, image or other medium.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrence_types','Occurrences are classified by the topics that refer to them. When a topic claims that an occurrence represents it somehow it uses a representation label (also called a subtype in topic map parlance). These partition occurrences into overlapping sets since an occurrence could represent several topics.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('topic_instances','Topic instances are called occurrences in topic map parlance.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('type_instances','Occurrences of a type of object. In a topic map, topics are type occurrences. In a policy, promises are type occurrences.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('associations','Trains of thought that link one topic with another.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('structural_relationships','When a part of something is contained within another part, or is a piece of a whole then we can call this a structural relationship','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('structural_relationships','img/clusters.png','5','image')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('location_relationships','Spatial relationships, like up,down,left,right','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('causative_relationships','When one topic impacts on another, we can say there is a cause-effect relationship.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('conceptual_relationships','Ideas that are related in documentable fashion.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('actions','Changes instigated by a system leading to observable effects.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('behaviours','Characteristics of a system that can be observed.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('facts','Irrefutable pieces of information that can be observed.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('topics','Topics are the way we represent abstract subjects in a topic map. A topic is implemented as a database entry.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('topic_types','Topics (in topic maps) are classified into non-overlapping or disjoint types.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('body_constraint','A description of the range of values that an agent promises to restrict its behaviour to.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('body_type','The subject of a promise is limited to a system of non-overlapping, or disjoint types, e.g. in cfengine files, processes, knowledge, etc','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_body','The body of a promise contains the details that describe its substance. This generally has the name of some variable and a constraint on the values that the promiser promises it will have.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_body','Attribute of a promise, like a template for parameterizing compound promise attributes','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_type','The type of cfengine rules are called promise types','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promisee','The recipient of a promise is called the promisee.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promiser','The agent or entity that makes a promise is called the promiser.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('announcements','The act of making public some information is called an announcement.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('conditional_promises','A promise that is made subject to a pre-condition is not a true promise unless the condition can also be promised. Condition promises are often used to express dependencies.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','http://research.iu.hio.no/papers/origin2.pdf','0','paper')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','https://cfengine.com/cftimes/stories/icackeynote','0','keynote speech')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','Assertions about the desired properties of a system','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','When an agent\'s intention becomes public knowledge we call it a promise.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_proposals','Before a promise has been accepted and made it can be written down as a proposal. A contract contains many promise proposals, for instance.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('policy','A declared set of intentions describing how systems should behave','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bundles','A container for grouping related promises','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bundles','Collections of promises are called bundles.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('intentions','Ideas that might lead to courses of action and some time past, present or future.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('agents','Describes any system components that can give or receive promises, either physical or abstract; e.g. files, processes, topics, etc.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergence','https://cfengine.com/cftimes/stories/icackeynote','0','keynote speech')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergence','https://cfengine.com/cftimes/stories/cm/4','0','popular')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergence','https://cfengine.com/img/outside/convergence.png','5','image')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergence','The idea that we always move closer to the desired state of a system on every promise verification. Once the desired state has been reached, the state hsa converged and no further changes take place. Normally convergence to the desired state happens in a single repair, unless there are dependencies that cannot be resolved. Convergence is a stronger constraint on system behaviour than idempotence.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('idempotence','Idempotence is a mathematical property of operations or changes. An operator is idempotent if applying it twice is the same as applying it once. This is not the same as convergence, because the result need not be anchored to anything. e.g. <pre>touch myfile</pre> is idempotent but it is not a fixed point because its effect depends on which directory the operation is performed in. <pre>touch /tmp/myfile</pre> is convergent.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('heterogeneous_system','Any computer system that is composed of multiple operating system types','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('homogeneous_system','Any computer system that is composed of only one type of operating system','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pattern_matching','Approaches to identifying and modelling objects','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pattern_matching','Classifying items by the patterns they form','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('topic_maps','http://www.ontopia.net/topicmaps/materials/tao.html','0','tutorial')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('topic_maps','http://www.ontopia.com','0','introduction')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('topic_maps','ISO standard electronic index with semantic annotations','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('self_healing','System exhibiting automated repair and maintenance','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('autonomics','IBM\'s name for the idea of self-healing and self-regulating systems','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('computer_immunology','https://cfengine.com/cftimes/stories/sick','0','popular science')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('computer_immunology','An idea developed by Mark Burgess of Oslo University College in 1998 for self-healing systems. Related ideas were proposed by Stephanie Forrest\'s group at the University of New Mexico.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergent_operators','https://cfengine.com/cftimes/stories/cm','0','popular science')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergent_operators','http://research.iu.hio.no/papers/MACE-v2.pdf','0','paper')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('convergent_operators','http://research.iu.hio.no/papers/immunity.pdf','0','paper')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_theory','A model for representing systems of interacting parts, such as computer networks. Orginally described by Mark Burgess to describe cfengine as no other models were suitable.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('BDMA','Build, Deploy, Manage, Audit, system lifecycle','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('audit','Traditionally servers needed to be audited to determine their actual state. Under cfengine, auditing simply means generating summary reports about recent compliance or repair operations.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('manage','Traditionally servers needed to be managed by responding to incidents. Under cfengine indicdents are repaired automatically and management is chiefly about observing and diving the success of the current system policy.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('deploy','Traditionally servers were deployed by pushing out a cloned image. Under cfengine the deployment of policy just means publishing a new version of policy on a policy server.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('build','Also called \"provisioning\" systems, i.e. the process of specifying and assembling the resources to fulfill a set of system promises. Traditionally servers were built as a \"golden image\" for cloning. Under cfengine, the build phase of the server lifecycle involves planning policy for the desired-state. Installation may also be considered a part of the build process.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('inventory','List of items within a datacenter, e.g. servers, applications, services. Expressed in terms of a static database that is being updated every now on then, depending on how often you scan the network. Cfengine holds accurate inventory lists and reports can easily be created on the fly. Cfengine dosen’t hold any static database that requires frequent scan (that takes a lot of resources) to describe the inventory.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_discovery','The ability to detect new items (see CIs) in a datacenter. Cfengine recognizes new items on the fly and you can have them reported automatically. No need for data mining or guesses.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('configuration_management','http://research.iu.hio.no/papers/alvamark.pdf','0','paper')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('configuration_management','The ability to configure a Datacenter through configuration settings. This is the core of Cfengine. Cfengine comes with many advanced tools to ease the configuration of Datacenters and have it self-repair according the settings you define. See also Configuration Items (CIs), Change Management.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('change_management','The ability to trace, log and manipulate change in files. Whenever a change occurs in a file, this probably affects some other parts of the system. The ability to track, log and potentially restore changes are important for many purposes.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reporting','Extracting information in human readable form','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('application_management','Deploying and patching applications, inclding the ability to configure specific applications, like Apache-servers, Microsoft Exchange, etc.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('release_management','The process of preparing for major changes to policy or enterprise activity.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('incident_management','Repairing policy violations','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('identity_management','Managing user accounts','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('identity_management','Managing user credentials','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('storage_management','Disk volumes, primary, redundant and secondary storage (backup)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('capacity_planning','Sometimes called dimensioning or service provisioning. Determining whether sufficient resources will be available to deliver planned services','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compliance','Frameworks for standardizing and regulating systems','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('team_work','Focused group in a flat organization for efficient communication and decision-making','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('maintenance_paradigms','Approaches to keeping systems in an operational state','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_detection','http://research.iu.hio.no/anomalypapers.php','0','paper')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('gentoo','Linux distro gentoo','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slackware','Linux distro slackware','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('debian','Linux distro debian','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('redhat','Linux distro redhat','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('fedora','Linux distro fedora','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ubuntu','Linux distro ubuntu','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('suse','http://www.opensuse.org','0','official website')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('suse','Linux distro suse','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine_2_control_settings','Parameters that affect the behaviour of the agent rather than affect the system configuration.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_knowledge_cf_15','0','promise_knowledge_cf_15')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_knowledge_cf_15','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_108','0','promise_webserver_cf_108')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_108','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_104','0','promise_webserver_cf_104')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_104','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_23','0','promise_webserver_cf_23')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_23','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_library_cf_111','0','promise_library_cf_111')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_library_cf_111','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_331','0','promise_site_cf_331')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_331','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_114','0','promise_site_cf_114')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_114','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_42','0','promise_site_cf_42')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_42','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_15','0','promise_site_cf_15')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_15','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_knowledge_cf_12','0','promise_knowledge_cf_12')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_knowledge_cf_12','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_17','0','promise_webserver_cf_17')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_17','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_11','0','promise_webserver_cf_11')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_11','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_laptop_cf_6','0','promise_laptop_cf_6')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_laptop_cf_6','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_330','0','promise_site_cf_330')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_330','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_19','0','promise_site_cf_19')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_19','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_13','0','promise_site_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_update_cf_7','0','promise_update_cf_7')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_update_cf_7','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('enterprise_level_cfengine','Cfengine Enterprise editions are commercially licensed versions of Cfengine 3 with build-time library extensions. They are directed at organizations and scenarios where the need for complex, supported management processes arises. They provide extended functionality and time-saving process simplifications to the automation of datacentres.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine_3_0_x','cfengine version','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine_2_2_x','cfengine version','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine_nova','cfengine version','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('functions','/inside/manuals/cf3-reference#Special-functions','0','cf3')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('functions','/inside/manuals/cf2-Reference#Setting-classes-with-special-functions','0','classes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('functions','/inside/manuals/cf2-Reference#Variable-expansion-and-contexts','0','variables')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('distributed_system','Any computer system consisting of more than one host','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('research','Cfengine research','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('files','Notable or special files and types','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy','A measure of the variation in an observation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('intermittency','The entropy of availability of a service','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('faults','Error messages and conditions','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rollback','A widely misunderstood idea from change management that describes the reversal of managed changes to a system. Rollback is a property of transaction based data processing, a model that cannot apply to dynamical objects like computers.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rollback','Refers to the fictitious ability to perform an undo operation in change management','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('role_based_access_control','/inside/manuals/cf3-reference#roles-in-server-promises','0','cf3')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','promises.cf.html#promise_site_cf_138','0','promise_site_cf_138')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','promises.cf.html#promise_site_cf_138','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitoring','reports/host_portal.html','0','portal')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitoring','Ability to monitor the current state of your datacenter. Cfengine can sample and trace behaviour. Unlike many other monitoring solutions, Cfengine scales easily to thousands of machines. Cfengine monitors your datacenter intelligently, using historic data to decide whether a behaviour should call for action or not.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('soft_classes','A classification of the dynamic environment that can change depending on context, e.g. about the system hardware','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hard_classes','A classification of the basic environment that is immutable, e.g. about the system hardware','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hard_classes','/inside/manuals/cf3-reference#Hard-classes','0','manual')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('provisioning','The ability to install and adopt new services, applications, OS and servers, e.g. using PXE-boot Cfengine can turn any server into anything kind of server with any kind of service running on it. Cfengine can easily deploy a service or new application throughout the datacenter, regardless of the number of servers.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pre_screening','The ability to foresee consequences of change. If you make a change one place, what’s the impact of the rest of the datacenter? Cfengine includes a dry-run mode which simulates the consequences of a change.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('CI','Configuration Items. Computer related items that can be configured. Items typically are servers, routers, switches, applications and storage devices. Cfengine can discover different types of CIs automatically, trace dependencies and make them available in reports through an online knowledge portal.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('baseline','Definition of what your system looks like or what it must look like. Many traditional vendors in the industry require a baseline to be able to roll out their system and get it running. In Cfengine, there is no need for a special baseline. Cfengine focuses on the opposite – the end state.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SLA','Service Level Agreement','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('BDIM','Business Driven IT Management','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('auditing','A process to verify that the datacenter operate according to the guidelines and legislations. A rapid growth in datacenter size and complexity, combined with government regulations, have called for a drastic increase in audits.  Cfengine comes with reporting and logging capabilties that make auditing straightforward.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('CMDB','Configuration Management Database. A form of extended inventort system, championed by ITIL.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SAS_70','Stands for Statement of Auditing Standards no. 70. SAS-70 is an auditing standard. SAS 70 defines the professional standards used by a service auditor to assess the internal controls of a service organization and issue a service auditor’s report. Cfengine can be used to ensure SAS-70 compliance in the same way you can achieve other standardization compliance.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('EUROSOX','The 8th EU data directive','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SOX','Stands for Sarbanes-Oxley Act is a United States federal law in response to a number of major corporate and accounting scandals including those affecting Enron, Tyco International, Adelphia, Peregrine Systems and WorldCom. The intention behind the law is to ensure financial reporting. This law calls for standardization of datacenters under this law and the need to show compliance. Cfengine can be used as a tool for ensuring specific parts of the compliance.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SOX','The Sarbanes-Oxley Act','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('eTOM','The Enhanced Telecom Operations Map','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('eTOM','http://en.wikipedia.org/wiki/ETOM','0','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('eTOM','The Extended Telecom Operations Map','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ISO_IEC_20000','A standards document based on a subset of ITIL','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ITIL','http://www.itframeworks.org/wiki/Information_Technology_Infrastructure_Library','0','free wiki')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ITIL','http://www.itil-officialsite.com','0','official website')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ITIL','http://en.wikipedia.org/wiki/ITIL','0','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ITIL','Stands for Information Technology Infrastructure Library. ITIL is a set of self-proclaimed best-practises originally developed by the British government. It contains a set of common sense rules. Cfengine can be used as a tool for organizations looking to becoming ITIL-compliant. By translating many of your best practises into policy rules, you can have Cfengine ensure parts of ITIL-compliance.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('samba','The Unix service for integrating with MS Windows','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('NFS','Network File System (storage)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('oracle','Commercial relational SQL database','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('postgres','Open source relational SQL database engine for transaction processing','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mysql','Open source SQL database engine optimized for web','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('DHCP','An IP configuration service.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('NIS','Sun\'s Network Information Service (previously called Yellow Pages).','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('LDAP','A well-known directory service. MS Active Directory purports to support this standard.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('DNS','Domain Name Service for translating Internet addresses into hostnames and vice versa.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('BOOTP','A simple broadcast protocol for booting a system directly from the network, e.g. for installation purposes.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('PXE','A composite network service based on the PXE protocol, allows systems to boot directly from the network.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('connectivity','Cables and hardware','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('routing','Router and switch configuration','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('login','The ability to log onto systems with a username and password','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database','Relational database services','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('storage','Remotely accessible disk volumes.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('directory_services','Directory services are look-up services, typically with identity or host information.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web','The WWW web service','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('configuration','Cfengine desired-state automation and self-repair service','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mail','Electronic mail service, as made famous by the Unix operating system.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitoring','Pseudo-real-time status information about system components','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('network_services','Network connectivity and availability','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('application_services','Software running on servers','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('boot_services','Boot services are a way of booting a computer directly from the network','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('installation','Installing computers from bare metal','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('userexists','True if user name or numerical id exists on this host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usemodule','Execute cfengine module script and set class if successful','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('translatepath','Translate path separators from Unix style to the host\'s native','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('strcmp','True if the two strings match exactly','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('splitstring','Convert a string in arg1 into a list of max arg3 strings by splitting on a regular expression in arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('splayclass','True if the first argument\'s time-slot has arrived, according to a policy in arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('selectservers','Select tcp servers which respond correctly to a query and return their number, set array of names','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rrange','Define a range of real numbers for cfengine internal use','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('returnszero','True if named shell command has exit status zero','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('remoteclassesmatching','Read persistent classes matching a regular expression from a remote cfengine server and add them into local context with prefix','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('remotescalar','Read a scalar value from a remote cfengine server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regldap','True if the regular expression in arg6 matches a value item in an ldap search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reglist','True if the regular expression in arg2 matches any item in the list whose id is arg1','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regline','True if the regular expression in arg1 matches a line in file arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('registryvalue','Returns a value for an MS-Win registry key,value pair','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regextract','True if the regular expression in arg 1 matches the string in arg2 and sets a non-empty array of backreferences named arg3','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regcmp','True if arg1 is a regular expression matching that matches string arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regarray','True if arg1 matches any item in the associative array with id=arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readtcp','Connect to tcp port, send string and assign result to variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readstringlist','Read and assign a list variable from a file of separated strings','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readstringarray','Read an array of strings from a file and assign the dimension to a variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readreallist','Read and assign a list variable from a file of separated real numbers','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readrealarray','Read an array of real numbers from a file and assign the dimension to a variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readintlist','Read and assign a list variable from a file of separated ints','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readintarray','Read an array of integers from a file and assign the dimension to a variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readfile','Read max number of bytes from named file and assign to variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('randomint','Generate a random integer between the given limits','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('peerleaders','Get a list of peer leaders from the named partitioning','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('peerleader','Get the assigned peer-leader of the partition to which we belong','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('peers','Get a list of peers (not including ourself) from the partition to which we belong','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','Convert an exact date/time to an integer system representation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('now','Convert the current time into system representation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ldapvalue','Extract the first matching named value from ldap','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ldaplist','Extract all named values from multiple ldap records','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ldaparray','Extract all values from an ldap record','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastnode','Extract the last of a separated string, e.g. filename from a path','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('join','Join the items of arg2 into a string, using the conjunction in arg1','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isvariable','True if the named variable is defined','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isplain','True if the named object is a plain/regular file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isnewerthan','True if arg1 is newer (modified later) than arg2 (mtime)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('islink','True if the named object is a symbolic link','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('islessthan','True if arg1 is numerically less than arg2, else compare strings like NOT strcmp','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isgreaterthan','True if arg1 is numerically greater than arg2, else compare strings like strcmp','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isdir','True if the named object is a directory','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('irange','Define a range of integer values for cfengine internal use','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('iprange','True if the current host lies in the range of IP addresses specified','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hostsseen','Extract the list of hosts last seen/not seen within the last arg1 hours','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hostrange','True if the current host lies in the range of enumerated hostnames specified','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hostinnetgroup','True if the current host is in the named netgroup','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('host2ip','Returns the primary name-service IP address for the named host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hashmatch','Compute the hash of arg1, of type arg2 and test if it matches the value in arg 3','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','Return the hash of arg1, type arg2 and assign to a variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('groupexists','True if group or numerical id exists on this host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('grep','Extract the sub-list if items matching the regular expression in arg1 of the list named in arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getuid','Return the integer user id of the named user on this host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getgid','Return the integer group id of the named group on this host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getenv','Return the environment variable named arg1, truncated at arg2 characters','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getindices','Get a list of keys to the array whose id is the argument and assign to variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getfields','Get an array of fields in the lines matching regex arg1 in file arg2, split on regex arg3 as array name arg4','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('filesexist','True if the named list of files can ALL be accessed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('fileexists','True if the named file can be accessed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('execresult','Execute named command and assign output to variable','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('escape','Escape regular expression characters in a string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('diskfree','Return the free space (in KB) available on the directory\'s current partition (0 if not found)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('countlinesmatching','Count the number of lines matching regex arg1 in file arg2','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('countclassesmatching','Count the number of defined classes matching regex arg1','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classmatch','True if the regular expression matches any currently defined class','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classify','True if the canonicalization of the argument is a currently defined class','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changedbefore','True if arg1 was changed before arg2 (ctime)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('canonify','Convert an abitrary string into a legal class name','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ago','Convert a time relative to now to an integer system representation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('accumulated','Convert an accumulated amount of time into a system representation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('accessedbefore','True if arg1 was accessed before arg2 (atime)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('class','A boolean returned by certain functions in classes promises','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('policy','The policy for (dis)allowing (re)definition of variables matching free,overridable,constant,ifdefined','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rlist','A list of real numbers matching -9.99999E100,9.99999E100','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ilist','A list of integers matching -99999999999,9999999999','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','A list of scalar strings matching ','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real','A scalar real number matching -9.99999E100,9.99999E100','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('int','A scalar integer matching -99999999999,9999999999','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','A scalar string matching ','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Generate knowledge associative graphs','4','promise_knowledge_cf_63')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_63','0','promise_knowledge_cf_63')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_63','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Generate config reports','4','promise_knowledge_cf_56')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_56','0','promise_knowledge_cf_56')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_56','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Update magnified view files for monitoring knowledge','4','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Update long view files for monitoring knowledge','4','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Make sure knowledge files are accessible to webserver','4','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Update the locate database','4','promise_site_cf_177')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_177','0','promise_site_cf_177')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_177','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Lay a tripwire for change management','4','promise_site_cf_138')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_138','0','promise_site_cf_138')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_138','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','This is inaccessible in new suse after cd opened/closed','4','promise_site_cf_93')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_93','0','promise_site_cf_93')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_93','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Check if there are still promises about cfengine 2 that need removing','4','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Test change detection','4','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#run_agent','0','run_agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#run_agent','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_349','0','promise_site_cf_349')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_349','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_163','0','promise_site_cf_163')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_163','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_76','0','promise_webserver_cf_76')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_76','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_site_cf_123','0','promise_site_cf_123')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_site_cf_123','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_library_cf_171','0','promise_library_cf_171')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_library_cf_171','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_177','0','promise_site_cf_177')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_177','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#run_agent','0','run_agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#run_agent','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('track_growing_file','If true, cfengine remembers the position to which is last read when opening the file, and resets to the start if the file has since been truncated','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('track_growing_file','If true, cfengine remembers the position to which is last read when opening the file, and resets to the start if the file has since been truncated','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','Regular expression that should contain a single backreference for extracting a value','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','Regular expression that should contain a single backreference for extracting a value','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_number','Read from the n-th line of the output (fixed format)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_number','Read from the n-th line of the output (fixed format)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','Criteria for extracting the measurement from a datastream','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','The engineering dimensions of this value or a note about its intent used in plots','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','Whether the data can be seen as a time-series or just an isolated value','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','The datatype being collected.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','The datatype being collected.','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('inference','Result of the syllogism','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('inference','Result of the syllogism','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('post_assoc_pattern','Name of backward/inverse association from associates to promiser topic','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('post_assoc_pattern','Name of backward/inverse association from associates to promiser topic','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pre_assoc_pattern','Name of forward association between promiser topic and associates','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('infer','Specify the promise-inference triangle from this topic','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('follow_topics','Use the knowledge promised by topics matching this pattern','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('path_root','Base path of the occurrence when locating file (replaced by web_root)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_root','Base URL of the occurrence when rendered as a web-URL (replaces path_root)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('representation','How to interpret the promiser string e.g. actual data or reference to data','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('represents','List of subtopics that explains the type(s) of information represented by the occurrence','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('associates','List of associated topics by this forward relationship','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('associates','List of associated topics by this forward relationship','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('backward_relationship','Name of backward/inverse association from associates to promiser topic','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('backward_relationship','Name of backward/inverse association from associates to promiser topic','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('forward_relationship','Name of forward association between promiser topic and associates','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('association','Declare associated topics','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('authorize','promises.cf.html#promise_site_cf_422','0','promise_site_cf_422')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('authorize','promises.cf.html#promise_site_cf_422','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('authorize','List of public-key user names that are allowed to activate the promised class during remote agent activation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('resource_type','The type of object being granted access (the default grants access to files)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifencrypted','true/false whether the current file access promise is conditional on the connection from the client being encrypted','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('maproot','List of host names or IP addresses to grant full read-privilege on the server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('deny','List of host names or IP addresses to deny access to file objects','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_418','0','promise_site_cf_418')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_418','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_414','0','promise_site_cf_414')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_414','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','List of host names or IP addresses to grant access to file objects','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('scan_arrivals','true/false generate pseudo-periodic disk change arrival distribution','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('scan_arrivals','true/false generate pseudo-periodic disk change arrival distribution','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sensible_count','Minimum number of files that should be defined on a sensible-looking storage device','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sensible_count','Minimum number of files that should be defined on a sensible-looking storage device','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sensible_size','Minimum size in bytes that should be used on a sensible-looking storage device','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('freespace','Absolute or percentage minimum disk space that should be available before warning','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('freespace','Absolute or percentage minimum disk space that should be available before warning','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('check_foreign','true/false verify storage that is mounted from a foreign system on this host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('volume','Criteria for monitoring/probing mounted volumes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('unmount','true/false unmount a previously mounted filesystem','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('unmount','true/false unmount a previously mounted filesystem','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_options','List of option strings to add to the file system table (\"fstab\")','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_options','List of option strings to add to the file system table (\"fstab\")','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_server','Hostname or IP or remote file system server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_source','Path of remote file system to mount','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_type','Protocol type of remote file system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_type','Protocol type of remote file system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_fstab','true/false add or remove entries to the file system table (\"fstab\")','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount','Criteria for mounting foreign file systems','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_dependence_chain','How to handle dependencies and dependent services','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_autostart_policy','Should the service be started automatically by the OS','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_autostart_policy','Should the service be started automatically by the OS','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_args','Parameters for starting the service','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_type','Service abstraction type','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_method','Details of the service specification','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_dependencies','A list of services on which this service depends','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('service_policy','Policy for service status to maintain','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','promises.cf.html#promise_site_cf_101','0','promise_site_cf_101')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','promises.cf.html#promise_site_cf_101','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','A list of menu options representing signals to be sent to a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_webserver_cf_38','0','promise_webserver_cf_38')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_webserver_cf_38','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_site_cf_155','0','promise_site_cf_155')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_site_cf_155','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_site_cf_152','0','promise_site_cf_152')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_site_cf_152','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_31','0','promise_update_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_30','0','promise_update_cf_30')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_30','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_29','0','promise_update_cf_29')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_29','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','A class to be defined globally if the process is not running, so that a command: rule can be referred to restart the process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_stop','promises.cf.html#promise_webserver_cf_44','0','promise_webserver_cf_44')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_stop','promises.cf.html#promise_webserver_cf_44','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_stop','A command used to stop a running process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('vsize','Range of integers matching the virtual memory size of a process, in kilobytes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('vsize','Range of integers matching the virtual memory size of a process, in kilobytes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('threads','Range of integers matching the threads (NLWP) field of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('threads','Range of integers matching the threads (NLWP) field of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('tty','Regular expression matching the tty field of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ttime_range','Range of integers matching the total elapsed time of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stime_range','Range of integers matching the start time of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stime_range','Range of integers matching the start time of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('status','Regular expression matching the status field of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('status','Regular expression matching the status field of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rsize','Range of integers matching the resident memory size of a process, in kilobytes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_result','Boolean class expression returning the logical combination of classes set by a process selection test','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_result','Boolean class expression returning the logical combination of classes set by a process selection test','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_owner','List of regexes matching the user of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('priority','Range of integers matching the priority field (PRI/NI) of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('priority','Range of integers matching the priority field (PRI/NI) of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ppid','Range of integers matching the parent process id of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ppid','Range of integers matching the parent process id of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pgid','Range of integers matching the parent group id of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pgid','Range of integers matching the parent group id of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pid','Range of integers matching the process id of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pid','Range of integers matching the process id of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('command','Regular expression matching the command/cmd field of a process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_select','Criteria for matching processes in the system process table','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('out_of_range_define','List of classes to define if the matches are out of range','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('out_of_range_define','List of classes to define if the matches are out of range','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_range','Integer range for acceptable number of matches for this process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_range','Integer range for acceptable number of matches for this process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('in_range_define','List of classes to define if the matches are in range','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_count','Criteria for constraining the number of processes matching other criteria','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_version','Version reference point for determining promised version','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_select','A criterion for first acceptable match relative to \"package_version\"','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','Criteria for package installation/upgrade on the current system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_multiline_start','Regular expression which matches the start of a new package in multiline output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_multiline_start','Regular expression which matches the start of a new package in multiline output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_version_regex','Regular expression with one backreference to extract package version string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_verify_command','Command to verify the correctness of an installed package','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','Command to update to the latest version a currently installed package','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','Command to update to the latest version a currently installed package','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','Regular expression with one backreference to extract update version string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','Regular expression with one backreference to extract update version string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','Regular expression with one backreference to extract update name string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','Command to obtain a list of available patches or updates','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','Command to obtain a list of available patches or updates','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','Regular expression which matches packages that are already installed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','Command to update to the latest patch release of an installed package','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','Command to update to the latest patch release of an installed package','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_arch_regex','Regular expression with one backreference to extract update architecture string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_arch_regex','Regular expression with one backreference to extract update architecture string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_returncode','Integer return code indicating package verification failure','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_returncode','Integer return code indicating package verification failure','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_regex','Regular expression to match verification failure output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_regex','Regular expression to match verification failure output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_regex','Regular expression with one backreference to extract package name string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_regex','Regular expression with one backreference to extract package name string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','This is how the package manager expects the package to be referred to, e.g. $(name).$(arch)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','This is how the package manager expects the package to be referred to, e.g. $(name).$(arch)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','Regular expression with one backreference to extract package version string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','Regular expression with one backreference to extract package version string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_update_ifelapsed','The ifelapsed locking time in between updates of the package list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_update_ifelapsed','The ifelapsed locking time in between updates of the package list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_update_command','Command to update the list of available packages (if any)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_update_command','Command to update the list of available packages (if any)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','Regular expression with one backreference to extract package name string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','Command to obtain a list of available packages','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','Command to obtain a list of available packages','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','Regular expression with one backreference to extract package architecture string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','Regular expression with one backreference to extract package architecture string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','Regular expression which matches packages that are already installed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','Regular expression which matches packages that are already installed','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_file_repositories','A list of machine-local directories to search for packages','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_file_repositories','A list of machine-local directories to search for packages','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_convention','This is how the package manager expects the package to be referred to in the deletion part of a package update, e.g. $(name)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','Command to remove a package from the system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','Menu option - whether to group packages into a single aggregate command','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','Menu option - whether to group packages into a single aggregate command','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_arch_regex','Regular expression with one backreference to extract package architecture string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','Command to install a package to the system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','Criteria for installation and verification','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_architectures','Select the architecture for package selection','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promiser_type','Output level to observe for the named promise or bundle (meta-promise)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('output_level','Output level to observe for the named promise or bundle (meta-promise)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usebundle','Specify the name of a bundle to run as a parameterized method','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ipv6_address','IPv6 address for the interface','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ipv4_netmask','Netmask for the interface','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ipv4_address','IPv4 address for the interface','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('tcp_ip','Interface tcp/ip properties','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_value','promises.cf.html#promise_library_cf_200','0','promise_library_cf_200')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_value','promises.cf.html#promise_library_cf_200','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_value','Value used to replace regular expression matches in search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','Menu option to replace all occurrences or just first (NB the latter is non-convergent)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_with','promises.cf.html#promise_library_cf_200','0','promise_library_cf_200')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_with','promises.cf.html#promise_library_cf_200','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_with','Search-replace pattern','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','Character separator for subfields inside the selected field','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','Integer index of the field required 1..n','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','Integer index of the field required 1..n','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','Set field value to a fixed value','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','Set field value to a fixed value','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','The regular expression used to separate fields in a line','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','Menu option policy for editing subfields','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','Menu option policy for editing subfields','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','true/false add new fields at end of line if necessary to complete edit','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','true/false add new fields at end of line if necessary to complete edit','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','true/false allow blank fields in a line (do not purge)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_137','0','promise_webserver_cf_137')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_137','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_130','0','promise_webserver_cf_130')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_130','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_library_cf_119','0','promise_library_cf_119')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_library_cf_119','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','Edit line-based file as matrix of fields','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('whitespace_policy','Criteria for matching and recognizing existing lines','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#apache_watch_memory','0','apache_watch_memory')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#apache_watch_memory','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#apache_watch_cpu','0','apache_watch_cpu')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#apache_watch_cpu','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','Regular expression for matching line location','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','Regular expression for matching file line location','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('first_last','Menu option, choose first or last occurrence of match in file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('before_after','Menu option, point cursor before of after matched line','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('location','Specify where in a file an insertion will be made','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_contains_from_list','Insert line if a regex in the list DOES NOT match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_contains_from_list','Insert line if a regex in the list DOES NOT match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_contains_from_list','Insert line if a regex in the list match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_contains_from_list','Insert line if a regex in the list match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_match_from_list','Insert line if it DOES NOT fully match a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_match_from_list','Insert line if it DOES NOT fully match a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_match_from_list','Insert line if it fully matches a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_match_from_list','Insert line if it fully matches a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_startwith_from_list','Insert line if it DOES NOT start with a string in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_startwith_from_list','Insert line if it DOES NOT start with a string in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_startwith_from_list','Insert line if it starts with a string in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_select','Insert only if lines pass filter criteria','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_type','Type of object the promiser string refers to','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expand_scalars','Expand any unexpanded variables','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('not_matching','true/false negate match criterion','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_contains_from_list','Delete line if a regex in the list DOES NOT match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_contains_from_list','Delete line if a regex in the list DOES NOT match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_contains_from_list','Delete line if a regex in the list match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_contains_from_list','Delete line if a regex in the list match a line fragment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_match_from_list','Delete line if it DOES NOT fully match a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_match_from_list','Delete line if it DOES NOT fully match a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_match_from_list','Delete line if it fully matches a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_match_from_list','Delete line if it fully matches a regex in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_startwith_from_list','Delete line if it DOES NOT start with a string in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_startwith_from_list','Delete line if it DOES NOT start with a string in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_startwith_from_list','Delete line if it starts with a string in the list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_select','Delete only if lines pass filter criteria','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_end','Regular expression matches end of edit region from start','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_end','Regular expression matches end of edit region from start','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_start','Regular expression matching start of edit region','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_start','Regular expression matching start of edit region','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_start_delimiter','Whether to include the section delimiter','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_region','Limit edits to a demarked region of the file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('transformer','Command (with full path) used to transform current file (no shell wrapper used)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('touch','true/false whether to touch time stamps on file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repository','Name of a repository for versioning','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rotate','Maximum number of file rotations to keep','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rotate','Maximum number of file rotations to keep','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('newname','The desired name for the current file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('newname','The desired name for the current file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_suffix','The suffix to add to files when disabling (.cfdisabled)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_suffix','The suffix to add to files when disabling (.cfdisabled)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_mode','The permissions to set when a file is disabled','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_mode','The permissions to set when a file is disabled','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable','true/false automatically rename and remove permissions','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rename','Criteria for renaming files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rxdirs','true/false add execute flag for directories if read flag is set','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rxdirs','true/false add execute flag for directories if read flag is set','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_site_cf_93','0','promise_site_cf_93')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_site_cf_93','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','List of acceptable owners or user ids, first is change target','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','List of acceptable owners or user ids, first is change target','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','File permissions (like posix chmod)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','File permissions (like posix chmod)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('groups','List of acceptable groups of group ids, first is change target','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bsdflags','List of menu options for bsd file system flags to set','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_site_cf_93','0','promise_site_cf_93')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_site_cf_93','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','Criteria for setting permissions on a file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pathtype','Menu option for interpreting promiser file object','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('move_obstructions','true/false whether to move obstructions to file-object creation','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_no_source','Behaviour when the source file to link to does not exist','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_linking_children','Policy for overriding existing files when linking directories of children','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_linking_children','Policy for overriding existing files when linking directories of children','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_349','0','promise_site_cf_349')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_349','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_343','0','promise_site_cf_343')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_343','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','The source file to which the link should point','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','The source file to which the link should point','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','The type of link used to alias the file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','The type of link used to alias the file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_children','true/false whether to link all directory\'s children to source originals','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_children','true/false whether to link all directory\'s children to source originals','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_patterns','A set of patterns that should be copied ansd synchronized instead of linked','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_from','Criteria for linking file from a source','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_210','0','promise_site_cf_210')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_210','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','Logical expression combining classes defined by file search criteria','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','Logical expression combining classes defined by file search criteria','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('issymlinkto','List of regular expressions to match file objects','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_types','List of acceptable file types from menu choices','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_program','Execute this command on each file and match if the exit status is zero','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_regex','Matches file if this regular expression matches any full line returned by the command','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atime','Range of access times (atime) for acceptable files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_210','0','promise_site_cf_210')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_210','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','Range of modification times (mtime) for acceptable files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ctime','Range of change times (ctime) for acceptable files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_bsdflags','String of flags for bsd file system flags expected set','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_bsdflags','String of flags for bsd file system flags expected set','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_groups','List of acceptable group names or ids for the file, or regexes to match','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_groups','List of acceptable group names or ids for the file, or regexes to match','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_owners','List of acceptable user names or ids for the file, or regexes to match','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_size','Integer range of file sizes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_size','Integer range of file sizes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_mode','A list of mode masks for acceptable file permissions','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('path_name','List of pathnames to match acceptable target','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('path_name','List of pathnames to match acceptable target','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','List of regexes that match an acceptable name','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_210','0','promise_site_cf_210')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_210','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','Choose which files select in a search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('recognize_join','Join together lines that end with a backslash, up to 4kB limit','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('recognize_join','Join together lines that end with a backslash, up to 4kB limit','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','Do not edit files bigger than this number of bytes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','Do not edit files bigger than this number of bytes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','Baseline memory model of file to zero/empty before commencing promised edits','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_backup','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_backup','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_backup','Menu option for backup policy on edit changes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_defaults','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_defaults','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_defaults','Default promise details for file edits','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_xml','XML editing model for file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_webserver_cf_76','0','promise_webserver_cf_76')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_webserver_cf_76','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','Line editing model for file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('xdev','true/false exclude directories that are on different devices','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('xdev','true/false exclude directories that are on different devices','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('traverse_links','true/false traverse symbolic links to directories (false)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('traverse_links','true/false traverse symbolic links to directories (false)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdeadlinks','true/false remove links that point to nowhere','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdeadlinks','true/false remove links that point to nowhere','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_dirs','List of regexes of directory names to include in depth search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_dirs','List of regexes of directory names to include in depth search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_basedir','true/false include the start/root dir of the search results','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_basedir','true/false include the start/root dir of the search results','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','List of regexes of directory names NOT to include in depth search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','List of regexes of directory names NOT to include in depth search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','Maximum depth level for search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_knowledge_cf_28','0','promise_knowledge_cf_28')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_knowledge_cf_28','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','Criteria for file depth searches','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_210','0','promise_site_cf_210')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_210','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','true/false whether to delete empty directories during recursive deletion','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_210','0','promise_site_cf_210')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_210','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','Menu option policy for dealing with symbolic links to directories during deletion','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_210','0','promise_site_cf_210')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_210','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_204','0','promise_site_cf_204')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_204','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','Criteria for deleting files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('create','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('create','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('create','true/false whether to create non-existing file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','true/false verify transferred file by hashing after copy (resource penalty)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','true/false verify transferred file by hashing after copy (resource penalty)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('type_check','true/false compare file types before copying and require match','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('type_check','true/false compare file types before copying and require match','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('trustkey','true/false trust public keys from remote server if previously unknown','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('trustkey','true/false trust public keys from remote server if previously unknown','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stealth','true/false whether to preserve time stamps on copied file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stealth','true/false whether to preserve time stamps on copied file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('purge','true/false purge files on client that do not match files on server when depth_search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('purge','true/false purge files on client that do not match files on server when depth_search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_349','0','promise_site_cf_349')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_349','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_343','0','promise_site_cf_343')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_343','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','true/false whether to preserve file permissions on copied file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('portnumber','Port number to connect to on server host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('portnumber','Port number to connect to on server host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_ipv4','true/false force use of ipv4 on ipv6 enabled network','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_ipv4','true/false force use of ipv4 on ipv6 enabled network','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_update','true/false force copy update always','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_update','true/false force copy update always','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Menu option for type of links to use when copying','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Menu option for type of links to use when copying','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linkcopy_patterns','List of patterns matching files that should be replaced with symbolic links','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('findertype','Menu option for default finder type on MacOSX','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_size','Integer range of file sizes that may be copied','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_size','Integer range of file sizes that may be copied','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copylink_patterns','List of patterns matching files that should be linked instead of copied','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copylink_patterns','List of patterns matching files that should be linked instead of copied','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('check_root','true/false check permissions on the root directory when depth_search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('check_root','true/false check permissions on the root directory when depth_search','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('encrypt','true/false use encrypted data stream to connect to remote host','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_349','0','promise_site_cf_349')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_349','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_343','0','promise_site_cf_343')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_343','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','Menu option policy for file backup/version control','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','Menu option policy for comparing source and image file attributes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','Menu option policy for comparing source and image file attributes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('collapse_destination_dir','true/false Place files in subdirectories into the root destination directory during copy','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('servers','List of servers in order of preference from which to copy','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','Reference source file from which to copy','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_349','0','promise_site_cf_349')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_349','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_343','0','promise_site_cf_343')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_343','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','Criteria for copying file from a source','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_diffs','Generate reports summarizing the major differences between individual text files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_diffs','Generate reports summarizing the major differences between individual text files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update_hashes','promises.cf.html#promise_site_cf_138','0','promise_site_cf_138')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update_hashes','promises.cf.html#promise_site_cf_138','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update_hashes','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update_hashes','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update_hashes','Update hash values immediately after change warning','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_changes','promises.cf.html#promise_site_cf_138','0','promise_site_cf_138')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_changes','promises.cf.html#promise_site_cf_138','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_changes','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_changes','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_changes','Specify criteria for change warnings','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','Hash files for change detection','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changes','promises.cf.html#promise_site_cf_138','0','promise_site_cf_138')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changes','promises.cf.html#promise_site_cf_138','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changes','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changes','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changes','Criteria for change management','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('specify_inherit_aces','Native settings for access control entry','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl_type','Access control list type for the affected file system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl_method','Editing method for access control list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl_directory_inherit','Access control list type for the affected file system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('aces','Native settings for access control entry','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl','Criteria for access control lists on file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('environment_type','Virtual environment type','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('environment_state','The desired dynamical state of the specified environment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_spec_file','The path to a file containing a technology specific set of promises for the virtual instance','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_baseline','The path to an image with which to baseline the virtual environment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_baseline','The path to an image with which to baseline the virtual environment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_disk','Amount of secondary storage (DISK) in the virtual environment (MB)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_disk','Amount of secondary storage (DISK) in the virtual environment (MB)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_memory','Amount of primary storage (RAM) in the virtual environment (KB)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_memory','Amount of primary storage (RAM) in the virtual environment (KB)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_cpus','Number of virtual CPUs in the environment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('environment_resources','Virtual environment resource description','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_network','The hostname of the virtual network','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_network','The hostname of the virtual network','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_name','The hostname of the virtual environment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_name','The hostname of the virtual environment','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('env_addresses','The IP addresses of the environment\'s network interfaces','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('environment_interface','Virtual environment outward identity and location','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('environment_host','The name of the virtual environment host (this must be promised uniquely)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('registry_exclude','A list of regular expressions to ignore in key/value verification','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_rows','An ordered list of row values to be promised by SQL databases','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_columns','A list of column definitions to be promised by SQL databases','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_operation','The nature of the promise - to be or not to be','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_type','The type of database that is to be manipulated','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_connection_db','The name of an existing database to connect to in order to create/manage other databases','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_type','The dialect of the database server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_type','The dialect of the database server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_host','Hostname or address for connection to database, blank means localhost','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_host','Hostname or address for connection to database, blank means localhost','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_password','Clear text password for database connection','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_password','Clear text password for database connection','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_owner','User name for database connection','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_server','Credentials for connecting to a local/remote database server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('module','true/false whether to expect the cfengine module protocol','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('no_output','true/false discard all output from the command','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('no_output','true/false discard all output from the command','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preview','true/false preview command when running in dry-run mode (with -n)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chroot','Directory of root sandbox for process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chdir','promises.cf.html#promise_knowledge_cf_56','0','promise_knowledge_cf_56')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chdir','promises.cf.html#promise_knowledge_cf_56','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chdir','Directory for setting current/base directory for the process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_timeout','Timeout in seconds for command completion','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_timeout','Timeout in seconds for command completion','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_group','The group name or id under which to run the process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_group','The group name or id under which to run the process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_owner','The user name or id under which to run the process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('umask','The umask value for the child process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('umask','The umask value for the child process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('useshell','true/false embed the command in a shell environment (true)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contain','promises.cf.html#promise_knowledge_cf_56','0','promise_knowledge_cf_56')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contain','promises.cf.html#promise_knowledge_cf_56','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contain','Containment options for the execution process','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('args','Alternative string of arguments for the command (concatenated with promiser string)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Retained comment about this promise\'s real intention','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','A comment about this promise\'s real intention that follows through the program','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depends_on','A list of promise handles that this promise builds on or depends on somehow (for knowledge management)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','A unique id-tag string for referring to this as a promisee elsewhere','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','Extended classes ANDed with context','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','promises.cf.html#promise_site_cf_123','0','promise_site_cf_123')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','promises.cf.html#promise_site_cf_123','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','Whether a persistent class restarts its counter when rediscovered','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','Whether a persistent class restarts its counter when rediscovered','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','promises.cf.html#promise_site_cf_123','0','promise_site_cf_123')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','promises.cf.html#promise_site_cf_123','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','A number of minutes the specified classes should remain active','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cancel_notkept','A list of classes to be cancelled if the promise is not kept for any reason','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cancel_repaired','A list of classes to be cancelled if the promise is repaired','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cancel_kept','A list of classes to be cancelled if the promise is kept','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_76','0','promise_webserver_cf_76')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_76','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','A list of classes to be defined globally','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_timeout','A list of classes to be defined globally','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_denied','A list of classes to be defined globally','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_failed','A list of classes to be defined globally','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_knowledge_cf_35','0','promise_knowledge_cf_35')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_knowledge_cf_35','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_76','0','promise_webserver_cf_76')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_76','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_64','0','promise_webserver_cf_64')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_64','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_site_cf_123','0','promise_site_cf_123')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_site_cf_123','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','A list of classes to be defined globally','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','Signalling behaviour','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('measurement_class','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('measurement_class','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('measurement_class','If set performance will be measured and recorded under this identifier','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_level','The reporting level for standard output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_level','The reporting level for standard output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_site_cf_177','0','promise_site_cf_177')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_site_cf_177','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#run_agent','0','run_agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#run_agent','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','true/false switch for parallelizing the promise repair','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','true/false switch for parallelizing the promise repair','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('audit','true/false switch for detailed audit records of this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('audit','true/false switch for detailed audit records of this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_notkept','A real number value (possibly negative) attributed to not keeping this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_notkept','A real number value (possibly negative) attributed to not keeping this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_repaired','A real number value attributed to reparing this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_repaired','A real number value attributed to reparing this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_kept','A real number value attributed to keeping this promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_failed','This should be filename of a file to which log_string will be saved, if undefined it goes to the system logger','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_repaired','This should be filename of a file to which log_string will be saved, if undefined it goes to the system logger','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_priority','The priority level of the log message, as interpreted by a syslog server','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_kept','This should be filename of a file to which log_string will be saved, if undefined it goes to the system logger','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_level','The reporting level sent to syslog','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_level','The reporting level sent to syslog','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_string','A message to be written to the log when a promise verification leads to a repair','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_string','A message to be written to the log when a promise verification leads to a repair','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_177','0','promise_site_cf_177')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_177','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#run_agent','0','run_agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#run_agent','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','Number of minutes before a repair action is interrupted and retried','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','Number of minutes before a repair action is interrupted and retried','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_knowledge_cf_43','0','promise_knowledge_cf_43')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_knowledge_cf_43','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_library_cf_171','0','promise_library_cf_171')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_library_cf_171','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_177','0','promise_site_cf_177')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_177','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_74','0','promise_site_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#run_agent','0','run_agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#run_agent','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','Number of minutes before next allowed assessment of promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_library_cf_171','0','promise_library_cf_171')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_library_cf_171','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_site_cf_81','0','promise_site_cf_81')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_site_cf_81','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','Whether to repair or report about non-kept promises','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','Output behaviour','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_401','0','promise_site_cf_401')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_401','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_394','0','promise_site_cf_394')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_394','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_387','0','promise_site_cf_387')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_387','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_380','0','promise_site_cf_380')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_380','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_373','0','promise_site_cf_373')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_373','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_366','0','promise_site_cf_366')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_366','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_359','0','promise_site_cf_359')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_359','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','List of services about which status reports should be reported to standard output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_to_file','The path and filename to which output should be appended','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('number_of_lines','Integer maximum number of lines to print from selected file','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_to_print','Path name to the file that is to be sent to standard output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('printfile','Quote part of a file to standard output','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastseen','Integer time threshold in hours since current peers were last seen, report absence','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('intermittency','Real number threshold [0,1] of intermittency about current peers, report above','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('friend_pattern','Regular expression to keep selected hosts from the friends report list','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('not','Evaluate the negation of string expression in normal form','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expression','Evaluate string expression of classes in normal form','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dist','Generate a probabilistic class distribution (strategy in cfengine 2)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('xor','Combine class sources with XOR','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('and','Combine class sources with AND','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('or','Combine class sources with inclusive OR','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('policy','The policy for (dis)allowing (re)definition of variables','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rlist','A list of real numbers','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ilist','A list of integers','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','A list of scalar strings','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real','A scalar real number','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('int','A scalar integer','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','A scalar string','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sys','cfengine\'s internal bundle of system specific values','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Represent type string and should match .*','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Represent type slist and should match .*','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Represent type real and should match ','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Represent type slist and should match ','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Represent type string and should match ','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Should match the generic pattern .*, i.e. unspecified characters','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('an_arbitrary_string','Should match the generic pattern , i.e. unspecified characters','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_syslog_level','Should match the generic pattern LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7, i.e. a syslog constant','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_file_path','Should match the generic pattern [cC]:\\.*|/.*, i.e. a system file path suitable for the target system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_user_group_id','Represent type slist and should match [a-zA-Z0-9_$.-]+','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_user_group_id','Should match the generic pattern [a-zA-Z0-9_$.-]+, i.e. an alphanumeric string with option underscores and hyphens','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_cfengine_identifier','Should match the generic pattern [a-zA-Z0-9_$.]+, i.e. an alphanumeric string with option underscores','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_cfengine_class_expression','Should match the generic pattern [a-zA-Z0-9_!&|.()]+, i.e. an alphanumeric string with option underscores and logical operators','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('posix_file_mode_or_permission','Represent type string and should match [0-7augorwxst,+-]+','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('posix_file_mode_or_permission','Represent type slist and should match [0-7augorwxst,+-]+','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('posix_file_mode_or_permission','Should match the generic pattern [0-7augorwxst,+-]+, i.e. something that you would give as an argument to chmod','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_single_character','Represent type string and should match ^.$','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_single_character','Should match the generic pattern ^.$, i.e. one symbol','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real_number','Represent type rlist and should match -9.99999E100,9.99999E100','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real_number','Represent type real and should match -9.99999E100,9.99999E100','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real_number','Should match the generic pattern -9.99999E100,9.99999E100, i.e. a number between the minus and positive maximum values','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integer','Represent type ilist and should match -99999999999,9999999999','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integer','Represent type int and should match -99999999999,9999999999','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integer','Should match the generic pattern -99999999999,9999999999, i.e. a number between the minus and positive maximum values','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_positive_integer','Represent type irange [int,int] and should match 0,99999999999','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_positive_integer','Represent type int and should match 0,99999999999','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_positive_integer','Should match the generic pattern 0,99999999999, i.e. a number between zero and the maximum value','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_time_range','Should match the generic pattern 0,2147483648, i.e. a value from zero to a maximum system time -- but you should use time functions to convert this','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Represent type (menu option) and should match symlink,hardlink,relative,absolute,none','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Should match the generic pattern symlink,hardlink,relative,absolute,none, i.e. a support link type','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('boolean','Represent type (menu option) and should match true,false,yes,no,on,off','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('boolean','Should match the generic pattern true,false,yes,no,on,off, i.e. a positive or a negative','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_signals','Represent type (option list) and should match hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_signals','Should match the generic pattern hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv, i.e. a unix signal name','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('values','Formal rvalues in constraint assignments and their legal ranges','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('functions','In-built functions that may be used to set variables or classes','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('body_constraints','The attributes that cfengine enables you to promise about the promiser','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_types','The types of promise that cfengine can keep','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','Complete occurrences of promiser + promisee + promise-body','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promisers','The objects affected by a promise','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promisees','Recipients of a promise, i.e. promise handles, or persons','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contexts','Class expressions that say where or when a promise applies','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bodies','A modular collection of body constraints for re-use','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bundles','A modular collection of promises of different types','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('available_patches_report','Patches already installed on system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('installed_patches_report','For systems with a separate patch model, this shows OS software patches. There are two reports (for current status and patches that are available but not already applied)','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('installed_patches_report','Patches not yet installed, but published by vendor','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('variables_report','Shows the values of variables used on each host. Some discovered, some defined.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('variables_report','Current variable values expanded on different hosts','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_report','Value estimate / ROI of cfengine configuration','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('setuid_report','List the discovered setuid/setgid root programs on the system (Unix only)','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('setuid_report','Known setuid programs found on system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises_not_kept_report','Show the log of recently failed or unkept promises. For debugging','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises_not_kept_report','Promises that were recently unkept','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises_repaired_report','Show the log of recently repaired promises. For debugging','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises_repaired_report','Promises that were recently repaired','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_report','Show the status of named policy promises, along with average compliance. Useful for quick auditing.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_report','Per-promise average compliance report','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('performance_report','Describes the real time it has taken on average to complete or keep promises','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('performance_report','Time cost of verifying system promises','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_summary_report','reports/allhosts.html','0','portal')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_summary_report','reports/host_portal.html','0','index portal')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_summary_report','Pseudo-real-time measurement','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('license_report','reports/license_report.html','0','site usage')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('license_report','License utilization statistics','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastseen_report','When was the last time the current host contacted or was contacted by another? (+) indicates contacted, (-) indictaes was contacted by.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastseen_report','Time and frequency of communications with peers','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('installed_software_report','OS software packages installed and their version information.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('installed_software_report','Software already installed on system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hashes_report','A list of actual computed hash values for promised files.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hashes_report','File hash values measured (change detection)','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_diffs_report','A log of diffs of files promised under change monitoring','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_diffs_report','Latest observed differences to system files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_changes_report','A log of changes to files promised under change monitoring','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_changes_report','Latest observed changes to system files','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compliance_report','Show the status of all promises, along with average compliance. Useful for quick auditing.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compliance_report','Total summary of host compliance','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes_report','Shows classes defined on the current host. This provides something like CMDB information but at level that can say something about the dynamics of the system. Probabilities that classes have been defined are based on actual observation.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes_report','User defined classes observed on the system','4','About')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('micro_audit_report','Detailed reports with every transaction made by cfengine - very storage consuming','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('micro_audit_report','Generated by cfengine self-auditing','4','About')
