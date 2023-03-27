/* Copyright (C) 2022 Mateus de Lima Oliveira */

#include <templatizer.h>

typedef int tmpl_safeint_t;

#if defined __GNUC__ && !defined __clang__ && defined __GLIBC__
int tmpl_add(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res)
{
    return __builtin_add_overflow(lhs, rhs, res);
}

int tmpl_sub(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res)
{
    return __builtin_sub_overflow(lhs, rhs, res);
}

int tmpl_mul(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res)
{
    return __builtin_mul_overflow(lhs, rhs, res);
}
#else
int tmpl_add(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res)
{
    *res = lhs + rhs;
    return 0;
}

int tmpl_sub(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res)
{
    *res = lhs - rhs;
    return 0;
}

int tmpl_mul(tmpl_safeint_t lhs, tmpl_safeint_t rhs, tmpl_safeint_t *res)
{
    *res = lhs * rhs;
    return 0;
}
#endif

