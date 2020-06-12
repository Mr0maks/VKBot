#include "common.h"
#include <curl/curl.h>

static pthread_mutex_t connlock;
static CURLSH *share = NULL;

static void lock_cb(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr)
{
  (void)access; /* unused */
  (void)userptr; /* unused */
  (void)handle; /* unused */
  (void)data; /* unused */
  pthread_mutex_lock(&connlock);
}

static void unlock_cb(CURL *handle, curl_lock_data data, void *userptr)
{
  (void)userptr; /* unused */
  (void)handle;  /* unused */
  (void)data;    /* unused */
  pthread_mutex_unlock(&connlock);
}

void curl_worker_share_init(void)
{
    pthread_mutex_init(&connlock, NULL);
    share = curl_share_init();
    curl_share_setopt(share, CURLSHOPT_LOCKFUNC, lock_cb);
    curl_share_setopt(share, CURLSHOPT_UNLOCKFUNC, unlock_cb);
    curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
}

void curl_worker_share_deinit(void)
{
    pthread_mutex_destroy(&connlock);
    curl_share_cleanup(share);
}

size_t curl_dynamic_string_writefunc( void *ptr, size_t size, size_t nmemb, void *data )
{
  string_t s = (string_t)data;

  if(ptr == NULL)
    return 0;

  string_strncat(s, (const char*)ptr, size*nmemb);

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

bool curl_get( void *curl_handle, const char *url, string_t useragent, string_t dataptr )
{
    if(!curl_handle)
        curl_handle = worker_get_vkapi_handle()->curl_handle;

  curl_easy_reset(curl_handle);

  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_SHARE, share);
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
      Con_Printf("libcurl error: %s\n", curl_easy_strerror(error_code));
      return false;
    }

  return true;
}

bool curl_post( void *curl_handle, const char *url, string_t post, string_t useragent, string_t dataptr )
{
    if(!curl_handle)
        curl_handle = worker_get_vkapi_handle()->curl_handle;

  curl_easy_reset(curl_handle);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_SHARE, share);
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
      Con_Printf("libcurl error: %s\n", curl_easy_strerror(error_code));
      return false;
    }

  return true;
}

bool curl_uploadfile( void *curl_handle, const char *url, const char *fieldname, const char *filename, string_t data, string_t useragent, string_t dataptr )
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
      Con_Printf("libcurl error: %s\n", curl_easy_strerror(error_code));
      return false;
    }

  curl_mime_free(form);

  return true;
}

void *curl_init()
{
  return curl_easy_init();
}

char *curl_urlencode(const char *data)
{
    void *curl_handle = worker_get_vkapi_handle()->curl_handle;
    char *encoded = curl_easy_escape(curl_handle, data, 0);
    char *duped = strdup(encoded);
    curl_free(encoded);
    return duped;
}

void curl_ptr_free(void *ptr)
{
	assert(ptr);
	curl_free(ptr);
}

void curl_cleanup(void *ptr)
{
	assert(ptr);
	curl_easy_cleanup(ptr);
}
