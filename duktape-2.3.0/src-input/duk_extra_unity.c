
#include "duk_internal.h"

DUK_EXTERNAL void duk_builtins_reg_put(duk_context *ctx, duk_uarridx_t key) {
    duk_push_heap_stash(ctx);
    duk_get_prop_index(ctx, -1, DUK_UNITY_STASH_BUILTINS); //duk_get_prop_string(ctx, -1, "c_builtins"); // obj, stash, builtins
    duk_dup(ctx, -3); // obj, stash, builtins, obj
    duk_put_prop_index(ctx, -2, key); // obj, stash, builtins
    duk_pop_3(ctx);
}

DUK_EXTERNAL void duk_builtins_reg_get(duk_context *ctx, duk_uarridx_t key) {
    duk_push_heap_stash(ctx);
    duk_get_prop_index(ctx, -1, DUK_UNITY_STASH_BUILTINS); //duk_get_prop_string(ctx, -1, "c_builtins"); // stash, builtins
    duk_get_prop_index(ctx, -1, key); // stash, builtins, obj
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
}

DUK_EXTERNAL void duk_unity_inherit(duk_context *ctx) {
    duk_get_prop_string(ctx, -2, "prototype");
    duk_get_prop_string(ctx, -2, "prototype");
    duk_set_prototype(ctx, -2);
    duk_pop(ctx);
}

DUK_INTERNAL void duk_unity_get_prop_object(duk_context *ctx, duk_idx_t idx, const char *key) {
    if (!duk_get_prop_string(ctx, idx, key)) {
        duk_pop(ctx);
        idx = duk_normalize_index(ctx, idx);
        duk_push_object(ctx);
        duk_dup_top(ctx);
        duk_put_prop_string(ctx, idx, key);
    }
}

DUK_LOCAL void duk_unity_array_assign(duk_context *ctx, duk_idx_t obj_idx, duk_uarridx_t dst, duk_uarridx_t from) {
    obj_idx = duk_normalize_index(ctx, obj_idx);
    duk_get_prop_index(ctx, obj_idx, from);
    duk_put_prop_index(ctx, obj_idx, dst);
}

DUK_INTERNAL void duk_unity_begin_class(duk_context *ctx, const char *key, duk_uarridx_t reg_idx, duk_c_function ctor, duk_c_function dtor) {
    duk_push_c_function(ctx, ctor, DUK_VARARGS); // ctor
    duk_dup_top(ctx);
    duk_builtins_reg_put(ctx, reg_idx);
    duk_dup_top(ctx);
    duk_put_prop_string(ctx, -3, key);
    duk_push_object(ctx); // ctor, prototype
    if (dtor != NULL) {
        duk_push_c_function(ctx, dtor, 1); // ctor, prototype, finalizer
        duk_set_finalizer(ctx, -2); // ctor, prototype
    }
    duk_dup_top(ctx); // ctor, prototype, prototype
    duk_put_prop_string(ctx, -3, "prototype"); // ctor, prototype    
}

DUK_INTERNAL void duk_unity_add_member(duk_context *ctx, const char *name, duk_c_function fn, duk_idx_t idx) {
    idx = duk_normalize_index(ctx, idx);
    duk_push_c_function(ctx, fn, DUK_VARARGS);
    duk_put_prop_string(ctx, idx, name);
}

DUK_INTERNAL void duk_unity_add_property(duk_context *ctx, const char *name, duk_c_function getter, duk_c_function setter, duk_idx_t idx) {
    idx = duk_normalize_index(ctx, idx);
    duk_uint_t flags = 0;
    duk_push_string(ctx, name);
    if (getter != NULL)
    {
        flags |= DUK_DEFPROP_HAVE_GETTER;
        duk_push_c_function(ctx, getter, 0);
    }
    if (setter != NULL)
    {
        flags |= DUK_DEFPROP_HAVE_SETTER;
        duk_push_c_function(ctx, setter, 1);
    }
    // [ctor, prototype, name, ?getter, ?setter]
    duk_def_prop(ctx, idx, flags | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);
}

DUK_INTERNAL void duk_unity_end_class(duk_context *ctx) {
    duk_pop_2(ctx);
}


/*
 *  unity helpers
 */

DUK_EXTERNAL void duk_unity_error_raw(duk_hthread *thr, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt) {
    duk_error_raw(thr, err_code, filename, line, fmt);
}

DUK_EXTERNAL const char *duk_unity_get_lstring(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_len) {
    duk_size_t size;
    const char *ret = duk_get_lstring(ctx, idx, &size);
    if (out_len != NULL) {
        *out_len = (duk_uint_t)size;
    } 
    return ret;
}

DUK_EXTERNAL duk_int_t duk_unity_compile_raw(duk_context *ctx, const char *src_buffer, duk_uint_t src_length, duk_uint_t flags) {
    return duk_compile_raw(ctx, src_buffer, (duk_size_t)src_length, flags);
}

DUK_EXTERNAL duk_int_t duk_unity_eval_raw(duk_context *ctx, const char *src_buffer, duk_uint_t src_length, duk_uint_t flags) {
    return duk_eval_raw(ctx, src_buffer, (duk_size_t)src_length, flags);
}

DUK_EXTERNAL duk_codepoint_t duk_unity_char_code_at(duk_context *ctx, duk_idx_t idx, duk_uint_t char_offset) {
    return duk_char_code_at(ctx, idx, (duk_size_t)char_offset);
}

