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
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"});
if (r.status_code >= 400) {
    std::cerr << "Error [" << r.status_code << "] making request" << std::endl;
} else {
    std::cout << "Request took " << r.elapsed << std::endl;
    std::cout << "Body:" << std::endl << r.text;
}
```

The `Header` is essentially a map with an important modification. Its keys are case insensitive as required by [RFC 7230](http://tools.ietf.org/html/rfc7230#section-3.2):

```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"});
std::cout << r.header["content-type"] << std::endl;
std::cout << r.header["Content-Type"] << std::endl;
std::cout << r.header["CoNtEnT-tYpE"] << std::endl;
```

All of these should print the same value, `"application/json"`. Cookies similarly are accessed through a map-like interface, but they're not case insensitive:

```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/cookies/set?cookies=yummy"});
std::cout << r.cookies["cookies"] << std::endl; // Prints yummy
std::cout << r.cookies["Cookies"] << std::endl; // Prints nothing
```

As you can see, the `Response` object is completely transparent. All of its data fields are accessible at all times, and since its only useful to you insofar as it has information to communicate, you can let it fall out of scope safely when you're done with it.

## Request Headers

Speaking of the `Header`, you can set custom headers in the request call. The object is exactly the same:

{% raw %}
```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/headers"},
                  cpr::Header{{"accept", "application/json"}});
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
auto header = cpr::Header{{"header-key", "header-value"}};
auto parameters = cpr::Parameters{{"parameter-key", "parameter-value"}};
auto payload = cpr::Payload{{"payload-key", "payload-value"}};
auto multipart = cpr::Multipart{{"multipart-key", "multipart-value"}};
```
{% endraw %}

This isn't an accident -- all of these are map-like objects and their syntax is identical because their semantics depends entirely on the object type. Additionally, it's practical to have `Parameters`, `Payload`, and `Multipart` be swappable because APIs sometimes don't strictly differentiate between them.

## Session Objects

Under the hood, all calls to the primary API modify an object called a `Session` before performing the request. This is the only truly stateful piece of the library, and for most applications it isn't necessary to act on a `Session` directly, preferring to let the library handle it for you.

However, in cases where it is useful to hold on to state, you can use a `Session`:

{% raw %}
```c++
auto url = cpr::Url{"http://www.httpbin.org/get"};
auto parameters = cpr::Parameters{{"hello", "world"}};
cpr::Session session;
session.SetUrl(url);
session.SetParameters(parameters);

auto r = session.Get();             // Equivalent to cpr::Get(url, parameters);
std::cout << r.url << std::endl     // Prints http://www.httpbin.org/get?hello=world

auto new_parameters = cpr::Parameters{{"key", "value"}};
session.SetParameters(new_parameters);

