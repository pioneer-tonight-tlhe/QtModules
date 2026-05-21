# Singleton 模板使用文档

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-19

## 概述

`Singleton` 是一个线程安全的单例模板类，使用 `std::shared_ptr` 管理实例，基于 `std::call_once` 确保多线程环境下只创建一次实例。

## 特性

- **线程安全**：使用 `std::call_once` 保证多线程环境下实例只创建一次
- **智能指针**：使用 `std::shared_ptr` 管理实例生命周期
- **禁止拷贝和赋值**：删除拷贝构造和赋值运算符，防止复制
- **地址打印**：提供 `PrintAddress()` 方法打印实例地址
- **析构提示**：析构时打印提示信息

## 模板参数

```cpp
template <typename T>
class Singleton
```

- `T`：要包装为单例的类类型

## 公共 API

### getInstance

```cpp
static std::shared_ptr<T> getInstance();
```

**说明**：获取单例实例。首次调用时创建实例，后续调用返回已存在的实例。

**返回值**：`std::shared_ptr<T>` - 单例实例的智能指针

**线程安全**：是

### printAddress

```cpp
void printAddress();
```

**说明**：打印单例实例的内存地址到标准输出。

---

## 使用示例

### 基础使用

```cpp
#include "singleton/singleton.h"

// 定义一个类，继承 Singleton 模板
class MyClass : public Singleton<MyClass> {
    friend class Singleton<MyClass>;  // 允许 Singleton 访问构造函数
    
private:
    MyClass() {
        std::cout << "MyClass constructed" << std::endl;
    }
    
public:
    void DoSomething() {
        std::cout << "Doing something..." << std::endl;
    }
};

int main() {
    // 获取单例实例
    auto instance = MyClass::getInstance();
    instance->DoSomething();
    
    // 打印实例地址
    instance->printAddress();
    
    // 再次获取，返回的是同一个实例
    auto instance2 = MyClass::getInstance();
    instance2->printAddress();  // 地址与第一次相同
    
    return 0;
}
```

### 完整示例（含多线程测试）

```cpp
#include "singleton/singleton.h"
#include <thread>
#include <vector>

class Database : public Singleton<Database> {
    friend class Singleton<Database>;
    
private:
    Database() {
        std::cout << "Database initialized" << std::endl;
    }
    
public:
    void Connect() {
        std::cout << "Connecting to database..." << std::endl;
    }
    
    void Query(const std::string& sql) {
        std::cout << "Executing: " << sql << std::endl;
    }
};

void worker(int id) {
    auto db = Database::GetInstance();
    db->Connect();
    db->Query("SELECT * FROM table_" + std::to_string(id));
}

int main() {
    std::vector<std::thread> threads;
    
    // 创建多个线程，每个线程获取单例
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    // 打印实例地址
    Database::getInstance()->printAddress();
    
    return 0;
}
```

### 配置类单例示例

```cpp
#include "singleton/singleton.h"

class Config : public Singleton<Config> {
    friend class Singleton<Config>;
    
private:
    Config() {
        // 加载默认配置
        port_ = 8080;
        host_ = "localhost";
        std::cout << "Config loaded with defaults" << std::endl;
    }
    
public:
    int port_;
    std::string host_;
    
    void SetPort(int port) { port_ = port; }
    void SetHost(const std::string& host) { host_ = host; }
    
    void Print() {
        std::cout << "Host: " << host_ << ", Port: " << port_ << std::endl;
    }
};

int main() {
    // 获取配置实例
    auto config = Config::getInstance();
    config->Print();  // 输出默认配置
    
    // 修改配置
    config->SetPort(9090);
    config->SetHost("example.com");
    
    // 再次获取，配置已更新
    auto config2 = Config::getInstance();
    config2->Print();  // 输出: Host: example.com, Port: 9090
    
    return 0;
}
```

## 实现原理

### 线程安全保证

```cpp
static std::shared_ptr<T> getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _instance = shared_ptr<T>(new T);
    });
    return _instance;
}
```

- `std::once_flag`：确保 lambda 只执行一次
- `std::call_once`：多线程环境下，只有一个线程会执行初始化代码
- 其他线程会阻塞等待初始化完成

### 生命周期管理

```cpp
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
```

- 使用静态成员变量存储实例
- 通过 `std::shared_ptr` 管理生命周期
- 程序退出时自动释放（析构函数会打印提示）

## 注意事项

1. **友元声明**：子类必须声明 `friend class Singleton<T>;`，否则无法访问私有构造函数

2. **构造函数私有**：子类构造函数必须声明为 `private` 或 `protected`，防止外部直接实例化

3. **默认构造函数**：子类需要提供默认构造函数（无参），否则需要修改 Singleton 模板以支持参数传递

4. **析构顺序**：单例实例在程序退出时自动析构，析构顺序由编译器决定

5. **智能指针**：返回的是 `std::shared_ptr`，可以安全地传递和持有

## 与 LoggerManager 的区别

| 特性 | Singleton 模板 | LoggerManager |
|------|----------------|---------------|
| 实现方式 | 模板类 + shared_ptr | 静态局部变量（Meyers' Singleton） |
| 实例获取 | `getInstance()` | `instance()` |
| 返回类型 | `shared_ptr<T>` | 引用 `T&` |
| 适用场景 | 通用单例模板 | 专用日志管理器 |
| 生命周期 | shared_ptr 管理 | 程序结束时自动释放 |

## 编译要求

- C++11 或更高版本（`std::once_flag`、`std::call_once`、`std::shared_ptr`）
- 支持 `<memory>`、`<mutex>`、`<iostream>` 标准库
