#ifndef CPR_SINGLETON_H
#define CPR_SINGLETON_H

#include <mutex>

#define DISABLE_COPY(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;

#define SINGLETON_DECL(Class) \
    public: \
        static Class* instance(); \
        static void exitInstance(); \
    private: \
        DISABLE_COPY(Class) \
        static Class* s_pInstance; \
        static std::mutex s_mutex;

#define SINGLETON_IMPL(Class) \
    Class* Class::s_pInstance = nullptr; \
    std::mutex Class::s_mutex; \
    Class* Class::instance() { \
        if (s_pInstance == nullptr) { \
            s_mutex.lock(); \
            if (s_pInstance == nullptr) { \
                s_pInstance = new Class; \
            } \
            s_mutex.unlock(); \
        } \
        return s_pInstance; \
    } \
    void Class::exitInstance() { \
        s_mutex.lock(); \
        if (s_pInstance) {  \
            delete s_pInstance; \
            s_pInstance = nullptr; \
        }   \
        s_mutex.unlock(); \
    }

#endif
