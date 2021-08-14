#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

/*
    コルーチンとは, 任意の箇所で停止・再開することができる関数のことである. 通常の関数では, その関数において定義
    されている処理が全て完了するまで待つ必要があるが, コルーチンを使用することで並列して処理を行うことが可能になる
 */

struct ReturnObject
{
    struct promise_type
    {
        ReturnObject get_return_object() { return {}; }
        // std::suspend_never - コルーチンを中断させない, トリビアルAwaitable型
        std::suspend_never initial_suspend() { return {}; }
        // noexcept - 関数が例外をスローしないことを定義
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
    };
};

struct Awaiter
{
    std::coroutine_handle<> *hp_;
    // constexprは, constant expressionの略語で汎用的に定数式を表現するための機能である. この機能を使用
    // することで, コンパイル時に値が決定する定数, コンパイル時に実行される関数, コンパイル時にリテラルとして振る舞
    // うクラスを定義できる
    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) { *hp_ = h; }
    constexpr void await_resume() const noexcept {}
};

ReturnObject
counter(std::coroutine_handle<> *continuation_out)
{
    Awaiter a{continuation_out};
    for (unsigned i = 0;; ++i)
    {
        // 中断されたコルーチンから呼出元へ, コルーチンハンドルを内包するコルーチン戻り値型オブジェクトを返す
        co_await a;
        std::cout << "counter: " << i << std::endl;
    }
}

int main()
{
    /*
        コルーチンに対応するコルーチンハンドル. テンプレートパラメータPromiseには, コルーチンのPromise型を指定
        する. coroutine_handle<void>または単にcoroutine_handle<>は, Promise型について型消去(Type
        -erased)されたコルーチンハンドルとして取り扱える.
     */
    std::coroutine_handle<> h;
    counter(&h);
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "In main function\n";
        h();
    }
    // 中断状態にあるコルーチンを明示的に破棄する
    h.destroy();
    return 0;
}

// main関数とcounter関数で何度も処理が切り替わっているが, counterの値が1ずつ増加していることに着目する
/* output
    In main function
    counter: 0
    In main function
    counter: 1
    In main function
    counter: 2
 */
