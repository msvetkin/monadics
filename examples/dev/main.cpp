#include <iostream>

/*
   box - any box like type
         raw pointers

  box | and_then(...)
      | transform(...)
      | or_error...)
      | transform_error(...)
      | unwrap_or(...)


  optional<T> -> and_then -> optional<T/U>
  optional<T> -> and_then -> expected<T/U>

  raw pointes int, 


  CURL *handle = curl_new();

  handle | and_then(curl_set_options, );
  handle | and_then(std::bind_back(curl_set_options, FLAG, MASK));

  handle | and_then(curl_set_options, FLAG, MASK);
  handle | and_then(FLAG, MASK, curl_set_options);
  handle | and_then(MASK, curl_set_options, FLAG);

  result = opt?
  result = result?
  result = result?

  LinkedList list


  list | and_then(&LinkedList::node) -> Node *

  int *p | and_then;


  template <v2::is_box_type<std::optional> BoxTraits>
  constexpr BoxTraits as_box() {
      using T = BoxTraits::box_type;

      return {
          .value     = &T::value,
          .has_value = &T::operator bool,
          .error = std::nullopt
      };
  }



  std::optional<int> opt;
  auto value = co_await opt;
*/

int main() {
  std::cout << "Hello" << std::endl;
}
