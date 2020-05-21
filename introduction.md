---
layout: default
title: cpr - Introduction
---

## Design

C++ Requests is designed to be as simple and pleasant to use as possible. HTTP method invokations through the primary API are short-lived and stateless -- this library firmly believes that [Resource Acquisition Is Initialization](https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization). Cleanup is as easy and intuitive as letting an object fall out of scope. You won't find any `init()` or `close()` methods here, and proper use of this library requires neither `delete` nor `free`.

In cpr, options are actually _options_ so if you don't set them they'll default to sensible values. This is facilitated by the keyword args-like interface:

{% raw %}
```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"},
                  cpr::Parameters{{"hello", "world"}}); // Url object before Parameters
```
{% endraw %}

{% raw %}
is exactly identical to

```c++
auto r = cpr::Get(cpr::Parameters{{"hello", "world"}},
                  cpr::Url{"http://www.httpbin.org/get"}); // Parameters object before Url
```
{% endraw %}

That is, the order of options is totally arbitrary, you can place them wherever you want and the outgoing call is the same. And that's pretty much it! Of course, there are lots of different ways to configure your requests, but by design they're all accessed through the same simple interface.

## GET Requests

Making a GET request with cpr is effortless:

```c++
#include <cpr/cpr.h> // Make sure this header is available in your include path

// Somewhere else
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"});
```

This gives us a `Response` object which we've called `r`. There's a lot of good stuff in there:

```c++
std::cout << r.url << std::endl; // http://www.httpbin.org/get
std::cout << r.status_code << std::endl; // 200
std::cout << r.header["content-type"] << std::endl; // application/json
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {},
 *   "headers": {
 *     ..
 *   },
 *   "url": "http://httpbin.org/get"
 * }
 */
```

To add URL-encoded parameters, throw in a `Parameters` object to the `Get` call:

{% raw %}
```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"},
                  cpr::Parameters{{"hello", "world"}});
std::cout << r.url << std::endl; // http://www.httpbin.org/get?hello=world
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {
 *     "hello": "world"
 *   },
 *   "headers": {
 *     ..
 *   },
 *   "url": "http://httpbin.org/get?hello=world"
 * }
 */
```
{% endraw %}

`Parameters` is an object with a map-like interface. You can construct it using a list of key/value pairs inside the `Get` method or have it outlive `Get` by constructing it outside:

{% raw %}
```c++
// Constructing it in place
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/get"},
                  cpr::Parameters{{"hello", "world"}, {"stay", "cool"}});
std::cout << r.url << std::endl; // http://www.httpbin.org/get?hello=world&stay=cool
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {
 *     "hello": "world"
 *     "stay": "cool"
 *   },
 *   "headers": {
 *     ..
 *   },
 *   "url": "http://httpbin.org/get?hello=world&stay=cool"
 * }
 */

 // Constructing it outside
auto parameters = cpr::Parameters{{"hello", "world"}, {"stay", "cool"}};
auto r_outside = cpr::Get(cpr::Url{"http://www.httpbin.org/get"}, parameters);
std::cout << r_outside.url << std::endl; // http://www.httpbin.org/get?hello=world&stay=cool
std::cout << r_outside.text << std::endl; // Same text response as above
```
{% endraw %}

A lot of the examples so far have constructed the option objects inside the method call, but there's no restriction on where those objects come from. The library is smart enough to know when it has to copy an argument and when it can safely [move](http://en.cppreference.com/w/cpp/utility/move) the object from the call site.

## POST Requests

Making a POST request is just as easy as a GET request:

{% raw %}
```c++
auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
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
 *   "url": "http://www.httpbin.org/post"
 * }
 */
```
{% endraw %}

This sends up `"key=value"` as a `"x-www-form-urlencoded"` pair in the POST request. To send data raw and unencoded, use `Body` instead of `Payload`:

{% raw %}
```c++
auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
                   cpr::Body{"This is raw POST data"},
                   cpr::Header{{"Content-Type", "text/plain"}});
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {},
 *   "data": "This is raw POST data",
 *   "files": {},
 *   "form": {},
 *   "headers": {
 *     ..
 *     "Content-Type": "text/plain",
 *     ..
 *   },
 *   "json": null,
 *   "url": "http://www.httpbin.org/post"
 * }
 */
```
{% endraw %}

Here you will notice that the `"Content-Type"` is being set explicitly to `"text/plain"`. This is because by default, `"x-www-form-urlencoded"` is used for raw data POSTs. For cases where the data being sent up is small, either `"x-www-form-urlencoded"` or `"text/plain"` is suitable. If the data package is large or contains a file, it's more appropriate to use a `Multipart` upload:

{% raw %}
```c++
auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
                   cpr::Multipart{{"key", "large value"},
                                  {"name", cpr::File{"path-to-file"}}});
std::cout << r.text << std::endl;

/*
 * {
 *   "args": {},
 *   "data": "",
 *   "files": {
 *     "name": <contents of file>
 *   },
 *   "form": {
 *     "key": "large value"
 *   },
 *   "headers": {
 *     ..
 *     "Content-Type": "multipart/form-data; boundary=--------------33b210e9d7b8bd02",
 *     ..
 *   },
 *   "json": null,
 *   "url": "http://www.httpbin.org/post"
 * }
 */
```
{% endraw %}

Notice how the `"Content-Type"` in the return header is different now; it's `"multipart/form-data"` as opposed to `"x-www-form-urlencoded"`. This facilitates larger and more generic data uploads with POST.

It is also possible to pass a buffer instead of a filename, if the file's content is already in memory.

{% raw %}
```c++
// STL containers like vector, string, etc.
std::vector<char> content{'t', 'e', 's', 't'};
auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
                   cpr::Multipart{{"key", "large value"},
                                  {"name", cpr::Buffer{content.begin(), content.end(), "filename.txt"}}});

// C-style pointers
const char *content = "test";
int length = 4;
auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
                   cpr::Multipart{{"key", "large value"},
                                  {"name", cpr::Buffer{content, content + length, "filename.txt"}}});
```
{% endraw %}

## Authentication

Any self-respecting networking library should have support for authentication. It's rare for a web API to allow unfettered access to the datasets they guard. Most often they require a username/password pair:

```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/basic-auth/user/pass"},
                  cpr::Authentication{"user", "pass"});
std::cout << r.text << std::endl;

/*
 * {
 *   "authenticated": true,
 *   "user": "user"
 * }
 */
```

This uses [Basic Authentication](https://en.wikipedia.org/wiki/Basic_access_authentication). To use [Digest Authentication](https://en.wikipedia.org/wiki/Digest_access_authentication), just use the `Digest` authentication object:

```c++
auto r = cpr::Get(cpr::Url{"http://www.httpbin.org/digest-auth/auth/user/pass"},
                  cpr::Digest{"user", "pass"});
std::cout << r.text << std::endl;

/*
 * {
 *   "authenticated": true,
 *   "user": "user"
 * }
 */
```

and like a good friend, cpr handles the negotiation for you.  `cpr::NTLM{"user", "pass"}` is also available for [NTLM authentication](https://en.wikipedia.org/wiki/NTLMSSP).

With these basic operations, modern C++ has access to a significant portion of the world wide web's APIs. For more complex applications, check out the [Advanced Usage guides](/cpr/advanced-usage.html).
