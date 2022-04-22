#include <iostream>
#include <array>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <utility>

#include "naming.hpp"

bool BondCMP::operator()(const std::pair<int, std::wstring>& lhs, const std::pair<int, std::wstring>& rhs) const
{
	return lhs.first < rhs.first;
}

bool operator==( const std::pair<int, std::wstring>& lhs, const std::pair<int, std::wstring>& rhs)
{
	return lhs.first == rhs.first && lhs.second == rhs.second;
}


StructuralEquiv::StructuralEquiv(std::wstring ocp):orgcomp(ocp){
	shrinkToFit();
}

void StructuralEquiv::shrinkToFit(){
	std::wstring t;
	for(const auto& m : orgcomp)
		if(m != L' ') t += m;
	orgcomp = t;
}

std::vector<int> StructuralEquiv::initialNumericSequence(const wchar_t *&ps){
	std::vector<int> innums;
	int n = 0;
	while(*ps && ((*ps >= L'0' && *ps <= L'9')||*ps == L',')){
		if(*ps == L',' && n > 0){ innums.push_back(n); n = 0; }
		else n = n * 10 + (*ps - L'0');
		++ps;
	}
	if(n > 0) innums.push_back(n);
	return innums;
}

std::vector<std::pair<int, std::wstring>> StructuralEquiv::matchWithNumericSequence(const wchar_t *&stp, const wchar_t *ep){
	auto seq = initialNumericSequence(stp);
	int count = seq.size(), curidx = 0;
	std::vector<std::pair<int, std::wstring>> matches;
	while(count){
		if(*stp && *stp == '-') ++stp;
		auto mult = matchWithMultiples(stp, ep);
		if(mult.second > 0)
			count -= mult.second;
		else --count;
		if(mult.first){
			std::wstring substname = isSub(stp, ep) ? _Sub(stp, ep) : _Name(stp, ep);
			while(mult.second > 0){
				matches.push_back({seq[curidx++], substname});
				--mult.second;
			}
		}else matches.push_back({seq[curidx++], isSub(stp, ep) ? _Sub(stp, ep) : _Name(stp, ep)});
	}
	return matches;
}

std::pair<bool, int> StructuralEquiv::matchWithMultiples(const wchar_t *&st, const wchar_t *ep){
	for(const auto& mul : multiple){
		if(ep-st-mul.first.size() > 0 && mul.first == makeSTR(st, mul.first.size())){
			st += mul.first.size();
			return { true, mul.second };
		}
	}
	return { false , -1 };
}

bool StructuralEquiv::isSub(const wchar_t *st, const wchar_t *ep){
	if(ep - st == 0) return false;
	size_t n = 0;
	while(n < SSN){
		if(*st == L'-') ++st;
		if((ep-st-subt[n].size() > 0) && makeSTR(st, subt[n].size()) == subt[n])
			return true;
		++n;
	}

	return false;
}

bool StructuralEquiv::isAlkyl(const wchar_t *st, const wchar_t *ep)
{
	for(const auto& nm : name){
		if(*st == L'-') ++st;
		std::wstring t = nm.first + L"yl";
		if((ep-st-t.size() > 0) && makeSTR(st, t.size()) == t)
			return true;
	}
	return false;
}

std::wstring StructuralEquiv::_Sub(const wchar_t *&st, const wchar_t *ep)
{
	if(ep - st == 0) return L"";
	auto rs = makeSub(st, ep);
	st += rs.size();
	return rs;
}
std::wstring StructuralEquiv::_Name(const wchar_t *&st, const wchar_t *ep)
{
	if(ep - st == 0) return L"";
	auto rs = makeName(st, ep);
	st += rs.size();
	return rs;
}

std::wstring StructuralEquiv::makeSub(const wchar_t *&st, const wchar_t *ep)
{
	std::wstring rs;
	size_t n = 0;
	while(n < SSN){
		if((ep - st > 0) && *st == L'-') ++st;
		if((ep-st-subt[n].size() > 0) && ((rs = makeSTR(st, subt[n].size())) == subt[n]))
			return rs;
		++n;
	}
	return rs;
}

