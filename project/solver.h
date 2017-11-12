#pragma once

#include "base/base.h"
#include "base/dynamic_bitset.h"

#include "project/project.h"

#include <unordered_map>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace ss {

// Find maximum number of views that pairwise disjoint 
// (no two of them share a member)
// https://en.wikipedia.org/wiki/Set_packing
// An NP-complete problem.
//
class LayoutSolver {
public:
	LayoutSolver(const Project& project);

	using BestViews_t = std::vector<ViewName_t>;
	bool Solve(BestViews_t& outBestViews);
	
	std::string ErrorMessage() const;

private:
	bool BuildFlatMembers();
	void BuildViewsAsBitsets();
	bool SolveDisjointViews(BestViews_t& outBestViews);

	const Project& m_project;
	std::stringstream m_error;

	using Bitset = base::dynamic_bitset<>;

	using MemberIndex = Bitset::size_type;
	using Members = Bitset;
	using ViewIndex = Bitset::size_type;
	using Views = Bitset;

	std::unordered_map<MemberName_t, ValueType> m_flatMembers;

	// Map MemberIndex -> MemberName
	std::vector<MemberName_t> m_indexToMember;
	// Map MemberName -> MemberIndex
	std::unordered_map<MemberName_t, MemberIndex> m_memberToIndex;

	// Map ViewIndex -> ViewMembers Bitset
	std::vector<Members> m_indexToViewBitset;
	// Map ViewIndex -> ViewName
	std::vector<ViewName_t> m_indexToViewName;
};

}
