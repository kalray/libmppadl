char * char_s = "this is a char *\n";

int
mppa_dl_main(int argc, char *argv[])
{
  int is_equal = 1;
  int i=0;
  is_equal &= char_s[i++]=='t';
  is_equal &= char_s[i++]=='h';
  is_equal &= char_s[i++]=='i';
  is_equal &= char_s[i++]=='s';
  is_equal &= char_s[i++]==' ';
  is_equal &= char_s[i++]=='i';
  is_equal &= char_s[i++]=='s';
  is_equal &= char_s[i++]==' ';
  is_equal &= char_s[i++]=='a';
  is_equal &= char_s[i++]==' ';
  is_equal &= char_s[i++]=='c';
  is_equal &= char_s[i++]=='h';
  is_equal &= char_s[i++]=='a';
  is_equal &= char_s[i++]=='r';
  is_equal &= char_s[i++]==' ';
  is_equal &= char_s[i++]=='*';
  is_equal &= char_s[i++]=='\n';

  return !is_equal;
}
