#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include "benchmark_utility.hpp"

template <typename Subject, typename Foo>
class Benchmark
{
    static std::vector<std::size_t> s_indices;
    static ChronoTimer s_timer;
    static Rng s_rng;

    static double calculate_score(std::size_t N, std::size_t limit, std::size_t count)
    {
        // Return the test size N divided by (the sample size / number of samples)
        return N / std::chrono::duration_cast<Delta_u>(Timer_u(limit / count)).count();
    }

    public:

    static void validation_assert(std::size_t N)
    {
        Rng rng;
        std::size_t count = 0;

        for (auto i = N; i != 0; --i, count += N)
        {
            Subject subject;
            {
                std::vector<Foo> foo_array(N);

                for (auto& foo : foo_array)
                {
                    Foo::connect_method(subject, foo);
                }
                Foo::emit_method(subject, rng);
            }
            // Make sure disconnect is working correctly
            Foo::emit_method(subject, rng);
        }
        // If the PRNG state is different
        // There is an error in the signal implementation
        Rng test;
        test.discard(count);
        assert(rng == test);
    }

    //--------------------------------------------------------------------------

    static double construction(std::size_t N)
    {
        std::size_t count = 1;
        std::size_t elapsed = 0;

        for (; elapsed < g_limit; ++count)
        {
            s_timer.reset();

            volatile Subject* subject = new Subject;
            std::vector<Foo> foo(N);

            elapsed += s_timer.count<Timer_u>();

            delete subject;
        }
        return calculate_score(N, elapsed, count);
    }

    //--------------------------------------------------------------------------

    static double destruction(std::size_t N)
    {
        std::size_t count = 1;
        std::size_t elapsed = 0;

        s_indices.resize(N);
        std::generate(s_indices.begin(), s_indices.end(), IncrementFill());

        for (; elapsed < g_limit; ++count)
        {
            std::shuffle(s_indices.begin(), s_indices.end(), s_rng);
            {
                Subject subject;
                std::vector<Foo> foo(N);

                for (auto index : s_indices)
                {
                    Foo::connect_method(subject, foo[index]);
                }
                s_timer.reset();
                
                // Destruction
            }
            elapsed += s_timer.count<Timer_u>();
        }
        return calculate_score(N, elapsed, count);
    }

    //--------------------------------------------------------------------------

    static double connection(std::size_t N)
    {
        std::size_t count = 1;
        std::size_t elapsed = 0;

        s_indices.resize(N);
        std::generate(s_indices.begin(), s_indices.end(), IncrementFill());

        for (; elapsed < g_limit; ++count)
        {
            std::shuffle(s_indices.begin(), s_indices.end(), s_rng);

            Subject subject;
            std::vector<Foo> foo(N);

            s_timer.reset();

            for (auto index : s_indices)
            {
                Foo::connect_method(subject, foo[index]);
            }
            elapsed += s_timer.count<Timer_u>();
        }
        return calculate_score(N, elapsed, count);
    }

    //--------------------------------------------------------------------------

    static double emission(std::size_t N)
    {
        std::size_t count = 1;
        std::size_t elapsed = 0;

        s_indices.resize(N);
        std::generate(s_indices.begin(), s_indices.end(), IncrementFill());
        
        for (; elapsed < g_limit; ++count)
        {
            std::shuffle(s_indices.begin(), s_indices.end(), s_rng);

            Subject subject;
            std::vector<Foo> foo(N);

            for (auto index : s_indices)
            {
                Foo::connect_method(subject, foo[index]);
            }
            s_timer.reset();

            Foo::emit_method(subject, s_rng);

            elapsed += s_timer.count<Timer_u>();
        }
        return calculate_score(N, elapsed, count);
    }

    //--------------------------------------------------------------------------

    static double combined(std::size_t N)
    {
        std::size_t count = 1;
        std::size_t elapsed = 0;

        s_indices.resize(N);
        std::generate(s_indices.begin(), s_indices.end(), IncrementFill());
        std::shuffle(s_indices.begin(), s_indices.end(), s_rng);

        s_timer.reset();
        
        for (; elapsed < g_limit; ++count, elapsed = s_timer.count<Timer_u>())
        {
            Subject subject;
            std::vector<Foo> foo(N);

            for (auto index : s_indices)
            {
                Foo::connect_method(subject, foo[index]);
            }
            Foo::emit_method(subject, s_rng);
        }
        return calculate_score(N, elapsed, count);
    }

    //--------------------------------------------------------------------------

    static double threaded(std::size_t N)
    {
        Subject subject;

        auto context = [&]()
        {
            std::size_t count = 1;
            std::size_t elapsed = 0;

            Rng rng (s_rng);
            ChronoTimer timer;

            for (; elapsed < g_limit; ++count, elapsed = timer.count<Timer_u>())
            {
                std::vector<Foo> foo(N);

                for (auto& foo_instance : foo)
                {
                    Foo::connect_method(subject, foo_instance);
                }
                Foo::emit_method(subject, rng);
            }
            return calculate_score(N, elapsed, count);
        };

        std::future<double> f1 = std::async(std::launch::async, context);
        std::future<double> f2 = std::async(std::launch::async, context);
        
        return (f1.get() + f2.get()) / 2.0;
    }
};

//------------------------------------------------------------------------------

template <typename Subject, typename Foo>
std::vector<std::size_t> Benchmark<Subject, Foo>::s_indices;

template <typename Subject, typename Foo>
ChronoTimer Benchmark<Subject, Foo>::s_timer;

template <typename Subject, typename Foo>
Rng Benchmark<Subject, Foo>::s_rng;

#endif // BENCHMARK_HPP
