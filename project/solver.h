#pragma once

#include "base/base.h"
#include "base/dynamic_bitset.h"

#include "project/project.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ss {

class LayoutSolver {
public:
	LayoutSolver(const Project& project, std::ostream& error);
	bool Solve();

private:
	bool BuildFlatMembers();
	void BuildViewsAsBitsets();
	void SolveDisjointViews();

	const Project& m_project;
	std::ostream& m_error;

	using Bitset = base::dynamic_bitset<>;
	using MemberIndex = Bitset::size_type;
	using ViewIndex = Bitset::size_type;

	std::unordered_map<Name_t, ValueType> m_flatMembers;

	// MemberIndex <-> Member
	std::vector<Name_t> m_indexToMember;
	std::unordered_map<Name_t, MemberIndex> m_memberToIndex;

	// ViewIndex <-> ViewBitset
	std::unordered_map<Name_t, ViewIndex> m_viewToIndex;
	std::vector<Bitset> m_indexToViewBitset;
};

}
