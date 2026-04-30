#ifndef SIMPLEAMQPCLIENT_NONCOPYABLE_H
#define SIMPLEAMQPCLIENT_NONCOPYABLE_H

namespace AmqpClient {

    struct NonCopyable {
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable(NonCopyable&&) noexcept = default;
        NonCopyable& operator=(const NonCopyable&) = delete;
        NonCopyable& operator=(NonCopyable&&) noexcept = default;
    };

}

#endif  // SIMPLEAMQPCLIENT_BASICMESSAGE_H
