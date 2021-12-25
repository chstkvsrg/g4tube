#pragma once

#include <G4NistManager.hh>
#include <G4Colour.hh>
#include <G4VisAttributes.hh>

#define Mat(x) (G4NistManager::Instance()->FindOrBuildMaterial(x))
#define Elem(x) (G4NistManager::Instance()->FindOrBuildElement(x))

#define COLOR_WHITE logic_->SetVisAttributes(G4VisAttributes(G4Colour(1, 1, 1, 1)));
#define COLOR_YELLOW logic_->SetVisAttributes(G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 1)));
#define COLOR_GREY logic_->SetVisAttributes(G4VisAttributes(G4Colour(128/255., 128/255., 128/255., 1)));
#define COLOR_TRANSPARENT logic_->SetVisAttributes(G4VisAttributes::Invisible);
#define COLOR_GLASS logic_->SetVisAttributes(G4VisAttributes(G4Colour(138/255., 204/255., 1, 0.6)));
#define COLOR_CU logic_->SetVisAttributes(G4VisAttributes(G4Colour(184/255., 115/255., 51/255., 1)));
#define COLOR_DETECTOR logic_->SetVisAttributes (G4Colour(82/255.,130/255.,96/255., 0.1));
#define COLOR_FERRUM logic_->SetVisAttributes (G4Colour(83/255.,98/255.,103/255., 1));
#define COLOR_TANTAL logic_->SetVisAttributes (G4Colour(171/255.,166/255.,173/255., 1));
#define COLOR_KILLER logic_->SetVisAttributes (G4Colour(255/255.,0/255.,0/255., 0.1));
#define COLOR_AL logic_->SetVisAttributes (G4Colour(165/255.,165/255.,165/255., 1));

//#define COLOR_1 logic_->SetVisAttributes(G4VisAttributes(G4Colour(227./255, 217./255, 255./255, 1)));
#define COLOR_TVEL logic_->SetVisAttributes (G4Colour(165/255.,165/255.,165/255., 0.3));
#define COLOR_ASPHALT logic_->SetVisAttributes(G4VisAttributes(G4Colour(80./255, 80./255, 80./255, 1)));

