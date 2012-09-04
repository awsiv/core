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
db["hosts"].mapReduce(
    function() {
	emit(this.kH, { count: 1 });
    },
    function(key, values) {
	var result = { count: 0 };

	values.forEach(function(value) {
	    result.count += value.count
	});

	return result;
    },
    { 
	out: "dups"
    }
);

print("Duplicate keys");
db["dups"].find({"value.count": { $gt: 1 }}).forEach(function(obj) {
    print(obj._id + " count: " + obj.value.count);
});