DUK_EXTERNAL void duk_unity_substring(duk_context *ctx, duk_idx_t idx, duk_uint_t start_char_offset, duk_uint_t end_char_offset) {
    duk_substring(ctx, idx, (duk_size_t)start_char_offset, (duk_size_t)end_char_offset);
}

DUK_EXTERNAL void *duk_unity_resize_buffer(duk_context *ctx, duk_idx_t idx, duk_uint_t new_size) {
    void *ptr = duk_resize_buffer(ctx, idx, (duk_size_t)new_size);
    return ptr;
}

DUK_EXTERNAL void *duk_unity_steal_buffer(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_size) {
    duk_size_t size;
    void *ptr = duk_steal_buffer(ctx, idx, &size);
    if (out_size != NULL) {
        *out_size = (duk_uint_t)size;
    }
    return ptr;
}

DUK_EXTERNAL void duk_unity_config_buffer(duk_context *ctx, duk_idx_t idx, void *ptr, duk_uint_t len) {
    duk_config_buffer(ctx, idx, ptr, (duk_size_t)len);
}

DUK_EXTERNAL duk_uint_t duk_unity_get_length(duk_context *ctx, duk_idx_t idx) {
    return (duk_uint_t)duk_get_length(ctx, idx);
}

DUK_EXTERNAL void duk_unity_set_length(duk_context *ctx, duk_idx_t idx, duk_uint_t len) {
    duk_set_length(ctx, idx, (duk_size_t)len);
}

DUK_EXTERNAL const char *duk_unity_safe_to_lstring(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_len) {
    duk_size_t size;
    const char *ret = duk_safe_to_lstring(ctx, idx, &size);
    if (out_len != NULL) {
        *out_len = (duk_uint_t)size;
    }
    return ret;
}

DUK_EXTERNAL void *duk_unity_alloc_raw(duk_context *ctx, duk_uint_t size) {
    return duk_alloc_raw(ctx, (duk_size_t)size);
}

DUK_EXTERNAL void *duk_unity_realloc_raw(duk_context *ctx, void *ptr, duk_uint_t size) {
    return duk_realloc_raw(ctx, ptr, (duk_size_t)size);
}

DUK_EXTERNAL void *duk_unity_alloc(duk_context *ctx, duk_uint_t size) {
    return duk_alloc(ctx, (duk_size_t)size);
}

DUK_EXTERNAL void *duk_unity_realloc(duk_context *ctx, void *ptr, duk_uint_t size) {
    return duk_realloc(ctx, ptr, (duk_size_t)size);
}

DUK_EXTERNAL const char *duk_unity_push_lstring(duk_context *ctx, const char *str, duk_uint_t len) {
    return duk_push_lstring(ctx, str, (duk_size_t)len);
}

DUK_EXTERNAL const char *duk_unity_require_lstring(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_len) {
    duk_size_t size;
    const char *ret = duk_require_lstring(ctx, idx, &size);
    if (out_len != NULL) {
        *out_len = (duk_uint_t)size;
    }
    return ret;
}

DUK_EXTERNAL void *duk_unity_require_buffer(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_size) {
    duk_size_t size;
    void *ret = duk_require_buffer(ctx, idx, &size);
    if (out_size != NULL) {
        *out_size = (duk_uint_t)size;
    }
    return ret;
}
DUK_EXTERNAL void *duk_unity_require_buffer_data(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_size) {
    duk_size_t size;
    void *ret = duk_require_buffer_data(ctx, idx, &size);
    if (out_size != NULL) {
        *out_size = (duk_uint_t)size;
    }
    return ret;
}

DUK_EXTERNAL const char *duk_unity_to_lstring(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_len) {
    duk_size_t size;
    const char *ret = duk_to_lstring(ctx, idx, &size);
    if (out_len != NULL) {
        *out_len = (duk_uint_t)size;
    }
    return ret;
}

DUK_EXTERNAL void *duk_unity_to_buffer_raw(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_size, duk_uint_t flags) {
    duk_size_t size;
    void *ret = duk_to_buffer_raw(ctx, idx, &size, flags);
    if (out_size != NULL) {
        *out_size = (duk_uint_t)size;
    }
    return ret;
}


DUK_EXTERNAL void *duk_unity_get_buffer(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_size) {
    duk_size_t size;
    void *ptr = duk_get_buffer(ctx, idx, &size);
    if (out_size != NULL) {
        *out_size = (duk_uint_t)size;
    }
    return ptr;
}

DUK_EXTERNAL void *duk_unity_get_buffer_data(duk_context *ctx, duk_idx_t idx, duk_uint_t *out_size) {
    duk_size_t size;
    void *ptr = duk_get_buffer_data(ctx, idx, &size);
    if (out_size != NULL) {
        *out_size = (duk_uint_t)size;
    }
    return ptr;
}

// int 

