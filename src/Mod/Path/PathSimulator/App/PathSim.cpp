/***************************************************************************
 *   Copyright (c) Shsi Seger (shaise at gmail) 2017                       *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <boost/regex.hpp>

#include <App/Application.h>
#include <App/Document.h>
#include <Base/Exception.h>

#include "PathSim.h"
//#include "VolSim.h"

using namespace Base;
using namespace PathSimulator;

TYPESYSTEM_SOURCE(PathSimulator::PathSim , Base::BaseClass);

PathSim::PathSim()
{
	m_stock = nullptr;
	m_tool = nullptr;
}

PathSim::~PathSim()
{
	if (m_stock != nullptr)
		delete m_stock;
	if (m_tool != nullptr)
		delete m_tool;
}


void PathSim::BeginSimulation(Part::TopoShape * stock, float resolution)
{
	Base::BoundBox3d & bbox = stock->getBoundBox();
	m_stock = new cStock(bbox.MinX, bbox.MinY, bbox.MinZ, bbox.LengthX(), bbox.LengthY(), bbox.LengthZ(), resolution);
}

void PathSim::SetCurrentTool(Tool * tool)
{
	cSimTool::Type tp = cSimTool::FLAT;
	float angle = 180;
	switch (tool->Type)
	{
	case Tool::BALLENDMILL:
		tp = cSimTool::ROUND;
		break;

	case Tool::CHAMFERMILL:
		tp = cSimTool::CHAMFER;
		angle = tool->CuttingEdgeAngle;
		break;
	}
	m_tool = new cSimTool(tp, tool->Diameter / 2.0, angle);
}


void PathSim::ApplyCommand(Base::Placement * pos, Command * cmd)
{
	Point3D fromPos(*pos);
	Point3D toPos(cmd->getPlacement());
	if (cmd->Name == "G0" || cmd->Name == "G1")
	{
		m_stock->ApplyLinearTool(fromPos, toPos, *m_tool);
	}
	else if (cmd->Name == "G2")
	{
		Point3D cent(cmd->getCenter());
		m_stock->ApplyCircularTool(fromPos, toPos, cent, *m_tool, true);
	}
	else if (cmd->Name == "G3")
	{
		Point3D cent(cmd->getCenter());
		m_stock->ApplyCircularTool(fromPos, toPos, cent, *m_tool, false);
	}
}





 
