#include <enginecallbacks.h>
#include "va_utils.h"

int vkapi_get_id_from(char *str);

void cmd_debug(vkapi_message_object *message, int argc, char **argv, const char *args)
{
//    if(argv[1])
//        VKAPI_SEND_MESSAGE(message->peer_id, va("ID: %i", vkapi_get_id_from(argv[1])), NULL, 0);

//  if(argv[1])
//    {
//      if(argv[2])
//    {
//      string_t s = STRING_INIT();

//          for( int c = 2; c < argc + 1; c++ )
//        {
//          if( !argv[c] )
//            break;

//          if(c > 2)
//            STRING_STRNCAT( s, " ", 1 );

//          STRING_STRNCAT( s, argv[c], strlen(argv[c]) );
//        }

//      MEMCACHE_PUSH(argv[1], s->ptr);
//      VKAPI_SEND_MESSAGE( message->peer_id, va("Memcache: push key \"%s\" value \"%s\"\n", argv[1], s->ptr), NULL, 0);
//      return;
//    }
//      const char *value = MEMCACHE_GET(argv[1]);
//      if(value)
//      VKAPI_SEND_MESSAGE( message->peer_id, va("Memcache: key is \"%s\" value is \"%s\"\n", argv[1], value), NULL, 0);
//      else
//    VKAPI_SEND_MESSAGE( message->peer_id, va("Memcache: value for key \"%s\" not found\n", argv[1]), NULL, 0);
//    }
}
