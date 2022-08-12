#pragma once
#include<ECF/ECF.h>


//
// base Move class
//
class Move
{};
typedef boost::shared_ptr<Move> MoveP;


//
// base Path class
//
class Path
{};
typedef boost::shared_ptr<Path> PathP;


//
// base Problem class
//
class Problem : public EvaluateOp
{
public:
	virtual MoveP randomMove(IndividualP)
	{	return MoveP();	}
	virtual bool applyMove(IndividualP&, MoveP) 
	{	return false;	}
	virtual PathP initPathTo(IndividualP, IndividualP)
	{	return PathP();	}
	virtual PathP initPathAwayFrom(IndividualP)
	{	return PathP();	}
	virtual int getPathLength(PathP)
	{	return -1;	}
	virtual MoveP nextRandomMove(PathP)
	{	return MoveP();	}
};