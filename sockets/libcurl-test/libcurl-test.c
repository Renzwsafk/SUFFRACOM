#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#define ERROR_EXIT(_e, ...) if (!_e) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }
#define ERROR_RESULT(_e, _r, ...) if (_e != _r) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

int main(int argc, char **argv)
{
	CURL *curl;
	CURLcode res;

	ERROR_EXIT((curl = curl_easy_init()), "Curl error: initialization failed\n");	
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");

	res = curl_easy_perform(curl);
	ERROR_RESULT(res, CURLE_OK, "Curl download error: %s\n", curl_easy_strerror(res));

	curl_easy_cleanup(curl);

	exit(EXIT_SUCCESS);
}
