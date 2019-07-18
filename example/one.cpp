//#include <iostream>

template <class T, int Id>
void foo(const T& value) {
    //std::cout << Id << std::endl;
}

template <int Count>
struct BitArray {
    int values[(Count + sizeof(int) - 1)/sizeof(int)];
};

struct A {
    int a;
    int dump() const { return a; }
};

struct B {
    int b0;
    struct {
        char b1b0;
        int b1b1[4];
    } b1[3];
    BitArray<72> b2;
    int dump() const { return b0; }
};

struct C {
    int* a;
};

struct D {
    int& a;
    D(int& other) : a(other) {}
};

struct BitFields {
    int a : 2;
    int b : 4;
    int c : 7;
};

union U {
    int a;
    struct {
        int b;
        int c;
        union {
            int a;
            char b;
        } d;
    } b;
};

enum StandardEnum {
    StandardEnum_A,
    StandardEnum_B
};

enum EnumWithSize : unsigned char {
    EnumWithSize_A,
    EnumWithSize_B
};

enum class EnumClassWithSize : unsigned char {
    A,
    B
};

struct EnumStruct {
    StandardEnum standard;
    EnumWithSize enumWithSize;
    EnumClassWithSize enumClassWithSize;
};

typedef EnumStruct TypedefedStruct;

struct TypedefUsingStruct {
    TypedefedStruct a;
};

#define EXAMPLE_DEFINE unsigned int
#define EXAMPLE_FUNC(A) unsigned A

struct DefineUsingStruct {
    EXAMPLE_DEFINE a;
    EXAMPLE_FUNC(short) b;
};

int main() {
    {
        A a = {};
        foo<A,5>(a);
    }
    {
        B b0 = {};
        foo<B,4>(b0);
    }
    {
        B b1 = {};
        foo<B,4+2>(b1);
    }
    {
        B b2 = {};
        foo<decltype(b2),4+2>(b2);
    }
    {
        C c = {};
        foo<C, 5>(c);
    }
    {
        int a = 0;
        D d{a};
        foo<D,12>(d);
    }
    {
        BitFields b = {};
        foo<BitFields, 16>(b);
    }
    {
        U a = {};
        foo<U, 17>(a);
    }
    {
        EnumStruct e = {};
        foo<EnumStruct, 18>(e);
    }
    {
        TypedefUsingStruct s = {};
        foo<TypedefUsingStruct, 19>(s);
    }
    {
        DefineUsingStruct s = {};
        foo<DefineUsingStruct, 20>(s);
    }
}