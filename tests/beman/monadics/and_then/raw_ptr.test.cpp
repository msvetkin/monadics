// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

#include "helpers/raw-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<int*>);
}

struct Foo {
  int value{};
};
struct Boo {
  Foo *foo{nullptr};
};

TEST_CASE("with-value") {
  constexpr int value = [] () {
    Foo foo{.value = 5};
    Boo boo{.foo = &foo};

    auto result = &boo
      | and_then([] (auto &&b) {
        return b.foo;
      })
      | and_then([] (auto &&f) {
        f.value += 5;
        return &f;
      });
    return result->value;
  }();

  STATIC_REQUIRE(value == 10);
}

TEST_CASE("curl") {
  /*
   *
   QNetworkRequest request;
  request.setUrl(QUrl("http://qt-project.org"));
  request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

  manager | and_then(manager::get, request)
          | inspect(setupSignalAndSlots, this)


QNetworkReply *reply = manager->get(request);

  auto curl = curl_easy_init();

  auto result = curl
    | value_or(CURLUE_BAD_HANDLE)                                                      -> expected<CURL*, CURLcode>
    | and_then(curl_easy_setopt  | as_expected_fn, CURLOPT_URL, "https://example.com") -> expected<CURL*, CURLcode>
    | and_then(curl_easy_perform | as_expected_fn);                                    -> expected<CURL*, CURLcode>
    | finally(curl_easy_cleanup)                                      ->

  auto result = curl
    | value_or(CURLUE_BAD_HANDLE)                                     -> expected<CURL*, CURLcode>
    | and_then(curl_easy_setopt, CURLOPT_URL, "https://example.com")  -> expected<CURL*, CURLcode>
    | and_then([] (auto c) {
        auto r = curl_easy_setopt(c, CURLOPT_URL, "https://example.com");
        if (r != CURLE_OK) {
          return unexpected{c, r};
        } else {
          return expected{c};
        }
      }
    | and_then(curl_easy_perform);                                    -> expected<CURL*, CURLcode>
    | finally(curl_easy_cleanup)                                      ->

  auto curl = curl_easy_init();

  auto result = curl
    | unwarp
    | and_then([] (auto c) { return curl }
    | and_then(curl_easy_perform);

  curl_easy_cleanup(curl);

  CURL *curl = curl_easy_init();
  if(curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  auto curl = curl_easy_init();

  auto result = curl
    | and_then(curl_easy_setopt, CURLOPT_URL, "https://example.com")
    | and_then(curl_easy_perform)
    | and_then([&curl] {
          char *ct;
          return std::tuple{
            curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct),
            ct,
          }
        };
      });
    | flatten<1>

    | box_as_tuple_with<char *>
    | and_then([&curl] (code, char *ct) {
        return std::tuple{
          curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct),
          ct
        };
      });

  curl_easy_cleanup(curl);


    auto curl = curl_easy_init();

    auto result =
        curl
        | and_then(curl_easy_setopt, CURLOPT_URL, "https://www.example.com/")
        | map_to_monad<CURLcode>    // Switch from CURL* to CURLcode
        | and_then(curl_easy_perform)
        | and_then([curl](CURLcode res) -> CURLcode {
              if (res != CURLE_OK) return res;
              char* ct = nullptr;
              auto info_res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
              if (info_res == CURLE_OK && ct) {
                  std::printf("We received Content-Type: %s\n", ct);
              }
              return CURLE_OK;
          })
        | finally([curl] { curl_easy_cleanup(curl); });

    if (!has_value(result)) {
        std::fprintf(stderr, "Request failed\n");
        return 1;
    }
  */
}

} // namespace beman::monadics::tests
