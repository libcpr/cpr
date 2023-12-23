#ifndef CPR_API_H
#define CPR_API_H

#include <fstream>
#include <functional>
#include <future>
#include <string>
#include <utility>

#include "cpr/async.h"
#include "cpr/async_wrapper.h"
#include "cpr/auth.h"
#include "cpr/bearer.h"
#include "cpr/cprtypes.h"
#include "cpr/filesystem.h"
#include "cpr/multipart.h"
#include "cpr/multiperform.h"
#include "cpr/payload.h"
#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

using AsyncResponse = AsyncWrapper<Response>;

namespace priv {

template <bool processed_header, typename CurrentType>
void set_option_internal(Session& session, CurrentType&& current_option) {
    session.SetOption(std::forward<CurrentType>(current_option));
}

template <>
inline void set_option_internal<true, Header>(Session& session, Header&& current_option) {
    // Header option was already provided -> Update previous header
    session.UpdateHeader(std::forward<Header>(current_option));
}

template <bool processed_header, typename CurrentType, typename... Ts>
void set_option_internal(Session& session, CurrentType&& current_option, Ts&&... ts) {
    set_option_internal<processed_header, CurrentType>(session, std::forward<CurrentType>(current_option));

    if (std::is_same_v<CurrentType, Header>) {
        set_option_internal<true, Ts...>(session, std::forward<Ts>(ts)...);
    } else {
        set_option_internal<processed_header, Ts...>(session, std::forward<Ts>(ts)...);
    }
}

template <typename... Ts>
void set_option(Session& session, Ts&&... ts) {
    set_option_internal<false, Ts...>(session, std::forward<Ts>(ts)...);
}

// Idea: https://stackoverflow.com/a/19060157
template <typename Tuple, std::size_t... I>
void apply_set_option_internal(Session& session, Tuple&& t, std::index_sequence<I...>) {
    set_option(session, std::get<I>(std::forward<Tuple>(t))...);
}

// Idea: https://stackoverflow.com/a/19060157
template <typename Tuple>
void apply_set_option(Session& session, Tuple&& t) {
    using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
    apply_set_option_internal(session, std::forward<Tuple>(t), Indices());
}

template <typename T>
void setup_multiperform_internal(MultiPerform& multiperform, T&& t) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    apply_set_option(*session, t);
    multiperform.AddSession(session);
}

template <typename T, typename... Ts>
void setup_multiperform_internal(MultiPerform& multiperform, T&& t, Ts&&... ts) {
    std::shared_ptr<Session> session = std::make_shared<Session>();
    apply_set_option(*session, t);
    multiperform.AddSession(session);
    setup_multiperform_internal<Ts...>(multiperform, std::forward<Ts>(ts)...);
}

template <typename... Ts>
void setup_multiperform(MultiPerform& multiperform, Ts&&... ts) {
    setup_multiperform_internal<Ts...>(multiperform, std::forward<Ts>(ts)...);
}

using session_action_t = cpr::Response (cpr::Session::*)();

template <session_action_t SessionAction, typename T>
void setup_multiasync(std::vector<AsyncWrapper<Response, true>>& responses, T&& parameters) {
    std::shared_ptr<std::atomic_bool> cancellation_state = std::make_shared<std::atomic_bool>(false);

    std::function<Response(T)> execFn{[cancellation_state](T params) {
        if (cancellation_state->load()) {
            return Response{};
        }
        cpr::Session s{};
        s.SetCancellationParam(cancellation_state);
        apply_set_option(s, std::forward<T>(params));
        return std::invoke(SessionAction, s);
    }};
    responses.emplace_back(GlobalThreadPool::GetInstance()->Submit(std::move(execFn), std::forward<T>(parameters)), std::move(cancellation_state));
}

template <session_action_t SessionAction, typename T, typename... Ts>
void setup_multiasync(std::vector<AsyncWrapper<Response, true>>& responses, T&& head, Ts&&... tail) {
    setup_multiasync<SessionAction>(responses, std::forward<T>(head));
    if constexpr (sizeof...(Ts) > 0) {
        setup_multiasync<SessionAction>(responses, std::forward<Ts>(tail)...);
    }
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
    return cpr::async([](Ts... ts_inner) { return Get(std::move(ts_inner)...); }, std::move(ts)...);
}

