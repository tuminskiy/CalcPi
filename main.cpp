#include <iostream>
#include <string_view>
#include <random>
#include <cmath>

auto create_random_generator(double min_value, double max_value) {
  std::random_device rd;
  std::mt19937 gen{ rd() };
  std::uniform_real_distribution distrib{ min_value, max_value };

  return [gen = std::move(gen), distrib = std::move(distrib)]() mutable {
    return distrib(gen);
  };
}

template <class F>
auto create_test_runner(std::size_t MAX_RUNS, F test_case) {
  return [MAX_RUNS, test_case]() {
    std::size_t n_success = 0;

    for (std::size_t i = 0; i < MAX_RUNS; ++i) {
      if (test_case())
        ++n_success;
    }

    return n_success;
  };
}

double caluclate_pi(std::size_t n_success, std::size_t max_runs) {
  return 4.0 * n_success / static_cast<double>(max_runs);
}

template <class T>
auto input(std::string_view msg) {
  T value;
    
  std::cout << msg;
  std::cin >> value;

  return value;
}

int main() {
  const double x = input<double>("Input x: ");
  const double y = input<double>("Input y: ");
  const double r = input<double>("Input r: ");

  auto x_gen = create_random_generator(x - r, x + r);
  auto y_gen = create_random_generator(y - r, y + r);

  constexpr std::size_t MAX_RUNS = 10'000'000;

  auto circle_test = [&]() {
    const double x_ = std::pow(x_gen() - x, 2);
    const double y_ = std::pow(y_gen() - y, 2);
    const double r_ = r*r;
    
    return x_ + y_ - r_ < std::numeric_limits<double>::epsilon();
  };

  auto test_runner = create_test_runner(MAX_RUNS, circle_test);

  const std::size_t n_success = test_runner();

  std::cout << "Pi: " << caluclate_pi(n_success, MAX_RUNS);

  return 0;
}
