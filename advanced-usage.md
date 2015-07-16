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

## Setting a Timeout

## Using Proxies

## Sending Cookies
