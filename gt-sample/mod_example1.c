/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"
#include "apr_general.h"
#include "apr_strings.h"
#include "apr_errno.h"

#define LENGTH_OF_RANDOM_VALUE_STRING 256
#define LENGTH_OF_BYTE 8

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

void get_random_byte_string(char *buffer, int size)
{
    int i;
//    if( apr_generate_random_bytes(buffer, size) == APR_SUCCESS )
//        for(i=0; i < (size - 1);i++)
//            if(0x80 & buffer[i])
//                buffer[i] = 0xff ^ buffer[i];

    for(i=0; i< (size - 1);i++)
	buffer[i] = 'y';

    buffer[size-1] = '\0';
}
/* The handler function for our module.
 * This is where all the fun happens!
 */
static int example_handler(request_rec *r)
{
    FILE *fp = NULL;
    int i = -1;
    char *sub = NULL;

    int size = LENGTH_OF_RANDOM_VALUE_STRING / LENGTH_OF_BYTE;
    size = size + 1; // for null character
    char random_buffer[size];

    ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r,
        "Inside example_handler: %s, %s", r->filename, r->handler);


    // Is this redundant ? -> No - Log messages indicate that handler gets called
    // for each request
    if (!r->handler || strcmp(r->handler, "example_handler")) return (DECLINED);

    ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r,
        "Inside example_handler: %s, %s", r->filename, r->handler);

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

    get_random_byte_string(random_buffer, size);

    ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r,
        "random_buffer: %s", random_buffer);

    ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r, "header value : %s", 
           apr_pstrcat(r->pool, "script-nonce ", random_buffer,"; ", NULL));

    { //search and replace block
    	char buf[1024];
    	size_t len;

    	while (1) {
    		len = fread(buf, 1, 1024, fp);
    	    	if (len <= 0) {
    	        	break;
    	    	}

    		do {
    		    sub = strstr(buf, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		    if(sub) {
			i = sub - buf;
		    	for(int j=0; j < 32; j++, i++) {
    		    	//replacing jimmy with dummy
    		    	    buf[i] = random_buffer[j];
    		    	}
	            }
    		    //We need to handle the case when jimmy is spread
    		            //across 1024 buffer boundary
    		//keeping it simple for now as this is just POC
    		} while(sub);

    		ap_rwrite(buf, len, r);
    	}

    } //end of search and replace block

    apr_table_add(r->headers_out, "Content-Security-Policy", 
             apr_pstrcat(r->pool, "script-nonce ", random_buffer,"; ", NULL));

    return OK;
}
