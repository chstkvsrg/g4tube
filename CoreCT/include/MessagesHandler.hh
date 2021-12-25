/*
 * MessagesHandler.hh
 *
 *  Created on:
 *      Author: fna
 */


#pragma once

#include <map>

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4RunManager;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;
class G4UIExecutive;

class MessagesHandler: public G4UImessenger
{
public:
	static MessagesHandler* instance();
	static void free();

	void init(G4RunManager * runManager, G4UIExecutive *UIExecutive);
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);
    
    //bool shouldStartSession();

    G4String paramValue(const G4String & paramName);

private:
	MessagesHandler();
   ~MessagesHandler();

   static MessagesHandler* p_;

    G4RunManager*      		runManager_;
    G4UIExecutive*			UIExecutive_;

    G4UIdirectory*			confDirectory_;
    G4UIdirectory*			runDirectory_;
    G4UIdirectory*			sessionDirectory_;

    G4UIcmdWithAString*		runStepByStepBeamOnCmd_;
    G4UIcmdWithoutParameter* startSessionCmd_;

    std::map<G4String, G4String> mapParams;


};




