#include <iostream>
#include <string>
#include "PointerWrapper.h"

using std::cout;
using std::endl;
using std::string;

void print_header(const string& name) {
    cout << "\n===== " << name << " =====\n";
}

void print_line(const string& label, const string& expect, const string& actual) {
    cout << label << "\n"
         << "  EXPECT: " << expect << "\n"
         << "  ACTUAL: " << actual << "\n";
}

string bool_str(bool b) { return b ? "true" : "false"; }

// ---------- Tests ----------

void test_default_ctor_and_bool() {
    print_header("Default constructor + operator bool");

    PointerWrapper<int> w;

    bool b = static_cast<bool>(w);

    print_line("Empty wrapper should evaluate to false:",
               "false",
               bool_str(b));
}

void test_raw_pointer_ctor_and_deref() {
    print_header("Constructor from raw pointer + dereference");

    PointerWrapper<int> w(new int(42));

    int value = *w;
    bool b = static_cast<bool>(w);

    print_line("Dereference should give stored value:",
               "42",
               std::to_string(value));

    print_line("Non-empty wrapper should evaluate to true:",
               "true",
               bool_str(b));
}

void test_arrow_operator() {
    print_header("Arrow operator");

    struct Test {
        int x;
        int get() const { return x; }
    };

    PointerWrapper<Test> w(new Test{7});

    int value = w->get();

    print_line("w->get() should return stored x:",
               "7",
               std::to_string(value));
}

void test_get_and_exceptions() {
    print_header("get() and exceptions on null");

    PointerWrapper<int> empty;

    bool get_threw = false;
    try {
        empty.get();
    } catch (const std::runtime_error&) {
        get_threw = true;
    }

    print_line("get() on empty should throw std::runtime_error:",
               "true (should throw)",
               bool_str(get_threw));

    bool star_threw = false;
    try {
        *empty;
    } catch (const std::runtime_error&) {
        star_threw = true;
    }

    print_line("operator* on empty should throw std::runtime_error:",
               "true (should throw)",
               bool_str(star_threw));
}

void test_move_constructor() {
    print_header("Move constructor");

    PointerWrapper<int> src(new int(10));
    PointerWrapper<int> dst(std::move(src));

    bool src_bool = static_cast<bool>(src);
    bool dst_bool = static_cast<bool>(dst);
    int dst_value = *dst;

    print_line("After move: source should be empty (bool false):",
               "false",
               bool_str(src_bool));

    print_line("After move: destination should be non-empty (bool true):",
               "true",
               bool_str(dst_bool));

    print_line("Destination value after move:",
               "10",
               std::to_string(dst_value));
}

void test_move_assignment() {
    print_header("Move assignment operator");

    PointerWrapper<int> src(new int(5));
    PointerWrapper<int> dst; // empty

    dst = std::move(src);

    bool src_bool = static_cast<bool>(src);
    bool dst_bool = static_cast<bool>(dst);
    string dst_val_str;

    if (dst_bool) {
        dst_val_str = std::to_string(*dst);
    } else {
        dst_val_str = "N/A (wrapper empty)";
    }

    print_line("After move assignment: source should be empty (bool false):",
               "false",
               bool_str(src_bool));

    print_line("After move assignment: destination should be non-empty (bool true):",
               "true",
               bool_str(dst_bool));

    print_line("Destination value after move assignment:",
               "5",
               dst_val_str);
}

void test_release() {
    print_header("release()");

    PointerWrapper<int> w(new int(99));

    int* raw = w.release();
    bool w_bool = static_cast<bool>(w);

    print_line("After release: wrapper should be empty (bool false):",
               "false",
               bool_str(w_bool));

    print_line("After release: returned raw pointer should be non-null:",
               "non-null",
               raw ? "non-null" : "null");

    if (raw) {
        print_line("Value via released pointer:",
                   "99",
                   std::to_string(*raw));
        delete raw; // prevent leak
    }
}

void test_reset() {
    print_header("reset()");

    PointerWrapper<int> w(new int(1));

    w.reset(new int(2));

    bool w_bool = static_cast<bool>(w);
    int value = *w;

    print_line("After reset with new ptr: wrapper should be non-empty (bool true):",
               "true",
               bool_str(w_bool));

    print_line("After reset: stored value should be new one:",
               "2",
               std::to_string(value));

    // Also test reset() with default (nullptr)
    w.reset();
    bool empty = static_cast<bool>(w);

    print_line("After reset() with no args: wrapper should be empty (bool false):",
               "false",
               bool_str(empty));
}

void test_swap() {
    print_header("swap()");

    PointerWrapper<int> a(new int(11));
    PointerWrapper<int> b(new int(22));

    int before_a = *a;
    int before_b = *b;

    swap(a, b);

    int after_a = *a;
    int after_b = *b;

    print_line("Before/After swap: a should get b's old value:",
               "before a = 11, after a = 22",
               "before a = " + std::to_string(before_a) +
               ", after a = " + std::to_string(after_a));

    print_line("Before/After swap: b should get a's old value:",
               "before b = 22, after b = 11",
               "before b = " + std::to_string(before_b) +
               ", after b = " + std::to_string(after_b));
}

int main() {
    cout << "Running PointerWrapper tests...\n";

    test_default_ctor_and_bool();
    test_raw_pointer_ctor_and_deref();
    test_arrow_operator();
    test_get_and_exceptions();
    test_move_constructor();
    test_move_assignment();
    test_release();
    test_reset();
    test_swap();

    cout << "\nDone.\n";
    return 0;
}
