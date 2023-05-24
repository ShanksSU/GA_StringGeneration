#pragma once
#ifndef GA_H
#define GA_H
#include <iostream>
#include <string>
#include <vector>
#include <random>
using namespace std;

namespace Random {
    extern mt19937 mt;
    template<typename T>
    T getRandom(T min, T max, typename enable_if<is_integral<T>::value >::type* = nullptr);

    template<typename T>
    T getRandom(T min, T max, typename enable_if<is_floating_point<T>::value >::type* = nullptr);

    template<typename Container, typename UnaryFunction>
    size_t rouletteWheel(double total, const Container& container, UnaryFunction&& func);

    template<typename Container>
    size_t rouletteWheel(double total, const Container& container);
}

class GA {
private:
    struct DNA {
        string dna = "";
        double fitness = 0.0;
    };

public:
    GA(const string& target);
    void run();
    string best() const;

private:
    void initGeneration();  //��l�ƺظs
    void nextGeneration();  //�U�@�N�ظs
    void showGeneration();
    DNA crossOver();
    void mutate(DNA& child);

    double calcFitness(const string& s) const;
    DNA getBest(const DNA& currentBest, const DNA& other) const;

private:
    string mTarget;
    vector<DNA> mPopulation;
    DNA mBest;
    int mTimer;
    int generations;

    static constexpr size_t MAX_POPULATION = 2000;  //�ظs�j�p
    static constexpr double CROSSOVER_RATE = 0.9;   //��e���v
    static constexpr double MUTATION_RATE = 0.001;  //�ܲ����v
};

string entry(string targetStr);

#endif

