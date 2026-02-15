#include "cpr/callback.h"
#include "cpr/cprtypes.h"
#include <functional>

namespace cpr {
void CancellationCallback::SetProgressCallback(ProgressCallback& u_cb) {
    user_cb.emplace(std::reference_wrapper{u_cb});
}

bool CancellationCallback::operator()(cpr_pf_arg_t dltotal, cpr_pf_arg_t dlnow, cpr_pf_arg_t ultotal, cpr_pf_arg_t ulnow) const {
    const bool const_operation = !(cancellation_state->load());
    return user_cb ? (const_operation && (*user_cb)(dltotal, dlnow, ultotal, ulnow)) : const_operation;
}
} // namespace cpr
