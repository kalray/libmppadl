static const char * static_const_char_s = "this is a static const char *\n";

int
mppa_dl_main(int argc, char *argv[])
{
  int is_equal = 0;
  int i=0;
  is_equal |= static_const_char_s[i++]=='t';
  is_equal |= static_const_char_s[i++]=='h';
  is_equal |= static_const_char_s[i++]=='i';
  is_equal |= static_const_char_s[i++]=='s';
  is_equal |= static_const_char_s[i++]==' ';
  is_equal |= static_const_char_s[i++]=='i';
  is_equal |= static_const_char_s[i++]=='s';
  is_equal |= static_const_char_s[i++]==' ';
  is_equal |= static_const_char_s[i++]=='a';
  is_equal |= static_const_char_s[i++]==' ';
  is_equal |= static_const_char_s[i++]=='s';
  is_equal |= static_const_char_s[i++]=='t';
  is_equal |= static_const_char_s[i++]=='a';
  is_equal |= static_const_char_s[i++]=='t';
  is_equal |= static_const_char_s[i++]=='i';
  is_equal |= static_const_char_s[i++]=='c';
  is_equal |= static_const_char_s[i++]==' ';
  is_equal |= static_const_char_s[i++]=='c';
  is_equal |= static_const_char_s[i++]=='o';
  is_equal |= static_const_char_s[i++]=='n';
  is_equal |= static_const_char_s[i++]=='s';
  is_equal |= static_const_char_s[i++]=='t';
  is_equal |= static_const_char_s[i++]==' ';
  is_equal |= static_const_char_s[i++]=='c';
  is_equal |= static_const_char_s[i++]=='h';
  is_equal |= static_const_char_s[i++]=='a';
  is_equal |= static_const_char_s[i++]=='r';
  is_equal |= static_const_char_s[i++]==' ';
  is_equal |= static_const_char_s[i++]=='*';
  is_equal |= static_const_char_s[i++]=='\n';
  
  return !is_equal;
}
