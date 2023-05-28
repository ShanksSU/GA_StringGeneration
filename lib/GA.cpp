#include "GA.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cmath>
#include <cassert>
#include <windows.h>
using namespace std;

namespace Random {
    mt19937 mt(random_device{}());

    template<typename T>
    T getRandom(T min, T max, typename enable_if<is_integral<T>::value >::type*) {
        return uniform_int_distribution<T>(min, max)(mt);
    }

    template<typename T>
    T getRandom(T min, T max, typename enable_if<is_floating_point<T>::value >::type*) {
        return uniform_real_distribution<T>(min, max)(mt);
    }

    template<typename Container, typename UnaryFunction>
    size_t rouletteWheel(double total, const Container& container, UnaryFunction&& func) {
        double target = getRandom(0.0, total);
        size_t index = 0;
        for (; index < container.size(); ++index) {
            target -= func(index);
            if (target <= 0.0) {
                break;
            }
        }
        return index;
    }

    template<typename Container>
    size_t rouletteWheel(double total, const Container& container) {
        auto func = [&container](size_t index) { return container[index]; };
        return rouletteWheel(total, container, func);
    }
}

GA::GA(const std::string& target)
    :mTarget(target), mTimer(clock()), generations(1){ //構造函數成員初始化列表
    initGeneration();
}

void GA::run() {
    while (mBest.dna != mTarget) {
        nextGeneration();
        //showGeneration();
        generations++;
    }
    printf_s("Generations：%d\n", generations - 1);
}

/*
函數的詳細步驟如下：
1.  使用std::generate_n函數和std::back_inserter構造函數生成MAX_POPULATION一個體(DNA)
    並將這些體添加到中mPopulation
2.  在std::generate_n任數的第三個參數中義了一個lambda任數, 該任數創造並返回一個新的DNA對象
3.  在該lambda函數中，創建一個新的DNA對象d。
4.  使用另一個std::generate_n和std::back_inserter生成一個新的字串, 並將這個字串添加到中d.dna
    這個字串的長度等於mTarget的長度, 字串中的每個字串都是一個在 ASCII 32 到 126 範圍的隨機字符
5.  計算新生成字串的適應度(fitness), 調用calcFitness函數, 然後將回傳結果存儲在d.fitness中
6.  使用getBest函數將新生的DNA對比圖與當前最好的DNA對比圖(mBest)進行對比, 如果新生的DNA對比圖更好, 則將mBest更新為新生的DNA對比圖
7.  最後返回新生的DNA對比圖, 它將被添加到mPopulation中
這個怪數的目標是初期化種群, 以後繼續可以開始進行化的迭代過程, 以找到最接近目標字串的字串

mBest = getBest(mBest, d);
這行代碼比較當前最佳 DNA 對象（mBest）和新生成的 DNA 對象（d）。 
getBest(mBest, d) 是一個函數，它返回這兩個對像中適應度較高的那一個。
如果新生成的 DNA 對象的適應度高於當前最佳 DNA 對象的適應度，
那麼 mBest 將被設為新生成的 DNA 對象；否則，mBest 的值保持不變。
*/

void GA::initGeneration() {
    generate_n(back_inserter(mPopulation), MAX_POPULATION,
        [this]() -> DNA {
            DNA d;
            generate_n(back_inserter(d.dna), mTarget.size(), []() { return Random::getRandom(32, 126); });
            d.fitness = calcFitness(d.dna);
            mBest = getBest(mBest, d);
            return d;
        }
    );
}

void GA::nextGeneration() {
    DNA nextBest = mBest;
    std::vector<DNA> newGeneration;
    newGeneration.reserve(mPopulation.size());

    for (const auto& p : mPopulation) {
        if (Random::getRandom(0.0, 1.0) < CROSSOVER_RATE) {
            DNA child = crossOver();
            newGeneration.push_back(child);
        }
        else {
            newGeneration.push_back(p);
        }

        DNA& child = newGeneration.back();
        mutate(child);

        child.fitness = calcFitness(child.dna);

        nextBest = getBest(mBest, child);
    }

    newGeneration.pop_back();
    newGeneration.push_back(mBest);
    mBest = nextBest;
    mPopulation = newGeneration;
}

void GA::showGeneration() {
    printf_s("%dth：%s\n", generations, mBest.dna.c_str());
}

GA::DNA GA::crossOver() {
    double totalFitness = accumulate(mPopulation.begin(), mPopulation.end(), 0.0, [](double count, const DNA& d) { return count + d.fitness; });
    auto func = [this](size_t index) { return mPopulation[index].fitness; };

    const DNA& parent1 = mPopulation[Random::rouletteWheel(totalFitness, mPopulation, func)];
    const DNA& parent2 = mPopulation[Random::rouletteWheel(totalFitness, mPopulation, func)];

    DNA child;
    child.dna.reserve(parent1.dna.size());

    for (size_t i = 0; i < parent1.dna.size(); ++i) {
        child.dna.push_back(Random::getRandom(0, 1) == 0 ? parent1.dna[i] : parent2.dna[i]);
    }

    return child;
}

void GA::mutate(DNA& child) {
    for (auto& i : child.dna) {
        if (Random::getRandom(0.0, 1.0) < MUTATION_RATE) {
            i = Random::getRandom(32, 126);
        }
    }
}

double GA::calcFitness(const std::string& s) const {
    double r = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == mTarget[i]) {
            ++r;
        }
    }

    return pow(r / mTarget.size(), 4);
}

GA::DNA GA::getBest(const DNA& currentBest, const DNA& other) const {
    return currentBest.fitness > other.fitness ? currentBest : other;
}

std::string GA::best() const {
    return mBest.dna;
}

std::string entry(std::string targetStr) {
    if (targetStr.empty()) {
        MessageBox(NULL, L"targetStr cannot be empty!", L"錯誤", MB_OK);
        exit(0);
    }
    printf_s("Target String：%s\n", targetStr.c_str());
    GA ga(targetStr);
    ga.run();
    return ga.best();
}
