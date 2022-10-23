int isalnum(int c)
{
    if (c >= '0' && c <= '9') {
        return 1;
    } else if (c >= 'a' && c <= 'z') {
        return 1;
    } else if (c >= 'A' && c <= 'Z') {
        return 1;
    } else if (c == '_') {
        return 1;
    }
    return 0;
}
