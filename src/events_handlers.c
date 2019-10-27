#include "common.h"

double get_time_s( void );

bool message_new_handler(cJSON *raw)
{
    //FIXME: optimise to autodetect "" messages
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *peer_id = cJSON_GetObjectItem(object, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(object, "from_id");

    vkapi_handle *handle = worker_get_vkapi_handle();

    bool result = false;
  
    if(!object || !peer_id || !from_id) {
      cJSON_Delete(raw);
      return false;
      } else if(peer_id->valueint < 0 || from_id->valueint < 0)
      {
        cJSON_Delete(raw);
        return false;
      }
  
    vkapi_message_object *x = calloc(1, sizeof(vkapi_message_object));
  
    x->from_id = from_id->valueint;
    x->peer_id = peer_id->valueint;
  
    x->private_message = false;
  
    if(x->peer_id == x->from_id)
      x->private_message = true;

    if( handle->group_id == (x->from_id * -1) )
      {
        cJSON_Delete( raw );
        free(x);
        return false;
      }
  
    x->text = string_init();
  
    cJSON *text_string = cJSON_GetObjectItem(object, "text");
  
    if(text_string)
      string_copy(x->text, cJSON_GetStringValue(text_string));
  
    if(x->text->len == 0)
    {
        cJSON_Delete( raw );
        string_destroy(x->text);
        free(x);
        return false;
    }

    cJSON *attachments = cJSON_GetObjectItem(object, "attachments");
  
    x->attachmens = NULL;
  
    if(cJSON_GetArraySize(attachments) > 0)
      x->attachmens = cJSON_Duplicate(attachments, true);
  
    cJSON_Delete(raw);
    
    double start_time = get_time_s();

    if( x->text->len < 2048)
    {
     Con_Printf( "[Worker %i] Message peer_id: %i from_id: %i message: %s\n", worker_get_worker_id(), x->peer_id, x->from_id, x->text->ptr );

     if(cmd_handle( x ))
     {
         Con_Printf("[Worker %i] took at %f sec\n", worker_get_worker_id(), get_time_s() - start_time );
         result = true;
     }
    }

    if(x->attachmens)
    cJSON_Delete(x->attachmens);
    
    string_destroy(x->text);
    free(x);

    return result;
}
