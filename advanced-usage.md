---
layout: default
title: cpr - Advanced Usage
---

## Response Objects

`Response` objects are bags of data. Their sole purpose is to give the client information at the end of a request -- there's nothing in the API that uses a `Response` after it gets back to you. This reasoning drove the decision to make the member fields of the response public and mutable.

A `Response` has these fields:

```c++
long status_code; // The HTTP status code for the request
std::string text; // The body of the HTTP response
Header header;    // A map-like collection of the header fields
Url url;          // The effective URL of the ultimate request
double elapsed;   // The total time of the request in seconds
Cookies cookies;  // A map-like collection of cookies returned in the request
```

and they're dead simple to access:

```c++
auto r = cpr::Get(Url{"http://www.httpbin.org/get"});
if (r.status_code >= 400) {
    std::cerr << "Error [" << r.status_code << "] making request" << std::endl;
} else {
    std::cout << "Request took " << r.elapsed << std::endl;
    std::cout << "Body:" << std::endl << r.text;
}
```

The `Header` is essentially a map with an important modification. Its keys are case insensitive as required by [RFC 7230](http://tools.ietf.org/html/rfc7230#section-3.2):

```c++
auto r = cpr::Get(Url{"http://www.httpbin.org/get"});
std::cout << r.header["content-type"] << std::endl;
std::cout << r.header["Content-Type"] << std::endl;
std::cout << r.header["CoNtEnT-tYpE"] << std::endl;
```

All of these should print the same value, `"application/json"`. Cookies similarly are accessed through a map-like interface, but they're not case insensitive:

```c++
auto r = cpr::Get(Url{"http://www.httpbin.org/cookies/set?cookies=yummy"});
std::cout << r.cookies["cookies"] << std::endl; // Prints yummy
std::cout << r.cookies["Cookies"] << std::endl; // Prints nothing
```

As you can see, the `Response` object is completely transparent. All of its data fields are accessible at all times, and since its only useful to you insofar as it has information to communicate, you can let it fall out of scope safely when you're done with it.

## Request Headers

Speaking of the `Headers`, you can set custom headers in the request call. The object is exactly the same:

{% raw %}
```c++
auto r = cpr::Get(Url{"http://www.httpbin.org/headers"},
                  Header{{"accept", "application/json"}});
std::cout << r.text << std::endl;

/*
 * "headers": {
 *   "Accept": "application/json",
 *   "Host": "www.httpbin.org",
 *   "User-Agent": "curl/7.42.0-DEV"
 * }
 */
```
{% endraw %}

You've probably noticed a similarity between `Header`, `Parameters`, `Payload`, and `Multipart`. They all have constructors of the form:

{% raw %}
```c++
auto header = Header{{"header-key", "header-value"}};
auto parameters = Parameters{{"parameter-key", "parameter-value"}};
auto payload = Payload{{"payload-key", "payload-value"}};
auto multipart = Multipart{{"multipart-key", "multipart-value"}};
```
{% endraw %}

This isn't an accident -- all of these are map-like objects and their syntax is identical because their semantics depends entirely on the object type. Additionally, it's practical to have `Parameters`, `Payload`, and `Multipart` be swappable because APIs sometimes don't strictly differentiate between them.

## Session Objects

Under the hood, all calls to the primary API modify an object called a `Session` before performing the request. This is the only truly stateful piece of the library, and for most applications it isn't necessary to act on a `Session` directly, preferring to let the library handle it for you.

However, in cases where it is useful to hold on to state, you can use a `Session`:

{% raw %}
```c++
auto url = Url{"http://www.httpbin.org/get"};
auto parameters = Parameters{{"hello", "world"}};
Session session;
session.SetUrl(url);
session.SetParameters(parameters);

auto r = session.Get();             // Equivalent to cpr::Get(url, parameters);
std::cout << r.url << std::endl     // Prints http://www.httpbin.org/get?hello=world

auto new_parameters = Parameters{{"key", "value"}};
session.SetParameters(new_parameters);

auto new_r = session.Get();         // Equivalent to cpr::Get(url, new_parameters);
std::cout << new_r.url << std::endl // Prints http://www.httpbin.org/get?key=value
```
{% endraw %}

`Session` actually exposes two different interfaces for setting the same option. If you wanted you can do this instead of the above:

{% raw %}
```c++
auto url = Url{"http://www.httpbin.org/get"};
auto parameters = Parameters{{"hello", "world"}};
Session session;
session.SetOption(url);
session.SetOption(parameters);
auto r = session.Get();
```
{% endraw %}

This is important so it bears emphasizing: *for each configuration option (like `Url`, `Parameters`), there's a corresponding method `Set<ObjectName>` and a `SetOption(<Object>)`*. The second interface is to facilitate the template metaprogramming magic that lets the API expose order-less methods.

The key to all of this is actually the way [libcurl](http://curl.haxx.se/libcurl/) is designed. It uses a somewhat [policy-based design](https://en.wikipedia.org/wiki/Policy-based_design) that relies configuring a single library object (the `curl` handle). Each option configured into that object changes its behavior in mostly orthogonal ways.

`Session` leverages that and exposes a more modern interface that's free of the macro-heavy hulkiness of libcurl. Understanding the policy-based design of libcurl is important for understanding the way the `Session` object behaves.

## Asynchronous Requests

Making an asynchronous request uses a similar but separate interface:

```c++
auto fr = cpr::GetAsync(Url{"http://www.httpbin.org/get"});
// Sometime later
auto r = fr.get(); // This blocks until the request is complete
std::cout << r.text << std::endl;
```

The call is otherwise identical except instead of `Get`, it's `GetAsync`. Similarly for POST requests, you would call `PostAsync`. The return value of an asynchronous call is actually a `std::future<Response>`:

```c++
auto fr = cpr::GetAsync(Url{"http://www.httpbin.org/get"});
fr.wait() // This waits until the request is complete
auto r = fr.get(); // Since the request is complete, this returns immediately
std::cout << r.text << std::endl;
```

You can even put a bunch of requests into a `std` container and get them all later:

{% raw %}
```c++
auto container = std::vector<std::future<Response>>{};
auto url = Url{"http://www.httpbin.org/get"};
for (int i = 0; i < 10; ++i) {
    container.emplace_back(cpr::GetAsync(url, Parameters{{"i", std::to_string(i)}}));
}
// Sometime later
for (auto& fr: container) {
    auto r = fr.get();
    std::cout << r.text << std::endl;
}
```
{% endraw %}

An important note to make here is that arguments passed to an asynchronous call are copied. Under the hood, an asychronous call through the library's API is done with `std::async`. By default, for memory safety, all arguments are copied (or moved if temporary) because there's no syntax level guarantee that the arguments will live beyond the scope of the request.

It's possible to force `std::async` out of this default so that the arguments are passed by reference as opposed to value. Currently, however, `cpr::<method>Async` has no support for forced pass by reference, though this is planned for a future release.

## Setting a Timeout

## Using Proxies

## Sending Cookies
