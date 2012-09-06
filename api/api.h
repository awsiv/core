#ifndef CFENGINE_API_H
#define CFENGINE_API_H

#include "api-php.h"

/*
  NAMING CONVENTION FOR CFAPI FUNCTIONS.

  Each function corresponds to a REST call as intercepted by the HTTP server.
  URL paths separated by '/' have a corresponding '_' in CFAPI function names,
  ending with the HTTP verb for the request.

  Example:

  The resource /api/user lets you manage users.

  GET /api/user => cfapi_user_list (List all the users)
  GET /api/user/:username => cfapi_user_get (Get user details)
  PUT /api/user/:username => cfapi_user_put (Add/overwrite a user)
  DELETE /api/user/:username => cfapi_user_delete (Delete a user)

  etc.
*/

PHP_FUNCTION(cfapi);

/**
  Authenticates any request and as such is not bound to a resource
  */
PHP_FUNCTION(cfapi_auth);

// user
PHP_FUNCTION(cfapi_user_list);
PHP_FUNCTION(cfapi_user_get);
PHP_FUNCTION(cfapi_user_put);
PHP_FUNCTION(cfapi_user_post);
PHP_FUNCTION(cfapi_user_delete);
PHP_FUNCTION(cfapi_user_subscription_query_list);
PHP_FUNCTION(cfapi_user_subscription_query_get);
PHP_FUNCTION(cfapi_user_subscription_query_put);
PHP_FUNCTION(cfapi_user_subscription_query_delete);

// role
PHP_FUNCTION(cfapi_role_list);
PHP_FUNCTION(cfapi_role_get);
PHP_FUNCTION(cfapi_role_put);
PHP_FUNCTION(cfapi_role_post);
PHP_FUNCTION(cfapi_role_delete);

// settings
PHP_FUNCTION(cfapi_settings_get);
PHP_FUNCTION(cfapi_settings_post);

// host
PHP_FUNCTION(cfapi_host_list);
PHP_FUNCTION(cfapi_host_get);
PHP_FUNCTION(cfapi_host_context_list);
PHP_FUNCTION(cfapi_host_vital_list);
PHP_FUNCTION(cfapi_host_vital_get);

/**
  Reporting engine
  */
PHP_FUNCTION(cfapi_query_post);

#endif
