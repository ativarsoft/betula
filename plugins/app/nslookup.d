import templatizer;

extern(C)
@system
int inetaddr(const char *name_ptr, size_t name_length, char **ip_ptr, size_t *ip_length);

@trusted
int safeinetaddr(string name, ref string ip)
{
    char *ip_ptr = null;
    size_t ip_length = 0;
    int rc = -1;
    rc = inetaddr(name.ptr, name.length, &ip_ptr, &ip_length);
    if (rc) {
        ip = "";
    } else {
        ip = cast(string) ip_ptr[0..ip_length];
    }
    return rc;
}

struct query_context
{
    tmpl_ctx_t tmpl;
    templatizer_callbacks *cb;
    string s;
};

@safe
int _handle_key_pair
    (ref query_context data,
     string key, string value)
{
    if (key == "dns-name")
        data.s = value;
    return 0;
}

extern(C)
@trusted
int handle_key_pair
    (void *data,
     void *key, size_t key_len,
     void *value, size_t value_len)
{
    query_context *ctx = cast(query_context *) data;
    char *keystr = cast(char *) key;
    char *valuestr = cast(char *) value;
    return _handle_key_pair(*ctx, cast(string) keystr[0..key_len], cast(string) valuestr[0..value_len]);
}

extern(C)
@safe
static int init(tmpl_ctx_t data, templatizer_callbacks *cb)
{
    string method;
    string input_dns_name_placeholder;
    string input_submit_value;

    string ipstr;
    query_context query;

    query.tmpl = data;
    query.cb = cb;

    () @trusted {
        tmpl_parse_query_string_get(&query, &handle_key_pair);
    }();

    if (query.s)
	    safeinetaddr(query.s, ipstr);


    //method = getenv("REQUEST_METHOD");
    if (method == "GET") {
        tmpl_add_filler_text(ipstr);
    } else if (method == "POST") {
        return 1;
    } else {
        return 1;
    }

    return 0;
}

extern(C)
static void quit() {}

extern(C)
__gshared
templatizer_plugin templatizer_plugin_v1 = {
    &init,
    &quit
};
