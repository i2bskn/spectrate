#include "spectrate.h"

VALUE rb_cSpectrate;

struct spectrate_object {
    int range_size;
    int range_max;
    int *borders;
    VALUE objects;
    VALUE ranges;
};

static void
spectrate_mark(void *ptr) {
    struct spectrate_object *spectrum = ptr;
    rb_gc_mark(spectrum->objects);
    rb_gc_mark(spectrum->ranges);
}

static void
spectrate_free(void *ptr) {
    struct spectrate_object *spectrum = ptr;
    if (spectrum->borders != NULL) {
        free(spectrum->borders);
    }
}

static size_t
spectrate_memsize(const void *spectrum) {
    return sizeof(struct spectrate_object);
}

static const rb_data_type_t spectrate_type = {
    "spectrate",
    {
        spectrate_mark,
        spectrate_free,
        spectrate_memsize,
    },
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE
spectrate_alloc(VALUE klass) {
    struct spectrate_object *spectrum;
    VALUE obj = TypedData_Make_Struct(klass, struct spectrate_object, &spectrate_type, spectrum);
    return obj;
}

static VALUE
spectrate_init(VALUE obj, VALUE objects, VALUE ranges) {
    struct spectrate_object *spectrum;
    TypedData_Get_Struct(obj, struct spectrate_object, &spectrate_type, spectrum);

    int i, object_size;

    Check_Type(objects, T_ARRAY);
    Check_Type(ranges, T_ARRAY);
    spectrum->range_size = RARRAY_LENINT(ranges);
    object_size = RARRAY_LENINT(objects);
    if (object_size < spectrum->range_size) {
        rb_raise(rb_eArgError, "The size of the objects must be greater than or equal ranges");
    } else if (object_size == 0 || spectrum->range_size == 0) {
        rb_raise(rb_eArgError, "Objects and ranges must be non empty");
    }

    spectrum->range_max = 0;
    spectrum->borders = (int *)malloc(sizeof(int) * spectrum->range_size);

    for (i = 0; i < spectrum->range_size; i++) {
        spectrum->range_max += FIX2INT(RARRAY_AREF(ranges, i));
        spectrum->borders[i] = spectrum->range_max;
    }

    spectrum->objects = objects;
    spectrum->ranges = ranges;
    return obj;
}

static VALUE
spectrate_sample(VALUE obj) {
    struct spectrate_object *spectrum;
    TypedData_Get_Struct(obj, struct spectrate_object, &spectrate_type, spectrum);

    int i, tmin;

    tmin = rand() % spectrum->range_max;
    for (i = 0; i < spectrum->range_size; i++) {
        if (tmin < spectrum->borders[i]) {
            break;
        }
    }
    return RARRAY_AREF(spectrum->objects, i);
}

static VALUE
spectrate_select(VALUE obj, VALUE index) {
    struct spectrate_object *spectrum;
    TypedData_Get_Struct(obj, struct spectrate_object, &spectrate_type, spectrum);

    int i, tmin, tmax, rmin;
    VALUE result = Qnil;

    switch (TYPE(index)) {
        case T_FIXNUM:
            tmin = FIX2INT(index);

            for (i = 0; i < spectrum->range_size; i++) {
                if (tmin < spectrum->borders[i]) {
                    result = RARRAY_AREF(spectrum->objects, i);
                    break;
                }
            }
            break;
        default:
            if (rb_funcall(index, rb_intern("is_a?"), 1, rb_cRange) == Qtrue) {
                result = rb_ary_new();
                tmin = FIX2INT(rb_funcall(index, rb_intern("min"), 0));
                tmax = FIX2INT(rb_funcall(index, rb_intern("max"), 0));

                for (i = 0; i < spectrum->range_size; i++) {
                    if (i == 0) {
                        rmin = 0;
                    } else {
                        rmin = spectrum->borders[i - 1];
                    }

                    if (tmin < spectrum->borders[i] && rmin <= tmax) {
                        rb_ary_push(result, RARRAY_AREF(spectrum->objects, i));
                    }

                    if (rmin >= tmax) {
                        break;
                    }
                }
            } else {
                rb_raise(rb_eArgError, "Invalid index type");
            }
            break;
    }

    return result;
}

void
Init_spectrate(void) {
    rb_cSpectrate = rb_define_class("Spectrate", rb_cObject);
    rb_define_alloc_func(rb_cSpectrate, spectrate_alloc);
    rb_define_method(rb_cSpectrate, "initialize", spectrate_init, 2);
    rb_define_method(rb_cSpectrate, "sample", spectrate_sample, 0);
    rb_define_method(rb_cSpectrate, "[]", spectrate_select, 1);
}
