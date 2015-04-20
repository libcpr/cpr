#include "api.h"

#include "payload.h"
#include "session.h"


Response cpr::Get(const Url& url, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(const Url& url, const Parameters& parameters, const Timeout& timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(const Url& url, const Authentication& auth, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(const Url& url, const Header& header, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(const Url& url, const Parameters& parameters, const Authentication& auth,
                  const Timeout& timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(const Url& url, const Parameters& parameters, const Header& header,
                  const Timeout& timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    return session.Get();
}

Response cpr::Get(const Url& url, const Authentication& auth, const Parameters& parameters, 
                  const Timeout& timeout) {
    return cpr::Get(url, parameters, auth, timeout);
}

Response cpr::Get(const Url& url, const Authentication& auth, const Header& header,
                  const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(const Url& url, const Header& header, const Parameters& parameters,
                  const Timeout& timeout) {
    return cpr::Get(url, parameters, header, timeout);
}

Response cpr::Get(const Url& url, const Header& header, const Authentication& auth,
                  const Timeout& timeout) {
    return cpr::Get(url, auth, header, timeout);
}

Response cpr::Get(const Url& url, const Parameters& parameters, const Authentication& auth,
                  const Header& header, const Timeout& timeout) {
    Session session;
    session.SetUrl(url, parameters);
    session.SetHeader(header);
    session.SetTimeout(timeout);
    session.SetAuth(auth);
    return session.Get();
}

Response cpr::Get(const Url& url, const Parameters& parameters, const Header& header,
                  const Authentication& auth, const Timeout& timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(const Url& url, const Authentication& auth, const Parameters& parameters,
                  const Header& header, const Timeout& timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(const Url& url, const Authentication& auth, const Header& header,
                  const Parameters& parameters, const Timeout& timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(const Url& url, const Header& header, const Parameters& parameters,
                  const Authentication& auth, const Timeout& timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Get(const Url& url, const Header& header, const Authentication& auth,
                  const Parameters& parameters, const Timeout& timeout) {
    return cpr::Get(url, parameters, auth, header, timeout);
}

Response cpr::Post(const Url& url, Payload&& payload, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetPayload(std::move(payload));
    session.SetTimeout(timeout);
    return session.Post();
}

Response cpr::Post(const Url& url, const Payload& payload, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetPayload(std::move(payload));
    session.SetTimeout(timeout);
    return session.Post();
}

Response cpr::Post(const Url& url, Multipart&& multipart, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetMultipart(std::move(multipart));
    session.SetTimeout(timeout);
    return session.Post();
}

Response cpr::Post(const Url& url, const Multipart& multipart, const Timeout& timeout) {
    Session session;
    session.SetUrl(url);
    session.SetMultipart(multipart);
    session.SetTimeout(timeout);
    return session.Post();
}
