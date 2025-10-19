#include <iostream>
using namespace std;

template <typename T>
class Vector {
public:
    // 建议预留一点冗余容量
    explicit Vector(int initSize = 0)
        : theSize(initSize),
          theCapacity(initSize + SPARE_CAPACITY),
          objects(new T[theCapacity]) {}

    /*
    explicit Vector(int initSize = 0)
        : theSize(initSize),
          theCapacity(initsize + SPARE_CAPACITY)
        { objects = new T[theCapacity]; }
    */

    // 拷贝构造
    Vector(const Vector& rhs)
        : theSize(rhs.theSize),
          theCapacity(rhs.theCapacity),
          objects(new T[theCapacity]) {
        for (int i = 0; i < theSize; ++i) objects[i] = rhs.objects[i];
    }

    // 析构
    ~Vector() { delete[] objects; }

    // 赋值运算符
    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            T* newArray = new T[rhs.theCapacity];
            for (int i = 0; i < rhs.theSize; ++i) newArray[i] = rhs.objects[i];
            delete[] objects;
            objects = newArray;
            theSize = rhs.theSize;
            theCapacity = rhs.theCapacity;
        }
        return *this;
    }

    // 调整逻辑大小
    void resize(int newSize) {
        if (newSize > theCapacity) reserve(newSize * 2 + 1);
        theSize = newSize;
    }

    // 预留（注意是 reserve 不是 reverse）
    void reserve(int newCapacity) {
        if (newCapacity <= theCapacity) return;
        T* oldArray = objects;
        objects = new T[newCapacity];
        for (int k = 0; k < theSize; ++k) objects[k] = oldArray[k];
        theCapacity = newCapacity;
        delete[] oldArray;
    }

    // 下标
    T& operator[](int index) { return objects[index]; }
    const T& operator[](int index) const { return objects[index]; }

    // 基本查询
    bool empty() const { return size() == 0; }
    int size() const { return theSize; }
    int capacity() const { return theCapacity; }

    // 尾部增删
    void push_back(const T& x) {
        if (theSize == theCapacity) reserve(2 * theCapacity + 1);
        objects[theSize++] = x;
    }
    void pop_back() { if (theSize > 0) --theSize; }
    const T& back() const { return objects[theSize - 1]; }

    // 迭代器
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() { return objects; }
    const_iterator begin() const { return objects; }
    iterator end() { return objects + theSize; }
    const_iterator end() const { return objects + theSize; }

private:
    static const int SPARE_CAPACITY = 16;
    int theSize = 0;
    int theCapacity = 0;
    T* objects = nullptr;
};

// 简单测试
int main() {
    Vector<int> v;
    for (int i = 1; i <= 5; ++i) v.push_back(i);
    for (auto x : v) cout << x << ' ';
    cout << "\nsize=" << v.size() << " cap=" << v.capacity() << '\n';
    return 0;
}
