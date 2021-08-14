#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

struct ReturnObject
{
    struct promise_type
    {
        unsigned value_;

        ~promise_type()
        {
            std::cout << "promise_type destroyed" << std::endl;
        }
        ReturnObject get_return_object()
        {
            return {
                .h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        std::suspend_always yield_value(unsigned value)
        {
            value_ = value;
            return {};
        }
        void return_void() {}
    };

    std::coroutine_handle<promise_type> h_;
};

ReturnObject
counter()
{
    for (unsigned i = 0; i < 3; ++i)
        co_yield i;
}

int main()
{
    auto h = counter().h_;
    auto &promise = h.promise();
    while (!h.done())
    {
        std::cout << "counter: " << promise.value_ << std::endl;
        h();
    }
    h.destroy();
    return 0;
}
