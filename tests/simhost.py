#!/usr/bin/python
###############################################################################
#
# simhost.py
#
# This script simulates a host update to the hub.
#
###############################################################################

import time
import pymongo
import random
import re
import argparse
import abc

def canonify(string):
    return re.sub("[\W\.]", '_', string)

class SimHost:

    def __init__(self, mongo_connection, hostkey, hostname, ip, timestamp):
        self.conn = mongo_connection
        self.hostkey = hostkey
        self.hostname = hostname
        self.ip = ip
        self.timestamp = timestamp

    def update_contexts(self, extra_entries):
        """
        extra_entries is a dict of 'cl' entries, e.g.
        extra_entries[context] = { "e": 0.8, "d": 0.5, "t": 124 }
        """
        entries = dict()
        entries["PK_" + canonify(self.hostkey)] = { "e": 1.0, "d": 0.0, "t": self.timestamp }
        entries[canonify(self.ip)] = { "e": 1.0, "d": 0.0, "t": self.timestamp }
        entries["ipv4_" + canonify(self.ip)] = { "e": 1.0, "d": 0.0, "t": self.timestamp }
        entries.update(extra_entries)

        self.conn.cfreport.hosts.update({ "kH": self.hostkey }, {"$set":{ "ck": entries.keys(), "cl": entries }})

    def get_logs_notkept(self):
        host_entry = self.conn.cfreport.hosts.find_one({"kH": self.hostkey}, {"logs_nk": 1})
        if host_entry and 'logs_nk' in host_entry:
            return host_entry['logs_nk']
        else:
            return None

    def get_logs_repaired(self):
        host_entry = self.conn.cfreport.hosts.find_one({"kH": self.hostkey}, {"logs_rep": 1})
        if host_entry and 'logs_rep' in host_entry:
            return host_entry['logs_rep']
        else:
            return None

    def update_logs_notkept(self, entries):
        """
        entries is a dict of log enties, e.g.
        entries["handle" + "@" + "cause"] = { "h": "handle", "ca": "cause", t: [1, 2, 3] }
        """
        self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        { "$set": { "logs_nk": entries } }, upsert = True)

    def update_logs_repaired(self, entries):
        """
        entries is a dict of log enties, e.g.
        entries["handle" + "@" + "cause"] = { "h": "handle", "ca": "cause", t: [1, 2, 3] }
        """
        self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        { "$set": { "logs_rep": entries } }, upsert = True)

    def update_software(self, entries):
        """
        entries is a list of software entries, e.g.
        entries = [ { "n": "name", "v": "v1", "a": "x" } ]
        """
        self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        { "$set": { "sw": entries } }, upsert = True)


    @abc.abstractmethod
    def update(self):
        self.conn.cfreport.hosts.update({ "kH": self.hostkey },
                                        {"$set":{ "kH": self.hostkey,
                                          "ha": [ self.hostname ],
                                          "ip": [ self.ip ],
                                          "t": self.timestamp
                                        }},
                                        upsert = True)


class RealisticSimHost(SimHost):

    def __init__(self, mongo_connection, hostkey, hostname, ip, timestamp):
        SimHost.__init__(self, mongo_connection, hostkey, hostname, ip, timestamp)

    def __update_contexts(self):
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

        sample = random.sample(ck_catalog, min(len(ck_catalog), random.gauss(float(len(ck_catalog)), 10.0)))

        cl = {}
        for context in sample:
            cl[context] = { "e": random.gauss(0.5, 0.3),
                            "d": random.gauss(0.5, 0.1),
                            "t": self.timestamp
                          }

        SimHost.update_contexts(self, cl)

    def __update_logs_notkept(self):
        nk_data = {};
        for i in range(10):
            handle = 'cfengine_limit_robot_agents_processes_kill_cf_monitord_' + str(1);
            cause = " !! Couldn't send promised signal 'kill' (9) to pid 17409 (might be dead) " + str(i);
            nk_key = handle + "@" + cause;
            t = self.timestamp

            nk_value = {};
            nk_value["h"] = handle;
            nk_value["ca"] = cause;
            time_array = [];

            for j in range(100):
                time_array.append( t - j)

            nk_value["t"] = time_array;

            nk_data[nk_key] = nk_value;

        SimHost.update_logs_notkept(self, nk_data)

    def update(self):
        SimHost.update(self)
        self.__update_contexts()
        self.__update_logs_notkept()

class PredictableSimHost(SimHost):

    def __init__(self, mongo_connection, hostkey, hostname, ip, timestamp):
        SimHost.__init__(self, mongo_connection, hostkey, hostname, ip, timestamp)

    def __update_contexts(self):
        key = canonify("time_" + str(self.timestamp))
        entries = dict()
        entries[key] = { "e": 1.0, "d": 0.0, "t": self.timestamp }
        SimHost.update_contexts(self, entries)

    def __update_logs_repaired(self):
        entries = dict()
        handle = "repaired_handle"
        cause = "repaired_cause"
        key = handle + "@" + cause
        existing_logs = self.get_logs_repaired()
        if not existing_logs:
            record = { "ha": handle, "ca": cause, "t": [] }
        else:
            record = existing_logs[key]
        record['t'].append(self.timestamp)
        entries[key] = record
        SimHost.update_logs_repaired(self, entries)


    def __update_logs_notkept(self):
        entries = dict()
        handle = "notkept_handle"
        cause = "notkept_cause"
        key = handle + "@" + cause
        existing_logs = self.get_logs_notkept()
        if not existing_logs:
            record = { "ha": handle, "ca": cause, "t": [] }
        else:
            record = existing_logs[key]
        record['t'].append(self.timestamp)
        entries[key] = record
        SimHost.update_logs_notkept(self, entries)

    def __update_software(self):
        entries = []
        name = 'software1'
        version = 'v1'
        arch = 'x'
        entries.append({ "n": name, "v": version, "a": arch })
        SimHost.update_software(self, entries)


    def update(self):
        SimHost.update(self)
        self.__update_contexts()
        self.__update_logs_notkept()
        self.__update_logs_repaired()
        self.__update_software()


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description = 'Simulate a CFEngine host update to the database')
    parser.add_argument('hostkey', help='ID of host to simulate')
    parser.add_argument('hostname', help='Hostname of host to simulate')
    parser.add_argument('ip', help='IPv4 address of host to simulate')
    parser.add_argument('--time', help='Override update time written to DB (Unix time)', action='store', dest='time', type=int, default=int(time.time()))
    parser.add_argument('--db_host', help='Hostname of DB to connect to', action='store', dest='db_host', default='localhost')
    parser.add_argument('--db_port', help='Port of DB to connect to', action='store', dest='db_port', type=int, default=27017)
    parser.add_argument('--predictable', help='Use the predictable host simulation', action='store_true', dest='predictable', default=False)

    args = parser.parse_args()

    conn = pymongo.Connection(args.db_host, args.db_port)
    if args.predictable:
        host = PredictableSimHost(conn, args.hostkey, args.hostname, args.ip, args.time)
    else:
        host = RealisticSimHost(conn, args.hostkey, args.hostname, args.ip, args.time)

    host.update()
