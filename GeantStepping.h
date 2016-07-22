/*
 * GeantStepping.h
 *
 *  Created on: Jul 19, 2016
 *      Author: sorokin
 */

#ifndef GEANTSTEPPING_H_
#define GEANTSTEPPING_H_

#include "G4UserSteppingAction.hh"

namespace GFTest {

class GeantStepping : public G4UserSteppingAction
{
public:
    GeantStepping();
    virtual ~GeantStepping();

    virtual void UserSteppingAction(const G4Step*);
};

} /* namespace GFTest */

#endif /* GEANTSTEPPING_H_ */
