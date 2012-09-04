print("===============================");
print("cfreport.hosts clean duplicates");
print("===============================");

print("Removing all host records with missing or empty class keys")
db["hosts"].remove({"ck": { $exists: false }});
db["hosts"].remove({"ck": []});