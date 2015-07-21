#ifndef CPR_API_H
#define CPR_API_H

#include <functional>
#include <future>
#include <string>

#include "auth.h"
#include "cprtypes.h"
#include "digest.h"
#include "multipart.h"
#include "payload.h"
#include "response.h"
#include "session.h"


using AsyncResponse = std::future<Response>;

namespace cpr {
    namespace priv {
        template <typename T>
        void set_option(Session& session, T&& t) {
            session.SetOption(std::forward<T>(t));
        }

        template <typename T, typename... Ts>
        void set_option(Session& session, T&& t, Ts&&... ts) {
            set_option(session, std::forward<T>(t));
            set_option(session, std::forward<Ts>(ts)...);
        }
    }

    // Get methods
    template <typename... Ts>
    Response Get(Ts&&... ts) {
        Session session;
        priv::set_option(session, std::forward<Ts>(ts)...);
        return session.Get();
    }

    // Get async methods
    template <typename... Ts>
    AsyncResponse GetAsync(Ts... ts) {
        return std::async(std::launch::async, [] (Ts... ts) {
                                                  return Get(std::move(ts)...);
                                              }, std::move(ts)...);
    }

    // Post methods
    template <typename... Ts>
    Response Post(Ts&&... ts) {
        Session session;
        priv::set_option(session, std::forward<Ts>(ts)...);
        return session.Post();
    }

    // Post async methods
    template <typename... Ts>
    AsyncResponse PostAsync(Ts... ts) {
        return std::async(std::launch::async, [] (Ts... ts) {
                                                  return Post(std::move(ts)...);
                                              }, std::move(ts)...);
    }

    // Head methods
    template <typename... Ts>
    Response Head(Ts&&... ts) {
        Session session;
        priv::set_option(session, std::forward<Ts>(ts)...);
        return session.Head();
    }

    // Head async methods
    template <typename... Ts>
    AsyncResponse HeadAsync(Ts... ts) {
        return std::async(std::launch::async, [] (Ts... ts) {
                                                  return Head(std::move(ts)...);
                                              }, std::move(ts)...);
    }

    // Delete methods
    template <typename... Ts>
    Response Delete(Ts&&... ts) {
        Session session;
        priv::set_option(session, std::forward<Ts>(ts)...);
        return session.Delete();
    }

    // Delete async methods
    template <typename... Ts>
    AsyncResponse DeleteAsync(Ts... ts) {
        return std::async(std::launch::async, [] (Ts... ts) {
                                                  return Delete(std::move(ts)...);
                                              }, std::move(ts)...);
    }
};

#endif