std::wstring StructuralEquiv::makeName(const wchar_t *&st, const wchar_t *ep)
{
	for(const auto& fg : fgpn){
		for(const auto& nm : name){
			std::wstring ss(nm.first+fg);
			if((ep - st > 0) && *st == L'-') ++st;
			if((ep-st-ss.size() > 0) && makeSTR(st, ss.size()) == ss)
				return ss;
		}
	}
	return L"";
}

std::wstring StructuralEquiv::makeSTR(const wchar_t *st, size_t n)
{
	std::wstring substr;
	while(n){ substr += *st++; --n; }
	return substr;
}

std::wstring StructuralEquiv::functionalGroup(const std::wstring& s)
{
	if(s.size() < 2) return L"";
	if(fgpntable.find(s.substr(s.size()-2)) != fgpntable.end()) return s.substr(s.size()-2);
	if(s.size() > 2 && fgpntable.find(s.substr(s.size()-3)) != fgpntable.end()) return s.substr(s.size()-3);
	return L"";
}
std::wstring StructuralEquiv::_Rem(const std::wstring& src, const std::wstring& substr)
{
	size_t pos = -1;
	if((pos = src.find(substr)) == size_t(-1)) return src;
	return src.substr(0, pos) + src.substr(pos+substr.size());
}

void StructuralEquiv::createStructure()
{
	const wchar_t *st = &orgcomp[0], *ep = &orgcomp[0] + orgcomp.size();
	auto iniseq = matchWithNumericSequence(st, ep);
	std::multiset<std::pair<int, std::wstring>, BondCMP> full_list;
	while(!iniseq.empty() && *st){
		full_list.insert(iniseq.begin(), iniseq.end());
		if( *st && *st == '-') ++st;
		iniseq = matchWithNumericSequence(st, ep);
	}
	if(full_list.empty()){
		if(isSub(st, ep))
			full_list.insert({1, _Sub(st, ep)});
		else if(isAlkyl(st, ep))
			full_list.insert({1, _Name(st, ep)});
	}

	int chlen = -1;
	for(const auto& nm : name){
		std::wstring ss(nm.first);
		if((ep - st > 0) && *st == L'-') ++st;
		if((ep-st-ss.size() > 0) && makeSTR(st, ss.size()) == ss){
			parent_name = ss;
			chlen = nm.second;
			break;
		}
	}
	st += parent_name.size();
	std::vector<int> bondpos;
	if(*st == L'-'){
		++st;
		bondpos = initialNumericSequence(st);
		if(*st == L'-') ++st;
		if(bondpos.size() > 1 && ep-st-bondpos.size() > 0)
			for(const auto& mul : multiple){
				if(mul.second == bondpos.size()){
					st += mul.first.size();
					break;
				}
			}
		parent_name += makeSTR(st, ep-st);

		for(const auto& bp : bondpos)
			full_list.insert({bp, bondtypes.find(functionalGroup(parent_name))->second});

	}else if(*st) parent_name += makeSTR(st, ep-st);

	if(bondpos.empty()){
		bool foundany = false;
		for(const auto& li : full_list){
			if(li.second.empty()){
				const_cast<std::pair<int, std::wstring> &>(li)
					.second =
					bondtypes
						.find(functionalGroup(parent_name))
						->second;
				foundany = true;
			}
		}
		if(!foundany){
			auto tmp = bondtypes.find(functionalGroup(parent_name))->second;
			if(tmp != L"-")
				full_list.insert({1, tmp});
		}
	}
	if(chlen != -1){
		std::wstring pbnd;
		for(int n = 1; n <= chlen; ++n){
			structequiv += L'C';
			auto attach = Bonds(n, full_list);
			int remh = MAXB - attach.first;
			std::wstring tmp, prev; int count = 0; std::wstring bnd;
			for(const auto& addn : attach.second){
				if(prev == addn){
					++count;
					prev = addn;
					continue;
				}else{
					if(isBond(addn)){
						bnd = addn;
						prev = addn;
						continue;
					}
					tmp += formulaEquiv(addn);
				}
				prev = addn;
			}
			if(chlen != 1){
				if(!bnd.empty()){
					pbnd = bnd;
					if(n > 1 && n < chlen) --remh;
				}else if(!pbnd.empty()){
					remh -= bondnum.find(pbnd)->second;
					if(n > 1 && n < chlen) --remh;
					pbnd.clear();
				}else{
					if(n == 1||n == chlen) --remh;
					else remh -= 2;
				}
			}
			if(remh)
				structequiv += L"H";
			if(remh > 1) structequiv += std::to_wstring(remh);
			if(attach.second.size() > 1){
				if(!tmp.empty()){
					structequiv += L"(" + tmp + L")";
					if(count)
						structequiv += std::to_wstring(count+1);
				}
			}else if(!tmp.empty()) structequiv += tmp;
			if(!bnd.empty()) structequiv += bnd;
			tmp.clear();
			if(!isBond(std::wstring(1, structequiv.back())) && n < chlen)
				structequiv += '-';
		}
	}
	std::wcout<<"Parent name: "<<parent_name<<", structure: "<<structequiv<<'\n';
}

