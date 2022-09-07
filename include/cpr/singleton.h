#ifndef CPR_SINGLETON_H
#define CPR_SINGLETON_H

#include <mutex>

#ifndef CPR_DISABLE_COPY
#define CPR_DISABLE_COPY(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;
#endif

#ifndef CPR_SINGLETON_DECL
#define CPR_SINGLETON_DECL(Class) \
    public: \
        static Class* GetInstance(); \
        static void ExitInstance(); \
    private: \
        CPR_DISABLE_COPY(Class) \
        static Class* s_pInstance; \
        static std::mutex s_mutex;
#endif

#ifndef CPR_SINGLETON_IMPL
#define CPR_SINGLETON_IMPL(Class) \
    Class* Class::s_pInstance = nullptr; \
    std::mutex Class::s_mutex; \
    Class* Class::GetInstance() { \
        if (s_pInstance == nullptr) { \
            s_mutex.lock(); \
            if (s_pInstance == nullptr) { \
                s_pInstance = new Class; \
            } \
            s_mutex.unlock(); \
        } \
        return s_pInstance; \
    } \
    void Class::ExitInstance() { \
        s_mutex.lock(); \
        if (s_pInstance) {  \
            delete s_pInstance; \
            s_pInstance = nullptr; \
        }   \
        s_mutex.unlock(); \
    }
#endif

#endif
