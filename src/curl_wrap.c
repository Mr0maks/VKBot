#include "curl/curl.h"

#include "vkapi.h"

#include "dynamic_strings.h"

size_t curl_dynamic_string_writefunc( void *ptr, size_t size, size_t nmemb, void *data )
{
  string_t s = (string_t)data;

  if(ptr == NULL)
    return 0;

#ifdef DEBUG
  const char *ptr_dbg = (const char *)ptr;
  string_strncat(s, ptr_dbg, size*nmemb);
#else
  string_strncat(s, (const char*)ptr, size*nmemb);
#endif

  return size*nmemb;
}

void string_memcpy( string_t s, const void *data, size_t size );

size_t curl_dynamic_string_writefunc_binary( void *ptr, size_t size, size_t nmemb, void *data )
{
  string_t s = (string_t)data;

  if(ptr == NULL)
    return 0;

  string_memcpy(s, ptr, size*nmemb);

  return size*nmemb;
}

vkapi_boolean curl_get( void *curl_handle, string_t url, string_t useragent, string_t dataptr )
{
  curl_easy_reset(curl_handle);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url->ptr);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

  if(useragent)
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, useragent->ptr);

  if(dataptr)
    {
      curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_dynamic_string_writefunc_binary);
      curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, dataptr);
    }

  CURLcode error_code = curl_easy_perform(curl_handle);

  if(error_code != CURLE_OK)
    {
      printf("libcurl error: %s\n", curl_easy_strerror(error_code));
      return false;
    }

  return true;
}

vkapi_boolean curl_post( void *curl_handle, const char *url, string_t post, string_t useragent, string_t dataptr )
{
  curl_easy_reset(curl_handle);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
  //curl_easy_setopt(object->curl_handle, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post->ptr );
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, post->len );

  if(useragent)
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, useragent->ptr);

  if(dataptr)
    {
      curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_dynamic_string_writefunc_binary);
      curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, dataptr);
    }

  CURLcode error_code = curl_easy_perform(curl_handle);

  if(error_code != CURLE_OK)
    {
      printf("libcurl error: %s\n", curl_easy_strerror(error_code));
      return false;
    }

  return true;
}

vkapi_boolean curl_uploadfile( void *curl_handle, const char *url, const char *fieldname, const char *filename, string_t data, string_t useragent, string_t dataptr )
{
  curl_mime *form = NULL;
  curl_mimepart *field = NULL;

  form = curl_mime_init(curl_handle);
  field = curl_mime_addpart(form);
  curl_mime_name(field, fieldname);
  curl_mime_filename(field, filename);
  curl_mime_data(field, data->ptr, data->len);

  curl_easy_reset(curl_handle);
  curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, form);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

  if(useragent)
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, useragent->ptr);

  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_dynamic_string_writefunc);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, dataptr);

  CURLcode error_code = curl_easy_perform(curl_handle);

  if(error_code != CURLE_OK)
    {
      curl_mime_free(form);
      printf("libcurl error: %s\n", curl_easy_strerror(error_code));
      return false;
    }

  curl_mime_free(form);

  return true;
}

void *curl_init()
{
  return curl_easy_init();
}

void curl_cleanup(void *ptr)
{
  curl_easy_cleanup(ptr);
}
