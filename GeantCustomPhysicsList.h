/*
 * GeantPhysicsList.h
 *
 *  Created on: Jul 19, 2016
 *      Author: sorokin
 */

#ifndef GEANTPHYSICSLIST_H__
#define GEANTPHYSICSLIST_H__

#include "G4VUserPhysicsList.hh"

namespace GFTest {

class GeantCustomPhysicsList : public G4VUserPhysicsList
{
public:
    GeantCustomPhysicsList();
    virtual ~GeantCustomPhysicsList() {};
    virtual void ConstructParticle();
    virtual void ConstructProcess();

};

} /* namespace GFTest */

#endif /* GEANTPHYSICSLIST_H__ */
