#ifndef GA_H
#define GA_H
#include <iostream>
#include <string>
#include <vector>
#include <random>

namespace Random {
    extern std::mt19937 mt;
    template<typename T>
    T getRandom(T min, T max, typename std::enable_if<std::is_integral<T>::value >::type* = nullptr);

    template<typename T>
    T getRandom(T min, T max, typename std::enable_if<std::is_floating_point<T>::value >::type* = nullptr);

    template<typename Container, typename UnaryFunction>
    size_t rouletteWheel(double total, const Container& container, UnaryFunction&& func);

    template<typename Container>
    size_t rouletteWheel(double total, const Container& container);
}

class GA {
private:
    struct DNA {
        std::string dna = "";
        double fitness = 0.0;   //適應度
    };

public:
    GA(const std::string& target);
    void run();
    std::string best() const;

private:
    void initGeneration();  //初始化種群
    void nextGeneration();  //下一代種群
    void showGeneration();
    DNA crossOver();
    void mutate(DNA& child);

    double calcFitness(const std::string& s) const;
    DNA getBest(const DNA& currentBest, const DNA& other) const;

private:
    std::string mTarget;
    std::vector<DNA> mPopulation;
    DNA mBest;
    int mTimer;
    int generations;

    static constexpr size_t MAX_POPULATION = 2000;  //種群大小
    static constexpr double CROSSOVER_RATE = 0.9;   //交叉概率
    static constexpr double MUTATION_RATE = 0.001;  //變異概率
};
std::string entry(std::string targetStr);
#endif
