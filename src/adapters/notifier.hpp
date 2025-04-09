#ifndef ADAPTERS_NOTIFIER_HPP_INCLUDED
#define ADAPTERS_NOTIFIER_HPP_INCLUDED


#include "model/model.hpp"

class Notifier {
  public:
    Notifier(void);
    void show_update(const Model &model);
};

#endif