// Get callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto GetCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Get(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
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
    return cpr::async([](Ts... ts_inner) { return Post(std::move(ts_inner)...); }, std::move(ts)...);
}

// Post callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto PostCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Post(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
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
    return cpr::async([](Ts... ts_inner) { return Put(std::move(ts_inner)...); }, std::move(ts)...);
}

// Put callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto PutCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Put(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
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
    return cpr::async([](Ts... ts_inner) { return Head(std::move(ts_inner)...); }, std::move(ts)...);
}

// Head callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto HeadCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Head(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
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
    return cpr::async([](Ts... ts_inner) { return Delete(std::move(ts_inner)...); }, std::move(ts)...);
}

// Delete callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto DeleteCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Delete(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
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
    return cpr::async([](Ts... ts_inner) { return Options(std::move(ts_inner)...); }, std::move(ts)...);
}

// Options callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto OptionsCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Options(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
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
    return cpr::async([](Ts... ts_inner) { return Patch(std::move(ts_inner)...); }, std::move(ts)...);
}

// Patch callback methods
template <typename Then, typename... Ts>
// NOLINTNEXTLINE(fuchsia-trailing-return)
auto PatchCallback(Then then, Ts... ts) {
    return cpr::async([](Then then_inner, Ts... ts_inner) { return then_inner(Patch(std::move(ts_inner)...)); }, std::move(then), std::move(ts)...);
}

// Download methods
template <typename... Ts>
Response Download(std::ofstream& file, Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Download(file);
}

// Download async method
template <typename... Ts>
AsyncResponse DownloadAsync(fs::path local_path, Ts... ts) {
    return AsyncWrapper{std::async(
            std::launch::async,
            [](fs::path local_path_, Ts... ts_) {
                std::ofstream f(local_path_.c_str());
                return Download(f, std::move(ts_)...);
            },
            std::move(local_path), std::move(ts)...)};
}

// Download with user callback
template <typename... Ts>
Response Download(const WriteCallback& write, Ts&&... ts) {
    Session session;
    priv::set_option(session, std::forward<Ts>(ts)...);
    return session.Download(write);
}

// Multi requests
template <typename... Ts>
std::vector<Response> MultiGet(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Get();
}

template <typename... Ts>
std::vector<Response> MultiDelete(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Delete();
}

template <typename... Ts>
std::vector<Response> MultiPut(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Put();
}

template <typename... Ts>
std::vector<Response> MultiHead(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Head();
}

template <typename... Ts>
std::vector<Response> MultiOptions(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Options();
}

template <typename... Ts>
std::vector<Response> MultiPatch(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Patch();
}

template <typename... Ts>
std::vector<Response> MultiPost(Ts&&... ts) {
    MultiPerform multiperform;
    priv::setup_multiperform<Ts...>(multiperform, std::forward<Ts>(ts)...);
    return multiperform.Post();
}

template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiGetAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Get>(ret, std::forward<Ts>(ts)...);
    return ret;
}

template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiDeleteAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Delete>(ret, std::forward<Ts>(ts)...);
    return ret;
}

template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiHeadAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Head>(ret, std::forward<Ts>(ts)...);
    return ret;
}
template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiOptionsAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Options>(ret, std::forward<Ts>(ts)...);
    return ret;
}

template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiPatchAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Patch>(ret, std::forward<Ts>(ts)...);
    return ret;
}

template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiPostAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Post>(ret, std::forward<Ts>(ts)...);
    return ret;
}

template <typename... Ts>
std::vector<AsyncWrapper<Response, true>> MultiPutAsync(Ts&&... ts) {
    std::vector<AsyncWrapper<Response, true>> ret{};
    priv::setup_multiasync<&cpr::Session::Put>(ret, std::forward<Ts>(ts)...);
    return ret;
}


} // namespace cpr

#endif
