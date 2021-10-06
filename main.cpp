#include <iostream>
#include <string_view>
#include <random>
#include <cmath>
#include <algorithm>
#include <iterator>

#define RUN_PI

auto create_random_generator(unsigned int seed, double min_value, double max_value) {
  std::mt19937 gen{ seed };
  std::uniform_real_distribution distrib{ min_value, max_value };

  return [=]() mutable { return distrib(gen); };
}

auto create_circle_test(double x, double y, double r) {
  std::random_device rd;

  return [x_gen = create_random_generator(rd(), x - r, x + r),
          y_gen = create_random_generator(rd(), y - r, y + r),
          x, y, r_ = r*r]() mutable {

    const double x_ = std::pow(x_gen() - x, 2);
    const double y_ = std::pow(y_gen() - y, 2);

    return x_ + y_ - r_ < std::numeric_limits<double>::epsilon();
  };
}

double func(double value) { return value*value*value + 1; }

auto create_function_test(double a, double b) {
  std::random_device rd;

  return [x_gen = create_random_generator(rd(), a, b),
          y_gen = create_random_generator(rd(), 0, func(b))]() mutable { return func(x_gen()) > y_gen(); };
}

template <class F>
auto create_test_runner(std::size_t max_runs, F& test_case) {
  return [max_runs, &test_case = test_case]() mutable {
    std::size_t n_success = 0;

    for (std::size_t i = 0; i < max_runs; ++i) {
      if (test_case())
        ++n_success;
    }

    return n_success;
  };
}

double calculate_pi(std::size_t n_success, std::size_t max_runs) {
  return 4.0 * static_cast<double>(n_success) / static_cast<double>(max_runs);
}

double calculate_integral(double a, double b, std::size_t n_success, std::size_t max_runs) {
  return static_cast<double>(n_success) / static_cast<double>(max_runs)
    * (b - a) * func(b);
}

template <class T>
auto input(std::string_view msg) {
  T value;
    
  std::cout << msg;
  std::cin >> value;

  return value;
}

int main() {
#ifdef RUN_PI
  const double x = input<double>("Input x: ");
  const double y = input<double>("Input y: ");
  const double r = input<double>("Input r: ");
#else
  const double a = input<double>("Input a: ");
  const double b = input<double>("Input b: ");
#endif

  const std::vector<std::size_t> MAX_RUNS = {
    10'000,     // 10^4
    100'000,    // 10^5
    1'000'000,  // 10^6
    10'000'000, // 10^7
    100'000'000 // 10^8
  };

#ifdef RUN_PI
  auto test = create_circle_test(x, y, r);
#else
  auto test = create_function_test(a, b);
#endif

  std::vector<std::vector<double>> series(MAX_RUNS.size());

  for (int i = 0; i < MAX_RUNS.size(); ++i) {
    std::size_t seria_index = 0;

    for (const std::size_t max_runs : MAX_RUNS) {
      auto test_runner = create_test_runner(max_runs, test);
      const std::size_t n_success = test_runner();

#ifdef RUN_PI
      const double value = calculate_pi(n_success, max_runs);
#else
      const double value = calculate_integral(a, b, n_success, max_runs);
#endif
      series[seria_index++].push_back(value);
    }
  }

  std::cout.precision(10);

  for (std::size_t i = 0; i < MAX_RUNS.size(); ++i) {
    std::cout << "Seria " << i + 1 << ": ";
    std:copy(
      std::cbegin(series[i]),
      std::cend(series[i]),
      std::ostream_iterator<double>{ std::cout, " "}
    );
    std::cout << '\n';
  }

  std::vector<std::vector<double>> epsilons;
  epsilons.reserve(5);

  for (const std::vector<double>& seria : series) {
    std::vector<double> epsilon(MAX_RUNS.size());

    std::transform(std::cbegin(seria), std::cend(seria), std::begin(epsilon),
      [](double value) { return std::abs( (value - M_PI) / M_PI ); }
    );

    epsilons.push_back(std::move(epsilon));
  }

  for (std::size_t i = 0; i < MAX_RUNS.size(); ++i) {
    std::cout << "Epsilon " << i + 1 << ": ";
    std::copy(
      std::cbegin(epsilons[i]),
      std::cend(epsilons[i]),
      std::ostream_iterator<double>{ std::cout, " "}
    );
    std::cout << '\n';
  }


  std::vector<double> mid_results;
  mid_results.reserve(MAX_RUNS.size());

  for (std::size_t run_index = 0; run_index < MAX_RUNS.size(); ++run_index) {
    const double sum = std::accumulate(std::cbegin(series), std::cend(series), 0.0,
      [run_index](double init, const std::vector<double>& seria) {
        return init + seria[run_index];
      }
    );

#ifdef RUN_PI
    const double value = M_PI;
#else
    const double value = 6;
#endif

    mid_results.push_back(std::abs( ((sum / static_cast<double>(MAX_RUNS.size())) - value) / value ));
  }

  std::cout << "Mid results: ";
  std::copy(
    std::cbegin(mid_results),
    std::cend(mid_results),
    std::ostream_iterator<double>{ std::cout, " " }
  );


  return 0;
}