DUK_EXTERNAL void duk_unity_put2i(duk_context *ctx, duk_idx_t idx, duk_int_t v1, duk_int_t v2) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_int(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_int(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
}

DUK_EXTERNAL void duk_unity_put3i(duk_context *ctx, duk_idx_t idx, duk_int_t v1, duk_int_t v2, duk_int_t v3) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_int(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_int(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_int(ctx, v3);
    duk_put_prop_index(ctx, idx, 2);
}

DUK_EXTERNAL void duk_unity_put4i(duk_context *ctx, duk_idx_t idx, duk_int_t v1, duk_int_t v2, duk_int_t v3, duk_int_t v4) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_int(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_int(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_int(ctx, v3);
    duk_put_prop_index(ctx, idx, 2);
    duk_push_int(ctx, v4);
    duk_put_prop_index(ctx, idx, 3);
}

// float

DUK_EXTERNAL void duk_unity_put2f(duk_context *ctx, duk_idx_t idx, float v1, float v2) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_number(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
}

DUK_EXTERNAL void duk_unity_put3f(duk_context *ctx, duk_idx_t idx, float v1, float v2, float v3) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_number(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_number(ctx, v3);
    duk_put_prop_index(ctx, idx, 2);
}

DUK_EXTERNAL void duk_unity_put4f(duk_context *ctx, duk_idx_t idx, float v1, float v2, float v3, float v4) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_number(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_number(ctx, v3);
    duk_put_prop_index(ctx, idx, 2);
    duk_push_number(ctx, v4);
    duk_put_prop_index(ctx, idx, 3);
}

DUK_EXTERNAL void duk_unity_put2d(duk_context *ctx, duk_idx_t idx, double v1, double v2) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_number(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
}

DUK_EXTERNAL void duk_unity_put3d(duk_context *ctx, duk_idx_t idx, double v1, double v2, double v3) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_number(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_number(ctx, v3);
    duk_put_prop_index(ctx, idx, 2);
}

DUK_EXTERNAL void duk_unity_put4d(duk_context *ctx, duk_idx_t idx, double v1, double v2, double v3, double v4) {
    // duk_push_array(ctx);
    idx = duk_normalize_index(ctx, idx);
    duk_push_number(ctx, v1);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, v2);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_number(ctx, v3);
    duk_put_prop_index(ctx, idx, 2);
    duk_push_number(ctx, v4);
    duk_put_prop_index(ctx, idx, 3);
}

DUK_EXTERNAL duk_bool_t duk_unity_get2f(duk_context *ctx, duk_idx_t idx, float *v1, float *v2) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        duk_pop_2(ctx);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get2d(duk_context *ctx, duk_idx_t idx, double *v1, double *v2) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        duk_pop_2(ctx);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get3f(duk_context *ctx, duk_idx_t idx, float *v1, float *v2, float *v3) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 2)) {
            *v3 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        duk_pop_3(ctx);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get3d(duk_context *ctx, duk_idx_t idx, double *v1, double *v2, double *v3) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 2)) {
            *v3 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        duk_pop_3(ctx);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get4f(duk_context *ctx, duk_idx_t idx, float *v1, float *v2, float *v3, float *v4) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 2)) {
            *v3 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 3)) {
            *v4 = (float)duk_get_number_default(ctx, -1, 0.0);
        }
        duk_pop_n(ctx, 4);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get4d(duk_context *ctx, duk_idx_t idx, double *v1, double *v2, double *v3, double *v4) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 2)) {
            *v3 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        if (duk_get_prop_index(ctx, idx, 3)) {
            *v4 = (double)duk_get_number_default(ctx, -1, 0.0);
        }
        duk_pop_n(ctx, 4);
        return 1;
    // }
    // return 0;
}

// int

DUK_EXTERNAL duk_bool_t duk_unity_get2i(duk_context *ctx, duk_idx_t idx, duk_int_t *v1, duk_int_t *v2) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = duk_get_int_default(ctx, -1, 0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = duk_get_int_default(ctx, -1, 0);
        }
        duk_pop_2(ctx);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get3i(duk_context *ctx, duk_idx_t idx, duk_int_t *v1, duk_int_t *v2, duk_int_t *v3) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = duk_get_int_default(ctx, -1, 0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = duk_get_int_default(ctx, -1, 0);
        }
        if (duk_get_prop_index(ctx, idx, 2)) {
            *v3 = duk_get_int_default(ctx, -1, 0);
        }
        duk_pop_3(ctx);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL duk_bool_t duk_unity_get4i(duk_context *ctx, duk_idx_t idx, duk_int_t *v1, duk_int_t *v2, duk_int_t *v3, duk_int_t *v4) {
    idx = duk_normalize_index(ctx, idx);
    // /*if (duk_is_array(ctx, idx))*/ {
        if (duk_get_prop_index(ctx, idx, 0)) {
            *v1 = duk_get_int_default(ctx, -1, 0);
        }
        if (duk_get_prop_index(ctx, idx, 1)) {
            *v2 = duk_get_int_default(ctx, -1, 0);
        }
        if (duk_get_prop_index(ctx, idx, 2)) {
            *v3 = duk_get_int_default(ctx, -1, 0);
        }
        if (duk_get_prop_index(ctx, idx, 3)) {
            *v4 = duk_get_int_default(ctx, -1, 0);
        }
        duk_pop_n(ctx, 4);
        return 1;
    // }
    // return 0;
}

DUK_EXTERNAL void duk_unity_put4x4f(duk_context *ctx, duk_idx_t idx, const float *c0, const float *c1, const float *c2, const float *c3) {
    idx = duk_normalize_index(ctx, idx);

    duk_push_number(ctx, c0[0]);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, c0[1]);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_number(ctx, c0[2]);
    duk_put_prop_index(ctx, idx, 2);
    duk_push_number(ctx, c0[3]);
    duk_put_prop_index(ctx, idx, 3);

    duk_push_number(ctx, c1[0]);
    duk_put_prop_index(ctx, idx, 4);
    duk_push_number(ctx, c1[1]);
    duk_put_prop_index(ctx, idx, 5);
    duk_push_number(ctx, c1[2]);
    duk_put_prop_index(ctx, idx, 6);
    duk_push_number(ctx, c1[3]);
    duk_put_prop_index(ctx, idx, 7);

    duk_push_number(ctx, c2[0]);
    duk_put_prop_index(ctx, idx, 8);
    duk_push_number(ctx, c2[1]);
    duk_put_prop_index(ctx, idx, 9);
    duk_push_number(ctx, c2[2]);
    duk_put_prop_index(ctx, idx, 10);
    duk_push_number(ctx, c2[3]);
    duk_put_prop_index(ctx, idx, 11);

    duk_push_number(ctx, c3[0]);
    duk_put_prop_index(ctx, idx, 12);
    duk_push_number(ctx, c3[1]);
    duk_put_prop_index(ctx, idx, 13);
    duk_push_number(ctx, c3[2]);
    duk_put_prop_index(ctx, idx, 14);
    duk_push_number(ctx, c3[3]);
    duk_put_prop_index(ctx, idx, 15);
}

