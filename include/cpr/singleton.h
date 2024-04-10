#ifndef CPR_SINGLETON_H
#define CPR_SINGLETON_H

#include <cassert>
#include <mutex>

#ifndef CPR_DISABLE_COPY
#define CPR_DISABLE_COPY(Class)   \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;
#endif

#ifndef CPR_SINGLETON_DECL
#define CPR_SINGLETON_DECL(Class)    \
  public:                            \
    static Class* GetInstance();     \
    static void ExitInstance();      \
                                     \
  private:                           \
    CPR_DISABLE_COPY(Class)          \
    static Class* s_pInstance;       \
    static std::once_flag s_getFlag; \
    static std::once_flag s_exitFlag;
#endif

#ifndef CPR_SINGLETON_IMPL
#define CPR_SINGLETON_IMPL(Class)                                            \
    Class* Class::s_pInstance = nullptr;                                     \
    std::once_flag Class::s_getFlag{};                                       \
    std::once_flag Class::s_exitFlag{};                                      \
    Class* Class::GetInstance() {                                            \
        std::call_once(Class::s_getFlag, []() { s_pInstance = new Class; }); \
        return s_pInstance;                                                  \
    }                                                                        \
    void Class::ExitInstance() {                                             \
        std::call_once(Class::s_exitFlag, []() {                             \
            assert(s_pInstance);                                             \
            delete s_pInstance;                                              \
            s_pInstance = nullptr;                                           \
        });                                                                  \
    }
#endif

#endif
