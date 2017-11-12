#include "project/solver.h"

namespace ss {

LayoutSolver::LayoutSolver(const Project& project, std::ostream& error)
	: m_project(project)
	, m_error(error)
{
}

bool LayoutSolver::BuildFlatMembers()
{
	for (auto& view : m_project.m_meta.m_views) {
		for (auto& member : view->m_members) {
			auto kv = m_flatMembers.find(member.first);
			if (kv == m_flatMembers.end())
			{
				m_flatMembers.insert(std::make_pair(member.first, member.second));
				
				m_memberToIndex.insert(std::make_pair(member.first, m_indexToMember.size()));
				m_indexToMember.push_back(member.first);
			}
			else if (kv->second != member.second)
			{
				m_error << " flatMembers member " << kv->first <<
					" type differs: " << ToString(kv->second) << " vs " << ToString(member.second);
				return false;
			}
		}
	}

	return true;
}

void LayoutSolver::BuildViewsAsBitsets()
{
	const MemberIndex memberUniverseSize = m_indexToMember.size();

	for (auto& view : m_project.m_meta.m_views) {
		Bitset bitset(memberUniverseSize);
		for (auto& member : view->m_members) {
			MemberIndex index = m_memberToIndex[member.first];
			bitset[index] = true;
		}
		m_viewToIndex[view->m_name] = m_indexToViewBitset.size();
		m_indexToViewBitset.push_back(bitset);
		m_error << view->m_name << " " << bitset << "\n";
	}
}

void LayoutSolver::SolveDisjointViews()
{
	using Index = unsigned long;
	const Index viewUniverseSize = (Index)m_indexToViewBitset.size();
	const Index numPermutations = (Index(1) << viewUniverseSize);
	for (Index p = 1; p < numPermutations; ++p) {
		Bitset permutation(viewUniverseSize, p);
		m_error << "permutation=" << permutation << "\n";

		std::vector<Bitset> coverage;		
		for (Index viewIndex = 0; viewIndex < viewUniverseSize; ++viewIndex) {
			if (permutation.test(viewIndex)) {
				const Bitset& viewMembers = m_indexToViewBitset[viewIndex];
				coverage.push_back(viewMembers);
				m_error << " viewMembers=" << viewMembers << "\n";
			}
		}

		bool process = true;

		const MemberIndex memberUniverseSize = m_indexToMember.size();
		Bitset universe(memberUniverseSize);

		for (int v1 = 0; v1 < coverage.size() && process; ++v1) {
			universe |= coverage[v1];
			for (int v2 = v1+1; v2 < coverage.size() && process; ++v2) {
				Bitset intersection = coverage[v1] & coverage[v2];
				if (intersection.any()) {
					process = false;
					break;
				}
			}
		}

		if (process && universe.all()) {
			m_error << " disjoint=" << permutation << "\n";
		}
	}
}

bool LayoutSolver::Solve()
{
	if (!BuildFlatMembers())
		return false;

	BuildViewsAsBitsets();
	SolveDisjointViews();
	return true;
}

}