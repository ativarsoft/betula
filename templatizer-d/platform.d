module platform;

import core.stdc.stdio : FILE;

extern(C)
size_t strlen(const(char) *s)
{
    char c;
    size_t len, i;
    for (i = 0; (c = s[i]) != '\0'; i++) {}
    len = i;
    return len;
}

extern(C)
int tmpl_atoi(string s)
{
    import core.stdc.stdlib : atoi;
    return atoi(s.ptr);
}

size_t tmpl_fread_stdin(void *buffer, size_t b, size_t c)
{
    import core.stdc.stdio : fread;
    return fread(buffer, b, c, file);
}
