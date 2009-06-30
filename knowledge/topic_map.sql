# CREATE DATABASE cf_topic_map
# USE _topic_map
CREATE TABLE topics(topic_name varchar(256),topic_comment varchar(1024),CREATE TABLE associations(from_name varchar(256),from_type varchar(256),from_assoc varchar(256),to_assoc varchar(256),to_type varchar(256),to_name varchar(256));
CREATE TABLE occurrences(topic_name varchar(256),locator varchar(1024),locator_type varchar(256),subtype varchar(256));
delete from topics
delete from associations
delete from occurrences
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Service Level Agreements','Service_Level_Agreements','Contracts_and_Agreements')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('DB','DB','Services','Relational database service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('LDAP','LDAP','Services','Directory Service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('NFS','NFS','Services','Network File System (storage)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('DNS','DNS','Services','Domain Name Service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('WWW','WWW','Services','Web service')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('Cfengine','Cfengine','Services','Cfengine automation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_41','promise_knowledge_cf_41','promises','Generate knowledge associative graphs')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/cfknowledgebase/enterprise.cf -g','_usr_local_sbin_cf_know__f__srv_www_htdocs_cfknowledgebase_enterprise_cf__g','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('web_server','web_server','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_34','promise_knowledge_cf_34','promises','Generate config reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/local/sbin/cf-promises -r','_usr_local_sbin_cf_promises__r','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_24','promise_knowledge_cf_24','promises','Make sure knowledge files are accessible to webserver')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/srv/www/htdocs/cfknowledgebase','_srv_www_htdocs_cfknowledgebase','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_knowledge_cf_12','promise_knowledge_cf_12','promises','(Uncommented promise of type <i>vars</i> made by: doc_root..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('doc_root','doc_root','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('setup_knowledge','setup_knowledge','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_135','promise_webserver_cf_135','promises','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_128','promise_webserver_cf_128','promises','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('APACHE_MODULES=.*','APACHE_MODULES___','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_115','promise_webserver_cf_115','promises','(Uncommented promise of type <i>insert_lines</i> made by: APACHE_CONF_INCLUDE_FILES..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('APACHE_CONF_INCLUDE_FILES="/home/mark/cfengine-inputs/httpd.conf"','APACHE_CONF_INCLUDE_FILES___home_mark_cfengine_inputs_httpd_conf_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_106','promise_webserver_cf_106','promises','(Uncommented promise of type <i>vars</i> made by: del_modules..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('del_modules','del_modules','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_102','promise_webserver_cf_102','promises','(Uncommented promise of type <i>vars</i> made by: add_modules..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('add_modules','add_modules','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('fixapache','fixapache','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_83','promise_webserver_cf_83','promises','(Uncommented promise of type <i>reports</i> made by: The web server software c..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('The web server software could not be installed','The_web_server_software_could_not_be_installed','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!software_ok.on','_software_ok_on','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_74','promise_webserver_cf_74','promises','(Uncommented promise of type <i>files</i> made by: /etc/sysconfig/apache2..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/sysconfig/apache2','_etc_sysconfig_apache2','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('software_ok','software_ok','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('php5','php5','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('apache2-prefork','apache2_prefork','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('apache2-mod_php5','apache2_mod_php5','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_webserver_cf_62','promise_webserver_cf_62','promises','(Uncommented promise of type <i>packages</i> made by: apache2..)')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_200','promise_library_cf_200','promises','(Uncommented promise of type <i>reports</i> made by: Added a 5 minute schedule..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Added a 5 minute schedule to crontabs','Added_a_5_minute_schedule_to_crontabs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_194','promise_library_cf_194','promises','(Uncommented promise of type <i>replace_patterns</i> made by: cfexecd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfexecd','cfexecd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_190','promise_library_cf_190','promises','(Uncommented promise of type <i>insert_lines</i> made by: 0,5,10,15,20,25,30,35,40,..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','0_5_10_15_20_25_30_35_40_45_50_55__________var_cfengine_bin_cf_execd__F','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('upgrade_cfexecd','upgrade_cfexecd','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_165','promise_library_cf_165','promises','(Uncommented promise of type <i>delete_lines</i> made by: $(regex)..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('$(regex)','__regex_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('DeleteLinesMatching','DeleteLinesMatching','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_113','promise_library_cf_113','promises','(Uncommented promise of type <i>field_edits</i> made by: NETCONFIG_DNS_STATIC_SEAR..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','NETCONFIG_DNS_STATIC_SEARCHLIST__','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_library_cf_105','promise_library_cf_105','promises','(Uncommented promise of type <i>vars</i> made by: search..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('search','search','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('resolver','resolver','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_366','promise_site_cf_366','promises','(Uncommented promise of type <i>roles</i> made by: .*..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('.*','__','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_362','promise_site_cf_362','promises','(Uncommented promise of type <i>access</i> made by: /home/mark/.cfagent/bin/c..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/home/mark/.cfagent/bin/cf-agent','_home_mark__cfagent_bin_cf_agent','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_358','promise_site_cf_358','promises','(Uncommented promise of type <i>access</i> made by: /home/mark/LapTop..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/home/mark/LapTop','_home_mark_LapTop','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('access_rules','access_rules','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_345','promise_site_cf_345','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous low entropy (3d..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','Anomalous_low_entropy__3dev__incoming_ICMP_traffic_on___mon_host__at_Tue_Jun_30_12_46_08_2009________measured_value_0_av___mon_average_icmp_in__pm___mon_stddev_icmp_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('anomaly_hosts.icmp_in_high_anomaly.!entropy_icmp_in_high','anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_338','promise_site_cf_338','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous (2dev) incoming..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','Anomalous__2dev__incoming__non_DNS__UDP_traffic_on___mon_host__at_Tue_Jun_30_12_46_08_2009________measured_value_0_av___mon_average_udp_in__pm___mon_stddev_udp_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_dns_in_low.anomaly_hosts.udp_in_high_dev2','entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_331','promise_site_cf_331','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous (3dev) incoming..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(average_dns_in) pm $(mon.stddev_dns_in)','Anomalous__3dev__incoming_DNS_packets_on___mon_host__at_Tue_Jun_30_12_46_08_2009________measured_value_0_av___average_dns_in__pm___mon_stddev_dns_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_dns_in_low.anomaly_hosts.dns_in_high_anomaly','entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_324','promise_site_cf_324','promises','(Uncommented promise of type <i>reports</i> made by: Anomalous number of new T..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Anomalous number of new TCP connections on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','Anomalous_number_of_new_TCP_connections_on___mon_host__at_Tue_Jun_30_12_46_08_2009________measured_value_0_av___mon_average_tcpsyn_in__pm___mon_stddev_tcpsyn_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_tcpsyn_in_low.anomaly_hosts.tcpsyn_in_high_dev2','entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_317','promise_site_cf_317','promises','(Uncommented promise of type <i>reports</i> made by: LOW ENTROPY Incoming www ..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009
     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','LOW_ENTROPY_Incoming_www_anomaly_high_anomaly_dev___on___mon_host__at_Tue_Jun_30_12_46_08_2009________measured_value___svalue_www_in__av___average_www_in__pm___stddev_www_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_www_in_low.anomaly_hosts.www_in_high_anomaly','entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_310','promise_site_cf_310','promises','(Uncommented promise of type <i>reports</i> made by: HIGH ENTROPY Incoming www..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_www_in) pm $(mon.stddev_www_in)','HIGH_ENTROPY_Incoming_www_anomaly_high_anomaly_dev___on___mon_host__at_Tue_Jun_30_12_46_08_2009________measured_value_0_av___mon_average_www_in__pm___mon_stddev_www_in_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('entropy_www_in_high&anomaly_hosts.www_in_high_anomaly','entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_303','promise_site_cf_303','promises','(Uncommented promise of type <i>reports</i> made by: RootProc anomaly high 2 d..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('RootProc anomaly high 2 dev on $(mon.host) at Tue Jun 30 12:46:08 2009 
    measured value 100 av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','RootProc_anomaly_high_2_dev_on___mon_host__at_Tue_Jun_30_12_46_08_2009______measured_value_100_av___mon_average_rootprocs__pm___mon_stddev_rootprocs_','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('rootprocs_high_dev2','rootprocs_high_dev2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/tmp/shadow','_tmp_shadow','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_293','promise_site_cf_293','promises','(Uncommented promise of type <i>files</i> made by: /tmp/passwd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/tmp/passwd','_tmp_passwd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs/shadow','_var_cfengine_inputs_shadow','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_287','promise_site_cf_287','promises','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs/pass..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs/passwd','_var_cfengine_inputs_passwd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_275','promise_site_cf_275','promises','(Uncommented promise of type <i>vars</i> made by: files..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_274','promise_site_cf_274','promises','(Uncommented promise of type <i>vars</i> made by: sysdir..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('sysdir','sysdir','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('anomalies','anomalies','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('free_disk_watch','free_disk_watch','promises','(Uncommented promise of type <i>measurements</i> made by: /bin/df..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/bin/df','_bin_df','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('free_memory_watch','free_memory_watch','promises','(Uncommented promise of type <i>measurements</i> made by: /proc/meminfo..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/proc/meminfo','_proc_meminfo','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('linux','linux','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('monitor_self_watch','monitor_self_watch','promises','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_ro..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/state/cf_rootprocs','_var_cfengine_state_cf_rootprocs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('atlas','atlas','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('watch','watch','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_180','promise_site_cf_180','promises','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf\..*..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/resolv.conf\..*','_etc_resolv_conf____','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_174','promise_site_cf_174','promises','(Uncommented promise of type <i>files</i> made by: /var/cfengine/outputs..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/outputs','_var_cfengine_outputs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('garbage_collection','garbage_collection','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_157','promise_site_cf_157','promises','Install patches')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('no match','no_match','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_148','promise_site_cf_148','promises','Update the locate database')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/bin/updatedb --prunepaths=/windows','_usr_bin_updatedb___prunepaths__windows','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Hr12','Hr12','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_141','promise_site_cf_141','promises','(Uncommented promise of type <i>commands</i> made by: /usr/bin/zypper -non-inte..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/usr/bin/zypper -non-interactive patch','_usr_bin_zypper__non_interactive_patch','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_139','promise_site_cf_139','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_131','promise_site_cf_131','promises','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_120','promise_site_cf_120','promises','(Uncommented promise of type <i>processes</i> made by: cfenvd..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfenvd','cfenvd','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_106','promise_site_cf_106','promises','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/etc/resolv.conf','_etc_resolv_conf','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_98','promise_site_cf_98','promises','(Uncommented promise of type <i>reports</i> made by: Laptop wakes up..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('Laptop wakes up','Laptop_wakes_up','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!up_and_running','_up_and_running','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_90','promise_site_cf_90','promises','(Uncommented promise of type <i>vars</i> made by: component..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('component','component','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('main','main','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_77','promise_site_cf_77','promises','(Uncommented promise of type <i>processes</i> made by: cron..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cron','cron','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('exec_fix','exec_fix','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_70','promise_site_cf_70','promises','This is inaccessible in new suse after cd opened/closed')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/dev/sr0','_dev_sr0','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_66','promise_site_cf_66','promises','(Uncommented promise of type <i>files</i> made by: /var/spool/cron/tabs/root..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/spool/cron/tabs/root','_var_spool_cron_tabs_root','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_58','promise_site_cf_58','promises','Check if there are still promises about cfengine 2 that need removing')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs/.*','_var_cfengine_inputs___','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_48','promise_site_cf_48','promises','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cfagent..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/bin/cfagent','_var_cfengine_bin_cfagent','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('integrate_cfengine2','integrate_cfengine2','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_40','promise_site_cf_40','promises','(Uncommented promise of type <i>vars</i> made by: cf2bits..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cf2bits','cf2bits','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cfengine','cfengine','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_site_cf_17','promise_site_cf_17','promises','(Uncommented promise of type <i>vars</i> made by: crontab..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('!SuSE','_SuSE','contexts')
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
INSERT INTO topics (topic_name,topic_id,topic_type) values ('/var/cfengine/inputs','_var_cfengine_inputs','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_update_cf_7','promise_update_cf_7','promises','(Uncommented promise of type <i>vars</i> made by: master_location..)')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('master_location','master_location','promisers')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('any','any','contexts')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('update','update','bundles')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('userexists','userexists','functions','True if user name or numerical id exists on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('usemodule','usemodule','functions','Execute cfengine module script and set class if successful')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('strcmp','strcmp','functions','True if the two strings match exactly')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('splitstring','splitstring','functions','Convert a string in arg1 into a list of max arg3 strings by splitting on a regular expression in arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('splayclass','splayclass','functions','True if the first argument's time-slot has arrived, according to a policy in arg2')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('selectservers','selectservers','functions','Select tcp servers which respond correctly to a query and return their number, set array of names')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rrange','rrange','functions','Define a range of real numbers for cfengine internal use')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('returnszero','returnszero','functions','True if named shell command has exit status zero')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('remotescalar','remotescalar','functions','Read a scalar value from a remote cfengine server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regldap','regldap','functions','True if arg6 is a regular expression matching a value item in an ldap search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('reglist','reglist','functions','True if arg2 matches any item in the list with id=arg1')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regline','regline','functions','True if arg2 is a regular expression matching a line in file arg1')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('registryvalue','registryvalue','functions','Returns a value for an MS-Win registry key,value pair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('regcmp','regcmp','functions','True if arg2 is a regular expression matching arg1')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isvariable','isvariable','functions','True if the named variable is defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isplain','isplain','functions','True if the named object is a plain/regular file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isnewerthan','isnewerthan','functions','True if arg1 is newer (modified later) than arg2 (mtime)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('islink','islink','functions','True if the named object is a symbolic link')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('islessthan','islessthan','functions','True if arg1 is numerically less than arg2, else compare strings like NOT strcmp')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isgreaterthan','isgreaterthan','functions','True if arg1 is numerically greater than arg2, else compare strings like strcmp')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('isdir','isdir','functions','True if the named object is a directory')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('irange','irange','functions','Define a range of integer values for cfengine internal use')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('iprange','iprange','functions','True if the current host lies in the range of IP addresses specified')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hostinnetgroup','hostinnetgroup','functions','True if the current host is in the named netgroup')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hostrange','hostrange','functions','True if the current host lies in the range of enumerated hostnames specified')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hashmatch','hashmatch','functions','Compute the hash of arg1, of type arg2 and test if it matches the value in arg 3')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hash','hash','functions','Return the hash of arg1, type arg2 and assign to a variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('groupexists','groupexists','functions','True if group or numerical id exists on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getuid','getuid','functions','Return the integer user id of the named user on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getgid','getgid','functions','Return the integer group id of the named group on this host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('getindices','getindices','functions','Get a list of keys to the array whose id is the argument and assign to variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('filesexist','filesexist','functions','True if the named list of files can ALL be accessed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('fileexists','fileexists','functions','True if the named file can be accessed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('execresult','execresult','functions','Execute named command and assign output to variable')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classmatch','classmatch','functions','True if the regular expression matches any currently defined class')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classify','classify','functions','True if the canonicalization of the argument is a currently defined class')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('changedbefore','changedbefore','functions','True if arg1 was changed before arg2 (ctime)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('canonify','canonify','functions','Convert an abitrary string into a legal class name')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ago','ago','functions','Convert a time relative to now to an integer system representation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('accumulated','accumulated','functions','Convert an accumulated amount of time into a system representation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('accessedbefore','accessedbefore','functions','True if arg1 was accessed before arg2 (atime)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('class','class','data_types','A boolean returned by certain functions in classes promises')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('policy','policy','data_types','The policy for (dis)allowing redefinition of variables matching free,overridable,constant')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('resource_type','resource_type','body_constraints','The type of object being granted access. The default is access to files.')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_fstab','edit_fstab','body_constraints','true/false add or remove entries to the file system table ("fstab")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_fstab','edit_fstab','values','true/false add or remove entries to the file system table ("fstab")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_options','mount_options','body_constraints','List of option strings to add to the file system table ("fstab")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_options','mount_options','values','List of option strings to add to the file system table ("fstab")')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_server','mount_server','body_constraints','Hostname or IP or remote file system server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_server','mount_server','values','Hostname or IP or remote file system server')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_source','mount_source','body_constraints','Path of remote file system to mount')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount_type','mount_type','body_constraints','Protocol type of remote file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mount','mount','body_constraints','Criteria for mounting foreign file systems')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('storage','storage','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('restart_class','restart_class','body_constraints','A class to be set if the process is not running, so that a command: rule can be referred to restart the process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_result','process_result','body_constraints','Boolean class expression returning the logical combination of classes set by a process selection test')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_result','process_result','values','Boolean class expression returning the logical combination of classes set by a process selection test')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('threads','threads','body_constraints','Range of integers matching the threads (NLWP) field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('priority','priority','body_constraints','Range of integers matching the priority field (PRI/NI) of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('priority','priority','values','Range of integers matching the priority field (PRI/NI) of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('tty','tty','body_constraints','Regular expression matching the tty field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('tty','tty','values','Regular expression matching the tty field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('command','command','body_constraints','Regular expression matching the command/cmd field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stime_range','stime_range','body_constraints','Range of integers matching the start time of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ttime_range','ttime_range','body_constraints','Range of integers matching the total elapsed time of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ttime_range','ttime_range','values','Range of integers matching the total elapsed time of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('status','status','body_constraints','Regular expression matching the status field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('status','status','values','Regular expression matching the status field of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('vsize','vsize','body_constraints','Range of integers matching the virtual memory size of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('vsize','vsize','values','Range of integers matching the virtual memory size of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rsize','rsize','body_constraints','Range of integers matching the resident memory size of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rsize','rsize','values','Range of integers matching the resident memory size of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pgid','pgid','body_constraints','Range of integers matching the parent group id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pgid','pgid','values','Range of integers matching the parent group id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ppid','ppid','body_constraints','Range of integers matching the parent process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ppid','ppid','values','Range of integers matching the parent process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pid','pid','body_constraints','Range of integers matching the process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pid','pid','values','Range of integers matching the process id of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_owner','process_owner','body_constraints','List of regexes matching the user of a process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_select','process_select','body_constraints','Criteria for matching processes in the system process table')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('out_of_range_define','out_of_range_define','body_constraints','List of classes to define if the matches are out of range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('out_of_range_define','out_of_range_define','values','List of classes to define if the matches are out of range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('in_range_define','in_range_define','body_constraints','List of classes to define if the matches are in range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('in_range_define','in_range_define','values','List of classes to define if the matches are in range')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('match_range','match_range','body_constraints','Integer range for acceptable number of matches for this process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_count','process_count','body_constraints','Criteria for constraining the number of processes matching other criteria')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('process_stop','process_stop','body_constraints','A command used to stop a running process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('signals','signals','body_constraints','A list of menu options representing signals to be sent to a process')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('processes','processes','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_select','package_select','body_constraints','A criterion for first acceptable match relative to "package_version"')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_architectures','package_architectures','body_constraints','Select the architecture for package selection')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_version','package_version','body_constraints','Version reference point for determining promised version')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_convention','package_name_convention','body_constraints','This is how the package manager expects the file to be referred to, e.g. $(name).$(arch)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_convention','package_name_convention','values','This is how the package manager expects the file to be referred to, e.g. $(name).$(arch)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_returncode','package_noverify_returncode','body_constraints','Integer return code indicating package verification failure')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_returncode','package_noverify_returncode','values','Integer return code indicating package verification failure')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_regex','package_noverify_regex','body_constraints','Regular expression to match verification failure output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_noverify_regex','package_noverify_regex','values','Regular expression to match verification failure output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_verify_command','package_verify_command','body_constraints','Command to verify the correctness of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_verify_command','package_verify_command','values','Command to verify the correctness of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_command','package_patch_command','body_constraints','Command to update to the latest patch release of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_command','package_patch_command','values','Command to update to the latest patch release of an installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_update_command','package_update_command','body_constraints','Command to update to the latest version a currently installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_update_command','package_update_command','values','Command to update to the latest version a currently installed package')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_delete_command','package_delete_command','body_constraints','Command to remove a package from the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_delete_command','package_delete_command','values','Command to remove a package from the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_add_command','package_add_command','body_constraints','Command to install a package to the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_add_command','package_add_command','values','Command to install a package to the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_installed_regex','package_installed_regex','body_constraints','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_installed_regex','package_installed_regex','values','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_arch_regex','package_arch_regex','body_constraints','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_arch_regex','package_arch_regex','values','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_regex','package_name_regex','body_constraints','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_name_regex','package_name_regex','values','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_version_regex','package_version_regex','body_constraints','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_version_regex','package_version_regex','values','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_installed_regex','package_patch_installed_regex','body_constraints','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_installed_regex','package_patch_installed_regex','values','Regular expression which matches packages that are already installed')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_arch_regex','package_patch_arch_regex','body_constraints','Regular expression with one backreference to extract update architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_arch_regex','package_patch_arch_regex','values','Regular expression with one backreference to extract update architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_name_regex','package_patch_name_regex','body_constraints','Regular expression with one backreference to extract update name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_name_regex','package_patch_name_regex','values','Regular expression with one backreference to extract update name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_version_regex','package_patch_version_regex','body_constraints','Regular expression with one backreference to extract update version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_version_regex','package_patch_version_regex','values','Regular expression with one backreference to extract update version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_list_command','package_patch_list_command','body_constraints','Command to obtain a list of available patches or updates')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_patch_list_command','package_patch_list_command','values','Command to obtain a list of available patches or updates')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_arch_regex','package_list_arch_regex','body_constraints','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_arch_regex','package_list_arch_regex','values','Regular expression with one backreference to extract package architecture string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_name_regex','package_list_name_regex','body_constraints','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_name_regex','package_list_name_regex','values','Regular expression with one backreference to extract package name string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_version_regex','package_list_version_regex','body_constraints','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_version_regex','package_list_version_regex','values','Regular expression with one backreference to extract package version string')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_command','package_list_command','body_constraints','Command to obtain a list of installed packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_list_command','package_list_command','values','Command to obtain a list of installed packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_file_repositories','package_file_repositories','body_constraints','A list of machine-local directories to search for packages')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_changes','package_changes','body_constraints','Menu option - whether to group packages into a single aggregate command')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_method','package_method','body_constraints','Criteria for installation and verification')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('package_policy','package_policy','body_constraints','Criteria for package installation/upgrade on the current system')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('packages','packages','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('usebundle','usebundle','body_constraints','Specify the name of a bundle to run as a parameterized method')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('methods','methods','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ipv6_address','ipv6_address','body_constraints','IPv6 address for the interface')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ipv4_netmask','ipv4_netmask','body_constraints','Netmask for the interface')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ipv4_address','ipv4_address','body_constraints','IPv4 address for the interface')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('tcp_ip','tcp_ip','body_constraints','Interface tcp/ip properties')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('interfaces','interfaces','promise_types')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('not_matching','not_matching','body_constraints','true/false negate match criterion')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('delete_lines','delete_lines','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('occurrences','occurrences','body_constraints','Menu option to replace all occurrences or just first (NB the latter is non-convergent)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('occurrences','occurrences','values','Menu option to replace all occurrences or just first (NB the latter is non-convergent)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('replace_value','replace_value','body_constraints','Value used to replace regular expression matches in search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('replace_with','replace_with','body_constraints','Search-replace pattern')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('replace_patterns','replace_patterns','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('allow_blank_fields','allow_blank_fields','body_constraints','true/false allow blank fields in a line (do not purge)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('allow_blank_fields','allow_blank_fields','values','true/false allow blank fields in a line (do not purge)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('extend_fields','extend_fields','body_constraints','true/false add new fields at end of line if necessary to complete edit')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_operation','field_operation','body_constraints','Menu option policy for editing subfields')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_operation','field_operation','values','Menu option policy for editing subfields')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_value','field_value','body_constraints','Set field value to a fixed value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_value','field_value','values','Set field value to a fixed value')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_separator','value_separator','body_constraints','Character separator for subfields inside the selected field')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('value_separator','value_separator','values','Character separator for subfields inside the selected field')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_field','select_field','body_constraints','Integer index of the field required 1..n')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_field','select_field','values','Integer index of the field required 1..n')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('field_separator','field_separator','body_constraints','The regular expression used to separate fields in a line')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_field','edit_field','body_constraints','Edit line-based file as matrix of fields')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('field_edits','field_edits','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('expand_scalars','expand_scalars','body_constraints','Expand any unexpanded variables')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('insert_type','insert_type','body_constraints','Type of object the promiser string refers to (default literal)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('first_last','first_last','body_constraints','Menu option, choose first or last occurrence of match in file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('before_after','before_after','body_constraints','Menu option, point cursor before of after matched line')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('before_after','before_after','values','Menu option, point cursor before of after matched line')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_line_matching','select_line_matching','body_constraints','Regular expression for matching file line location')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('location','location','body_constraints','Specify where in a file an insertion will be made')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('insert_lines','insert_lines','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('specify_inherit_aces','specify_inherit_aces','body_constraints','Native settings for access control entry')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('aces','aces','body_constraints','Native settings for access control entry')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl_directory_inherit','acl_directory_inherit','body_constraints','Access control list type for the affected file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl_type','acl_type','body_constraints','Access control list type for the affected file system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl_method','acl_method','body_constraints','Editing method for access control list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('acl','acl','body_constraints','Criteria for access control lists on file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('pathtype','pathtype','body_constraints','Menu option for interpreting promiser file object')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('transformer','transformer','body_constraints','Shell command (with full path) used to transform current file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('move_obstructions','move_obstructions','body_constraints','true/false whether to move obstructions to file-object creation')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('create','create','body_constraints','true/false whether to create non-existing file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('touch','touch','body_constraints','true/false whether to touch time stamps on file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rmdeadlinks','rmdeadlinks','body_constraints','true/false remove links that point to nowhere')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rmdeadlinks','rmdeadlinks','values','true/false remove links that point to nowhere')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('traverse_links','traverse_links','body_constraints','true/false traverse symbolic links to directories (false)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('traverse_links','traverse_links','values','true/false traverse symbolic links to directories (false)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('xdev','xdev','body_constraints','true/false exclude directories that are on different devices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('xdev','xdev','values','true/false exclude directories that are on different devices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depth','depth','body_constraints','Maximum depth level for search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depth','depth','values','Maximum depth level for search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_basedir','include_basedir','body_constraints','true/false include the start/root dir of the search results')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_basedir','include_basedir','values','true/false include the start/root dir of the search results')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exclude_dirs','exclude_dirs','body_constraints','List of regexes of directory names NOT to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exclude_dirs','exclude_dirs','values','List of regexes of directory names NOT to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('include_dirs','include_dirs','body_constraints','List of regexes of directory names to include in depth search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depth_search','depth_search','body_constraints','Criteria for file depth searches')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('empty_file_before_editing','empty_file_before_editing','body_constraints','Baseline memory model of file to zero/empty before commencing promised edits')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('empty_file_before_editing','empty_file_before_editing','values','Baseline memory model of file to zero/empty before commencing promised edits')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('max_file_size','max_file_size','body_constraints','Do not edit files bigger than this number of bytes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_backup','edit_backup','body_constraints','Menu option for backup policy on edit changes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_defaults','edit_defaults','body_constraints','Default promise details for file edits')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_xml','edit_xml','body_constraints','XML editing model for file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('edit_line','edit_line','body_constraints','Line editing model for file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repository','repository','body_constraints','Name of a repository for versioning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_mode','disable_mode','body_constraints','The permissions to set when a file is disabled')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rotate','rotate','body_constraints','Maximum number of file rotations to keep')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rotate','rotate','values','Maximum number of file rotations to keep')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable','disable','body_constraints','true/false automatically rename and remove permissions')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable','disable','values','true/false automatically rename and remove permissions')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_suffix','disable_suffix','body_constraints','The suffix to add to files when disabling (.cfdisabled)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('disable_suffix','disable_suffix','values','The suffix to add to files when disabling (.cfdisabled)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('newname','newname','body_constraints','The desired name for the current file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rename','rename','body_constraints','Criteria for renaming files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rmdirs','rmdirs','body_constraints','true/false whether to delete empty directories during recursive deletion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('dirlinks','dirlinks','body_constraints','Menu option policy for dealing with symbolic links to directories during deletion')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('delete','delete','body_constraints','Criteria for deleting files')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('update_hashes','update_hashes','body_constraints','Update hash values immediately after change warning')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_changes','report_changes','body_constraints','Specify criteria for change warnings')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('hash','hash','body_constraints','Hash files for change detection')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('changes','changes','body_constraints','Criteria for change management')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('bsdflags','bsdflags','body_constraints','List of menu options for bsd file system flags to set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('bsdflags','bsdflags','values','List of menu options for bsd file system flags to set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rxdirs','rxdirs','body_constraints','true/false add execute flag for directories if read flag is set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rxdirs','rxdirs','values','true/false add execute flag for directories if read flag is set')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('groups','groups','body_constraints','List of acceptable groups of group ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('groups','groups','values','List of acceptable groups of group ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('owners','owners','body_constraints','List of acceptable owners or user ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('owners','owners','values','List of acceptable owners or user ids, first is change target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('mode','mode','body_constraints','File permissions (like posix chmod)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('perms','perms','body_constraints','Criteria for setting permissions on a file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_linking_children','when_linking_children','body_constraints','Policy for overriding existing files when linking directories of children')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_linking_children','when_linking_children','values','Policy for overriding existing files when linking directories of children')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_children','link_children','body_constraints','true/false whether to link all directory's children to source originals')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_no_source','when_no_source','body_constraints','Behaviour when the source file to link to does not exist')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('when_no_source','when_no_source','values','Behaviour when the source file to link to does not exist')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_patterns','copy_patterns','body_constraints','A set of patterns that should be copied ansd synchronized instead of linked')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_patterns','copy_patterns','values','A set of patterns that should be copied ansd synchronized instead of linked')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','body_constraints','The type of link used to alias the file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','values','The type of link used to alias the file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_from','link_from','body_constraints','Criteria for linking file from a source')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('findertype','findertype','body_constraints','Menu option for default finder type on MacOSX')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('findertype','findertype','values','Menu option for default finder type on MacOSX')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('check_root','check_root','body_constraints','true/false check permissions on the root directory when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('check_root','check_root','values','true/false check permissions on the root directory when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('purge','purge','body_constraints','true/false purge files on client that do not match files on server when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('purge','purge','values','true/false purge files on client that do not match files on server when depth_search')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('verify','verify','body_constraints','true/false verify transferred file by hashing after copy (resource penalty)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('verify','verify','values','true/false verify transferred file by hashing after copy (resource penalty)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('encrypt','encrypt','body_constraints','true/false use encrypted data stream to connect to remote host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('encrypt','encrypt','values','true/false use encrypted data stream to connect to remote host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('trustkey','trustkey','body_constraints','true/false trust public keys from remote server if previously unknown')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_size','copy_size','body_constraints','Integer range of file sizes that may be copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_size','copy_size','values','Integer range of file sizes that may be copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_ipv4','force_ipv4','body_constraints','true/false force use of ipv4 on ipv6 enabled network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_ipv4','force_ipv4','values','true/false force use of ipv4 on ipv6 enabled network')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_update','force_update','body_constraints','true/false force copy update always')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('force_update','force_update','values','true/false force copy update always')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('type_check','type_check','body_constraints','true/false compare file types before copying and require match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('type_check','type_check','values','true/false compare file types before copying and require match')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('link_type','link_type','body_constraints','Menu option for type of links to use when copying')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('compare','compare','body_constraints','Menu option policy for comparing source and image file attributes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('compare','compare','values','Menu option policy for comparing source and image file attributes')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copylink_patterns','copylink_patterns','body_constraints','List of patterns matching files that should be linked instead of copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copylink_patterns','copylink_patterns','values','List of patterns matching files that should be linked instead of copied')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('linkcopy_patterns','linkcopy_patterns','body_constraints','List of patterns matching symbolic links that should be replaced with copies')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('linkcopy_patterns','linkcopy_patterns','values','List of patterns matching symbolic links that should be replaced with copies')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('preserve','preserve','body_constraints','true/false whether to preserve file permissions on copied file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('preserve','preserve','values','true/false whether to preserve file permissions on copied file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('stealth','stealth','body_constraints','true/false whether to preserve time stamps on copied file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_backup','copy_backup','body_constraints','Menu option policy for file backup/version control')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('portnumber','portnumber','body_constraints','Port number to connect to on server host')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('servers','servers','body_constraints','List of servers in order of preference from which to copy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('servers','servers','values','List of servers in order of preference from which to copy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('source','source','body_constraints','Reference source file from which to copy')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copy_from','copy_from','body_constraints','Criteria for copying file from a source')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_result','file_result','body_constraints','Logical expression combining classes defined by file search criteria')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_result','file_result','values','Logical expression combining classes defined by file search criteria')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('issymlinkto','issymlinkto','body_constraints','List of regular expressions to match file objects')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_types','file_types','body_constraints','List of acceptable file types from menu choices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_types','file_types','values','List of acceptable file types from menu choices')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_program','exec_program','body_constraints','Execute this command on each file and match if the exit status is zero')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('exec_program','exec_program','values','Execute this command on each file and match if the exit status is zero')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('search_mode','search_mode','values','A list of mode masks for acceptable file permissions')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('path_name','path_name','body_constraints','List of pathnames to match acceptable target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('path_name','path_name','values','List of pathnames to match acceptable target')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('leaf_name','leaf_name','body_constraints','List of regexes that match an acceptable name')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_select','file_select','body_constraints','Choose which files select in a search')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('files','files','promise_types')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('preview','preview','values','true/false preview command when running in dry-run mode (with -n)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('chroot','chroot','body_constraints','Directory of root sandbox for process')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('chroot','chroot','values','Directory of root sandbox for process')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_end','select_end','body_constraints','Regular expression matches end of edit region from start')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_end','select_end','values','Regular expression matches end of edit region from start')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_start','select_start','body_constraints','Regular expression matching start of edit region')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('select_region','select_region','body_constraints','Limit edits to a demarked region of the file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('comment','comment','body_constraints','A comment about this promise's real intention that follows through the program')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('depends_on','depends_on','body_constraints','A list of promise handles that this promise builds on or depends on somehow (for knowledge management)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('handle','handle','body_constraints','A unique id-tag string for referring to this as a promisee elsewhere')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ifvarclass','ifvarclass','body_constraints','Extended classes ANDed with context')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('timer_policy','timer_policy','body_constraints','Whether a persistent class restarts its counter when rediscovered')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('timer_policy','timer_policy','values','Whether a persistent class restarts its counter when rediscovered')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('persist_time','persist_time','body_constraints','A number of minutes the specified classes should remain active')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('persist_time','persist_time','values','A number of minutes the specified classes should remain active')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_kept','promise_kept','body_constraints','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_kept','promise_kept','values','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_timeout','repair_timeout','body_constraints','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_timeout','repair_timeout','values','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_denied','repair_denied','body_constraints','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_denied','repair_denied','values','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_failed','repair_failed','body_constraints','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('repair_failed','repair_failed','values','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('promise_repaired','promise_repaired','body_constraints','A list of classes to be defined')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('classes','classes','body_constraints','Signalling behaviour')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('measurement_class','measurement_class','body_constraints','If set performance will be measured and recorded under this identifier')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_level','report_level','body_constraints','The reporting level for standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_level','report_level','values','The reporting level for standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('background','background','body_constraints','true/false switch for parallelizing the promise repair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('background','background','values','true/false switch for parallelizing the promise repair')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('audit','audit','body_constraints','true/false switch for detailed audit records of this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('audit','audit','values','true/false switch for detailed audit records of this promise')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_failed','log_failed','body_constraints','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_failed','log_failed','values','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_repaired','log_repaired','body_constraints','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_repaired','log_repaired','values','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('log_kept','log_kept','body_constraints','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('report_to_file','report_to_file','body_constraints','The path and filename to which output should be appended')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('friend_pattern','friend_pattern','body_constraints','Regular expression to keep selected hosts from the friends report list')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_to_print','file_to_print','body_constraints','Path name to the file that is to be sent to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('file_to_print','file_to_print','values','Path name to the file that is to be sent to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('number_of_lines','number_of_lines','body_constraints','Integer maximum number of lines to print from selected file')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('printfile','printfile','body_constraints','Quote part of a file to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('showstate','showstate','body_constraints','List of services about which status reports should be reported to standard output')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('intermittency','intermittency','body_constraints','Real number threshold [0,1] of intermittency about current peers, report above')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('lastseen','lastseen','body_constraints','Integer time threshold in hours since current peers were last seen, report absence')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('reports','reports','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('not','not','body_constraints','Evaluate the negation of string expression in normal form')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('expression','expression','body_constraints','Evaluate string expression of classes in normal form')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('dist','dist','body_constraints','Generate a probabilistic class distribution (strategy in cfengine 2)')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('xor','xor','body_constraints','Combine class sources with XOR')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('and','and','body_constraints','Combine class sources with AND')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('or','or','body_constraints','Combine class sources with inclusive OR')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('classes','classes','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('policy','policy','body_constraints','The policy for (dis)allowing redefinition of variables')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('rlist','rlist','body_constraints','A list of real numbers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('ilist','ilist','body_constraints','A list of integers')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('slist','slist','body_constraints','A list of scalar strings')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('real','real','body_constraints','A scalar real number')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('int','int','body_constraints','A scalar integer')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('string','string','body_constraints','A scalar string')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('vars','vars','promise_types')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('sys','sys','bundles','cfengine's internal bundle of system specific values')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('An arbitrary string','An_arbitrary_string','values','Should match the generic pattern , i.e. unspecified characters')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a syslog level','a_syslog_level','values','Should match the generic pattern LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7, i.e. a syslog constant')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('a file path','a_file_path','values','Should match the generic pattern [cC]:\.*|/.*, i.e. a system file path suitable for the target system')
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
INSERT INTO topics (topic_name,topic_id,topic_type) values ('variables report','variables_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('software patches report','software_patches_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('setuid report','setuid_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('performance report','performance_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('monitor summary report','monitor_summary_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('monitor history report','monitor_history_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('monitor now report','monitor_now_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('lastseen report','lastseen_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('installed software report','installed_software_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('hashes report','hashes_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('file_changes report','file_changes_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('compliance report','compliance_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('classes report','classes_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('active_locks report','active_locks_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('all_locks report','all_locks_report','system_reports')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('audit report','audit_report','system_reports')
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
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('subjects','subjects','promise_theory','Any thing that can be discussed')
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
INSERT INTO topics (topic_name,topic_id,topic_type) values ('bundles','bundles','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('intentions','intentions','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('agents','agents','promise_theory')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('convergence','convergence','convergent_operators')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('idempotence','idempotence','convergent_operators')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('overlapping sets','overlapping_sets','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('cascading','cascading','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('object paradigm','object_paradigm','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('aspect paradigm','aspect_paradigm','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('service orientation','service_orientation','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('hierarchical structure','hierarchical_structure','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('peer to peer network','peer_to_peer_network','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('heterogeneous system','heterogeneous_system','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('homogeneous system','homogeneous_system','models_of_organization')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('topic maps','topic_maps','models_of_knowledge')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('self-healing','self_healing','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('autonomics','autonomics','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('computer immunology','computer_immunology','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('convergent operators','convergent_operators','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('promise theory','promise_theory','models_of_system_administration')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('models of organization','models_of_organization','Research')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('models of knowledge','models_of_knowledge','Research')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('models of system administration','models_of_system_administration','Research')
INSERT INTO topics (topic_name,topic_id,topic_type) values ('ontology','ontology','cfengine_knowledge_base')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('copernicus','copernicus','cfengine_knowledge_base','Cfengine's online support knowledge base')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system reports','system_reports','cfengine_knowledge_base','Reports from around the system')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('system policy','system_policy','cfengine_knowledge_base','Local policy description with comments and dependencies')
INSERT INTO topics (topic_name,topic_id,topic_type,topic_comment) values ('cfengine knowledge base','cfengine_knowledge_base','any','Cfengine knowledge console')
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_41','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_41','web_server','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/cfknowledgebase/enterprise.cf -g','promise_knowledge_cf_41','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/cfknowledgebase/enterprise.cf -g','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-know -f /srv/www/htdocs/cfknowledgebase/enterprise.cf -g','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_34','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_34','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-promises -r','promise_knowledge_cf_34','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-promises -r','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/local/sbin/cf-promises -r','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_24','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_24','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/cfknowledgebase','promise_knowledge_cf_24','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/cfknowledgebase','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/srv/www/htdocs/cfknowledgebase','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_12','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_knowledge_cf_12','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('doc_root','promise_knowledge_cf_12','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('doc_root','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('doc_root','setup_knowledge','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_135','field_edits','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_135','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_128','field_edits','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_128','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','promise_webserver_cf_135','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','promise_webserver_cf_128','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','field_edits','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_MODULES=.*','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_115','insert_lines','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_115','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_CONF_INCLUDE_FILES="/home/mark/cfengine-inputs/httpd.conf"','promise_webserver_cf_115','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_CONF_INCLUDE_FILES="/home/mark/cfengine-inputs/httpd.conf"','insert_lines','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('APACHE_CONF_INCLUDE_FILES="/home/mark/cfengine-inputs/httpd.conf"','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_106','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_106','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('del_modules','promise_webserver_cf_106','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('del_modules','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('del_modules','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_102','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_102','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('add_modules','promise_webserver_cf_102','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('add_modules','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('add_modules','fixapache','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_83','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_83','!software_ok.on','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('The web server software could not be installed','promise_webserver_cf_83','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('The web server software could not be installed','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('The web server software could not be installed','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_74','promise_webserver_cf_74','has direct influence on','is directly influenced by','promises','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_74','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_74','software_ok','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','/etc/sysconfig/apache2','has direct influence on','is directly influenced by','promisers','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','promise_webserver_cf_74','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/sysconfig/apache2','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('software_ok','software_ok','is activated by class context','activates promises','contexts','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','software_ok','has direct influence on','is directly influenced by','promisers','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','promise_webserver_cf_62','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('php5','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','software_ok','has direct influence on','is directly influenced by','promisers','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','promise_webserver_cf_62','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-prefork','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','software_ok','has direct influence on','is directly influenced by','promisers','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','promise_webserver_cf_62','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2-mod_php5','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_62','software_ok','has direct influence on','is directly influenced by','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_62','packages','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_62','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_53','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_53','start_apache','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/apache2 restart','promise_webserver_cf_53','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/apache2 restart','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/init.d/apache2 restart','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_44','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_44','off','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_38','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_38','web_ok.on','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','software_ok','has direct influence on','is directly influenced by','promisers','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','promise_webserver_cf_62','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','promise_webserver_cf_44','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','promise_webserver_cf_38','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('apache2','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_23','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_23','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_package','promise_webserver_cf_23','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_package','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_package','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_17','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_17','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('site_http_conf','promise_webserver_cf_17','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('site_http_conf','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('site_http_conf','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_11','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_webserver_cf_11','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('document_root','promise_webserver_cf_11','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('document_root','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('document_root','web_server','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_31','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_31','!usb_backup_device_loaded','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/backup','promise_laptop_cf_31','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/backup','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/backup','laptop','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_22','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_22','usb_backup_device_loaded','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/media/disk-1/CfengineBackupDevice','promise_laptop_cf_22','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/media/disk-1/CfengineBackupDevice','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/media/disk-1/CfengineBackupDevice','laptop','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_6','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_laptop_cf_6','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_point','promise_laptop_cf_6','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_point','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_point','laptop','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_200','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_200','exec_fix','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Added a 5 minute schedule to crontabs','promise_library_cf_200','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Added a 5 minute schedule to crontabs','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Added a 5 minute schedule to crontabs','upgrade_cfexecd','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_194','replace_patterns','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_194','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','promise_library_cf_194','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','replace_patterns','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfexecd','upgrade_cfexecd','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_190','insert_lines','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_190','exec_fix','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','promise_library_cf_190','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','insert_lines','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F','upgrade_cfexecd','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_165','delete_lines','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_165','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(regex)','promise_library_cf_165','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(regex)','delete_lines','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('$(regex)','DeleteLinesMatching','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_113','field_edits','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_113','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','promise_library_cf_113','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','field_edits','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('NETCONFIG_DNS_STATIC_SEARCHLIST.*','resolver','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_105','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_library_cf_105','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search','promise_library_cf_105','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search','resolver','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_366','roles','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_366','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('.*','promise_site_cf_366','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('.*','roles','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('.*','access_rules','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_362','access','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_362','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/.cfagent/bin/cf-agent','promise_site_cf_362','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/.cfagent/bin/cf-agent','access','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/.cfagent/bin/cf-agent','access_rules','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_358','access','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_358','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/LapTop','promise_site_cf_358','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/LapTop','access','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/home/mark/LapTop','access_rules','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_345','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_345','anomaly_hosts.icmp_in_high_anomaly.!entropy_icmp_in_high','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','promise_site_cf_345','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_338','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_338','entropy_dns_in_low.anomaly_hosts.udp_in_high_dev2','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','promise_site_cf_338','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_udp_in) pm $(mon.stddev_udp_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_331','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_331','entropy_dns_in_low.anomaly_hosts.dns_in_high_anomaly','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(average_dns_in) pm $(mon.stddev_dns_in)','promise_site_cf_331','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(average_dns_in) pm $(mon.stddev_dns_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous (3dev) incoming DNS packets on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(average_dns_in) pm $(mon.stddev_dns_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_324','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_324','entropy_tcpsyn_in_low.anomaly_hosts.tcpsyn_in_high_dev2','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous number of new TCP connections on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','promise_site_cf_324','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous number of new TCP connections on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Anomalous number of new TCP connections on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_317','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_317','entropy_www_in_low.anomaly_hosts.www_in_high_anomaly','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009
     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','promise_site_cf_317','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009
     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009
     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_310','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_310','entropy_www_in_high&anomaly_hosts.www_in_high_anomaly','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_www_in) pm $(mon.stddev_www_in)','promise_site_cf_310','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_www_in) pm $(mon.stddev_www_in)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_www_in) pm $(mon.stddev_www_in)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_303','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_303','rootprocs_high_dev2','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('RootProc anomaly high 2 dev on $(mon.host) at Tue Jun 30 12:46:08 2009 
    measured value 100 av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','promise_site_cf_303','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('RootProc anomaly high 2 dev on $(mon.host) at Tue Jun 30 12:46:08 2009 
    measured value 100 av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('RootProc anomaly high 2 dev on $(mon.host) at Tue Jun 30 12:46:08 2009 
    measured value 100 av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/shadow','promise_site_cf_293','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/shadow','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/shadow','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_293','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_293','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/passwd','promise_site_cf_293','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/passwd','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/tmp/passwd','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/shadow','promise_site_cf_287','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/shadow','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/shadow','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_287','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_287','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','setup_knowledge','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','web_server','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','laptop','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','anomalies','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/passwd','promise_site_cf_287','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/passwd','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/passwd','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_275','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_275','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','promise_site_cf_275','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_274','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_274','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sysdir','promise_site_cf_274','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sysdir','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sysdir','anomalies','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_disk_watch','measurements','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_disk_watch','linux','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/bin/df','free_disk_watch','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/bin/df','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/bin/df','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_memory_watch','measurements','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('free_memory_watch','linux','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/proc/meminfo','free_memory_watch','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/proc/meminfo','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/proc/meminfo','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('monitor_self_watch','measurements','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('monitor_self_watch','atlas','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_rootprocs','monitor_self_watch','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_rootprocs','measurements','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/state/cf_rootprocs','watch','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_180','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_180','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf\..*','promise_site_cf_180','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf\..*','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf\..*','garbage_collection','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_174','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_174','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/outputs','promise_site_cf_174','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/outputs','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/outputs','garbage_collection','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_157','packages','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_157','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no match','promise_site_cf_157','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no match','packages','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no match','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_148','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_148','Hr12','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/updatedb --prunepaths=/windows','promise_site_cf_148','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/updatedb --prunepaths=/windows','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/updatedb --prunepaths=/windows','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_141','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_141','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/zypper -non-interactive patch','promise_site_cf_141','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/zypper -non-interactive patch','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/usr/bin/zypper -non-interactive patch','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_139','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_139','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_131','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_131','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_120','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_120','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvd','promise_site_cf_120','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvd','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cfenvd','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_106','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_106','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf','promise_site_cf_106','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/etc/resolv.conf','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_98','promise_site_cf_98','has direct influence on','is directly influenced by','promises','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_98','reports','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_98','!up_and_running','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','Laptop wakes up','has direct influence on','is directly influenced by','promisers','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','promise_site_cf_98','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','reports','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('Laptop wakes up','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('!up_and_running','!up_and_running','is activated by class context','activates promises','contexts','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_90','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_90','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('component','promise_site_cf_90','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('component','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('component','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_77','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_77','exec_fix','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cron','promise_site_cf_77','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cron','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cron','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_70','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_70','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/dev/sr0','promise_site_cf_70','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/dev/sr0','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/dev/sr0','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_66','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_66','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/spool/cron/tabs/root','promise_site_cf_66','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/spool/cron/tabs/root','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/spool/cron/tabs/root','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_58','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_58','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/.*','promise_site_cf_58','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/.*','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs/.*','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_48','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_48','integrate_cfengine2','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cfagent','promise_site_cf_48','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cfagent','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cfagent','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_40','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_40','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf2bits','promise_site_cf_40','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf2bits','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf2bits','cfengine','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_17','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_17','!SuSE','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_13','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_site_cf_13','SuSE','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','promise_site_cf_17','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','promise_site_cf_13','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('crontab','g','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_45','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_45','reload.start_exec','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-execd','promise_update_cf_45','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-execd','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-execd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_41','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_41','reload.start_monitor','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','promise_site_cf_139','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','promise_update_cf_41','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-monitord','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_37','commands','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_37','reload.start_server','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','promise_site_cf_139','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','promise_update_cf_37','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','commands','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin/cf-serverd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_31','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_31','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','promise_update_cf_31','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-execd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_30','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_30','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','promise_site_cf_131','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','promise_update_cf_30','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-monitord','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_29','processes','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_29','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','promise_site_cf_131','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','promise_update_cf_29','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','processes','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','main','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cf-serverd','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_21','promise_update_cf_21','has direct influence on','is directly influenced by','promises','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_21','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_21','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','/var/cfengine/bin','has direct influence on','is directly influenced by','promisers','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','promise_update_cf_21','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/bin','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_13','promise_update_cf_13','has direct influence on','is directly influenced by','promises','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_13','files','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_13','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','/var/cfengine/inputs','has direct influence on','is directly influenced by','promisers','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','promise_update_cf_13','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','files','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('/var/cfengine/inputs','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_7','vars','is a promise of type','has current exemplars','promises','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_update_cf_7','any','is activated by class context','activates promises','promises','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('master_location','promise_update_cf_7','makes promises','is a promise made by','promisers','promises');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('master_location','vars','makes promise of type','promises have been made by','promisers','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('master_location','update','occurs in bundle','bundle contains promiser','promisers','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('userexists','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('usemodule','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('strcmp','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('splitstring','slist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('splayclass','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('selectservers','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rrange','rrange','returns data-type','is returned by function','functions','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('returnszero','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('remotescalar','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regldap','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reglist','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regline','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registryvalue','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regcmp','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('regarray','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readtcp','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readstringlist','slist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readstringarray','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readreallist','rlist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readrealarray','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readintlist','ilist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readintarray','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('readfile','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('randomint','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('peerleaders','slist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('peerleader','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('peers','slist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('on','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('now','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ldapvalue','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ldaplist','slist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ldaparray','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastnode','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isvariable','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isplain','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isnewerthan','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('islink','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('islessthan','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isgreaterthan','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('isdir','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('irange','irange','returns data-type','is returned by function','functions','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('iprange','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hostinnetgroup','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hostrange','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hashmatch','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hash','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groupexists','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getuid','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getgid','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('getindices','slist','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('filesexist','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('fileexists','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('execresult','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classmatch','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classify','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('changedbefore','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('canonify','string','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ago','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('accumulated','int','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('accessedbefore','class','returns data-type','is returned by function','functions','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extraction_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extraction_regex','match_value','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_number','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_number','match_value','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_value','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('units','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('units','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('history_type','history_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('history_type','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('data_type','data_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('data_type','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stream_type','stream_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stream_type','measurements','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('measurements','watch','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_root','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_root','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('web_root','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('web_root','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('representation','representation','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('representation','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('represents','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('represents','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('represents','occurrences','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associates','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associates','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associates','association','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('backward_relationship','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('backward_relationship','association','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('forward_relationship','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('forward_relationship','association','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('association','topics','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('authorize','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('authorize','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('authorize','roles','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('roles','access_rules','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('resource_type','resource_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('resource_type','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifencrypted','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifencrypted','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('maproot','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('maproot','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('maproot','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deny','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deny','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('deny','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('admit','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('admit','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('admit','access','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('access','access_rules','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('scan_arrivals','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('scan_arrivals','volume','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_count','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_count','volume','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_size','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('sensible_size','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('freespace','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('freespace','volume','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_foreign','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_foreign','volume','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('volume','storage','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('unmount','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('unmount','mount','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_fstab','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_fstab','mount','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_options','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_options','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_options','mount','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_server','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_server','mount','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_source','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_source','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_type','mount_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount_type','mount','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mount','storage','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('restart_class','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('restart_class','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_result','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_result','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('threads','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('threads','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('priority','priority','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('priority','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('tty','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('tty','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('command','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('command','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stime_range','stime_range','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stime_range','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ttime_range','ttime_range','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ttime_range','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('status','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('status','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vsize','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vsize','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rsize','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rsize','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pgid','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pgid','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ppid','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ppid','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pid','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pid','process_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_owner','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_owner','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_owner','process_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_select','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('out_of_range_define','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('out_of_range_define','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('out_of_range_define','process_count','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('in_range_define','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('in_range_define','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('in_range_define','process_count','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_range','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('match_range','process_count','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_count','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_stop','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('process_stop','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('signals','system signals','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('signals','processes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','web_server','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','main','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','cfengine','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('processes','update','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_select','package_select','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_select','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_architectures','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_architectures','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_architectures','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_convention','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_convention','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_returncode','integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_returncode','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_noverify_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_verify_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_verify_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_update_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_update_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_delete_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_delete_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_add_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_add_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_installed_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_installed_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_arch_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_arch_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_name_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_version_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_installed_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_installed_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_arch_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_arch_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_name_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_name_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_version_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_version_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_list_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_patch_list_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_arch_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_arch_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_name_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_name_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_version_regex','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_version_regex','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_command','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_list_command','package_method','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_file_repositories','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_file_repositories','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_file_repositories','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_changes','package_changes','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_changes','package_method','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_method','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_policy','package_policy','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('package_policy','packages','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('packages','web_server','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('packages','main','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('usebundle','usebundle','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('usebundle','methods','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv6_address','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv6_address','tcp_ip','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_netmask','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_netmask','tcp_ip','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_address','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ipv4_address','tcp_ip','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('tcp_ip','interfaces','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_contains_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_contains_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_contains_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_contains_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_contains_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_contains_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_match_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_match_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_match_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_match_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_match_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_match_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_startwith_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_startwith_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_not_startwith_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_startwith_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_startwith_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_if_startwith_from_list','delete_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_select','delete_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not_matching','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not_matching','delete_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete_lines','DeleteLinesMatching','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrences','occurrences','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrences','replace_with','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_value','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_value','replace_with','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_with','replace_patterns','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('replace_patterns','upgrade_cfexecd','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('allow_blank_fields','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('allow_blank_fields','edit_field','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extend_fields','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('extend_fields','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_operation','field_operation','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_operation','edit_field','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_value','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_value','edit_field','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_separator','a single character','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('value_separator','edit_field','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_field','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_field','edit_field','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_separator','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_separator','edit_field','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_field','field_edits','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_edits','fixapache','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('field_edits','resolver','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expand_scalars','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expand_scalars','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_contains_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_contains_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_contains_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_contains_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_contains_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_contains_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_match_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_match_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_match_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_match_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_match_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_match_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_startwith_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_startwith_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_not_startwith_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_startwith_from_list','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_startwith_from_list','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_if_startwith_from_list','insert_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_select','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_type','insert_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_type','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('first_last','first_last','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('first_last','location','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('before_after','before_after','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('before_after','location','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_matching','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_matching','match_value','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_line_matching','location','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('location','insert_lines','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_lines','fixapache','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('insert_lines','upgrade_cfexecd','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('specify_inherit_aces','slist','is a list of type','is used in','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('specify_inherit_aces','slist','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('specify_inherit_aces','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aces','slist','is a list of type','is used in','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aces','slist','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aces','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_directory_inherit','acl_directory_inherit','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_directory_inherit','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_type','acl_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_type','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_method','acl_method','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl_method','acl','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('acl','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pathtype','pathtype','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('pathtype','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('transformer','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('transformer','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('move_obstructions','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('move_obstructions','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('create','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('create','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('touch','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('touch','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdeadlinks','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdeadlinks','depth_search','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('traverse_links','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('traverse_links','depth_search','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xdev','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xdev','depth_search','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depth','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depth','depth_search','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_basedir','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_basedir','depth_search','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exclude_dirs','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exclude_dirs','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exclude_dirs','depth_search','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_dirs','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_dirs','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('include_dirs','depth_search','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depth_search','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('empty_file_before_editing','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('empty_file_before_editing','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('max_file_size','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('max_file_size','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_backup','edit_backup','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_backup','edit_defaults','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_defaults','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_xml','edit_xml','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_xml','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_line','edit_line','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('edit_line','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repository','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repository','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_mode','posix file mode or permission','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_mode','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rotate','int','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rotate','rename','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable','rename','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_suffix','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('disable_suffix','rename','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('newname','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('newname','rename','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rename','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdirs','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rmdirs','delete','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dirlinks','dirlinks','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dirlinks','delete','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('delete','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('update_hashes','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('update_hashes','changes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_changes','report_changes','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_changes','changes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hash','hash','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hash','changes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('changes','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bsdflags','bsdflags','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bsdflags','perms','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rxdirs','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rxdirs','perms','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groups','a user/group id','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groups','a user/group id','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('groups','perms','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('owners','a user/group id','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('owners','a user/group id','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('owners','perms','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mode','posix file mode or permission','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mode','perms','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('perms','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_linking_children','when_linking_children','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_linking_children','link_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_children','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_children','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_no_source','when_no_source','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('when_no_source','link_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_patterns','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_patterns','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_patterns','link_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','link type','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','link_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_from','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('findertype','findertype','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('findertype','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_root','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('check_root','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('purge','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('purge','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('verify','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('verify','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('encrypt','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('encrypt','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('trustkey','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('trustkey','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_size','copy_size','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_size','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_ipv4','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_ipv4','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_update','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('force_update','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('type_check','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('type_check','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','link type','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link_type','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compare','compare','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compare','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copylink_patterns','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copylink_patterns','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copylink_patterns','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('linkcopy_patterns','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('linkcopy_patterns','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('linkcopy_patterns','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preserve','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preserve','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stealth','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stealth','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_backup','copy_backup','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_backup','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('portnumber','int','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('portnumber','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('servers','slist','is a list of type','is used in','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('servers','slist','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('servers','copy_from','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','link_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('source','copy_from','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('copy_from','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_result','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_result','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('issymlinkto','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('issymlinkto','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('issymlinkto','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_types','file_types','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_types','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_program','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_program','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_regex','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_regex','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('atime','atime','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('atime','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mtime','mtime','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('mtime','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ctime','ctime','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ctime','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_bsdflags','string','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_bsdflags','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_groups','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_groups','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_groups','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_owners','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_owners','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_owners','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_size','search_size','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_size','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_mode','posix file mode or permission','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_mode','posix file mode or permission','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('search_mode','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_name','a file path','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_name','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('path_name','file_select','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('leaf_name','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('leaf_name','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('leaf_name','file_select','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_select','files','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','garbage_collection','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','main','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','cfengine','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('files','update','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registry_exclude','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registry_exclude','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('registry_exclude','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_rows','slist','is a list of type','is used in','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_rows','slist','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_rows','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_columns','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_columns','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_columns','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_operation','database_operation','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_operation','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_type','database_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_type','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_connection_db','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_connection_db','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_type','db_server_type','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_type','database_server','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_host','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_host','database_server','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_password','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_password','database_server','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_owner','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('db_server_owner','database_server','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('database_server','databases','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('module','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('module','commands','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no_output','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('no_output','contain','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preview','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('preview','contain','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chroot','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chroot','contain','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chdir','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('chdir','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_timeout','int','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_timeout','contain','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_group','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_group','contain','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_owner','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('exec_owner','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('umask','umask','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('umask','contain','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('useshell','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('useshell','contain','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('contain','commands','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('args','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('args','commands','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','setup_knowledge','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','web_server','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','main','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','cfengine','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('commands','update','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_end','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_end','select_region','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_start','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_start','select_region','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('select_region','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comment','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comment','topics','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comment','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depends_on','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depends_on','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('depends_on','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('handle','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('handle','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifvarclass','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifvarclass','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('timer_policy','timer_policy','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('timer_policy','classes','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('persist_time','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('persist_time','classes','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_kept','a cfengine identifier','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_kept','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_kept','classes','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_timeout','a cfengine identifier','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_timeout','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_timeout','classes','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_denied','a cfengine identifier','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_denied','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_denied','classes','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_failed','a cfengine identifier','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_failed','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('repair_failed','classes','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_repaired','a cfengine identifier','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_repaired','a cfengine identifier','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise_repaired','classes','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classes','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('measurement_class','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('measurement_class','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_level','report_level','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_level','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('background','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('background','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('audit','boolean','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('audit','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_failed','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_failed','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_repaired','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_repaired','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_kept','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_kept','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_level','log_level','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_level','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_string','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('log_string','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expireafter','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expireafter','action','is a possible sub-body constraint for','may have sub-body constraints','values','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifelapsed','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ifelapsed','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('action_policy','action_policy','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('action_policy','action','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('action','*','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_to_file','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('report_to_file','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('friend_pattern','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('friend_pattern','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_to_print','a file path','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_to_print','printfile','is a possible sub-body constraint for','may have sub-body constraints','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('number_of_lines','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('number_of_lines','printfile','is a possible sub-body constraint for','may have sub-body constraints','body_constraints','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('printfile','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('showstate','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('showstate','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('showstate','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('intermittency','real','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('intermittency','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen','a positive integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen','reports','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','web_server','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','upgrade_cfexecd','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','anomalies','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('reports','main','is employed in bundle','employs promises of type','promise_types','contexts');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not','class','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('not','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expression','class','is a body constraint of type','has possible body constraints','body_constraints','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('expression','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dist','real number','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dist','real number','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dist','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xor','xor','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('xor','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('and','and','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('and','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('or','or','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('or','classes','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('policy','policy','is a body constraint of type','has possible body constraints','body_constraints','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('policy','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','real number','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','real number','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('rlist','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','integer','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('ilist','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','An arbitrary string','is a list of type','is used in','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('slist','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real','real number','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('int','integer','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('int','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('string','An arbitrary string','is a body constraint of type','has possible body constraints','body_constraints','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('string','vars','is a possible body constraint for','can have body constraints','body_constraints','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','setup_knowledge','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','fixapache','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','web_server','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','laptop','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','resolver','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','anomalies','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','main','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','cfengine','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','g','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('vars','update','is employed in bundle','employs promises of type','promise_types','bundles');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','extraction_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','units','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','path_root','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','web_root','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','represents','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','associates','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','backward_relationship','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','forward_relationship','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','authorize','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','maproot','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','deny','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','admit','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','mount_options','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','mount_server','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','tty','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','status','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','process_owner','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','out_of_range_define','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','in_range_define','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_architectures','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_version','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_name_convention','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_noverify_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_installed_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_arch_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_name_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_version_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_patch_installed_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_patch_arch_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_patch_name_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_patch_version_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_list_arch_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_list_name_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_list_version_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','package_file_repositories','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','delete_if_not_contains_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','delete_if_contains_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','delete_if_not_match_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','delete_if_match_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','delete_if_not_startwith_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','delete_if_startwith_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','replace_value','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','field_value','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','field_separator','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','insert_if_not_contains_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','insert_if_contains_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','insert_if_not_match_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','insert_if_match_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','insert_if_not_startwith_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','insert_if_startwith_from_list','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','select_line_matching','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','exclude_dirs','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','include_dirs','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','disable_suffix','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','newname','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','copy_patterns','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','source','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','copylink_patterns','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','linkcopy_patterns','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','issymlinkto','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','search_groups','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','search_owners','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','leaf_name','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','registry_exclude','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','database_columns','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','db_server_connection_db','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','db_server_host','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','db_server_password','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','db_server_owner','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','exec_group','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','exec_owner','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','args','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','select_end','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','select_start','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','comment','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','depends_on','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','ifvarclass','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','measurement_class','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','log_failed','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','log_repaired','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','log_kept','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','log_string','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','friend_pattern','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','showstate','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','slist','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','string','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('An arbitrary string','string','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a syslog level','string','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','mount_source','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','process_stop','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_verify_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_patch_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_update_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_delete_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_add_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_patch_list_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','package_list_command','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','transformer','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','repository','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','source','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','exec_program','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','exec_regex','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','path_name','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','chroot','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','chdir','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','report_to_file','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','file_to_print','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a file path','string','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a user/group id','groups','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a user/group id','owners','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a user/group id','string','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','restart_class','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','handle','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','promise_kept','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','repair_timeout','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','repair_denied','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','repair_failed','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','promise_repaired','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine identifier','string','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a cfengine class expression','class','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('posix file mode or permission','disable_mode','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('posix file mode or permission','mode','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('posix file mode or permission','search_mode','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a single character','value_separator','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','dist','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','rlist','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','real','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('real number','real','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','package_noverify_returncode','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','ilist','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','int','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('integer','int','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','select_line_number','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','sensible_count','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','sensible_size','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','threads','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','vsize','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','rsize','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','pgid','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','ppid','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','pid','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','match_range','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','select_field','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','depth','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','max_file_size','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','persist_time','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','expireafter','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','ifelapsed','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','number_of_lines','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','lastseen','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a positive integer','int','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('a time range','int','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('link type','link_type','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','ifencrypted','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','scan_arrivals','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','check_foreign','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','unmount','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','edit_fstab','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','not_matching','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','allow_blank_fields','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','extend_fields','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','expand_scalars','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','move_obstructions','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','create','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','touch','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','rmdeadlinks','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','traverse_links','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','xdev','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','include_basedir','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','empty_file_before_editing','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','disable','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','rmdirs','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','update_hashes','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','rxdirs','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','link_children','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','check_root','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','purge','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','verify','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','encrypt','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','trustkey','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','force_ipv4','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','force_update','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','type_check','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','preserve','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','stealth','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','module','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','no_output','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','preview','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','useshell','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','background','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','audit','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('boolean','int','is a special case of','is the generic type for','values','data_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('system signals','signals','are used in constraint parameters','takes value','values','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('comments','comment','see instances of','is one of a number of','system_policy','values');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('software patches report','packages','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('setuid report','files','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('performance report','promises','is based on','reported in','system_reports','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen report','access','is affected by','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('lastseen report','copy_from','is affected by','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('installed software report','packages','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hashes report','changes','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hashes report','files','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_changes report','changes','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('file_changes report','files','is generated with','reported in','system_reports','body_constraints');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('compliance report','promises','is based on','reported in','system_reports','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('classes report','classes','is generated with','reported in','system_reports','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('roles','agents','are identified in','play different','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('roles','promises','are recurring patterns of','define','promise_theory','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('versions','occurrences','label variations in','vary according to','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('dependencies','conditional promises','result in','require the use of','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('workflows','actions','are formed by associating','combine into','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stories','workflows','can talk about','can be the subject of','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('stories','topics','are formed by associating','can be combined into','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('services','promises','are formed by associating','combine into','promise_theory','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('subjects','topics','are represented as','are knowledge-map instances of','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrences','occurrences','are classified into disjoint','are disjoint classes of','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('occurrence-types','occurrence-types','are also called','might be called','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topic-instances','occurrences','are simply called','might be called','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topic-instances','topics','are realizations of','are discussed in','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('type-instances','topics','is another term for','may be described as','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','topics','form networks of','are inter-related through','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','structural relationships','represent','can be represented as','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','causative relationships','represent','can be represented as','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','location relationships','represent','can be represented as','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('associations','conceptual relationships','represent','can be represented as','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('actions','intentions','can be the subject of','can talk about','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('behaviours','intentions','can be the subject of','can talk about','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('facts','intentions','can be the subject of','can talk about','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topics','topics','can be classified into disjoint','are disjoint classes of','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topics','stories','can be combined into','can be broken down into','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('topics','promises','can talk about','can be represented as','promise_theory','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('body-constraint','body-constraint','describes the extent (or quantity) of a','is quantified by its','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('body-type','body-constraint','describes the subject (nature or quality) of a','is qualified by its','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise body','body-constraint','has attribute','is an attribute of','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise body','body-type','has attribute','is an attribute of','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise-type','promise-type','has instances','is an instance of','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise-type','promise-type','describes the nature (or quality) of a','is qualified by its','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promisee','promisee','is the recipient of','is made to a','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promiser','promiser','makes a','is made by a','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('announcements','communicated information','are a form of','includes','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('conditional promises','conditional promises','are completed by','complete','promise_theory','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promises','communicated information','are a form of','includes','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promises','topics','can be made about','can be the subject of','promise_theory','promise_types');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promises','intentions','reveal','are revealed in the form of','promise_theory','promise_theory');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise proposals','promises','are a basis for','may be based on','promise_theory','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('bundles','promises','are collections of','combine to form','promise_theory','system_policy');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('cascading','hierarchical structure','$(tm.same[f])','$(tm.same[f])','models_of_organization','models_of_organization');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('object paradigm','object paradigm','$(tm.same[f])','$(tm.same[f])','models_of_organization','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('aspect paradigm','aspect paradigm','$(tm.same[f])','$(tm.same[f])','models_of_organization','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('service orientation','peer to peer network','$(tm.same[f])','$(tm.same[f])','models_of_organization','models_of_organization');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('hierarchical structure','hierarchical structure','$(tm.same[f])','$(tm.same[f])','models_of_organization','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('self-healing','self-healing','$(tm.impr[f])','$(tm.impr[b])','models_of_system_administration','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('autonomics','autonomics','$(tm.impr[f])','$(tm.impr[b])','models_of_system_administration','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('computer immunology','computer immunology','$(tm.impr[f])','$(tm.impr[b])','models_of_system_administration','(null)');
INSERT INTO associations (from_name,to_name,from_assoc,to_assoc,from_type,to_type) values ('promise theory','cfengine','is implemented in','is the reference implementation of','models_of_system_administration','bundles');
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('DB','Relational database service','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('LDAP','Directory Service','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('NFS','Network File System (storage)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('DNS','Domain Name Service','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('WWW','Web service','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Cfengine','Cfengine automation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_41','promises.cf.html#promise_knowledge_cf_41','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_41','promises.cf.html#promise_knowledge_cf_41','0','web_server')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_41','Generate knowledge associative graphs','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_41','Generate knowledge associative graphs','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#promise_knowledge_cf_41','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#promise_knowledge_cf_41','0','/usr/local/sbin/cf-know -f /srv/www/htdocs/cfknowledgebase/enterprise.cf -g')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#promise_knowledge_cf_41','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_server','promises.cf.html#bundle_web_server','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_34','promises.cf.html#promise_knowledge_cf_34','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_34','promises.cf.html#promise_knowledge_cf_34','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_34','Generate config reports','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_34','Generate config reports','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_24','promises.cf.html#promise_knowledge_cf_24','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_24','promises.cf.html#promise_knowledge_cf_24','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_24','Make sure knowledge files are accessible to webserver','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_24','Make sure knowledge files are accessible to webserver','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_12','promises.cf.html#promise_knowledge_cf_12','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_12','promises.cf.html#promise_knowledge_cf_12','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_knowledge_cf_12','(Uncommented promise of type <i>vars</i> made by: doc_root..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('setup_knowledge','promises.cf.html#bundle_setup_knowledge','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_135','promises.cf.html#promise_webserver_cf_135','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_135','promises.cf.html#promise_webserver_cf_135','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_135','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_128','promises.cf.html#promise_webserver_cf_128','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_128','promises.cf.html#promise_webserver_cf_128','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_128','(Uncommented promise of type <i>field_edits</i> made by: APACHE_MODULES=.*..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_115','promises.cf.html#promise_webserver_cf_115','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_115','promises.cf.html#promise_webserver_cf_115','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_115','(Uncommented promise of type <i>insert_lines</i> made by: APACHE_CONF_INCLUDE_FILES..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_106','promises.cf.html#promise_webserver_cf_106','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_106','promises.cf.html#promise_webserver_cf_106','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_106','(Uncommented promise of type <i>vars</i> made by: del_modules..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_102','promises.cf.html#promise_webserver_cf_102','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_102','promises.cf.html#promise_webserver_cf_102','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_102','(Uncommented promise of type <i>vars</i> made by: add_modules..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('fixapache','promises.cf.html#bundle_fixapache','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_83','promises.cf.html#promise_webserver_cf_83','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_83','promises.cf.html#promise_webserver_cf_83','0','!software_ok.on')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_83','(Uncommented promise of type <i>reports</i> made by: The web server software c..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_software_ok_on','promises.cf.html#promise_webserver_cf_83','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_software_ok_on','promises.cf.html#promise_webserver_cf_83','0','The web server software could not be installed')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_software_ok_on','promises.cf.html#promise_webserver_cf_83','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_74','promises.cf.html#promise_webserver_cf_74','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_74','promises.cf.html#promise_webserver_cf_74','0','software_ok')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_74','(Uncommented promise of type <i>files</i> made by: /etc/sysconfig/apache2..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_ok','promises.cf.html#promise_webserver_cf_74','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_ok','promises.cf.html#promise_webserver_cf_74','0','/etc/sysconfig/apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_ok','promises.cf.html#promise_webserver_cf_74','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_62','promises.cf.html#promise_webserver_cf_62','0','packages')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_62','promises.cf.html#promise_webserver_cf_62','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_62','(Uncommented promise of type <i>packages</i> made by: php5..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_62','(Uncommented promise of type <i>packages</i> made by: apache2-prefork..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_62','(Uncommented promise of type <i>packages</i> made by: apache2-mod_php5..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_62','(Uncommented promise of type <i>packages</i> made by: apache2..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_53','promises.cf.html#promise_webserver_cf_53','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_53','promises.cf.html#promise_webserver_cf_53','0','start_apache')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_53','(Uncommented promise of type <i>commands</i> made by: /etc/init.d/apache2 resta..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_apache','promises.cf.html#promise_webserver_cf_53','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_apache','promises.cf.html#promise_webserver_cf_53','0','/etc/init.d/apache2 restart')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('start_apache','promises.cf.html#promise_webserver_cf_53','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_44','promises.cf.html#promise_webserver_cf_44','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_44','promises.cf.html#promise_webserver_cf_44','0','off')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_44','(Uncommented promise of type <i>processes</i> made by: apache2..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('off','promises.cf.html#promise_webserver_cf_44','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('off','promises.cf.html#promise_webserver_cf_44','0','apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('off','promises.cf.html#promise_webserver_cf_44','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_38','promises.cf.html#promise_webserver_cf_38','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_38','promises.cf.html#promise_webserver_cf_38','0','web_ok.on')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_38','(Uncommented promise of type <i>processes</i> made by: apache2..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_ok_on','promises.cf.html#promise_webserver_cf_38','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_ok_on','promises.cf.html#promise_webserver_cf_38','0','apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_ok_on','promises.cf.html#promise_webserver_cf_38','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_23','promises.cf.html#promise_webserver_cf_23','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_23','promises.cf.html#promise_webserver_cf_23','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_23','(Uncommented promise of type <i>vars</i> made by: match_package..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_17','promises.cf.html#promise_webserver_cf_17','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_17','promises.cf.html#promise_webserver_cf_17','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_17','(Uncommented promise of type <i>vars</i> made by: site_http_conf..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_11','promises.cf.html#promise_webserver_cf_11','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_11','promises.cf.html#promise_webserver_cf_11','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_webserver_cf_11','(Uncommented promise of type <i>vars</i> made by: document_root..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_31','promises.cf.html#promise_laptop_cf_31','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_31','promises.cf.html#promise_laptop_cf_31','0','!usb_backup_device_loaded')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_31','(Uncommented promise of type <i>files</i> made by: /home/backup..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_31','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_31','0','/home/backup')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_31','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_22','promises.cf.html#promise_laptop_cf_22','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_22','promises.cf.html#promise_laptop_cf_22','0','usb_backup_device_loaded')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_22','(Uncommented promise of type <i>files</i> made by: /media/disk-1/CfengineBac..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_22','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_22','0','/media/disk-1/CfengineBackupDevice')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usb_backup_device_loaded','promises.cf.html#promise_laptop_cf_22','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_6','promises.cf.html#promise_laptop_cf_6','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_6','promises.cf.html#promise_laptop_cf_6','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_laptop_cf_6','(Uncommented promise of type <i>vars</i> made by: mount_point..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('laptop','promises.cf.html#bundle_laptop','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_200','promises.cf.html#promise_library_cf_200','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_200','promises.cf.html#promise_library_cf_200','0','exec_fix')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_200','(Uncommented promise of type <i>reports</i> made by: Added a 5 minute schedule..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_194','promises.cf.html#promise_library_cf_194','0','replace_patterns')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_194','promises.cf.html#promise_library_cf_194','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_194','(Uncommented promise of type <i>replace_patterns</i> made by: cfexecd..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_190','promises.cf.html#promise_library_cf_190','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_190','promises.cf.html#promise_library_cf_190','0','exec_fix')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_190','(Uncommented promise of type <i>insert_lines</i> made by: 0,5,10,15,20,25,30,35,40,..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('upgrade_cfexecd','promises.cf.html#bundle_upgrade_cfexecd','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_165','promises.cf.html#promise_library_cf_165','0','delete_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_165','promises.cf.html#promise_library_cf_165','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_165','(Uncommented promise of type <i>delete_lines</i> made by: $(regex)..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('DeleteLinesMatching','promises.cf.html#bundle_DeleteLinesMatching','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_113','promises.cf.html#promise_library_cf_113','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_113','promises.cf.html#promise_library_cf_113','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_113','(Uncommented promise of type <i>field_edits</i> made by: NETCONFIG_DNS_STATIC_SEAR..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_105','promises.cf.html#promise_library_cf_105','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_105','promises.cf.html#promise_library_cf_105','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_library_cf_105','(Uncommented promise of type <i>vars</i> made by: search..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('resolver','promises.cf.html#bundle_resolver','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_366','promises.cf.html#promise_site_cf_366','0','roles')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_366','promises.cf.html#promise_site_cf_366','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_366','(Uncommented promise of type <i>roles</i> made by: .*..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_362','promises.cf.html#promise_site_cf_362','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_362','promises.cf.html#promise_site_cf_362','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_362','(Uncommented promise of type <i>access</i> made by: /home/mark/.cfagent/bin/c..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_358','promises.cf.html#promise_site_cf_358','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_358','promises.cf.html#promise_site_cf_358','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_358','(Uncommented promise of type <i>access</i> made by: /home/mark/LapTop..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('access_rules','promises.cf.html#bundle_access_rules','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_345','promises.cf.html#promise_site_cf_345','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_345','promises.cf.html#promise_site_cf_345','0','anomaly_hosts.icmp_in_high_anomaly.!entropy_icmp_in_high')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_345','(Uncommented promise of type <i>reports</i> made by: Anomalous low entropy (3d..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','promises.cf.html#promise_site_cf_345','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','promises.cf.html#promise_site_cf_345','0','Anomalous low entropy (3dev) incoming ICMP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_icmp_in) pm $(mon.stddev_icmp_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomaly_hosts_icmp_in_high_anomaly__entropy_icmp_in_high','promises.cf.html#promise_site_cf_345','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_338','promises.cf.html#promise_site_cf_338','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_338','promises.cf.html#promise_site_cf_338','0','entropy_dns_in_low.anomaly_hosts.udp_in_high_dev2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_338','(Uncommented promise of type <i>reports</i> made by: Anomalous (2dev) incoming..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','promises.cf.html#promise_site_cf_338','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','promises.cf.html#promise_site_cf_338','0','Anomalous (2dev) incoming (non-DNS) UDP traffic on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_udp_in) pm $(mon.stddev_udp_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_udp_in_high_dev2','promises.cf.html#promise_site_cf_338','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_331','promises.cf.html#promise_site_cf_331','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_331','promises.cf.html#promise_site_cf_331','0','entropy_dns_in_low.anomaly_hosts.dns_in_high_anomaly')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_331','(Uncommented promise of type <i>reports</i> made by: Anomalous (3dev) incoming..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','promises.cf.html#promise_site_cf_331','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','promises.cf.html#promise_site_cf_331','0','Anomalous (3dev) incoming DNS packets on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(average_dns_in) pm $(mon.stddev_dns_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_dns_in_low_anomaly_hosts_dns_in_high_anomaly','promises.cf.html#promise_site_cf_331','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_324','promises.cf.html#promise_site_cf_324','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_324','promises.cf.html#promise_site_cf_324','0','entropy_tcpsyn_in_low.anomaly_hosts.tcpsyn_in_high_dev2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_324','(Uncommented promise of type <i>reports</i> made by: Anomalous number of new T..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','promises.cf.html#promise_site_cf_324','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','promises.cf.html#promise_site_cf_324','0','Anomalous number of new TCP connections on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_tcpsyn_in) pm $(mon.stddev_tcpsyn_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_tcpsyn_in_low_anomaly_hosts_tcpsyn_in_high_dev2','promises.cf.html#promise_site_cf_324','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_317','promises.cf.html#promise_site_cf_317','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_317','promises.cf.html#promise_site_cf_317','0','entropy_www_in_low.anomaly_hosts.www_in_high_anomaly')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_317','(Uncommented promise of type <i>reports</i> made by: LOW ENTROPY Incoming www ..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_317','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_317','0','LOW ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009
     - measured value $(svalue_www_in) av $(average_www_in) pm $(stddev_www_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_low_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_317','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_310','promises.cf.html#promise_site_cf_310','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_310','promises.cf.html#promise_site_cf_310','0','entropy_www_in_high&anomaly_hosts.www_in_high_anomaly')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_310','(Uncommented promise of type <i>reports</i> made by: HIGH ENTROPY Incoming www..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_310','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_310','0','HIGH ENTROPY Incoming www anomaly high anomaly dev!! on $(mon.host) at Tue Jun 30 12:46:08 2009 
    - measured value 0 av $(mon.average_www_in) pm $(mon.stddev_www_in)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('entropy_www_in_high_anomaly_hosts_www_in_high_anomaly','promises.cf.html#promise_site_cf_310','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_303','promises.cf.html#promise_site_cf_303','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_303','promises.cf.html#promise_site_cf_303','0','rootprocs_high_dev2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_303','(Uncommented promise of type <i>reports</i> made by: RootProc anomaly high 2 d..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rootprocs_high_dev2','promises.cf.html#promise_site_cf_303','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rootprocs_high_dev2','promises.cf.html#promise_site_cf_303','0','RootProc anomaly high 2 dev on $(mon.host) at Tue Jun 30 12:46:08 2009 
    measured value 100 av $(mon.average_rootprocs) pm $(mon.stddev_rootprocs)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rootprocs_high_dev2','promises.cf.html#promise_site_cf_303','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_293','promises.cf.html#promise_site_cf_293','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_293','promises.cf.html#promise_site_cf_293','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_293','(Uncommented promise of type <i>files</i> made by: /tmp/shadow..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_293','(Uncommented promise of type <i>files</i> made by: /tmp/passwd..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_287','promises.cf.html#promise_site_cf_287','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_287','promises.cf.html#promise_site_cf_287','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_287','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs/shad..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_287','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs/pass..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_275','promises.cf.html#promise_site_cf_275','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_275','promises.cf.html#promise_site_cf_275','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_275','(Uncommented promise of type <i>vars</i> made by: files..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_274','promises.cf.html#promise_site_cf_274','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_274','promises.cf.html#promise_site_cf_274','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_274','(Uncommented promise of type <i>vars</i> made by: sysdir..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('anomalies','promises.cf.html#bundle_anomalies','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_disk_watch','promises.cf.html#free_disk_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_disk_watch','promises.cf.html#free_disk_watch','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_disk_watch','(Uncommented promise of type <i>measurements</i> made by: /bin/df..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_memory_watch','promises.cf.html#free_memory_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_memory_watch','promises.cf.html#free_memory_watch','0','linux')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('free_memory_watch','(Uncommented promise of type <i>measurements</i> made by: /proc/meminfo..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_disk_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_disk_watch','0','/bin/df')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_disk_watch','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_memory_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_memory_watch','0','/proc/meminfo')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linux','promises.cf.html#free_memory_watch','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_self_watch','promises.cf.html#monitor_self_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_self_watch','promises.cf.html#monitor_self_watch','0','atlas')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('monitor_self_watch','(Uncommented promise of type <i>measurements</i> made by: /var/cfengine/state/cf_ro..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atlas','promises.cf.html#monitor_self_watch','0','measurements')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atlas','promises.cf.html#monitor_self_watch','0','/var/cfengine/state/cf_rootprocs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atlas','promises.cf.html#monitor_self_watch','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('watch','promises.cf.html#bundle_watch','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_180','promises.cf.html#promise_site_cf_180','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_180','promises.cf.html#promise_site_cf_180','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_180','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf\..*..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_174','promises.cf.html#promise_site_cf_174','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_174','promises.cf.html#promise_site_cf_174','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_174','(Uncommented promise of type <i>files</i> made by: /var/cfengine/outputs..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('garbage_collection','promises.cf.html#bundle_garbage_collection','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_157','promises.cf.html#promise_site_cf_157','0','packages')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_157','promises.cf.html#promise_site_cf_157','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_157','Install patches','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_157','Install patches','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_148','promises.cf.html#promise_site_cf_148','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_148','promises.cf.html#promise_site_cf_148','0','Hr12')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_148','Update the locate database','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_148','Update the locate database','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Hr12','promises.cf.html#promise_site_cf_148','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Hr12','promises.cf.html#promise_site_cf_148','0','/usr/bin/updatedb --prunepaths=/windows')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('Hr12','promises.cf.html#promise_site_cf_148','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_141','promises.cf.html#promise_site_cf_141','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_141','promises.cf.html#promise_site_cf_141','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_141','(Uncommented promise of type <i>commands</i> made by: /usr/bin/zypper -non-inte..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_139','promises.cf.html#promise_site_cf_139','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_139','promises.cf.html#promise_site_cf_139','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_139','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-serv..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_139','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','promises.cf.html#promise_site_cf_131','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','promises.cf.html#promise_site_cf_131','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','(Uncommented promise of type <i>processes</i> made by: cf-serverd..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_131','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_120','promises.cf.html#promise_site_cf_120','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_120','promises.cf.html#promise_site_cf_120','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_120','(Uncommented promise of type <i>processes</i> made by: cfenvd..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_106','promises.cf.html#promise_site_cf_106','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_106','promises.cf.html#promise_site_cf_106','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_106','(Uncommented promise of type <i>files</i> made by: /etc/resolv.conf..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_98','promises.cf.html#promise_site_cf_98','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_98','promises.cf.html#promise_site_cf_98','0','!up_and_running')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_98','(Uncommented promise of type <i>reports</i> made by: Laptop wakes up..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_up_and_running','promises.cf.html#promise_site_cf_98','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_up_and_running','promises.cf.html#promise_site_cf_98','0','Laptop wakes up')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_up_and_running','promises.cf.html#promise_site_cf_98','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_90','promises.cf.html#promise_site_cf_90','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_90','promises.cf.html#promise_site_cf_90','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_90','(Uncommented promise of type <i>vars</i> made by: component..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('main','promises.cf.html#bundle_main','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_77','promises.cf.html#promise_site_cf_77','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_77','promises.cf.html#promise_site_cf_77','0','exec_fix')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_77','(Uncommented promise of type <i>processes</i> made by: cron..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_200','0','reports')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_200','0','Added a 5 minute schedule to crontabs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_200','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_190','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_190','0','0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/cfengine/bin/cf-execd -F')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_library_cf_190','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_site_cf_77','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_site_cf_77','0','cron')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_fix','promises.cf.html#promise_site_cf_77','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_70','promises.cf.html#promise_site_cf_70','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_70','promises.cf.html#promise_site_cf_70','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_70','This is inaccessible in new suse after cd opened/closed','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_70','This is inaccessible in new suse after cd opened/closed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_66','promises.cf.html#promise_site_cf_66','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_66','promises.cf.html#promise_site_cf_66','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_66','(Uncommented promise of type <i>files</i> made by: /var/spool/cron/tabs/root..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_58','promises.cf.html#promise_site_cf_58','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_58','promises.cf.html#promise_site_cf_58','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_58','Check if there are still promises about cfengine 2 that need removing','4','comment')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_58','Check if there are still promises about cfengine 2 that need removing','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_48','promises.cf.html#promise_site_cf_48','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_48','promises.cf.html#promise_site_cf_48','0','integrate_cfengine2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_48','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cfagent..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integrate_cfengine2','promises.cf.html#promise_site_cf_48','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integrate_cfengine2','promises.cf.html#promise_site_cf_48','0','/var/cfengine/bin/cfagent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integrate_cfengine2','promises.cf.html#promise_site_cf_48','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_40','promises.cf.html#promise_site_cf_40','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_40','promises.cf.html#promise_site_cf_40','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_40','(Uncommented promise of type <i>vars</i> made by: cf2bits..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine','promises.cf.html#bundle_cfengine','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_17','promises.cf.html#promise_site_cf_17','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_17','promises.cf.html#promise_site_cf_17','0','!SuSE')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_17','(Uncommented promise of type <i>vars</i> made by: crontab..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_SuSE','promises.cf.html#promise_site_cf_17','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_SuSE','promises.cf.html#promise_site_cf_17','0','crontab')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('_SuSE','promises.cf.html#promise_site_cf_17','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_13','promises.cf.html#promise_site_cf_13','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_13','promises.cf.html#promise_site_cf_13','0','SuSE')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_site_cf_13','(Uncommented promise of type <i>vars</i> made by: crontab..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_13','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_13','0','crontab')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('SuSE','promises.cf.html#promise_site_cf_13','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('g','promises.cf.html#bundle_g','0','bundle reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_45','promises.cf.html#promise_update_cf_45','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_45','promises.cf.html#promise_update_cf_45','0','reload.start_exec')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_45','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-exec..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_exec','promises.cf.html#promise_update_cf_45','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_exec','promises.cf.html#promise_update_cf_45','0','/var/cfengine/bin/cf-execd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_exec','promises.cf.html#promise_update_cf_45','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_41','promises.cf.html#promise_update_cf_41','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_41','promises.cf.html#promise_update_cf_41','0','reload.start_monitor')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_41','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-moni..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_monitor','promises.cf.html#promise_update_cf_41','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_monitor','promises.cf.html#promise_update_cf_41','0','/var/cfengine/bin/cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_monitor','promises.cf.html#promise_update_cf_41','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_37','promises.cf.html#promise_update_cf_37','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_37','promises.cf.html#promise_update_cf_37','0','reload.start_server')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_37','(Uncommented promise of type <i>commands</i> made by: /var/cfengine/bin/cf-serv..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_server','promises.cf.html#promise_update_cf_37','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_server','promises.cf.html#promise_update_cf_37','0','/var/cfengine/bin/cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reload_start_server','promises.cf.html#promise_update_cf_37','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_31','promises.cf.html#promise_update_cf_31','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_31','promises.cf.html#promise_update_cf_31','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_31','(Uncommented promise of type <i>processes</i> made by: cf-execd..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_30','promises.cf.html#promise_update_cf_30','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_30','promises.cf.html#promise_update_cf_30','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_30','(Uncommented promise of type <i>processes</i> made by: cf-monitord..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_29','promises.cf.html#promise_update_cf_29','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_29','promises.cf.html#promise_update_cf_29','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_29','(Uncommented promise of type <i>processes</i> made by: cf-serverd..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_21','promises.cf.html#promise_update_cf_21','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_21','promises.cf.html#promise_update_cf_21','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_21','(Uncommented promise of type <i>files</i> made by: /var/cfengine/bin..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_13','promises.cf.html#promise_update_cf_13','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_13','promises.cf.html#promise_update_cf_13','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_13','(Uncommented promise of type <i>files</i> made by: /var/cfengine/inputs..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_7','promises.cf.html#promise_update_cf_7','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_7','promises.cf.html#promise_update_cf_7','0','any')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_update_cf_7','(Uncommented promise of type <i>vars</i> made by: master_location..)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_34','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_34','0','/usr/local/sbin/cf-promises -r')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_34','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_24','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_24','0','/srv/www/htdocs/cfknowledgebase')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_24','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_12','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_12','0','doc_root')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_knowledge_cf_12','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_135','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_135','0','APACHE_MODULES=.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_135','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_128','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_128','0','APACHE_MODULES=.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_128','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_115','0','insert_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_115','0','APACHE_CONF_INCLUDE_FILES="/home/mark/cfengine-inputs/httpd.conf"')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_115','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_106','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_106','0','del_modules')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_106','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_102','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_102','0','add_modules')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_102','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_62','0','php5')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_62','0','apache2-prefork')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_62','0','apache2-mod_php5')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_62','0','packages')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_62','0','apache2')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_webserver_cf_62','0','promise')
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
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_194','0','replace_patterns')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_194','0','cfexecd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_194','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_165','0','delete_lines')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_165','0','$(regex)')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_165','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_113','0','field_edits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_113','0','NETCONFIG_DNS_STATIC_SEARCHLIST.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_113','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_105','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_105','0','search')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_library_cf_105','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_366','0','roles')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_366','0','.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_366','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_362','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_362','0','/home/mark/.cfagent/bin/cf-agent')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_362','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_358','0','access')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_358','0','/home/mark/LapTop')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_358','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_293','0','/tmp/shadow')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_293','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_293','0','/tmp/passwd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_293','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_287','0','/var/cfengine/inputs/shadow')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_287','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_287','0','/var/cfengine/inputs/passwd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_287','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_275','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_275','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_275','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_274','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_274','0','sysdir')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_274','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_180','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_180','0','/etc/resolv.conf\..*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_180','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_174','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_174','0','/var/cfengine/outputs')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_174','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_157','0','packages')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_157','0','no match')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_157','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_141','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_141','0','/usr/bin/zypper -non-interactive patch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_141','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_139','0','/var/cfengine/bin/cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_139','0','commands')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_139','0','/var/cfengine/bin/cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_139','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','cf-serverd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','cf-monitord')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_131','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_120','0','processes')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_120','0','cfenvd')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_120','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_106','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_106','0','/etc/resolv.conf')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_106','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_90','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_90','0','component')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_90','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_70','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_70','0','/dev/sr0')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_70','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_66','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_66','0','/var/spool/cron/tabs/root')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_66','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_58','0','files')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_58','0','/var/cfengine/inputs/.*')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_58','0','promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_40','0','vars')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_40','0','cf2bits')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('any','promises.cf.html#promise_site_cf_40','0','promise')
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
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('userexists','True if user name or numerical id exists on this host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usemodule','Execute cfengine module script and set class if successful','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('strcmp','True if the two strings match exactly','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('splitstring','Convert a string in arg1 into a list of max arg3 strings by splitting on a regular expression in arg2','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('splayclass','True if the first argument's time-slot has arrived, according to a policy in arg2','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('selectservers','Select tcp servers which respond correctly to a query and return their number, set array of names','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rrange','Define a range of real numbers for cfengine internal use','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('returnszero','True if named shell command has exit status zero','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('remotescalar','Read a scalar value from a remote cfengine server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regldap','True if arg6 is a regular expression matching a value item in an ldap search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('reglist','True if arg2 matches any item in the list with id=arg1','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regline','True if arg2 is a regular expression matching a line in file arg1','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('registryvalue','Returns a value for an MS-Win registry key,value pair','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regcmp','True if arg2 is a regular expression matching arg1','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('regarray','True if arg1 matches any item in the associative array with id=arg2','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readtcp','Connect to tcp port, send string and assign result to variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readstringlist','Read and assign a list variable from a file of separated strings','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readstringarray','Read an array of strings from a file and assign the dimension to a variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readreallist','Read and assign a list variable from a file of separated real numbers','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readrealarray','Read an array of real numbers from a file and assign the dimension to a variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readintlist','Read and assign a list variable from a file of separated ints','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readintarray','Read an array of integers from a file and assign the dimension to a variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('readfile','Read max number of bytes from named file and assign to variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('randomint','Generate a random integer between the given limits','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('peerleaders','Get a list of peer leaders from the named partitioning','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('peerleader','Get the assigned peer-leader of the partition to which we belong','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('peers','Get a list of peers (not including ourself) from the partition to which we belong','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('on','Convert an exact date/time to an integer system representation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('now','Convert the current time into system representation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ldapvalue','Extract the first matching named value from ldap','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ldaplist','Extract all named values from multiple ldap records','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ldaparray','Extract all values from an ldap record','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastnode','Extract the last of a separated string, e.g. filename from a path','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isvariable','True if the named variable is defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isplain','True if the named object is a plain/regular file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isnewerthan','True if arg1 is newer (modified later) than arg2 (mtime)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('islink','True if the named object is a symbolic link','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('islessthan','True if arg1 is numerically less than arg2, else compare strings like NOT strcmp','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isgreaterthan','True if arg1 is numerically greater than arg2, else compare strings like strcmp','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('isdir','True if the named object is a directory','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('irange','Define a range of integer values for cfengine internal use','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('iprange','True if the current host lies in the range of IP addresses specified','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hostinnetgroup','True if the current host is in the named netgroup','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hostrange','True if the current host lies in the range of enumerated hostnames specified','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hashmatch','Compute the hash of arg1, of type arg2 and test if it matches the value in arg 3','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','Return the hash of arg1, type arg2 and assign to a variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('groupexists','True if group or numerical id exists on this host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getuid','Return the integer user id of the named user on this host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getgid','Return the integer group id of the named group on this host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('getindices','Get a list of keys to the array whose id is the argument and assign to variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('filesexist','True if the named list of files can ALL be accessed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('fileexists','True if the named file can be accessed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('execresult','Execute named command and assign output to variable','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classmatch','True if the regular expression matches any currently defined class','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classify','True if the canonicalization of the argument is a currently defined class','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changedbefore','True if arg1 was changed before arg2 (ctime)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('canonify','Convert an abitrary string into a legal class name','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ago','Convert a time relative to now to an integer system representation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('accumulated','Convert an accumulated amount of time into a system representation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('accessedbefore','True if arg1 was accessed before arg2 (atime)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('class','A boolean returned by certain functions in classes promises','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('policy','The policy for (dis)allowing redefinition of variables matching free,overridable,constant','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rlist','A list of real numbers matching -9.99999E100,9.99999E100','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ilist','A list of integers matching -99999999999,9999999999','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_106','0','promise_webserver_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_102','0','promise_webserver_cf_102')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_102','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_23','0','promise_webserver_cf_23')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_webserver_cf_23','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_library_cf_105','0','promise_library_cf_105')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_library_cf_105','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_275','0','promise_site_cf_275')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_275','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_90','0','promise_site_cf_90')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_90','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_40','0','promise_site_cf_40')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','promises.cf.html#promise_site_cf_40','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','A list of scalar strings matching ','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real','A scalar real number matching -9.99999E100,9.99999E100','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('int','A scalar integer matching -99999999999,9999999999','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_knowledge_cf_12','0','promise_knowledge_cf_12')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_knowledge_cf_12','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_17','0','promise_webserver_cf_17')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_17','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_11','0','promise_webserver_cf_11')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_webserver_cf_11','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_laptop_cf_6','0','promise_laptop_cf_6')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_laptop_cf_6','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_274','0','promise_site_cf_274')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_274','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_17','0','promise_site_cf_17')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_17','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_13','0','promise_site_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_site_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_update_cf_7','0','promise_update_cf_7')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','promises.cf.html#promise_update_cf_7','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','A scalar string matching ','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Generate knowledge associative graphs','4','promise_knowledge_cf_41')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_41','0','promise_knowledge_cf_41')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_41','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Generate config reports','4','promise_knowledge_cf_34')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_34','0','promise_knowledge_cf_34')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_34','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Make sure knowledge files are accessible to webserver','4','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_24','0','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_knowledge_cf_24','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Install patches','4','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Update the locate database','4','promise_site_cf_148')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_148','0','promise_site_cf_148')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_148','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','This is inaccessible in new suse after cd opened/closed','4','promise_site_cf_70')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_70','0','promise_site_cf_70')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_70','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Check if there are still promises about cfengine 2 that need removing','4','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_293','0','promise_site_cf_293')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_293','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_139','0','promise_site_cf_139')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','promises.cf.html#promise_site_cf_139','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_74','0','promise_webserver_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_site_cf_98','0','promise_site_cf_98')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_site_cf_98','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_library_cf_165','0','promise_library_cf_165')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_library_cf_165','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_148','0','promise_site_cf_148')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_148','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_141','0','promise_site_cf_141')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_141','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_48','0','promise_site_cf_48')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_site_cf_48','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','Regular expression that should contain a single backreference for extracting a value','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extraction_regex','Regular expression that should contain a single backreference for extracting a value','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_number','Read from the n-th line of the output (fixed format)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_number','Read from the n-th line of the output (fixed format)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_value','Criteria for extracting the measurement from a datastream','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('units','The engineering dimensions of this value or a note about its intent used in plots','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('history_type','Whether the data can be seen as a time-series or just an isolated value','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('data_type','The datatype being collected.','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stream_type','The datatype being collected.','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('path_root','Base path of the occurrence when locating file (replaced by web_root)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('web_root','Base URL of the occurrence when rendered as a web-URL (replaces path_root)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('representation','How to interpret the promiser string e.g. actual data or reference to data','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('represents','List of subtopics that explains the type(s) of information represented by the occurrence','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','promises.cf.html#promise_library_cf_194','0','promise_library_cf_194')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','promises.cf.html#promise_library_cf_194','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('associates','List of associated topics by this forward relationship','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('associates','List of associated topics by this forward relationship','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('backward_relationship','Name of backward/inverse association from associates to promiser topic','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('backward_relationship','Name of backward/inverse association from associates to promiser topic','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('forward_relationship','Name of forward association between promiser topic and associates','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('association','Declare associated topics','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('authorize','promises.cf.html#promise_site_cf_366','0','promise_site_cf_366')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('authorize','promises.cf.html#promise_site_cf_366','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('authorize','List of public-key user names that are allowed to activate the promised class during remote agent activation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('resource_type','The type of object being granted access. The default is access to files.','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifencrypted','true/false whether the current file access promise is conditional on the connection from the client being encrypted','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('maproot','List of host names or IP addresses to grant full read-privilege on the server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('deny','List of host names or IP addresses to deny access to file objects','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_362','0','promise_site_cf_362')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_362','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_358','0','promise_site_cf_358')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','promises.cf.html#promise_site_cf_358','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('admit','List of host names or IP addresses to grant access to file objects','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('scan_arrivals','true/false generate pseudo-periodic disk change arrival distribution','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('scan_arrivals','true/false generate pseudo-periodic disk change arrival distribution','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sensible_count','Minimum number of files that should be defined on a sensible-looking storage device','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sensible_count','Minimum number of files that should be defined on a sensible-looking storage device','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sensible_size','Minimum size in bytes that should be used on a sensible-looking storage device','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('freespace','Absolute or percentage minimum disk space that should be available before warning','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('freespace','Absolute or percentage minimum disk space that should be available before warning','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('check_foreign','true/false verify storage that is mounted from a foreign system on this host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('volume','Criteria for monitoring/probing mounted volumes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('unmount','true/false unmount a previously mounted filesystem','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('unmount','true/false unmount a previously mounted filesystem','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_fstab','true/false add or remove entries to the file system table ("fstab")','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_fstab','true/false add or remove entries to the file system table ("fstab")','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_options','List of option strings to add to the file system table ("fstab")','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_options','List of option strings to add to the file system table ("fstab")','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_server','Hostname or IP or remote file system server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_server','Hostname or IP or remote file system server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_source','Path of remote file system to mount','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount_type','Protocol type of remote file system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mount','Criteria for mounting foreign file systems','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_webserver_cf_38','0','promise_webserver_cf_38')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_webserver_cf_38','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_site_cf_131','0','promise_site_cf_131')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_site_cf_131','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_31','0','promise_update_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_30','0','promise_update_cf_30')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_30','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_29','0','promise_update_cf_29')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','promises.cf.html#promise_update_cf_29','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('restart_class','A class to be set if the process is not running, so that a command: rule can be referred to restart the process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_result','Boolean class expression returning the logical combination of classes set by a process selection test','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_result','Boolean class expression returning the logical combination of classes set by a process selection test','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('threads','Range of integers matching the threads (NLWP) field of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('priority','Range of integers matching the priority field (PRI/NI) of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('priority','Range of integers matching the priority field (PRI/NI) of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('tty','Regular expression matching the tty field of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('tty','Regular expression matching the tty field of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('command','Regular expression matching the command/cmd field of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stime_range','Range of integers matching the start time of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ttime_range','Range of integers matching the total elapsed time of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ttime_range','Range of integers matching the total elapsed time of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('status','Regular expression matching the status field of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('status','Regular expression matching the status field of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('vsize','Range of integers matching the virtual memory size of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('vsize','Range of integers matching the virtual memory size of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rsize','Range of integers matching the resident memory size of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rsize','Range of integers matching the resident memory size of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pgid','Range of integers matching the parent group id of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pgid','Range of integers matching the parent group id of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ppid','Range of integers matching the parent process id of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ppid','Range of integers matching the parent process id of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pid','Range of integers matching the process id of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pid','Range of integers matching the process id of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_owner','List of regexes matching the user of a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_select','Criteria for matching processes in the system process table','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('out_of_range_define','List of classes to define if the matches are out of range','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('out_of_range_define','List of classes to define if the matches are out of range','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('in_range_define','List of classes to define if the matches are in range','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('in_range_define','List of classes to define if the matches are in range','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('match_range','Integer range for acceptable number of matches for this process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_count','Criteria for constraining the number of processes matching other criteria','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_stop','promises.cf.html#promise_webserver_cf_44','0','promise_webserver_cf_44')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_stop','promises.cf.html#promise_webserver_cf_44','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('process_stop','A command used to stop a running process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','promises.cf.html#promise_site_cf_120','0','promise_site_cf_120')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','promises.cf.html#promise_site_cf_120','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','promises.cf.html#promise_site_cf_77','0','promise_site_cf_77')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','promises.cf.html#promise_site_cf_77','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('signals','A list of menu options representing signals to be sent to a process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_select','A criterion for first acceptable match relative to "package_version"','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_architectures','Select the architecture for package selection','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_version','Version reference point for determining promised version','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','This is how the package manager expects the file to be referred to, e.g. $(name).$(arch)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_convention','This is how the package manager expects the file to be referred to, e.g. $(name).$(arch)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_returncode','Integer return code indicating package verification failure','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_returncode','Integer return code indicating package verification failure','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_regex','Regular expression to match verification failure output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_noverify_regex','Regular expression to match verification failure output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_verify_command','Command to verify the correctness of an installed package','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_verify_command','Command to verify the correctness of an installed package','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','Command to update to the latest patch release of an installed package','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_command','Command to update to the latest patch release of an installed package','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','Command to update to the latest version a currently installed package','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_update_command','Command to update to the latest version a currently installed package','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','Command to remove a package from the system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_delete_command','Command to remove a package from the system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','Command to install a package to the system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_add_command','Command to install a package to the system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','Regular expression which matches packages that are already installed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_installed_regex','Regular expression which matches packages that are already installed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_arch_regex','Regular expression with one backreference to extract package architecture string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_arch_regex','Regular expression with one backreference to extract package architecture string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_regex','Regular expression with one backreference to extract package name string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_name_regex','Regular expression with one backreference to extract package name string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_version_regex','Regular expression with one backreference to extract package version string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_version_regex','Regular expression with one backreference to extract package version string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','Regular expression which matches packages that are already installed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_installed_regex','Regular expression which matches packages that are already installed','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_arch_regex','Regular expression with one backreference to extract update architecture string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_arch_regex','Regular expression with one backreference to extract update architecture string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','Regular expression with one backreference to extract update name string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_name_regex','Regular expression with one backreference to extract update name string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','Regular expression with one backreference to extract update version string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_version_regex','Regular expression with one backreference to extract update version string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','Command to obtain a list of available patches or updates','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_patch_list_command','Command to obtain a list of available patches or updates','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','Regular expression with one backreference to extract package architecture string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_arch_regex','Regular expression with one backreference to extract package architecture string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','Regular expression with one backreference to extract package name string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_name_regex','Regular expression with one backreference to extract package name string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','Regular expression with one backreference to extract package version string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_version_regex','Regular expression with one backreference to extract package version string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','Command to obtain a list of installed packages','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_list_command','Command to obtain a list of installed packages','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_file_repositories','A list of machine-local directories to search for packages','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_changes','Menu option - whether to group packages into a single aggregate command','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_method','Criteria for installation and verification','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','promises.cf.html#promise_site_cf_157','0','promise_site_cf_157')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','promises.cf.html#promise_site_cf_157','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('package_policy','Criteria for package installation/upgrade on the current system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('usebundle','Specify the name of a bundle to run as a parameterized method','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ipv6_address','IPv6 address for the interface','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ipv4_netmask','Netmask for the interface','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ipv4_address','IPv4 address for the interface','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('tcp_ip','Interface tcp/ip properties','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_contains_from_list','Delete line if a regex in the list DOES NOT match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_contains_from_list','Delete line if a regex in the list DOES NOT match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_contains_from_list','Delete line if a regex in the list match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_contains_from_list','Delete line if a regex in the list match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_match_from_list','Delete line if it DOES NOT fully match a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_match_from_list','Delete line if it DOES NOT fully match a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_match_from_list','Delete line if it fully matches a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_match_from_list','Delete line if it fully matches a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_startwith_from_list','Delete line if it DOES NOT start with a string in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_not_startwith_from_list','Delete line if it DOES NOT start with a string in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_if_startwith_from_list','Delete line if it starts with a string in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete_select','Delete only if lines pass filter criteria','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('not_matching','true/false negate match criterion','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','Menu option to replace all occurrences or just first (NB the latter is non-convergent)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('occurrences','Menu option to replace all occurrences or just first (NB the latter is non-convergent)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_value','promises.cf.html#promise_library_cf_194','0','promise_library_cf_194')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_value','promises.cf.html#promise_library_cf_194','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_value','Value used to replace regular expression matches in search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_with','promises.cf.html#promise_library_cf_194','0','promise_library_cf_194')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_with','promises.cf.html#promise_library_cf_194','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('replace_with','Search-replace pattern','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','true/false allow blank fields in a line (do not purge)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('allow_blank_fields','true/false allow blank fields in a line (do not purge)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('extend_fields','true/false add new fields at end of line if necessary to complete edit','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','Menu option policy for editing subfields','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_operation','Menu option policy for editing subfields','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','Set field value to a fixed value','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_value','Set field value to a fixed value','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','Character separator for subfields inside the selected field','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('value_separator','Character separator for subfields inside the selected field','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','Integer index of the field required 1..n','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_field','Integer index of the field required 1..n','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('field_separator','The regular expression used to separate fields in a line','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_135','0','promise_webserver_cf_135')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_135','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_128','0','promise_webserver_cf_128')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_webserver_cf_128','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_library_cf_113','0','promise_library_cf_113')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','promises.cf.html#promise_library_cf_113','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_field','Edit line-based file as matrix of fields','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expand_scalars','Expand any unexpanded variables','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_contains_from_list','Insert line if a regex in the list DOES NOT match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_contains_from_list','Insert line if a regex in the list DOES NOT match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_contains_from_list','Insert line if a regex in the list match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_contains_from_list','Insert line if a regex in the list match a line fragment','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_match_from_list','Insert line if it DOES NOT fully match a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_match_from_list','Insert line if it DOES NOT fully match a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_match_from_list','Insert line if it fully matches a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_match_from_list','Insert line if it fully matches a regex in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_startwith_from_list','Insert line if it DOES NOT start with a string in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_not_startwith_from_list','Insert line if it DOES NOT start with a string in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_if_startwith_from_list','Insert line if it starts with a string in the list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_select','Insert only if lines pass filter criteria','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('insert_type','Type of object the promiser string refers to (default literal)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('first_last','Menu option, choose first or last occurrence of match in file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('before_after','Menu option, point cursor before of after matched line','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('before_after','Menu option, point cursor before of after matched line','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_memory_watch','0','free_memory_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#free_memory_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#monitor_self_watch','0','monitor_self_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','promises.cf.html#monitor_self_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','Regular expression for matching line location','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_line_matching','Regular expression for matching file line location','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('location','Specify where in a file an insertion will be made','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('specify_inherit_aces','Native settings for access control entry','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('aces','Native settings for access control entry','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl_directory_inherit','Access control list type for the affected file system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl_type','Access control list type for the affected file system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl_method','Editing method for access control list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('acl','Criteria for access control lists on file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('pathtype','Menu option for interpreting promiser file object','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('transformer','Shell command (with full path) used to transform current file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('move_obstructions','true/false whether to move obstructions to file-object creation','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('create','promises.cf.html#promise_site_cf_106','0','promise_site_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('create','promises.cf.html#promise_site_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('create','true/false whether to create non-existing file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('touch','true/false whether to touch time stamps on file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdeadlinks','true/false remove links that point to nowhere','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdeadlinks','true/false remove links that point to nowhere','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('traverse_links','true/false traverse symbolic links to directories (false)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('traverse_links','true/false traverse symbolic links to directories (false)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('xdev','true/false exclude directories that are on different devices','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('xdev','true/false exclude directories that are on different devices','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_knowledge_cf_24','0','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_knowledge_cf_24','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','Maximum depth level for search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth','Maximum depth level for search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_basedir','true/false include the start/root dir of the search results','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_basedir','true/false include the start/root dir of the search results','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','List of regexes of directory names NOT to include in depth search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exclude_dirs','List of regexes of directory names NOT to include in depth search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('include_dirs','List of regexes of directory names to include in depth search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_knowledge_cf_24','0','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_knowledge_cf_24','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depth_search','Criteria for file depth searches','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','promises.cf.html#promise_site_cf_106','0','promise_site_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','promises.cf.html#promise_site_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','Baseline memory model of file to zero/empty before commencing promised edits','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('empty_file_before_editing','Baseline memory model of file to zero/empty before commencing promised edits','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','promises.cf.html#promise_site_cf_106','0','promise_site_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','promises.cf.html#promise_site_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('max_file_size','Do not edit files bigger than this number of bytes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_backup','promises.cf.html#promise_site_cf_106','0','promise_site_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_backup','promises.cf.html#promise_site_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_backup','Menu option for backup policy on edit changes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_defaults','promises.cf.html#promise_site_cf_106','0','promise_site_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_defaults','promises.cf.html#promise_site_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_defaults','Default promise details for file edits','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_xml','XML editing model for file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_webserver_cf_74','0','promise_webserver_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_webserver_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_106','0','promise_site_cf_106')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_106','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_66','0','promise_site_cf_66')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_66','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('edit_line','Line editing model for file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repository','Name of a repository for versioning','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_mode','The permissions to set when a file is disabled','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rotate','Maximum number of file rotations to keep','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rotate','Maximum number of file rotations to keep','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable','true/false automatically rename and remove permissions','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable','true/false automatically rename and remove permissions','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_suffix','The suffix to add to files when disabling (.cfdisabled)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('disable_suffix','The suffix to add to files when disabling (.cfdisabled)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('newname','The desired name for the current file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rename','Criteria for renaming files','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_180','0','promise_site_cf_180')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_180','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rmdirs','true/false whether to delete empty directories during recursive deletion','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_180','0','promise_site_cf_180')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_180','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dirlinks','Menu option policy for dealing with symbolic links to directories during deletion','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_180','0','promise_site_cf_180')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_180','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('delete','Criteria for deleting files','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('update_hashes','Update hash values immediately after change warning','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_changes','Specify criteria for change warnings','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hash','Hash files for change detection','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('changes','Criteria for change management','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bsdflags','List of menu options for bsd file system flags to set','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bsdflags','List of menu options for bsd file system flags to set','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rxdirs','true/false add execute flag for directories if read flag is set','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rxdirs','true/false add execute flag for directories if read flag is set','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('groups','List of acceptable groups of group ids, first is change target','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('groups','List of acceptable groups of group ids, first is change target','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_24','0','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_knowledge_cf_24','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_site_cf_70','0','promise_site_cf_70')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','promises.cf.html#promise_site_cf_70','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','List of acceptable owners or user ids, first is change target','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('owners','List of acceptable owners or user ids, first is change target','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_24','0','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_knowledge_cf_24','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mode','File permissions (like posix chmod)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_24','0','promise_knowledge_cf_24')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_knowledge_cf_24','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_site_cf_70','0','promise_site_cf_70')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_site_cf_70','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('perms','Criteria for setting permissions on a file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_linking_children','Policy for overriding existing files when linking directories of children','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_linking_children','Policy for overriding existing files when linking directories of children','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_children','true/false whether to link all directory's children to source originals','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_no_source','Behaviour when the source file to link to does not exist','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('when_no_source','Behaviour when the source file to link to does not exist','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_patterns','A set of patterns that should be copied ansd synchronized instead of linked','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_patterns','A set of patterns that should be copied ansd synchronized instead of linked','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','The type of link used to alias the file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','The type of link used to alias the file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_from','Criteria for linking file from a source','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('findertype','Menu option for default finder type on MacOSX','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('findertype','Menu option for default finder type on MacOSX','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('check_root','true/false check permissions on the root directory when depth_search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('check_root','true/false check permissions on the root directory when depth_search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('purge','true/false purge files on client that do not match files on server when depth_search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('purge','true/false purge files on client that do not match files on server when depth_search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','true/false verify transferred file by hashing after copy (resource penalty)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('verify','true/false verify transferred file by hashing after copy (resource penalty)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('encrypt','true/false use encrypted data stream to connect to remote host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('encrypt','true/false use encrypted data stream to connect to remote host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('trustkey','true/false trust public keys from remote server if previously unknown','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_size','Integer range of file sizes that may be copied','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_size','Integer range of file sizes that may be copied','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_ipv4','true/false force use of ipv4 on ipv6 enabled network','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_ipv4','true/false force use of ipv4 on ipv6 enabled network','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_update','true/false force copy update always','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('force_update','true/false force copy update always','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('type_check','true/false compare file types before copying and require match','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('type_check','true/false compare file types before copying and require match','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Menu option for type of links to use when copying','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','Menu option policy for comparing source and image file attributes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compare','Menu option policy for comparing source and image file attributes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copylink_patterns','List of patterns matching files that should be linked instead of copied','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copylink_patterns','List of patterns matching files that should be linked instead of copied','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linkcopy_patterns','List of patterns matching symbolic links that should be replaced with copies','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('linkcopy_patterns','List of patterns matching symbolic links that should be replaced with copies','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_293','0','promise_site_cf_293')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_293','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_287','0','promise_site_cf_287')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','promises.cf.html#promise_site_cf_287','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','true/false whether to preserve file permissions on copied file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preserve','true/false whether to preserve file permissions on copied file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stealth','true/false whether to preserve time stamps on copied file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_293','0','promise_site_cf_293')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_293','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_287','0','promise_site_cf_287')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_site_cf_287','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_backup','Menu option policy for file backup/version control','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('portnumber','Port number to connect to on server host','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('servers','List of servers in order of preference from which to copy','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('servers','List of servers in order of preference from which to copy','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_293','0','promise_site_cf_293')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_293','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_287','0','promise_site_cf_287')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_site_cf_287','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','The source file to which the link should point','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('source','Reference source file from which to copy','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_293','0','promise_site_cf_293')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_293','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_287','0','promise_site_cf_287')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_site_cf_287','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copy_from','Criteria for copying file from a source','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_180','0','promise_site_cf_180')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_180','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','Logical expression combining classes defined by file search criteria','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_result','Logical expression combining classes defined by file search criteria','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('issymlinkto','List of regular expressions to match file objects','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_types','List of acceptable file types from menu choices','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_types','List of acceptable file types from menu choices','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_program','Execute this command on each file and match if the exit status is zero','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_program','Execute this command on each file and match if the exit status is zero','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_regex','Matches file if this regular expression matches any full line returned by the command','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('atime','Range of access times (atime) for acceptable files','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_180','0','promise_site_cf_180')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_180','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('mtime','Range of modification times (mtime) for acceptable files','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ctime','Range of change times (ctime) for acceptable files','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_bsdflags','String of flags for bsd file system flags expected set','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_bsdflags','String of flags for bsd file system flags expected set','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_groups','List of acceptable group names or ids for the file, or regexes to match','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_groups','List of acceptable group names or ids for the file, or regexes to match','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_owners','List of acceptable user names or ids for the file, or regexes to match','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_size','Integer range of file sizes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_size','Integer range of file sizes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_mode','A list of mode masks for acceptable file permissions','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('search_mode','A list of mode masks for acceptable file permissions','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('path_name','List of pathnames to match acceptable target','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('path_name','List of pathnames to match acceptable target','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('leaf_name','List of regexes that match an acceptable name','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_180','0','promise_site_cf_180')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_180','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_174','0','promise_site_cf_174')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_174','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_select','Choose which files select in a search','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('registry_exclude','A list of regular expressions to ignore in key/value verification','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_rows','An ordered list of row values to be promised by SQL databases','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_columns','A list of column definitions to be promised by SQL databases','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_operation','The nature of the promise - to be or not to be','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_type','The type of database that is to be manipulated','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_connection_db','The name of an existing database to connect to in order to create/manage other databases','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_type','The dialect of the database server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_type','The dialect of the database server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_host','Hostname or address for connection to database, blank means localhost','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_host','Hostname or address for connection to database, blank means localhost','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_password','Clear text password for database connection','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_password','Clear text password for database connection','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('db_server_owner','User name for database connection','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('database_server','Credentials for connecting to a local/remote database server','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('module','true/false whether to expect the cfengine module protocol','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('no_output','true/false discard all output from the command','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('no_output','true/false discard all output from the command','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preview','true/false preview command when running in dry-run mode (with -n)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('preview','true/false preview command when running in dry-run mode (with -n)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chroot','Directory of root sandbox for process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chroot','Directory of root sandbox for process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chdir','promises.cf.html#promise_knowledge_cf_34','0','promise_knowledge_cf_34')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chdir','promises.cf.html#promise_knowledge_cf_34','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('chdir','Directory for setting current/base directory for the process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_timeout','Timeout in seconds for command completion','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_timeout','Timeout in seconds for command completion','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_group','The group name or id under which to run the process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_group','The group name or id under which to run the process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('exec_owner','The user name or id under which to run the process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('umask','The umask value for the child process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('umask','The umask value for the child process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('useshell','true/false embed the command in a shell environment (true)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contain','promises.cf.html#promise_knowledge_cf_34','0','promise_knowledge_cf_34')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contain','promises.cf.html#promise_knowledge_cf_34','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contain','Containment options for the execution process','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('args','Alternative string of arguments for the command (concatenated with promiser string)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_end','Regular expression matches end of edit region from start','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_end','Regular expression matches end of edit region from start','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_start','Regular expression matching start of edit region','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('select_region','Limit edits to a demarked region of the file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','Retained comment about this promise's real intention','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('comment','A comment about this promise's real intention that follows through the program','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('depends_on','A list of promise handles that this promise builds on or depends on somehow (for knowledge management)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('handle','A unique id-tag string for referring to this as a promisee elsewhere','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifvarclass','Extended classes ANDed with context','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','promises.cf.html#promise_site_cf_98','0','promise_site_cf_98')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','promises.cf.html#promise_site_cf_98','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','Whether a persistent class restarts its counter when rediscovered','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('timer_policy','Whether a persistent class restarts its counter when rediscovered','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','promises.cf.html#promise_site_cf_98','0','promise_site_cf_98')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','promises.cf.html#promise_site_cf_98','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','A number of minutes the specified classes should remain active','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('persist_time','A number of minutes the specified classes should remain active','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_74','0','promise_webserver_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_kept','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_timeout','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_timeout','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_denied','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_denied','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_failed','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('repair_failed','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_74','0','promise_webserver_cf_74')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_74','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_62','0','promise_webserver_cf_62')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_webserver_cf_62','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_site_cf_98','0','promise_site_cf_98')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_site_cf_98','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_repaired','A list of classes to be defined','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes','Signalling behaviour','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('measurement_class','If set performance will be measured and recorded under this identifier','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_level','The reporting level for standard output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_level','The reporting level for standard output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_site_cf_148','0','promise_site_cf_148')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_site_cf_148','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_site_cf_48','0','promise_site_cf_48')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','promises.cf.html#promise_site_cf_48','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','true/false switch for parallelizing the promise repair','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('background','true/false switch for parallelizing the promise repair','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('audit','true/false switch for detailed audit records of this promise','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('audit','true/false switch for detailed audit records of this promise','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_failed','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_failed','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_repaired','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_repaired','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_kept','This should be filename of a file to which log_string will be saved, if undefined it goes to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_level','The reporting level sent to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_level','The reporting level sent to syslog','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_string','A message to be written to the log when a promise verification leads to a repair','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('log_string','A message to be written to the log when a promise verification leads to a repair','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_148','0','promise_site_cf_148')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_148','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_141','0','promise_site_cf_141')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_141','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_48','0','promise_site_cf_48')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','promises.cf.html#promise_site_cf_48','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','Number of minutes before a repair action is interrupted and retried','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expireafter','Number of minutes before a repair action is interrupted and retried','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_31','0','promise_laptop_cf_31')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_31','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_22','0','promise_laptop_cf_22')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_laptop_cf_22','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_library_cf_165','0','promise_library_cf_165')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_library_cf_165','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#free_disk_watch','0','free_disk_watch')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#free_disk_watch','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_148','0','promise_site_cf_148')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_148','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_141','0','promise_site_cf_141')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_141','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_48','0','promise_site_cf_48')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_site_cf_48','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_21','0','promise_update_cf_21')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_21','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_13','0','promise_update_cf_13')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','promises.cf.html#promise_update_cf_13','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ifelapsed','Number of minutes before next allowed assessment of promise','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_library_cf_165','0','promise_library_cf_165')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_library_cf_165','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_site_cf_58','0','promise_site_cf_58')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','promises.cf.html#promise_site_cf_58','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action_policy','Whether to repair or report about non-kept promises','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('action','Output behaviour','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('report_to_file','The path and filename to which output should be appended','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('friend_pattern','Regular expression to keep selected hosts from the friends report list','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_to_print','Path name to the file that is to be sent to standard output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_to_print','Path name to the file that is to be sent to standard output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('number_of_lines','Integer maximum number of lines to print from selected file','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('printfile','Quote part of a file to standard output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_345','0','promise_site_cf_345')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_345','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_338','0','promise_site_cf_338')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_338','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_331','0','promise_site_cf_331')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_331','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_324','0','promise_site_cf_324')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_324','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_317','0','promise_site_cf_317')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_317','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_310','0','promise_site_cf_310')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_310','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_303','0','promise_site_cf_303')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','promises.cf.html#promise_site_cf_303','0','used in promise')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('showstate','List of services about which status reports should be reported to standard output','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('intermittency','Real number threshold [0,1] of intermittency about current peers, report above','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastseen','Integer time threshold in hours since current peers were last seen, report absence','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('not','Evaluate the negation of string expression in normal form','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('expression','Evaluate string expression of classes in normal form','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dist','Generate a probabilistic class distribution (strategy in cfengine 2)','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('xor','Combine class sources with XOR','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('and','Combine class sources with AND','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('or','Combine class sources with inclusive OR','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('policy','The policy for (dis)allowing redefinition of variables','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rlist','A list of real numbers','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('ilist','A list of integers','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('slist','A list of scalar strings','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real','A scalar real number','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('int','A scalar integer','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('string','A scalar string','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('sys','cfengine's internal bundle of system specific values','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('An_arbitrary_string','Represent type slist and should match .*','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('An_arbitrary_string','Represent type string and should match .*','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('An_arbitrary_string','Represent type slist and should match ','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('An_arbitrary_string','Represent type string and should match ','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('An_arbitrary_string','Should match the generic pattern .*, i.e. unspecified characters','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('An_arbitrary_string','Should match the generic pattern , i.e. unspecified characters','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_syslog_level','Should match the generic pattern LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7, i.e. a syslog constant','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_file_path','Represent type slist and should match [cC]:\.*|/.*','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_file_path','Represent type string and should match [cC]:\.*|/.*','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_file_path','Should match the generic pattern [cC]:\.*|/.*, i.e. a system file path suitable for the target system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_user_group_id','Represent type slist and should match [a-zA-Z0-9_$.-]+','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_user_group_id','Should match the generic pattern [a-zA-Z0-9_$.-]+, i.e. an alphanumeric string with option underscores and hyphens','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_cfengine_identifier','Represent type string and should match [a-zA-Z0-9_$.]+','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_cfengine_identifier','Represent type slist and should match [a-zA-Z0-9_$.]+','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_cfengine_identifier','Should match the generic pattern [a-zA-Z0-9_$.]+, i.e. an alphanumeric string with option underscores','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_cfengine_class_expression','Should match the generic pattern [a-zA-Z0-9_!&|.()]+, i.e. an alphanumeric string with option underscores and logical operators','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('posix_file_mode_or_permission','Represent type string and should match [0-7augorwxst,+-]+','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('posix_file_mode_or_permission','Represent type slist and should match [0-7augorwxst,+-]+','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('posix_file_mode_or_permission','Should match the generic pattern [0-7augorwxst,+-]+, i.e. something that you would give as an argument to chmod','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_single_character','Represent type string and should match ^.$','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_single_character','Should match the generic pattern ^.$, i.e. one symbol','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real_number','Represent type rlist and should match -9.99999E100,9.99999E100','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real_number','Represent type real and should match -9.99999E100,9.99999E100','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('real_number','Should match the generic pattern -9.99999E100,9.99999E100, i.e. a number between the minus and positive maximum values','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integer','Represent type ilist and should match -99999999999,9999999999','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integer','Represent type int and should match -99999999999,9999999999','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('integer','Should match the generic pattern -99999999999,9999999999, i.e. a number between the minus and positive maximum values','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_positive_integer','Represent type irange [int,int] and should match 0,99999999999','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_positive_integer','Represent type int and should match 0,99999999999','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_positive_integer','Should match the generic pattern 0,99999999999, i.e. a number between zero and the maximum value','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('a_time_range','Should match the generic pattern 0,2147483648, i.e. a value from zero to a maximum system time -- but you should use time functions to convert this','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Represent type (menu option) and should match symlink,hardlink,relative,absolute,none','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('link_type','Should match the generic pattern symlink,hardlink,relative,absolute,none, i.e. a support link type','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('boolean','Represent type (menu option) and should match true,false,yes,no,on,off','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('boolean','Should match the generic pattern true,false,yes,no,on,off, i.e. a positive or a negative','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_signals','Represent type (option list) and should match hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_signals','Should match the generic pattern hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv, i.e. a unix signal name','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('values','Formal rvalues in constraint assignments and their legal ranges','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('functions','In-built functions that may be used to set variables or classes','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('body_constraints','The attributes that cfengine enables you to promise about the promiser','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_types','The types of promise that cfengine can keep','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','Complete occurrences of promiser + promisee + promise-body','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promisers','The objects affected by a promise','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promisees','Recipients of a promise, i.e. promise handles, or persons','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('contexts','Class expressions that say where or when a promise applies','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bodies','A modular collection of body constraints for re-use','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bundles','A modular collection of promises of different types','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('variables_report','Shows the values of variables used on each host. Some discovered, some defined.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('software_patches_report','For systems with a separate patch model, this shows OS software patches. There are two reports (for current status and patches that are available but not already applied)','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('setuid_report','List the discovered setuid/setgid root programs on the system (Unix only)','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('performance_report','Describes the real time it has taken on average to complete or keep promises','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lastseen_report','When was the last time the current host contacted or was contacted by another? (+) indicates contacted, (-) indictaes was contacted by.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('installed_software_report','OS software packages installed and their version information.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('hashes_report','A list of actual computed hash values for promised files.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('file_changes_report','A log of changes to files promised under change monitoring','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('compliance_report','Show the status of each policy promise, along with average compliance. Useful for quick auditing.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('classes_report','Shows non-hard classes defined on the current host. This provides something like CMDB information but at level that can say something about the dynamics of the system. Probabilities that classes have been defined are based on actual observation.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('active_locks_report','Currently active locks -- this should normally be an unstable short list or nothing. Could indicate hung cfengine processes.','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('all_locks_report','A list of locks that have been acquired, indicating promise checks','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('audit_report','Detailed reports with every transaction made by cfengine - very storage consuming','4','description')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('the_tao','topics, associations and occurrences','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lvalue','/inside/manuals/cf3-reference#Types-in-cfengine-3','0','Cfengine reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('lvalue','Refers to the left hand side of an assignment "lvalue => rvalue"','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rvalue','/inside/manuals/cf3-reference#Types-in-cfengine-3','0','Cfengine reference')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('rvalue','Refers to the right hand side of an assignment "lvalue => rvalue"','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('voluntary_cooperation','Describes the approach to modelling in which one does not assume absolute power to control, but rather assumes best effort of the system parts. A promise is an expression of voluntary cooperation - the opposite of an obligation.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('versions','An indentity string that distinguishes altered states of an agent or occurrence of a topic.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('dependencies','Promises or promisees that provide the pre-requisites for conditional promises may be called dependencies of the conditional promise.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('workflows','A representation or story about actions or services. ','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('stories','A sequence of topics that relates a start and an end point through a topic map. Different
aths through a topic map can relate different stories. An entire story promises to have some intention.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('services','An abstraction for actions or processes that are associated with a specific set of promises. Services are economically motivated black-boxes for promises.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('communicated_information','Any information that is transmitted falls into this category. It is a container class for more specific messages, e.g. promises.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('subjects','A subject is an abstract concept -- it is any thing that can be talked about or discussed.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('subjects','Any thing that can be discussed','4','Explanation')
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
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','Assertions about the desired properties of a system','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promises','When an agent's intention becomes public knowledge we call it a promise.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_proposals','Before a promise has been accepted and made it can be written down as a proposal. A contract contains many promise proposals, for instance.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bundles','A container for grouping related promises','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('bundles','Collections of promises are called bundles.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('intentions','Ideas that might lead to courses of action and some time past, present or future.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('agents','Describes any system components that can give or receive promises, either physical or abstract; e.g. files, processes, topics, etc.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('promise_theory','A model for representing systems of interacting parts, such as computer networks. Orginally described by Mark Burgess to describe cfengine as no other models were suitable.','4','definitions')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copernicus','https://cfengine.com/copernicus/','0','Support knowledge')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('copernicus','Cfengine's online support knowledge base','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_reports','Reports generated by cfengine','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_reports','Reports from around the system','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('system_policy','Local policy description with comments and dependencies','4','Explanation')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine_knowledge_base','/img/cfknowledge.png','5','Origin')
INSERT INTO occurrences (topic_name,locator,locator_type,subtype) values ('cfengine_knowledge_base','Cfengine knowledge console','4','Explanation')
