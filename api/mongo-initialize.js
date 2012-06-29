/* Usage: /var/cfengine/bin/mongo cfreport mongo-initialize-admin.js*/

/** roles records **/
if(!db["roles"].findOne())
{
    db["roles"].insert({
	"_id": ObjectId("4df78a754a235ac761000000"),
	"name": "admin"
    });
}

/** users records **/
if(!db["users"].findOne())
{
    db["users"].insert({
	"_id" : ObjectId("4fed9e33b88e310e7c076063"), 
	"username" : "admin", 
	"password" : "SHA=aa459b45ecf9816d472c2252af0b6c104f92a6faf2844547a03338e42e426f52", 
	"salt" : "eWAbKQmxNP",
	"active": 1,
	"roles": [
	    "admin"
	],
    });
}