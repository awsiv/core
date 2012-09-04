print("==========================");
print("cfreport.hosts diagnostics");
print("==========================");

total = db["hosts"].count();
print("TOTAL RECORDS: " + total);

distinct = db["hosts"].distinct("kH");
print("TOTAL DISTINCT: " + distinct.length);
print("TOTAL DUPLICATES: " + (total - distinct.length));

missing_ck = db["hosts"].count({ck: { $exists: false }});
print("MISSING CK: " + missing_ck);

empty_ck = db["hosts"].count({ck: []});
print("EMPTY CK: " + empty_ck);

missing_t = db["hosts"].count({t: { $exists: false }});
print("MISSING TIMESTAMP: " + missing_t);

// list duplicates
groups = db["hosts"].group({
    key: { kH: true },
    initial: { count: 0 },
    reduce: function(record, state) {
	state.count++;
    }
});

duplicates = [];
for (i = 0; i < groups.length; i++) {
    record = groups[i];
    if (record.count > 1) {
	duplicates.push(record);
    }
}

print("Duplicate record groups: " + duplicates.length); 

for (i = 0; i < duplicates.length; i++)
{
    print(duplicates[i].kH + " : " + duplicates[i].count + " times");
}
