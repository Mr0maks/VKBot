// void

void worker_main_thread( const char *token, const char *group_id, int num_workers );

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  worker_main_thread( "nice", "null", 0 );

  return 0;
}
