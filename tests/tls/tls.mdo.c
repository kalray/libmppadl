__thread int some_tls_variable;

int
mppa_dl_main(int argc, char *argv[])
{
  return argc == some_tls_variable;
}