DUK_EXTERNAL duk_bool_t duk_unity_get4x4f(duk_context *ctx, duk_idx_t idx, float *c0, float *c1, float *c2, float *c3) {
    idx = duk_normalize_index(ctx, idx);

    if (duk_get_prop_index(ctx, idx, 0)) {
        c0[0] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 1)) {
        c0[1] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 2)) {
        c0[2] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 3)) {
        c0[3] = (float)duk_get_number_default(ctx, -1, 0);
    }

    if (duk_get_prop_index(ctx, idx, 4)) {
        c1[0] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 5)) {
        c1[1] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 6)) {
        c1[2] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 7)) {
        c1[3] = (float)duk_get_number_default(ctx, -1, 0);
    }

    if (duk_get_prop_index(ctx, idx, 8)) {
        c2[0] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 9)) {
        c2[1] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 10)) {
        c2[2] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 11)) {
        c2[3] = (float)duk_get_number_default(ctx, -1, 0);
    }

    if (duk_get_prop_index(ctx, idx, 12)) {
        c3[0] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 13)) {
        c3[1] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 14)) {
        c3[2] = (float)duk_get_number_default(ctx, -1, 0);
    }
    if (duk_get_prop_index(ctx, idx, 15)) {
        c3[3] = (float)duk_get_number_default(ctx, -1, 0);
    }
    duk_pop_n(ctx, 16);
    return 1;
}

DUK_EXTERNAL duk_bool_t duk_unity_set_prop_i(duk_context *ctx, duk_idx_t idx, const char *key, duk_int_t val) {
    idx = duk_normalize_index(ctx, idx);
    duk_push_int(ctx, val);
    return duk_put_prop_string(ctx, idx, key);
}

DUK_LOCAL void duk_refsys_open(duk_context *ctx) {
    duk_push_heap_stash(ctx); // [stash]
    
    duk_push_array(ctx); // [stash, array]
    duk_dup_top(ctx); // [stash, array, array]
    duk_put_prop_index(ctx, -3, DUK_UNITY_STASH_REGISTRY); //duk_put_prop_string(ctx, -3, "c_registry"); // [stash, array]
    duk_push_int(ctx, 0); // [stash, array, 0]
    duk_put_prop_index(ctx, -2, 0); // [stash, array]
    duk_pop(ctx); // [stash]

    duk_push_object(ctx);
    duk_put_prop_index(ctx, -2, DUK_UNITY_STASH_BUILTINS);//duk_put_prop_string(ctx, -2, "c_builtins"); // [stash, builtins]

    duk_pop(ctx); // .
}

/// Creates and returns a reference for the object at the top of the stack (and pops the object).
DUK_EXTERNAL duk_uint_t duk_unity_ref(duk_context *ctx) {
    if (duk_is_null_or_undefined(ctx, -1)) {
        return 0;
    }
    duk_push_heap_stash(ctx); // obj, stash
    duk_get_prop_index(ctx, -1, DUK_UNITY_STASH_REGISTRY); //duk_get_prop_string(ctx, -1, "c_registry"); // obj, stash, array
    duk_get_prop_index(ctx, -1, 0); // obj, stash, array, array[0]
    duk_uint_t refid = duk_get_uint(ctx, -1); // obj, stash, array, array[0]
    if (refid > 0) {
        duk_get_prop_index(ctx, -2, refid); // obj, stash, array, array[0], array[refid]
        duk_uint_t freeid = duk_get_uint(ctx, -1);
        duk_put_prop_index(ctx, -3, 0); // obj, stash, array, array[0] ** update free ptr
        duk_dup(ctx, -4); // obj, stash, array, array[0], obj
        duk_put_prop_index(ctx, -3, refid); // obj, stash, array, array[0]
        duk_pop_n(ctx, 4); // []
    } else {
        refid = (int)duk_unity_get_length(ctx, -2);
        duk_dup(ctx, -4); // obj, stash, array, array[0], obj
        duk_put_prop_index(ctx, -3, refid); // obj, stash, array, array[0]
        duk_pop_n(ctx, 4); // []
    }
    return refid;
}

// push object referenced by refid to top of the stack
DUK_EXTERNAL void duk_unity_getref(duk_context *ctx, duk_uint_t refid) {
    if (refid == 0) {
        duk_push_undefined(ctx);
        return;
    }
    duk_push_heap_stash(ctx); // stash
    duk_get_prop_index(ctx, -1, DUK_UNITY_STASH_REGISTRY); //duk_get_prop_string(ctx, -1, "c_registry"); // stash, array
    duk_get_prop_index(ctx, -1, refid); // stash, array, array[refid]
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
}

