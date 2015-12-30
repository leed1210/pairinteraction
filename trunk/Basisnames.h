#ifndef BASISNAMES_H
#define BASISNAMES_H

#include "dtypes.h"
#include "Iter.h"
#include "State.h"
#include "ConfParser.hpp"

#include <vector>
#include <unordered_set>

template<class T> class Basisnames {
public:
    Basisnames() {
    }
    void configure(const Configuration &config) {
        conf["deltaN"] = config["deltaN"];
        conf["deltaL"] = config["deltaL"];
        conf["deltaJ"] = config["deltaJ"];
        conf["deltaM"] = config["deltaM"];
        conf["deltaN"] >> delta_n;
        conf["deltaL"] >> delta_l;
        conf["deltaJ"] >> delta_j;
        conf["deltaM"] >> delta_m;
    }
    size_t size() const {
        return names_.size();
    }
    size_t dim() const {
        return dim_;
    }
    T& get (size_t idx) {
        return names_[idx];
    }
    const T& get (size_t idx) const {
        return names_[idx];
    }
    void set (size_t i, const T &v) {
        names_[i] = v;
    }
    Iter<Basisnames, T> begin() const {
        return Iter<Basisnames, T>( this, 0 );
    }
    Iter<Basisnames, T> end() const {
        return Iter<Basisnames, T>( this, names_.size() );
    }
    const Configuration& getConf() const { // TODO in Configurable Klasse auslagern, von der geerbt werrden soll
        return conf;
    }

protected:
    int delta_n, delta_l, delta_j, delta_m;
    Configuration conf;
    std::vector<T> names_;
    size_t dim_;

};


class BasisnamesOne : public Basisnames<StateOne>{
public:
    /*BasisnamesOne(const Configuration &config, const StateOne &startstate);
    BasisnamesOne(const Configuration &config, const StateTwo &startstate);*/
    static BasisnamesOne fromFirst(const Configuration &config);
    static BasisnamesOne fromSecond(const Configuration &config);
    static BasisnamesOne fromBoth(const Configuration &config);
    const std::vector<StateOne>& initial() const;
    void removeUnnecessaryStates(const std::vector<bool> &is_necessary);
private:
    BasisnamesOne();
    void build(StateOne startstate, std::string species);
    void build(StateTwo startstate, std::string species);
    std::vector<StateOne> states_initial;
};




class BasisnamesTwo : public Basisnames<StateTwo>{
public:
    BasisnamesTwo(const BasisnamesOne &basis_one1, const BasisnamesOne &basis_one2);
    void removeUnnecessaryStates(const std::vector<bool> &isNecessary);
};

#endif