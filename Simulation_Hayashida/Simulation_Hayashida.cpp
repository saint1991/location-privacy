// LocationPrivacy.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "PaisSimulator.h"
#include "ISimulator.h"
#include "BasicDbMap.h"
#include "BasicUser.h"
#include "Dummy.h"
#include "BasicRequirement.h"
#include "SemanticTrajectory.h"
#include "TimeUtility.h"
#include "Rectangle.h"
#include "PauseMobileEntity.h"
#include "HayashidaSimulator.h"


int _tmain(int argc, _TCHAR* argv[])
{

	constexpr int USER_ID = 1;
	constexpr double TESTSET_PROPORTION = 0.3;
	const Graph::Rectangle<Geography::LatLng> MAP_BOUNDARY(35.615, 139.660, 35.695, 139.740);//(35.635, 139.675, 35.680, 139.720)
	std::stringstream export_path;
	export_path << "C:/Users/Shuhei/Desktop/EvaluationResults/" << Time::TimeUtility::current_timestamp() << "/";

	//������ISimulator�����������N���X���g��������
	//std::unique_ptr<Simulation::BaseSimulator> simulator
	//	= std::make_unique<Simulation::PaisSimulator>(USER_ID, TESTSET_PROPORTION);
	//simulator->prepare();
	//simulator->run();
	//simulator->evaluate();
	//simulator->export_evaluation_result(export_path.str());

	std::unique_ptr<Simulation::ISimulator<Map::BasicDbMap, Entity::PauseMobileEntity<Geography::LatLng>, Entity::PauseMobileEntity<Geography::LatLng>, Requirement::KatoMethodRequirement, Geography::LatLng, Graph::Trajectory<Geography::LatLng>>> simulator
		= std::make_unique<Simulation::HayashidaSimulator>(MAP_BOUNDARY);
	simulator->prepare();
	simulator->run();
	simulator->evaluate();
	simulator->export_evaluation_result();

	return 0;
}