#include "../hpp/benchmark_bs1.hpp"

NOINLINE(void Bs1::initialize())
{
    // NOOP
}
NOINLINE(void Bs1::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Bs1>::validation_assert(N);
}    
NOINLINE(double Bs1::construction(std::size_t N))
{
    return Benchmark<Signal, Bs1>::construction(N);
}
NOINLINE(double Bs1::destruction(std::size_t N))
{
    return Benchmark<Signal, Bs1>::destruction(N);
}
NOINLINE(double Bs1::connection(std::size_t N))
{
    return Benchmark<Signal, Bs1>::connection(N);
}
NOINLINE(double Bs1::emission(std::size_t N))
{
    return Benchmark<Signal, Bs1>::emission(N);
}
NOINLINE(double Bs1::combined(std::size_t N))
{
    return Benchmark<Signal, Bs1>::combined(N);
}
NOINLINE(double Bs1::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
