// void

void worker_main_thread( const char *token, int group_id, int num_workers );

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  worker_main_thread( "nice", 0, 4 );

  return 0;
}
