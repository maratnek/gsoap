/*
	curlclient.cpp

	Example CURL-based calculator service client in C++

	Service definitions in calc.h (not generated by wsdl2h, but similar)

	$ soapcpp2 -j calc.h
	$ c++ -Iplugin -I. -o curlclient curlclient.cpp stdsoap2.cpp soapC.cpp soapcalcProxy.cpp plugin/curlapi.c -lcurl
	where stdsoap2.cpp is in the 'gsoap' directory, or use libgsoap++:
	$ c++ -Iplugin -I. -o curlclient curlclient.cpp soapC.cpp soapcalcProxy.cpp plugin/curlapi.c -lcurl -lgsoap++

--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2001-2017, Robert van Engelen, Genivia, Inc. All Rights Reserved.
This software is released under one of the following two licenses:
GPL or Genivia's license for commercial use.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#include "soapcalcProxy.h"
#include "calc.nsmap"
#include "curlapi.h"

const char server[] = "http://websrv.cs.fsu.edu/~engelen/calcserver.cgi";

int main(int argc, char **argv)
{ if (argc < 4)
  { fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");
    exit(0);
  }
  /* should init curl once - this call is not thread safe */
  curl_global_init(CURL_GLOBAL_ALL);
  /* create curl handle and set options */
  CURL *curl = curl_easy_init();
  /* init soap context (with mode parameter) */
  calcProxy calc(SOAP_IO_CHUNK | SOAP_XML_INDENT);
  /* set endpoint URL */
  calc.soap_endpoint = server;
  /* or you can set the CURL endpoint:
     calc.soap_endpoint = "";
     curl_easy_setopt(curl, CURLOPT_URL, server);
     */
  /* register the soap_curl plugin (-j option used, with -i option use &calc below) */
  soap_register_plugin_arg(calc.soap, soap_curl, curl);
  /* or you can simply call w/o setting up a curl handle (the plugin uses a temporary handle):
     soap_register_plugin(&calc, soap_curl);
     */
  /* set timeouts, also used by the plugin */
  calc.soap->connect_timeout = 60;
  calc.soap->send_timeout = 10;
  calc.soap->recv_timeout = 10;
  double a, b, result;
  a = strtod(argv[2], NULL);
  b = strtod(argv[3], NULL);
  int res = SOAP_OK;
  switch (*argv[1])
  { case 'a':
      res = calc.add(a, b, &result);
      break;
    case 's':
      res = calc.sub(a, b, &result);
      break;
    case 'm':
      res = calc.mul(a, b, &result);
      break;
    case 'd':
      res = calc.div(a, b, &result);
      break;
    case 'p':
      res = calc.pow(a, b, &result);
      break;
    default:
      fprintf(stderr, "Unknown command\n");
      exit(0);
  }
  if (res != SOAP_OK)
  {
    soap_curl_reset(calc.soap);
    calc.soap_stream_fault(std::cerr);
  }
  else
    printf("result = %g\n", result);
  calc.destroy(); /* clean up mem */
  /* cleanup handle */
  curl_easy_cleanup(curl);
  /* we're done so clean up curl */
  curl_global_cleanup();
  return 0;
}

