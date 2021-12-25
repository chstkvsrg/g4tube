/*
 * constants.h
 *
 *  Created on: Jun 24, 2019
 *      Author: fna
 */

#pragma once

#define DEFAULT_PORT		1532

#define BUFFER_LEN			30 * 1024 * 1024

// channels IDs and their REQUESTs
#define TEMPERATURE_CHANNEL_ID						1
#define TEMPERATURE_CHANNEL_GET_TEMPERATURE_REQ		1

#define GEANT4CALC_CHANNEL_ID						2
#define GEANT4CALC_CHANNEL_UPDATE_REQ				1
#define GEANT4CALC_CHANNEL_RESTART_REQ				2
#define GEANT4CALC_CHANNEL_GET_UNIT_INFO			3 // should bring to service channel
#define GEANT4CALC_CHANNEL_RUN_REQ					4
#define GEANT4CALC_CHANNEL_STOP_REQ					5
#define GEANT4CALC_CHANNEL_GET_PROCESSED_EVENTS_REQ	6
#define GEANT4CALC_CHANNEL_DOWNLOAD_STATS_REQ		7
#define GEANT4CALC_CHANNEL_GET_TEMP					8 // should bring to service channel
