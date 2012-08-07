/** appsettings records **/
if(db["appsettings"].findOne())
{
    /* New addition in Nova 2.2.0*/
    db["appsettings"].update({},{$set:{
	"bluehost_threshold_global": "900",
	"external_admin_username": "",
	"rbac" : "false",
        "admin_role":"admin",
        "fall_back_for":"admin",        
        "business_room": "1",
        "planning_room": "1"
    }}, false, true);
}
else
{
    db["appsettings"].insert({
	"_id": ObjectId("4e36aa774a235a3e49000003"),
	"active_directory_domain": "",
	"admin_role": "admin",
	"appemail": "",
	"base_dn": "",
	"bluehost_threshold_global": "900",
	"encryption": "plain",
	"experimental": true,
	"external_admin_username": "",
	"fall_back_for": "admin",
	"host": "",
	"login_attribute": "",
	"member_attribute": "",
	"mode": "database",
	"rbac": "false",
	"users_directory": "",
        "business_room": "1",
        "planning_room": "1"        
    });
}

/** onlineusers records **/
if(!db["onlineusers"].findOne())
{
    db["onlineusers"].insert({
	"_id": ObjectId("4f7c87d54a235a594a000000"),
	"ip": "10.0.0.42",
	"user_id": "admin",
	"time": 1333561301,
	"uri": "\/index.php\/engineering",
	"bot": false
    });
}

/** roles records **/
if(db["groups"].findOne())
{
    /* groups collection is called roles from Nova 2.2.0*/
    db["groups"].find().forEach( function(x){db["roles"].insert(x)} );
    db["groups"].drop();
}
else if(!db["roles"].findOne())
{
    db["roles"].insert({
	"_id": ObjectId("4df78a754a235ac761000000"),
	"name": "admin"
    });
}

/** users records **/
if(db["users"].findOne())
{
    /* groups renames to roles from Nova 2.2.0*/
    db["users"].update({},{$rename:{
	"group": "roles"
    }}, false, true);
}
else
{
    db["users"].insert({
	"_id": ObjectId("4df786844a235a7d61000002"),
	"active": 1,
	"created_on": 1308067460,
	"email": " ",
	"ip_address": "10.0.0.9",
	"last_login": 1333561299,
	"password": "fbca3dac873569d2152483c148e14eb5d84a1e87",
	"roles": [
	    "admin"
	],
	"username": "admin"
    });
}
