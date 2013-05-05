/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int example_handler(request_rec *r);

/* Define our module as an entity and assign a function for registering hooks  */



module AP_MODULE_DECLARE_DATA   example_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            // Per-directory configuration handler
    NULL,            // Merge handler for per-directory configurations
    NULL,            // Per-server configuration handler
    NULL,            // Merge handler for per-server configurations
    NULL,    // Any directives we may have for httpd
    register_hooks   // Our hook registering function
};


/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{
    
    /* Hook the request handler */
    ap_hook_handler(example_handler, NULL, NULL, APR_HOOK_LAST);
}

/* The handler function for our module.
 * This is where all the fun happens!
 */

static int example_handler(request_rec *r)
{
    FILE *fp = NULL; 
    int i = -1;
    char *sub = NULL;
    ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r,
                  "Inside example_handler: %s, %s", r->filename, r->handler);

    // Is this redundant ? -> No - Log messages indicate that handler gets called 
    // for each request
    if (!r->handler || strcmp(r->handler, "example-handler")) return (DECLINED);

    apr_table_add(r->headers_out, "TEST", "jimmy");

    fp = fopen(r->filename,"r"); 

    if (fp == NULL) {
        switch (errno) {
        case ENOENT:
            return HTTP_NOT_FOUND;
        case EACCES:
            return HTTP_FORBIDDEN;
        default:
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "open error, errno: %d\n", errno);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    char buf[1024];
    size_t len;
    while (1) {
        len = fread(buf, 1, 1024, fp);
        if (len <= 0) {
            break;
        }

	do {
		sub = strstr(buf, "dummy");

		if(sub) {
		//replacing jimmy with dummy
			i = sub - buf;
			buf[i] = 'j';
                	buf[i+1] = 'i';
	  	}
		//We need to handle the case when jimmy is spread
                //across 1024 buffer boundary
		//keeping it simple for now as this is just POC
	} while(sub);

        ap_rwrite(buf, len, r);
    }
 

 //
 //   // The first thing we will do is write a simple "Hello, world!" back to the client.
//    ap_rputs("<html> Hello, world! 23234<br/> </html>", r);
    return OK;
}