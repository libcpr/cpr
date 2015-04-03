#include "api.h"

#include "session.h"


Response cpr::Get(Url url, Timeout timeout) {
    Session session;
    session.SetUrl(url);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(Url url, Parameters parameters, Timeout timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(Url url, Authentication auth, Timeout timeout) {
    Session session;
    session.SetUrl(url);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(Url url, Header header, Timeout timeout) {
    Session session;
    session.SetUrl(url);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(Url url, Parameters parameters, Authentication auth, Timeout timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(Url url, Parameters parameters, Header header, Timeout timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(Url url, Authentication auth, Parameters parameters, Timeout timeout) {
    return cpr::Get(url, parameters, auth, timeout);
}

Response cpr::Get(Url url, Authentication auth, Header header, Timeout timeout) {
    Session session;
    session.SetUrl(url);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(Url url, Header header, Parameters parameters, Timeout timeout) {
    return cpr::Get(url, parameters, header, timeout);
}

Response cpr::Get(Url url, Header header, Authentication auth, Timeout timeout) {
    return cpr::Get(url, auth, header, timeout);
}

Response cpr::Get(Url url, Parameters parameters, Authentication auth, Header header,
                  Timeout timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(Url url, Parameters parameters, Header header, Authentication auth,
                  Timeout timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(Url url, Authentication auth, Parameters parameters, Header header,
                  Timeout timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(Url url, Authentication auth, Header header, Parameters parameters,
                  Timeout timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(Url url, Header header, Parameters parameters, Authentication auth,
                  Timeout timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(Url url, Header header, Authentication auth, Parameters parameters,
                  Timeout timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Post(Url url, Payload payload, Timeout timeout) {
    Session session;
    session.SetUrl(url);
    session.SetPayload(payload);
    session.SetTimeout(timeout);
    return session.Post();
}
