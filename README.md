How to use mod_script_nonce module

Add the following in  conf/httpd.conf 
	LoadModule script_nonce_module modules/mod_script_nonce.so

	<Location />¬                                                                    
    	 AddOutputFilterByType ADD-SCRIPT-NONCE text/html                            
         Addscriptnonce s/server-secret/                          
	</Location>


How to compile and install the module:

	apxs -iac <path to mod_script_nonce.c>  -I <path to apr-util and apr source files>



Lalit, Ameya and GT

Instructions:
Install Chrome Dev build. Stable installations of chrome do not support script-nonce yet.

How to install Chrome Dev build ? 
On Ubuntu: 
#If google chrome is already installed
sudo apt-purge google-chrome-stable
sudo apt-get install google-chrome-unstable


I tested this on Chrome 28.0.1496.0 (Official Build 197749) dev

How to see script-nonce in action ?
in sample.html file, uncomment the meta directive in the head section of the page to cross-check if the browser supports script-nonce.

meta -> an alternative way to send HTTP headers in a web-page.

Now, in our sample module, we are adding the same HTTP header.(read mod_example1.c)
Benefit of adding the HTTP header in apache:
The web-developer need not worry about enforcing the script-nonce.
The web-admin gets to decide the policy for the pages served from an apache installation. This is a huge win. 

This will take care of stored XSS attacks.


Approach:
mod_example1 will generate a nonce and set the HTTP header.
and set the notes nonce-s, with value of the form s|pattern|nonce|ni

This nonce-s will be used by the tweaked mod_substitute to substitute all the
occurrences of the pattern in the response.

Lalit pointed out that we could simply hack mod_substitute to our own needs and avoid using request->notes. And so we did.

Implemented Approach:
Hacked mod_substitute to replace the given string (mentioned in httpd.conf) with a nonce, that is generated per request.
