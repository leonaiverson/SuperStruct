#include "project/solver.h"

#include <iostream>

namespace ss {

LayoutSolver::LayoutSolver(const Project& project)
	: m_project(project)
{
}

bool LayoutSolver::BuildFlatMembers()
{
	for (auto& file : m_project.m_metadataFiles) {
		for (auto& view : file->m_meta.m_views) {
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
	}
	return true;
}

void LayoutSolver::BuildViewsAsBitsets()
{
	std::ostream& debug = std::cout;

	const MemberIndex memberUniverseSize = m_indexToMember.size();

	for (auto& file : m_project.m_metadataFiles) {
		for (auto& view : file->m_meta.m_views) {
			Members bitset(memberUniverseSize);
			for (auto& member : view->m_members) {
				MemberIndex index = m_memberToIndex[member.first];
				bitset[index] = true;
			}
			m_indexToViewName.push_back(view->m_name);
			m_indexToViewBitset.push_back(bitset);
			debug << view->m_name << " " << bitset << "\n";
		}
	}
}

bool LayoutSolver::SolveDisjointViews(BestViews_t& outBestViews)
{
	std::ostream& debug = std::cout;

	if (m_indexToViewBitset.size() > 64) {
		m_error << "More than 64 views to brute force";
		return false;
	}

	// TODO: precompute which views are disjoint at the start.

	using ViewsIndices = std::vector<ViewIndex>;
	ViewsIndices bestViews;
	size_t bestMemberCoverage;

	using Index = uint64_t;
	// TODO: if view1 and view2 are not disjoint, then never deal
	// with any bigger combinations containing those two.
	const Index viewUniverseSize = Index(m_indexToViewBitset.size());

	Index numPermutations;
	if (viewUniverseSize == 64)
		numPermutations = ULLONG_MAX;
	else
		numPermutations = (Index(1) << viewUniverseSize) - 1;

	// Iterate throw solution space.
	for (Index p = 1; p <= numPermutations && p != 0; ++p) {
		Views permutation(viewUniverseSize, p);

		// Skip single views.
		if (permutation.count() < 2)
			continue;

		// Convert bits back to a set of views (indices).
		ViewsIndices coverage;
		for (Index viewIndex = 0; viewIndex < viewUniverseSize; ++viewIndex) {
			if (permutation.test(viewIndex)) {
				coverage.push_back(viewIndex);
			}
		}

		bool isDisjoint = true;

		const MemberIndex memberUniverseSize = m_indexToMember.size();
		Members memberCoverage(memberUniverseSize);

		// Check views in the coverage if they are pairwise disjoint.
		for (int v1 = 0; v1 < coverage.size() && isDisjoint; ++v1) {
			const Members& v1members = m_indexToViewBitset[coverage[v1]];
			// Also build a union for member coverage:
			memberCoverage |= v1members;
			for (int v2 = v1+1; v2 < coverage.size() && isDisjoint; ++v2) {
				const Members& v2members = m_indexToViewBitset[coverage[v2]];

				Members intersection = v1members & v2members;
				// Break loops if not disjoint (if any shared member).
				if (intersection.any())
					isDisjoint = false;
			}
		}

		if (isDisjoint) {
			debug << " disjoint=" << permutation << "\n";
			size_t memberCoverageCount = memberCoverage.count();

			bool found = bestViews.empty();
			if (!found)
			{
				// Choose better coverage
				if (memberCoverageCount > bestMemberCoverage)
					found = true;
			}

			if (found)
			{
				bestViews.swap(coverage);
				bestMemberCoverage = memberCoverageCount;
			}
		}
	}

	if (bestViews.empty()) {
		m_error << "No disjoint views found";
		return false;
	}

	for (int v = 0; v < bestViews.size(); ++v) {
		const auto& viewName = m_indexToViewName[bestViews[v]];
		debug << "best view=" << viewName << "\n";
		outBestViews.push_back(viewName);
	}

	return true;
}

bool LayoutSolver::Solve(BestViews_t& outBestViews)
{
	if (!BuildFlatMembers())
		return false;

	BuildViewsAsBitsets();

	if (!SolveDisjointViews(outBestViews))
		return false;

	return true;
}

std::string LayoutSolver::ErrorMessage() const
{
	return m_error.str();
}

}