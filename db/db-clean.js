print("====================");
print("cfreport.hosts clean");
print("====================");

missing_ck = db["hosts"].count({ck: { $exists: false }});
print("Found " + missing_ck + " hosts with missing class keys");
if (missing_ck > 0) {
    print("Removing all host records with missing class keys");
    db["hosts"].remove({"ck": { $exists: false }});
    print("Done");
}

empty_ck = db["hosts"].count({"ck": []});
print("Found " + empty_ck + " hosts with empty class keys");
if (empty_ck > 0) {
    print("Removing all host records with empty class keys");
    db["hosts"].remove({"ck": []});
    print("Done");
}

print("Finding all host records with duplicates")

db["hosts"].mapReduce(
    function() {
	var t = 0;
	if (typeof this.t === "number") {
	    t = this.t
	}

	emit(this.kH, {
	    id: this._id,
	    timestamp: t,
	    duplicates: []
	});
    },
    function(key, vals) {
	var res = {
	    id: null,
	    timestamp: -1,
	    duplicates: []
	};

	vals.forEach(function(val) {
	    if (val.timestamp < res.timestamp) {
		res.duplicates.push(val.id);
	    }
	    else {
		if (res.id !== null) {
		    res.duplicates.push(res.id);
		}
		res.id = val.id;
		res.timestamp = val.timestamp;
	    }
	});

	return res;
    },
    {
	out: "dups"
    }
);

db["dups"].find().forEach(function(obj) {
    if (obj.value.duplicates.length >= 1) {
	print("Removing duplicates for key: " + obj._id);
	obj.value.duplicates.forEach(function(id) {
	    db["hosts"].remove({"_id": id});
	});
    }
});

print("Done");