/// Releases reference refid
DUK_EXTERNAL void duk_unity_unref(duk_context *ctx, duk_uint_t refid) {
    if (refid == 0) {
        // do nothing for null/undefined reference
        return;
    }
    duk_push_heap_stash(ctx); // stash
    duk_get_prop_index(ctx, -1, DUK_UNITY_STASH_REGISTRY); //duk_get_prop_string(ctx, -1, "c_registry"); // stash, array
    duk_get_prop_index(ctx, -1, 0); // stash, array, array[0]
    duk_uint_t freeid = duk_get_int(ctx, -1); // stash, array, array[0]
    duk_push_uint(ctx, refid); // stash, array, array[0], refid
    duk_put_prop_index(ctx, -3, 0); // stash, array, array[0] ** set t[freeid] = refid
    duk_push_uint(ctx, freeid); // stash, array, array[0], freeid
    duk_put_prop_index(ctx, -3, refid); // stash, array, array[0] ** set t[refid] = freeid
    duk_pop_3(ctx); // []
}

DUK_EXTERNAL void *duk_unity_push_buffer_raw(duk_context *ctx, duk_uint_t size, duk_uint_t flags) {
    return duk_push_buffer_raw(ctx, (duk_size_t) size, (duk_small_uint_t) flags);
}

DUK_EXTERNAL void duk_unity_push_buffer_object(duk_context *ctx, duk_idx_t idx_buffer, duk_uint_t byte_offset, duk_uint_t byte_length, duk_uint_t flags) {
    duk_push_buffer_object(ctx, idx_buffer, byte_offset, byte_length, flags);
}

DUK_EXTERNAL duk_idx_t duk_unity_push_error_object_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt) {
    return duk_push_error_object_raw(ctx, err_code, filename, line, fmt);
}

DUK_EXTERNAL duk_bool_t duk_unity_put_target_i(duk_context *ctx, duk_idx_t idx) {
    if (!duk_is_null_or_undefined(ctx, idx)) {
        return duk_put_prop_string(ctx, idx, "target");
    } 
    duk_pop(ctx);
    return 0;
}

// debugger support

struct duk_unity_debugger {
	duk_unity_debug_read_function dbg_read_cb;                /* required, NULL implies detached */
	duk_unity_debug_write_function dbg_write_cb;              /* required */
	duk_unity_debug_peek_function dbg_peek_cb;
    duk_unity_debug_read_flush_function dbg_read_flush_cb;
    duk_unity_debug_write_flush_function dbg_write_flush_cb;
    duk_unity_debug_request_function dbg_request_cb;
    duk_unity_debug_detached_function dbg_detached_cb;
    duk_int_t udata;
};
typedef struct duk_unity_debugger duk_unity_debugger;

DUK_LOCAL duk_size_t duk_unity_wrap_debug_read_function(void *udata, char *buffer, duk_size_t length) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        // 
        return (duk_size_t)debugger->dbg_read_cb(debugger->udata, buffer, (duk_int_t)length);
    }
    return 0;
}

DUK_LOCAL duk_size_t duk_unity_wrap_debug_write_function(void *udata, const char *buffer, duk_size_t length) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        return (duk_size_t)debugger->dbg_write_cb(debugger->udata, buffer, (duk_int_t)length);
    }
    return 0;
}

DUK_LOCAL duk_size_t duk_unity_wrap_debug_peek_function(void *udata) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        return (duk_size_t)debugger->dbg_peek_cb(debugger->udata);
    }
    return 0;
}

DUK_LOCAL void duk_unity_wrap_debug_read_flush_function(void *udata) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        debugger->dbg_read_flush_cb(debugger->udata);
    }
}

DUK_LOCAL void duk_unity_wrap_debug_write_flush_function(void *udata) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        debugger->dbg_write_flush_cb(debugger->udata);
    }
}

DUK_LOCAL duk_idx_t duk_unity_wrap_debug_request_function(duk_context *ctx, void *udata, duk_idx_t nvalues) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        return debugger->dbg_request_cb(ctx, debugger->udata, nvalues);
    }
    return 0;
}

DUK_LOCAL void duk_unity_wrap_debug_detached_function(duk_context *ctx, void *udata) {
    duk_unity_debugger *debugger = (duk_unity_debugger *)udata;
    if (debugger != NULL) {
        debugger->dbg_detached_cb(ctx, debugger->udata);
    }
}

DUK_EXTERNAL void *duk_unity_attach_debugger(duk_context *ctx, 
                                           duk_unity_debug_read_function read_cb, 
                                           duk_unity_debug_write_function write_cb, 
                                           duk_unity_debug_peek_function peek_cb, 
                                           duk_unity_debug_read_flush_function read_flush_cb,
                                           duk_unity_debug_write_flush_function write_flush_cb,
                                           duk_unity_debug_request_function request_cb,
                                           duk_unity_debug_detached_function detached_cb, 
                                           duk_int_t udata) {
#if defined(DUK_USE_DEBUGGER_SUPPORT)
    duk_unity_debugger *debugger = duk_alloc(ctx, sizeof(duk_unity_debugger));
    if (debugger != NULL) {
        debugger->dbg_read_cb = read_cb;
        debugger->dbg_write_cb = write_cb;
        debugger->dbg_peek_cb = peek_cb;
        debugger->dbg_read_flush_cb = read_flush_cb;
        debugger->dbg_write_flush_cb = write_flush_cb;
        debugger->dbg_request_cb = request_cb;
        debugger->dbg_detached_cb = detached_cb;
        debugger->udata = udata;
        duk_debugger_attach(ctx, 
                            duk_unity_wrap_debug_read_function,     /* read callback */
                            duk_unity_wrap_debug_write_function,    /* write callback */
                            duk_unity_wrap_debug_peek_function,     /* peek callback (optional) */
                            duk_unity_wrap_debug_read_flush_function,    /* read flush callback (optional) */
                            duk_unity_wrap_debug_write_flush_function,   /* write flush callback (optional) */
                            duk_unity_wrap_debug_request_function,       /* app request callback (optional) */
                            duk_unity_wrap_debug_detached_function,    /* debugger detached callback */
                            debugger);                              /* debug udata */
    }
    return debugger;
#else
    return NULL;
#endif
}

