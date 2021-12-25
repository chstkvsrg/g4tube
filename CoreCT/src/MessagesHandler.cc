/*
 * DetectorMessenger.cc
 *
 *  Created on:
 *      Author: fna
 */

#include <chrono>

#include "MessagesHandler.hh"
#include "DetectorConstruction.hh"
#include "TargetsManager.hh"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4RunManager.hh>
#include <G4UIExecutive.hh>

extern G4UIExecutive * ui;

MessagesHandler* MessagesHandler::p_ = nullptr;

extern args_info_corect args_info;

MessagesHandler::MessagesHandler() : G4UImessenger(),
		runManager_(nullptr), UIExecutive_(nullptr)
{ 
	confDirectory_ = new G4UIdirectory("/conf/");
	confDirectory_->SetGuidance("Configuration parameters");

	runDirectory_ = new G4UIdirectory("/run/");
	runDirectory_->SetGuidance("Run");

	sessionDirectory_ = new G4UIdirectory("/session/");
	sessionDirectory_->SetGuidance("Session");

	runStepByStepBeamOnCmd_ = new G4UIcmdWithAString("/run/stepByStepBeamOn",this);
	runStepByStepBeamOnCmd_->SetParameterName("stepbystepbeamon", false);
	runStepByStepBeamOnCmd_->SetGuidance("Step by step running with prints and savings of gathered statistics into the disk on each step");

	startSessionCmd_ = new G4UIcmdWithoutParameter("/session/start",this);
	startSessionCmd_->SetGuidance("Start session");

}

MessagesHandler* MessagesHandler::instance()
{
	if (!p_)
		p_ = new MessagesHandler();
	return p_;
}

void MessagesHandler::free()
{
	if (p_) delete p_;
	p_ = nullptr;
}

void MessagesHandler::init(G4RunManager * runManager, G4UIExecutive *UIExecutive)
{
	runManager_ = runManager;
	UIExecutive_ = UIExecutive;
}


MessagesHandler::~MessagesHandler()
{
	delete confDirectory_;
	delete runDirectory_;
	delete sessionDirectory_;

	delete runStepByStepBeamOnCmd_;
	delete startSessionCmd_;
}


void MessagesHandler::SetNewValue(G4UIcommand* command, G4String newValue)
{
	printf("%s\n", command->GetCommandName().c_str());

	if (command == runStepByStepBeamOnCmd_)
	{
		if (args_info.distributed_flag) return; // ignore in this mode

		G4int eventsPerItr;
		unsigned long long eventsDesired;
		sscanf(newValue.c_str(), "%d %llu", &eventsPerItr, &eventsDesired);

		unsigned long long events = 0;

		while (1)
		{
			auto t1 = std::chrono::high_resolution_clock::now();
			std::cout << "beam on - start" << std::endl;
			runManager_->BeamOn(eventsPerItr);
			std::cout << "beam on - end" << std::endl;
			auto t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double,std::milli> elapsed = t2 - t1;

			TargetsManager::instance()->saveHisto();

			events += eventsPerItr;

			//G4cout << events << " events processed" << G4endl;
			std::cout << events << " events processed. this step has taken " << elapsed.count() << " ms" << std::endl;

			if (eventsDesired && events >= eventsDesired)
				break;
		}
	}
	else if (command == startSessionCmd_)
	{
		if (args_info.distributed_flag) return; // ignore in this mode

		UIExecutive_->SessionStart();
	}
	else
	{
		mapParams[command->GetCommandName()] = newValue;
	}

}


G4String MessagesHandler::paramValue(const G4String & paramName)
{
	return mapParams.count(paramName) ? mapParams[paramName] : "";
}



