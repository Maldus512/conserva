#ifndef ADAPTERS_TRAY_H_INCLUDED
#define ADAPTERS_TRAY_H_INCLUDED


#include "model/model.hpp"


class Tray {
  public:
    Tray(void);
    ~Tray(void);
    bool manage(Model &model);
    void update(Model &model);
};


#endif
