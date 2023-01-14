module runtime;

import templatizer;

alias tmpl_safeint_t = int;

extern(C) {

int tmpl_add(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res);
int tmpl_sub(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res);
int tmpl_mul(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res);

} /* extern(C) */

extern(C++)
final class SafeInt {
    extern(C) private tmpl_safeint_t value;

    public static SafeInt createSafeInt() {
        import core.stdc.stdlib : calloc;
        SafeInt n = cast(SafeInt) calloc(SafeInt.sizeof, 1);
	n.value = 0;
        return n;
    }

    this() {
       value = 0;
    }

    this(tmpl_safeint_t n) {
        value = n;
    }

    tmpl_safeint_t get() {
        return value;
    }

    SafeInt opAssign(tmpl_safeint_t n) {
        this.value = n;
        return this;
    }

    SafeInt opBinary(string op : "+")(tmpl_safeint_t rhs) {
        tmpl_safeint_t result;
        int rc;

        rc = tmpl_add(value, rhs, &result);
        assert(rc == 0);
        this.value = result;
        return this;
    }

    SafeInt opBinary(string op : "+")(scope SafeInt rhs) {
        tmpl_safeint_t result;
        int rc;
        tmpl_safeint_t value;

        value = rhs.get();
        rc = tmpl_add(this.value, value, &result);
        assert(rc == 0);
        this.value = result;
        return this;
    }

    SafeInt opBinary(string op : "-")(tmpl_safeint_t rhs) {
        tmpl_safeint_t result;
        int rc;

        rc = tmpl_sub(value, rhs, &result);
        assert(rc == 0);
        this.value = result;
        return this;
    }

    SafeInt opBinary(string op : "-")(scope SafeInt rhs) {
        tmpl_safeint_t result;
        int rc;
        tmpl_safeint_t value;

        value = rhs.get();
        rc = tmpl_sub(this.value, value, &result);
        assert(rc == 0);
        this.value = result;
        return this;
    }

    SafeInt opBinary(string op : "*")(tmpl_safeint_t rhs) {
        tmpl_safeint_t result;
        int rc;

        rc = tmpl_mul(value, rhs, &result);
        assert(rc == 0);
        this.value = result;
        return this;
    }

    SafeInt opBinary(string op : "*")(scope SafeInt rhs) {
        tmpl_safeint_t result;
        int rc;
        tmpl_safeint_t value;

        value = rhs.get();
        rc = tmpl_mul(this.value, value, &result);
        assert(rc == 0);
        this.value = result;
        return this;
    }
};