DUK_EXTERNAL void duk_unity_detach_debugger(duk_context *ctx, void *debugger) {
    duk_debugger_detach(ctx);
    duk_free(ctx, debugger);
}

/**
 * handler, dispatcher, eventdispatcher
 */

DUK_LOCAL duk_ret_t duk_events_handler_constructor(duk_context *ctx) {
    duk_push_this(ctx);
    duk_dup(ctx, 0);
    duk_put_prop_string(ctx, -2, "caller");
    duk_dup(ctx, 1);
    duk_put_prop_string(ctx, -2, "fn");
    duk_dup(ctx, 2);
    duk_to_boolean(ctx, -1);
    duk_put_prop_string(ctx, -2, "once");
    return 0;
}

DUK_LOCAL duk_ret_t duk_events_handler_equals(duk_context *ctx) {
    // equals(caller, fn?)
    duk_idx_t nargs = duk_get_top(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "caller");
    if (duk_equals(ctx, 0, -1)) {
        duk_pop(ctx);  // [this]
        if (nargs < 2 || duk_is_null_or_undefined(ctx, 1)) {
            duk_pop(ctx); // []
            duk_push_true(ctx);
            return 1;
        }
        
        duk_get_prop_string(ctx, -1, "fn"); // [this, fn]
        if (duk_equals(ctx, 1, -1)) {
            duk_pop(ctx); // [this]
            duk_pop(ctx); // []
            duk_push_true(ctx);
            return 1;
        }
        duk_pop(ctx);
    }
    duk_push_false(ctx);
    return 1;
}

DUK_LOCAL duk_ret_t duk_events_handler_invoke(duk_context *ctx) {
    duk_idx_t nargs = duk_get_top(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "fn");
    duk_get_prop_string(ctx, -2, "caller");
    for (duk_idx_t i = 0; i < nargs; i++) {
        duk_dup(ctx, i);
    }
    if (duk_pcall_method(ctx, nargs) != DUK_EXEC_SUCCESS) {
        return duk_throw(ctx);
    }
    return 1;
}

DUK_LOCAL duk_ret_t duk_events_dispatcher_constructor(duk_context *ctx) {
    duk_push_this(ctx);
    duk_push_array(ctx);
    duk_push_uint(ctx, 0);
    duk_put_prop_index(ctx, -2, 0);
    duk_put_prop_string(ctx, -2, "handlers");
    duk_pop(ctx);
    return 0;
}

DUK_LOCAL duk_ret_t duk_events_dispatcher_on(duk_context *ctx) {
    if (!duk_is_function(ctx, 1)) {
        return duk_generic_error(ctx, "fn not a function");
    }
    duk_bool_t once = duk_get_boolean_default(ctx, 2, 0);
    duk_builtins_reg_get(ctx, DUK_UNITY_BUILTINS_HANDLER);
    duk_dup(ctx, 0);
    duk_dup(ctx, 1);
    duk_push_boolean(ctx, once);
    if (duk_pnew(ctx, 3) != 0) {
        return duk_throw(ctx);
    }
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "handlers"); // handler, this, handlers
    duk_get_prop_index(ctx, -1, 0); // handler, this, handlers, freeslot
    duk_uint_t freeslot = duk_get_uint_default(ctx, -1, 0);
    duk_pop(ctx); // handler, this, handlers
    if (freeslot == 0) {
        duk_size_t length = duk_get_length(ctx, -1);
        duk_dup(ctx, -3); // handler, this, handlers, handler
        duk_put_prop_index(ctx, -2, (duk_uarridx_t)length); // // handler, this, handlers
        duk_pop_3(ctx);
    } else {
        duk_get_prop_index(ctx, -1, freeslot);
        duk_uint_t next_slot = duk_get_uint(ctx, -1);
        duk_pop(ctx);
        duk_push_uint(ctx, next_slot);
        duk_put_prop_index(ctx, -2, 0); // handler, this, handlers
        duk_dup(ctx, -3);
        duk_put_prop_index(ctx, -2, freeslot); // handler, this, handlers
        duk_pop_3(ctx);
    }
    duk_push_this(ctx);
    return 1;
}

