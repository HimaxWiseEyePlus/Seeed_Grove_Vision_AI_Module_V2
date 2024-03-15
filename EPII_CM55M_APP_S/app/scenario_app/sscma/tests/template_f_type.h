#pragma once

#include <cstdio>
#include <cstdint>

template <typename T>
void foo(T v) {
    class A {
    public:
        A(T i) : n(i) {
            puts("\t A -> l T");
        }
        T n;
    };
    A a(v);
    printf("\t foo sizeof(a) -> %d\n", sizeof(a));
}

template <typename T>
void bar(T& v) {
    class A {
    	public:
        A(T& i) : n(i) {
            puts("\t A -> ref T&");
        }
        T& n;
    };
    A a(v);
    printf("\t bar sizeof(a) -> %d\n", sizeof(a));
}

void template_f_type() {
    {
        puts("[template_f_type] test 1");
        uint16_t u = 1;
        foo<uint16_t>(u);
        float b = 2.0;
        foo<float>(b);
    }

    {
        puts("[template_f_type] test 2");
        uint16_t u = 1;
        bar<uint16_t&>(u);
        float b = 2.0;
        bar<float&>(b);
    }
}
