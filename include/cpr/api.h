#ifndef CPR_API_H
#define CPR_API_H

#include <fstream>
#include <functional>
#include <future>
#include <string>
#include <utility>

#include "cpr/auth.h"
#include "cpr/bearer.h"
#include "cpr/cprtypes.h"
#include "cpr/digest.h"
#include "cpr/multipart.h"
#include "cpr/ntlm.h"
#include "cpr/payload.h"
#include "cpr/response.h"
#include "cpr/session.h"
#include <utility>

namespace cpr {

using AsyncResponse = std::future<Response>;

namespace priv {

template <typename... Ts>
void set_option(Session& session, Ts&&... ts) {
    std::initializer_list<int> ignore = { (session.SetOption(std::forward<Ts>(ts)), 0)... };
    (void)ignore;
}

} // namespace priv

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
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Get(std::move(ts_inner)...); }, std::move(ts)...);
}

// Get callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto GetCallback(Then then, Ts... ts) -> std::future<decltype(then(Get(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Get(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
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
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Post(std::move(ts_inner)...); }, std::move(ts)...);
}

// Post callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto PostCallback(Then then, Ts... ts) -> std::future<decltype(then(Post(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Post(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
}

// Put methods
template <typename... Ts>
Response Put(Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Put();
}

// Put async methods
template <typename... Ts>
AsyncResponse PutAsync(Ts... ts) {
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Put(std::move(ts_inner)...); }, std::move(ts)...);
}

// Put callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto PutCallback(Then then, Ts... ts) -> std::future<decltype(then(Put(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Put(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
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
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Head(std::move(ts_inner)...); }, std::move(ts)...);
}

// Head callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto HeadCallback(Then then, Ts... ts) -> std::future<decltype(then(Head(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Head(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
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
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Delete(std::move(ts_inner)...); },
            std::move(ts)...);
}

// Delete callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto DeleteCallback(Then then, Ts... ts) -> std::future<decltype(then(Delete(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Delete(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
}

// Options methods
template <typename... Ts>
Response Options(Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Options();
}

// Options async methods
template <typename... Ts>
AsyncResponse OptionsAsync(Ts... ts) {
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Options(std::move(ts_inner)...); },
            std::move(ts)...);
}

// Options callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto OptionsCallback(Then then, Ts... ts)
        -> std::future<decltype(then(Options(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Options(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
}

// Patch methods
template <typename... Ts>
Response Patch(Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Patch();
}

// Patch async methods
template <typename... Ts>
AsyncResponse PatchAsync(Ts... ts) {
    return std::async(
            std::launch::async, [](Ts... ts_inner) { return Patch(std::move(ts_inner)...); }, std::move(ts)...);
}

// Patch callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto PatchCallback(Then then, Ts... ts) -> std::future<decltype(then(Patch(std::move(ts)...)))> {
    return std::async(
            std::launch::async, [](Then then_inner, Ts... ts_inner) { return then_inner(Patch(std::move(ts_inner)...)); },
            std::move(then), std::move(ts)...);
}

// Download methods
template <typename... Ts>
Response Download(std::ofstream& file, Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Download(file);
}

// Download with user callback
template <typename... Ts>
Response Download(const WriteCallback& write, Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Download(write);
}

} // namespace cpr

#endif
