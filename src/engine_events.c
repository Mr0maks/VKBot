#include "common.h"

double get_time_s( void );

bool message_new_handler(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(message, "from_id");
    cJSON *fwd_messages = cJSON_GetObjectItem(message, "fwd_messages");

    vkapi_handle *handle = worker_get_vkapi_handle();

    bool result = false;
  
    if(!message || !peer_id || !from_id) {
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
  
    cJSON *text_string = cJSON_GetObjectItem(message, "text");
  
    if(text_string)
      string_copy(x->text, cJSON_GetStringValue(text_string));
  
    if(x->text->len == 0)
    {
        cJSON_Delete( raw );
        string_destroy(x->text);
        free(x);
        return false;
    }

    cJSON *attachments = cJSON_GetObjectItem(message, "attachments");
  
    x->attachmens = NULL;
  
    if(cJSON_GetArraySize(attachments) > 0)
      x->attachmens = cJSON_Duplicate(attachments, true);

    /*
    if(fwd_messages)
    {
        int i = 0;
        x->reply_message = calloc(cJSON_GetArraySize(fwd_messages), sizeof (vkapi_reply_message));
        cJSON *fwd_message = NULL;
        cJSON_ArrayForEach(fwd_message, fwd_messages)
        {
            cJSON *fwd_text = cJSON_GetObjectItem(fwd_message, "text");
            cJSON *fwd_peer_id = cJSON_GetObjectItem(fwd_message, "peer_id");
            cJSON *fwd_from_id = cJSON_GetObjectItem(fwd_message, "from_id");

            x->reply_message->peer_id = fwd_peer_id->valueint;
            x->reply_message->from_id = fwd_from_id->valueint;
            i++;
        }


    }
  */
    cJSON_Delete(raw);

    if( x->text->len < 2048)
    {
     Con_Printf( "[Worker %i] Message peer_id: %i from_id: %i message: %s\n", worker_get_worker_id(), x->peer_id, x->from_id, x->text->ptr );

     double start_time = get_time_s();

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