DUK_LOCAL duk_ret_t duk_events_dispatcher_off(duk_context *ctx) {
    duk_idx_t nargs = duk_get_top(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "handlers"); // [argN], this, handlers
    duk_size_t length = duk_get_length(ctx, -1);
    for (duk_size_t i = 1; i < length; i++) {
        duk_get_prop_index(ctx, -1, (duk_uarridx_t)i); // [argN], this, handlers, el
        if (!duk_is_number(ctx, -1)) {
            duk_push_string(ctx, "equals");
            duk_dup(ctx, 0);
            if (nargs > 1) {
                duk_dup(ctx, 1);
            } else {
                duk_push_undefined(ctx);
            }
            if (duk_pcall_prop(ctx, -4, 2) != DUK_EXEC_SUCCESS) {
                return duk_throw(ctx);
            }
            // if (duk_get_top(ctx) != 4 + nargs) {
            //     return duk_generic_error(ctx, "aaa, nargs: %d top: %d", nargs, duk_get_top(ctx));
            // }
            // [argN], this, handlers, el, el.ret
            duk_bool_t eq = duk_get_boolean_default(ctx, -1, 0);
            duk_pop(ctx); // pop el.ret
            // [argN], this, handlers, el
            if (eq) {
                // this._handlers[i] = this._handlers[0]
                // this._handlers[0] = i
                duk_unity_array_assign(ctx, -2, (duk_uarridx_t)i, 0);
                duk_push_uint(ctx, (duk_uint_t)i); // [argN], this, handlers, el, i
                duk_put_prop_index(ctx, -3, 0);
            }
        }
        duk_pop(ctx); // pop el
        // if (duk_get_top(ctx) != 2 + nargs) {
        //     return duk_generic_error(ctx, "aaa, nargs: %d top: %d", nargs, duk_get_top(ctx));
        // }
    }
    duk_pop_2(ctx); // .
    return 0;
}

DUK_LOCAL duk_ret_t duk_events_dispatcher_dispatch(duk_context *ctx) {
    duk_idx_t nargs = duk_get_top(ctx);
    duk_push_undefined(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "handlers"); // stub, this, handlers
    duk_size_t length = duk_get_length(ctx, -1);
    for (duk_size_t i = 1; i < length; i++) {
        duk_get_prop_index(ctx, -1, (duk_uarridx_t)i); // stub, this, handlers, el
        if (!duk_is_number(ctx, -1)) {
            duk_push_string(ctx, "invoke");
            for (duk_idx_t argi = 0; argi < nargs; argi++) {
                duk_dup(ctx, argi);
            }
            if (duk_pcall_prop(ctx, -nargs - 2, nargs) != DUK_EXEC_SUCCESS) {
                return duk_throw(ctx);
            }
            // stub, this, handlers, el, ret
            duk_replace(ctx, -5); // ret, this, handlers, el
        } 
        duk_pop(ctx); // pop el
    }
    duk_pop_2(ctx); // .
    return 1;
}

/*
for (let i = 1; i < this._handlers.length; i++) {
    let el = this._handlers[i]
    if (typeof el != "number") {
        this._handlers[i] = this._handlers[0]
        this._handlers[0] = i
    }
}
*/
DUK_LOCAL duk_ret_t duk_events_dispatcher_clear(duk_context *ctx) {
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "handlers"); // this, handlers
    duk_size_t length = duk_get_length(ctx, -1);
    for (duk_size_t i = 1; i < length; i++) {
        duk_get_prop_index(ctx, -1, (duk_uarridx_t)i); // this, handlers, el
        if (!duk_is_number(ctx, -1)) {
            duk_unity_array_assign(ctx, -2, (duk_uarridx_t)i, 0);
            duk_push_uint(ctx, (duk_uint_t)i);
            duk_put_prop_index(ctx, -3, 0);
        }
        duk_pop(ctx);
    }
    duk_pop_2(ctx);
    return 0;
}

DUK_LOCAL duk_ret_t duk_events_eventdispatcher_constructor(duk_context *ctx) {
    duk_push_this(ctx);
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "events");
    duk_pop(ctx);
    return 0;
}

DUK_INTERNAL duk_ret_t duk_events_eventdispatcher_on(duk_context *ctx) {
    // on(type, caller, fn, once?)
    duk_idx_t nargs = duk_get_top(ctx);
    if (nargs < 3) {
        return duk_generic_error(ctx, "3 args at least");
    }
    if (!duk_is_string(ctx, 0)) {
        return duk_generic_error(ctx, "event type require string");
    }
    if (!duk_is_function(ctx, 2)) {
        return duk_generic_error(ctx, "fn require function");
    }
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "events"); // this, events
    const char *type = duk_get_string_default(ctx, 0, NULL);
    if (type == NULL) {
        return duk_generic_error(ctx, "type is null");
    }
    if (!duk_get_prop_string(ctx, -1, type)) { // this, events, events[type]
        duk_pop(ctx);
        duk_builtins_reg_get(ctx, DUK_UNITY_BUILTINS_DISPATCHER); 
        if (duk_pnew(ctx, 0) != 0) {
            return duk_throw(ctx);
        }
        duk_dup(ctx, -1);
        // this, events, newdispatcher, newdispatcher
        duk_put_prop_string(ctx, -3, type); // this, events, dispatcher
    }
    duk_push_string(ctx, "on");
    duk_dup(ctx, 1); // caller
    duk_dup(ctx, 2); // fn
    if (nargs < 4) {
        duk_push_false(ctx); // once:false
    } else {
        duk_dup(ctx, 3); // once
    }
    if (duk_pcall_prop(ctx, -5, 3) != 0) {
        return duk_throw(ctx);
    }
    // this, events, dispatcher, ret
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    return 1;
}