auto new_r = session.Get();         // Equivalent to cpr::Get(url, new_parameters);
std::cout << new_r.url << std::endl // Prints http://www.httpbin.org/get?key=value
```
{% endraw %}

`Session` actually exposes two different interfaces for setting the same option. If you wanted you can do this instead of the above:

{% raw %}
```c++
auto url = cpr::Url{"http://www.httpbin.org/get"};
auto parameters = cpr::Parameters{{"hello", "world"}};
cpr::Session session;
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
auto fr = cpr::GetAsync(cpr::Url{"http://www.httpbin.org/get"});
// Sometime later
auto r = fr.get(); // This blocks until the request is complete
std::cout << r.text << std::endl;
```

The call is otherwise identical except instead of `Get`, it's `GetAsync`. Similarly for POST requests, you would call `PostAsync`. The return value of an asynchronous call is actually a `std::future<Response>`:

```c++
auto fr = cpr::GetAsync(cpr::Url{"http://www.httpbin.org/get"});
fr.wait() // This waits until the request is complete
auto r = fr.get(); // Since the request is complete, this returns immediately
std::cout << r.text << std::endl;
```

You can even put a bunch of requests into a `std` container and get them all later:

{% raw %}
```c++
auto container = std::vector<std::future<cpr::Response>>{};
auto url = cpr::Url{"http://www.httpbin.org/get"};
for (int i = 0; i < 10; ++i) {
    container.emplace_back(cpr::GetAsync(url, cpr::Parameters{{"i", std::to_string(i)}}));
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

## Asynchronous Callbacks

C++ Requests also supports a callback interface for asynchronous requests. Using the callback interface, you pass in a functor (lambda, function pointer, etc.) as the first argument, and then pass in the rest of the options you normally would in a blocking request. The functor needs to have a single parameter, a `Response` object -- this response is populated when the request completes and the function body executes.

Here's a simple example:

```c++
auto future_text = cpr::GetCallback([](cpr::Response r) {
        return r.text;
    }, cpr::Url{"http://www.httpbin.org/get"});
// Sometime later
if (future_text.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    std::cout << future_text.get() << std::endl;
}
```

There are a couple of key features to point out here:

1. The return value is a `std::string`. This isn't hardcoded -- the callback is free to return any value it pleases! When it's time to get that value, a check for if the request is complete is made, and a simple `.get()` call on the future grabs the correct value. This flexibility makes the callback interface delightfully simple, generic, and effective!
2. The lambda capture is empty, but absolutely doesn't need to be. Anything that can be captured inside a lambda normally can be captured in a lambda passed into the callback interface. This additional vector of flexibility makes it highly preferable to use lambdas, though any functor with a `Response` parameter will compile and work.

Additionally, you can enforce immutability of the `Response` simply with a `const Response&` parameter instead of `Response`.

## Setting a Timeout

It's possible to set a timeout for your request if you have strict timing requirements:

```c++
#include <assert.h>

auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"},
                  cpr::Timeout{1000}); // Let's hope we aren't using Time Warner Cable
assert(r.elapsed <= 1); // Less than one second should have elapsed
```

Setting the `Timeout` option sets the maximum allowed time the transfer operation can take. Since C++ Requests is built on top of libcurl, it's important to know what setting this `Timeout` does to the request. You can find more information about the specific libcurl option [here](http://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT_MS.html).

## Using Proxies

`Proxies`, like `Parameters`, are map-like objects. It's easy to set one:

{% raw %}
```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"},
                  cpr::Proxies{{"http", "http://www.fakeproxy.com"}});
std::cout << r.url << std::endl; // Prints http://www.httpbin.org/get, not the proxy url
```
{% endraw %}

It doesn't look immediately useful to have `Proxies` behave like a map, but when used with a `Session` it's more obvious:

{% raw %}
```c++
cpr::Session session;
session.SetProxies({{"http", "http://www.fakeproxy.com"},
                    {"https", "http://www.anotherproxy.com"}})
session.SetUrl("http://www.httpbin.org/get");
{
    auto r = session.Get();
    std::cout << r.url << std::endl; // Prints http://www.httpbin.org/get after going
                                     // through http://www.fakeproxy.com
}
session.SetUrl("https://www.httpbin.org/get");
{
    auto r = session.Get();
    std::cout << r.url << std::endl; // Prints https://www.httpbin.org/get after going
                                     // through http://www.anotherproxy.com
}
```
{% endraw %}

Setting `Proxies` on a `Session` lets you intelligently route requests using different protocols through different proxies without having to respecify anything but the request `Url`.

## Sending Cookies

Earlier you saw how to grab a cookie from the request:

```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/cookies/set?cookies=yummy"});
std::cout << r.cookies["cookies"] << std::endl; // Prints yummy
std::cout << r.cookies["Cookies"] << std::endl; // Prints nothing
```

You can send back cookies using the same object:

```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/cookies/set?cookies=yummy"});
auto another_r = cpr::Get(cpr::Url{"http://www.httpbin.org/cookies"}, r.cookies);
std::cout << another_r.text << std::endl;

/*
 * {
 *   "cookies": {
 *     "cookie": "yummy"
 *   }
 * }
 */
```

 This is especially useful because `Cookies` often go from server to client and back to the server. Setting new `Cookies` should not look surprising at all:

{% raw %}
```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/cookies"},
                  cpr::Cookies{{"ice cream", "is delicious"}});
std::cout << r.text << std::endl;

/*
 * {
 *   "cookies": {
 *     "ice%20cream": "is%20delicious"
 *   }
 * }
 */
```
{% endraw %}

 Take note of how the cookies were encoded using a url-encoding pattern, as required by [RFC 2965](http://www.ietf.org/rfc/rfc2965.txt). Other than that quirk, using `Cookies` is fairly straightforward and just works.

## PUT and PATCH Requests

PUT and PATCH requests work identically to POST requests, with the only modification being that the specified HTTP method is `"PUT"` or `"PATCH"` instead of `"POST"`. Use this when the semantics of the API you're calling implements special behavior for these requests:

{% raw %}
```c++
#include <assert.h>

// We can't POST to the "/put" endpoint so the status code is rightly 405
assert(cpr::Post(cpr::Url{"http://www.httpbin.org/put"},
                 cpr::Payload{{"key", "value"}}).status_code == 405);

// On the other hand, this works just fine
auto r = cpr::Put(cpr::Url{"http://www.httpbin.org/put"},
                  cpr::Payload{{"key", "value"}});
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {},
 *   "data": "",
 *   "files": {},
 *   "form": {
 *     "key": "value"
 *   },
 *   "headers": {
 *     ..
 *     "Content-Type": "application/x-www-form-urlencoded",
 *     ..
 *   },
 *   "json": null,
 *   "url": "https://httpbin.org/put"
 * }
 */
```
{% endraw %}

Most often, PUTs are used to update an existing object with a new object. Of course, there's no guarantee that any particular API uses PUT semantics this way, so use it only when it makes sense to. Here's a sample PATCH request, it's essentially identical:

{% raw %}
```c++
#include <assert.h>

// We can't POST or PUT to the "/patch" endpoint so the status code is rightly 405
assert(cpr::Post(cpr::Url{"http://www.httpbin.org/patch"},
                 cpr::Payload{{"key", "value"}}).status_code == 405);
assert(cpr::Put(cpr::Url{"http://www.httpbin.org/patch"},
                cpr::Payload{{"key", "value"}}).status_code == 405);

// On the other hand, this works just fine
auto r = cpr::Patch(cpr::Url{"http://www.httpbin.org/patch"},
                    cpr::Payload{{"key", "value"}});
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {},
 *   "data": "",
 *   "files": {},
 *   "form": {
 *     "key": "value"
 *   },
 *   "headers": {
 *     ..
 *     "Content-Type": "application/x-www-form-urlencoded",
 *     ..
 *   },
 *   "json": null,
 *   "url": "https://httpbin.org/patch"
 * }
 */
```
{% endraw %}

As with PUT, PATCH only works if the method is supported by the API you're sending the request to.

## Other Request Methods

C++ Requests also supports `DELETE`, `HEAD`, and `OPTIONS` methods in the expected forms:

```c++
// Regular, blocking modes
auto delete_response = cpr::Delete(cpr::Url{"http://www.httpbin.org/delete"});
auto head_response = cpr::Head(cpr::Url{"http://www.httpbin.org/get"});
auto options_response = cpr::OPTIONS(cpr::Url{"http://www.httpbin.org/get"});

// Asynchronous, future mode
auto async_delete_response = cpr::DeleteAsync(cpr::Url{"http://www.httpbin.org/delete"});
auto async_head_response = cpr::HeadAsync(cpr::Url{"http://www.httpbin.org/get"});
auto async_options_response = cpr::OptionsAsync(cpr::Url{"http://www.httpbin.org/get"});

// Asynchronous, callback mode
auto cb_delete_response = cpr::DeleteCallback([](cpr::Response r) {
        return r.text;
    }, cpr::Url{"http://www.httpbin.org/delete"});
auto cb_head_response = cpr::HeadCallback([](cpr::Response r) {
        return r.status_code;
    }, cpr::Url{"http://www.httpbin.org/get"});
auto cb_options_response = cpr::OptionsCallback([](cpr::Response r) {
        return r.status_code;
    }, cpr::Url{"http://www.httpbin.org/get"});
```

Currently, `"PATCH"` is not an implemented HTTP method. It soon will be, and its mechanics will be identitical to the example above. Stay tuned!
