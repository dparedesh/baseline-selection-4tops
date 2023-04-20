#ifndef _OFFSM4TOPSCONFIGURATION_H_
#define _OFFSM4TOPSCONFIGURATION_H_

#include "TTHbbConfiguration/GlobalConfiguration.h"

#include <string>
#include <utility>
#include <map>
#include <functional>

namespace TTHbb{

  class OffSM4topsConfiguration : public GlobalConfiguration {
    /**
     * Implementation of the TTHbb::Dictbase abstract base class
     * Tools should inherit from this to be useable in the OfflineTTHbb code.
     * In an inherited class, the functions all need to be defined and
     * the m_name property should be set in the constructor.
     **/

  public:
    /// Default constructor
    OffSM4topsConfiguration();
    ~OffSM4topsConfiguration(){};
    void AddSM4topsConfiguration();
    static OffSM4topsConfiguration* get();

  private:
    static OffSM4topsConfiguration* m_instance_4tops;

  };

  
}

#endif

