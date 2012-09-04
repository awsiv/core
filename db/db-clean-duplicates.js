print("===============================");
print("cfreport.hosts clean duplicates");
print("===============================");

print("Removing all host records with duplicates, keeping the newest version")

groups = db["hosts"].group({
    key: { kH: true },
    initial: { count: 0, t: -10000, remove: [], best: 0 },
    reduce: function(record, state) {
	state.count++;
	if (typeof record.t !== "number") {
	    if (state.count > 1) {
		state.remove.push(record._id);
	    }
	    state.best = record._id;
	}
	else if (record.t < state.t) {
	    state.remove.push(record._id);
	}
	else {
	    state.t = record.t;
	    if (state.best !== 0) {
		state.remove.push(state.best);
	    }
	    state.best = record._id;
	}
    }
});

remove_keys = [];
for (i = 0; i < groups.length; i++) {
    record = groups[i];
    if (record.count > 1) {
	remove_keys = remove_keys.concat(record.remove);
    }
}

for (i = 0; i < remove_keys.length; i++)
{
    dup = remove_keys[i];
    print("Removing duplicate: " + dup);
    db["hosts"].remove(ObjectId(dup));
}