DUK_INTERNAL duk_ret_t duk_events_eventdispatcher_off(duk_context *ctx) {
    // off(type, caller, fn)
    duk_idx_t nargs = duk_get_top(ctx);
    if (nargs < 2) {
        return duk_generic_error(ctx, "2 args at least");
    }
    if (!duk_is_string(ctx, 0)) {
        return duk_generic_error(ctx, "event type require string");
    }
    if (nargs > 2 && !duk_is_function(ctx, 2) && !duk_is_null_or_undefined(ctx, 2)) {
        return duk_generic_error(ctx, "fn require function or null");
    }
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "events"); // this, events
    const char *type = duk_get_string_default(ctx, 0, NULL);
    if (type == NULL) {
        duk_pop_2(ctx);
        return 0;
    }
    if (!duk_get_prop_string(ctx, -1, type)) { // this, events, events[type]
        duk_pop_3(ctx);
        return 0;
    }
    // this, events, dispatcher
    duk_push_string(ctx, "off");
    duk_dup(ctx, 1); // caller
    if (nargs < 3) {
        duk_push_undefined(ctx);
    } else {
        duk_dup(ctx, 2); // fn
    }
    if (duk_pcall_prop(ctx, -4, 2) != 0) {
        return duk_throw(ctx);
    }
    // this, events, dispatcher, ret
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    return 1;
}

DUK_INTERNAL duk_ret_t duk_events_eventdispatcher_clear(duk_context *ctx) {
    // clear(type)
    duk_idx_t nargs = duk_get_top(ctx);
    if (nargs < 1) {
        return duk_generic_error(ctx, "1 args at least");
    }
    if (!duk_is_string(ctx, 0)) {
        return duk_generic_error(ctx, "event type require string");
    }
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "events"); // this, events
    const char *type = duk_get_string_default(ctx, 0, NULL);
    if (type == NULL) {
        duk_pop_2(ctx);
        return 0;
    }
    if (!duk_get_prop_string(ctx, -1, type)) { // this, events, events[type]
        duk_pop_3(ctx);
        return 0;
    }
    // this, events, dispatcher
    duk_push_string(ctx, "clear");
    if (duk_pcall_prop(ctx, -2, 0) != 0) {
        return duk_throw(ctx);
    }
    // this, events, dispatcher, ret
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    return 1;
}

DUK_INTERNAL duk_ret_t duk_events_eventdispatcher_dispatch(duk_context *ctx) {
    // dispatch(type, ...args)
    duk_idx_t nargs = duk_get_top(ctx);
    if (nargs < 1) {
        return duk_generic_error(ctx, "1 args at least");
    }
    if (!duk_is_string(ctx, 0)) {
        return duk_generic_error(ctx, "event type require string");
    }
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "events"); // this, events
    const char *type = duk_get_string_default(ctx, 0, NULL);
    if (type == NULL) {
        duk_pop_2(ctx);
        return 0;
    }
    if (!duk_get_prop_string(ctx, -1, type)) { // this, events, events[type]
        duk_pop_3(ctx);
        return 0;
    }
    // this, events, dispatcher
    duk_push_string(ctx, "dispatch");
    for (duk_idx_t i = 1; i < nargs; i++) {
        duk_dup(ctx, i);
    }
    if (duk_pcall_prop(ctx, -nargs - 1, nargs - 1) != 0) {
        return duk_throw(ctx);
    }
    // this, events, dispatcher, ret
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    duk_remove(ctx, -2);
    return 1;
}

DUK_LOCAL void duk_events_open(duk_context *ctx) {
    duk_push_global_object(ctx);
    duk_unity_get_prop_object(ctx, -1, "DuktapeJS");
    duk_unity_begin_class(ctx, "Handler", DUK_UNITY_BUILTINS_HANDLER, duk_events_handler_constructor, NULL);
    duk_unity_add_member(ctx, "equals", duk_events_handler_equals, -1);
    duk_unity_add_member(ctx, "invoke", duk_events_handler_invoke, -1);
    duk_unity_end_class(ctx);
    duk_unity_begin_class(ctx, "Dispatcher", DUK_UNITY_BUILTINS_DISPATCHER, duk_events_dispatcher_constructor, NULL);
    duk_unity_add_member(ctx, "on", duk_events_dispatcher_on, -1);
    duk_unity_add_member(ctx, "off", duk_events_dispatcher_off, -1);
    duk_unity_add_member(ctx, "clear", duk_events_dispatcher_clear, -1);
    duk_unity_add_member(ctx, "dispatch", duk_events_dispatcher_dispatch, -1);
    duk_unity_end_class(ctx);
    duk_unity_begin_class(ctx, "EventDispatcher", DUK_UNITY_BUILTINS_EVENTDISPATCHER, duk_events_eventdispatcher_constructor, NULL);
    duk_unity_add_member(ctx, "on", duk_events_eventdispatcher_on, -1);
    duk_unity_add_member(ctx, "off", duk_events_eventdispatcher_off, -1);
    duk_unity_add_member(ctx, "clear", duk_events_eventdispatcher_clear, -1);
    duk_unity_add_member(ctx, "dispatch", duk_events_eventdispatcher_dispatch, -1);
    duk_unity_end_class(ctx);
    duk_pop_2(ctx); // pop DuktapeJS and global
}

DUK_EXTERNAL duk_bool_t duk_unity_open(duk_context *ctx) {
    duk_refsys_open(ctx);
    // duk_rws_open(ctx);
    duk_events_open(ctx);
    duk_unity_valuetypes_open(ctx);
    duk_timeout_open(ctx);
    duk_websocket_open(ctx);
    duk_fmath_open(ctx);
    return 1;
}

// DUK_EXTERNAL void duk_unity_suspend(duk_hthread *thr) {
//     duk_suspend(thr, &thr->__state);
// }

// DUK_EXTERNAL void duk_unity_resume(duk_hthread *thr) {
//     duk_resume(thr, &thr->__state);
// }
