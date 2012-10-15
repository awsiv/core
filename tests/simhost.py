#!/usr/bin/python
###############################################################################
#
# simhost.py
#
# This script simulates a host update to the hub.
#
###############################################################################

import sys
import time
import pymongo
import random
import re

def canonify(string):
    return re.sub("[\W\.]", '_', string)

class SimHost:
    
    def __init__(self, mongo_connection, hostkey, hostname, ip):
        self.conn = mongo_connection
        self.hostkey = hostkey
        self.hostname = hostname
        self.ip = ip

       

    def __update_classes(self):
        ck_catalog = [ "x86_64", "users_high", "redhat",  "otherprocs_high", "nova_edition", 
                       "nova_2_3_0_a1_4c17320", "nova_2_3_0_a1", "nova_2_3_0", "nova_2_3", "nova_2", 
                       "nova", "net_iface_eth0", "mem_total_high_normal", "mem_swap_high_normal",
                       "mem_freeswap_high_normal", "loadavg_high_ldt", "mem_cached_high_normal", 
                       "mac_52_54_00_14_3a_29", "loghost", "cpu0_high_ldt", 
                       "linux_x86_64_2_6_18_274_17_1_el5__1_SMP_Tue_Jan_10_17_25_58_EST_2012", 
                       "linux_x86_64_2_6_18_274_17_1_el5", "linux_x86_64", 
                       "linux_2_6_18_274_17_1_el5", "linux", "ipv4_172_20", "ipv4_172", 
                       "entropy_postgresql_out_low", "entropy_postgresql_in_low", "entropy_misc_out_low",
                       "entropy_misc_in_low", "enterprise_edition", "enterprise_2_3_0_a1_4c17320",
                       "enterprise_2_3_0_a1", "enterprise_2_3_0", "enterprise_2_3", "enterprise_2", 
                       "enterprise", "compiled_on_linux_gnu", "com", "cfengine_in_high", "cfengine_com", 
                       "cfengine_3_5_0a1_787a246", "cfengine_3_5_0a1", "cfengine_3_5", "cfengine_3",
                       "cfengine", "centos_5_7", "centos_5", "centos", "agent", "64_bit", "1_cpu", "ssh_in_high",
                       "cpu_high_ldt", "loadavg_normal", "mem_free_high_normal", "io_writtendata_normal", 
                       "io_writes_normal", "cfengine_out_high_ldt", "entropy_cfengine_in_low", "entropy_ssh_in_low",
                       "loadavg_high", "io_writes_high_ldt", "messages_high_ldt", "io_writtendata_high_ldt", 
                       "diskfree_high", "entropy_cfengine_out_low", "rootprocs_high", "io_writes_low_normal", 
                       "mem_free_high", "cpu0_low", "io_writtendata_low", "mem_cached_high","loadavg_low", "cpu_low",
                       "ssh_in_normal", "cfengine_out_low", "io_writes_low", "messages_low_normal", "messages_high_normal", 
                       "cpu_high", "messages_high_dev1", "rootprocs_high_ldt", "cpu0_high", "mem_free_low_dev1",
                       "io_writtendata_high", "io_writes_high", "io_readdata_low", "messages_low_dev1", "io_writes_low_dev1",
                       "cfengine_in_low", "otherprocs_low", "otherprocs_high_ldt", "messages_low_dev2", "mem_free_high_ldt",
                       "cfengine_in_normal", "mem_total_high", "mem_free_low_dev2", "mem_free_low_anomaly", "cpu0_normal",
                       "cpu_normal", "io_writes_high_normal", "mem_cached_normal", "mem_cached_low", "rootprocs_normal", 
                       "mem_free_low", "mem_cached_high_ldt", "io_reads_high_ldt", "io_reads_high_dev2", "io_readdata_high_ldt",
                       "mem_free_low_microanomaly", "io_writes_high_dev1", "cfengine_out_high", "cpu_high_dev1", "cpu0_high_dev1",
                       "io_reads_low", "cfengine_in_high_ldt", "entropy_cfengine_in_high", "mem_free_normal",
                       "service_databaseserver", "location_paris", "centos5_12", "centos5_12_stage_cfengine_com" ]
        
        sample = random.sample(ck_catalog, min(len(ck_catalog), random.gauss(len(ck_catalog), 10.0)))
        sample.append("PK_" + canonify(self.hostkey));
        sample.append(canonify(ip))
        sample.append("ipv4_" + canonify(ip))
        
        cl = {}
        for context in sample:
            cl[context] = { "e": random.gauss(0.5, 0.3),
                            "d": random.gauss(0.5, 0.1),
                            "t": int(time.time())
                          }
        
        self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        {"$set":{ "ck": sample,
                                          "cl": cl 
                                        }})

    def __update_logs_notkept(self):
        nk_data = {};
        for i in range(10):
            handle = 'cfengine_limit_robot_agents_processes_kill_cf_monitord_' + str(1);
            cause = " !! Couldn't send promised signal 'kill' (9) to pid 17409 (might be dead) " + str(i);
            nk_key = handle + "@" + cause;
            t = int(time.time());

            nk_value = {};
            nk_value["h"] = handle;
            nk_value["ca"] = cause;
            time_array = [];

            for j in range(100):
                time_array.append( t - j)

            nk_value["t"] = time_array;

            nk_data[nk_key] = nk_value;

            self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        {"$set":{
                                            "logs_nk": nk_data
                                        }},
                                        upsert = True)

    def update(self):
        # update host id
        self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        {"$set":{ "kH": self.hostkey,
                                          "ha": [ self.hostname ], 
                                          "ip": [ self.ip ],
                                          "t": int(time.time())
                                        }},
                                        upsert = True)
        self.__update_classes()
        self.__update_logs_notkept()
        
                                        



def print_usage():
    print 'Usage: simhost.py <db-host> <db-port> <host-key> <host-name> <host-ip>'
    sys.exit()

if __name__ == '__main__':
    
    if len(sys.argv) != 6:
        print_usage()
        sys.exit()

    db_host = sys.argv[1]
    db_port = 0
    try:
        db_port = int(sys.argv[2])
    except ValueError:
        print 'Invalid port number'
        sys.exit()
    
    hostkey = sys.argv[3]
    hostname = sys.argv[4]
    ip = sys.argv[5]

    conn = pymongo.Connection(db_host, db_port)
    host = SimHost(conn, hostkey, hostname, ip)
    host.update()
    