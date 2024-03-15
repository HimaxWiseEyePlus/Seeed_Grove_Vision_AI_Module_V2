#pragma once

#include <cstdio>
#include <functional>
#include <forward_list>

using namespace std;

static int cnt = 0;
struct test {
    test() : i{cnt++} {
        std::printf("\ttest() -> %d\n", i);
    }

    test(const test& t) : i{cnt++} {
        std::printf("\ttest(const test& t) -> %d from %d\n", i, t.i);
    }

    test(test&& t) : i{cnt++} {
        std::printf("\ttest(test&& t) -> %d from %d\n", i, t.i);
    }

    ~test() {
        std::printf("\t~test() -> %d\n", i);
    }

    int i;
};

std::forward_list<std::function<void(int i)>> * vec = nullptr;

template <typename Callable>
void foo(Callable&& c) {
    vec->emplace_front(std::forward<Callable>(c));
}

void bar(std::function<void(int i)> c) {
    vec->emplace_front(std::move(c));
}

void bad(std::function<void(int i)> c) {
    vec->emplace_front(c);
}


void test_ctor_dtor() {
    std::forward_list<std::function<void(int i)>> v;

    vec = &v;

    {
        puts("[ctor_dtor] test1");
        foo([](int i){});
        foo([v = test()](int i){});
        auto v = test();
        foo([copy = v](int i){});
    }
    cnt = 0;
    {
        puts("[ctor_dtor] test2");
        bar([](int i){});
        bar([v = test()](int i){});
        auto v = test();
        bar([copy = v](int i){});
    }
    cnt = 0;
    {
        puts("[ctor_dtor] test3");
        bad([](int i){});
        bad([v = test()](int i){});
        auto v = test();
        bad([copy = v](int i){});
    }
    cnt = 0;
    {
        puts("[ctor_dtor] test4");
        const auto& v = test();
    }
        puts("end");
}
