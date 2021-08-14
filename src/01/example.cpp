#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

struct ReturnObject
{
    struct promise_type
    {
        ReturnObject get_return_object()
        {
            return {
                // from_promise - Promiseオブジェクトから対応するコルーチンハンドルを取得する
                .h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
    };

    std::coroutine_handle<promise_type> h_;
    operator std::coroutine_handle<promise_type>() const { return h_; }
    operator std::coroutine_handle<>() const { return h_; }
};

ReturnObject
counter()
{
    for (unsigned i = 0;; ++i)
    {
        // std::suspend_always - コルーチンを中断させる, トリビアルAwaitable型
        co_await std::suspend_always{};
        std::cout << "counter: " << i << std::endl;
    }
}

int main()
{
    std::coroutine_handle<> h = counter();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "In main function\n";
        h();
    }
    h.destroy();
    return 0;
}
