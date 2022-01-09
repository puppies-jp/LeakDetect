#include <pthread.h>
#include <thread>
#include <mutex>

template <typename T>
class scoped_lock_
{
    T &m_;

public:
    explicit scoped_lock_(T &m) : m_(m) { m.lock(); };
    ~scoped_lock_() { m_.unlock(); };

private:
    void operator=(const scoped_lock_ &src) {}
    scoped_lock_(const scoped_lock_ &src) {}
};

struct static_mutex
{
    pthread_mutex_t m_;
    void lock() { pthread_mutex_lock(&m_); }
    void unlock() { pthread_mutex_unlock(&m_); }
    typedef scoped_lock_<static_mutex> scoped_lock;
};

namespace
{
    static_mutex m = {PTHREAD_MUTEX_INITIALIZER};
}

/* how to use lock */
/*
void need_to_sync()
{
    static_mutex::scoped_lock lk(::m);
}
*/