std::pair<int, std::vector<std::wstring>> StructuralEquiv::Bonds(int n, const std::multiset<std::pair<int, std::wstring>, BondCMP>& list){
	std::vector<std::wstring> ret;
	int count = 0;
	for(const auto& li : list){
		if(li.first == n){
			if(isBond(li.second))
				count += bondnum.find(li.second)->second;
			else ++count;
			ret.push_back(li.second);
		}
	}
	return { count,	std::move(ret) };
}

bool StructuralEquiv::isBond(std::wstring s){
	return bondnum.find(s) != bondnum.end();
}

std::wstring StructuralEquiv::formulaEquiv(std::wstring s){
	auto p = name.find(_Rem(s, functionalGroup(s)));
	if(p != name.end()){
		auto pt = fgpntable.find(functionalGroup(s));
		if(pt != fgpntable.end()) return pt->second(p->second);
	}else{
		auto pp = subtable.find(_Rem(s, functionalGroup(s)));
		if(pp != subtable.end())
			return pp->second;
	}
	return L"";
}

const std::map<std::wstring, std::wstring> StructuralEquiv::bondtypes = {{L"ane", L"-"}, {L"ene", L"="}, {L"yne", L"≡"}};
const std::map<std::wstring, int> StructuralEquiv::bondnum = {{L"-", 1}, {L"=", 2}, {L"≡", 3}};
const  std::array<std::wstring, FGN> StructuralEquiv::fgpn = {L"yl", L"ane", L"ene", L"yne"};
const std::map<std::wstring, std::function<std::wstring(int)>> StructuralEquiv::fgpntable = {{L"yl", [](int n){ FORMULA(n, n * 2 + 1); }},
																							{L"ane", [](int n){ FORMULA(n, n * 2 + 2); }},
																							{L"ene", [](int n){ FORMULA(n, n * 2); }},
																							{L"yne", [](int n){ FORMULA(n, n * 2 - 2); }}};
const std::array<std::wstring, SSN> StructuralEquiv::subt = {L"fluoro", L"chloro", L"bromo", L"iodo"};
const std::map<std::wstring, std::wstring> StructuralEquiv::subtable = {{L"fluoro", L"F" },
																       {L"chloro",  L"Cl" },
																       {L"bromo", L"Br" },
																       {L"iodo", L"I" }};
const std::map<std::wstring, int> StructuralEquiv::name = {
    {L"meth", 1}, {L"eth", 2},  {L"prop", 3}, {L"but", 4}, {L"pent", 5},
    {L"hex", 6},  {L"hept", 7}, {L"oct", 8},  {L"non", 9}, {L"dec", 10}};
const std::map<std::wstring, int> StructuralEquiv::multiple = {{L"un", 1}, {L"di", 2}, {L"tri", 3},
												               {L"tetra", 4}, {L"penta", 5}, {L"hexa", 6},
												               {L"hepta", 7}, {L"octa", 8}, {L"nona", 9}};



int main(){
	enableUnicode();
	while(1){
		std::wstring ws;
		std::getline(std::wcin, ws);
		StructuralEquiv nm(ws);
		nm.createStructure();
	}
}
