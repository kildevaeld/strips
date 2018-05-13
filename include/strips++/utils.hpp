#pragma once


namespace strips {

template <class F, class... Args> void iterate_vaargs(F f, Args &... args) {
  int x[] = {(f(args), 0)...};
}

}