#ifndef NAMING_HPP
#define NAMING_HPP

#include <iostream>
#include <array>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <utility>


#ifdef __WIN32
#include  <fcntl.h>
    #include <io.h>
    void enableUnicode(){
        __setmode(_fileno(stdout), _O_WTEXT);
    }
#else
void enableUnicode(){
	std::locale::global(std::locale("en_US.utf-8"));
	std::wcout.imbue(std::locale());
}
#endif
#define FGN 4
#define SSN 4
#define MAXB 4
#define FORMULA(a, b)                                                          \
  {                                                                            \
    auto _a = (a);                                                             \
    auto _b = (b);                                                             \
    std::wstring t(L"C");                                                      \
    if (_a / 10) {                                                             \
      t += wchar_t(_a / 10 + 48);                                              \
      t += wchar_t(_a % 10 + 48);                                              \
    } else if (_a % 10 > 1) {                                                  \
      t += wchar_t(_a % 10 + 48);                                              \
    }                                                                          \
    t += L"H";                                                                 \
    if (_b / 10) {                                                             \
      t += wchar_t(_b / 10 + 48);                                              \
      t += wchar_t(_b % 10 + 48);                                              \
    } else if (_b % 10 > 1) {                                                  \
      t += wchar_t(_b % 10 + 48);                                              \
    };                                                                         \
    return t;                                                                  \
  };
//Build the functional group tree,

struct BondCMP{
	bool operator()( const std::pair<int, std::wstring>& lhs, const std::pair<int, std::wstring>& rhs) const;
	friend bool operator==(const std::pair<int, std::wstring>& lhs, const std::pair<int, std::wstring>& rhs);
};
class StructuralEquiv{
	static const std::map<std::wstring, std::wstring> bondtypes;
	static const std::map<std::wstring, int> bondnum;
	static const std::array<std::wstring, FGN> fgpn;
	static const std::map<std::wstring, std::function<std::wstring(int)>> fgpntable;
	static const std::array<std::wstring, SSN> subt;
	static const std::map<std::wstring, std::wstring> subtable;
	static const std::map<std::wstring, int> name;
	static const std::map<std::wstring, int> multiple;
	std::wstring orgcomp,
		structequiv,
		parent_name;
public:

	explicit StructuralEquiv(std::wstring ocp);

	void shrinkToFit();

	std::vector<int> initialNumericSequence(const wchar_t *&ps);

	std::vector<std::pair<int, std::wstring>> matchWithNumericSequence(const wchar_t *&stp, const wchar_t *ep);

	static std::pair<bool, int> matchWithMultiples(const wchar_t *&st, const wchar_t *ep);

	static bool isSub(const wchar_t *st, const wchar_t *ep);

	static bool isAlkyl(const wchar_t *st, const wchar_t *ep);

	std::wstring _Sub(const wchar_t *&st, const wchar_t *ep);

	std::wstring _Name(const wchar_t *&st, const wchar_t *ep);

	static std::wstring makeSub(const wchar_t *&st, const wchar_t *ep);

	static std::wstring makeName(const wchar_t *&st, const wchar_t *ep);

	static std::wstring makeSTR(const wchar_t *st, size_t n);

	static std::wstring functionalGroup(const std::wstring& s);

	static std::wstring _Rem(const std::wstring& src, const std::wstring& substr);

	void createStructure();

	std::pair<int, std::vector<std::wstring>> Bonds(int n, const std::multiset<std::pair<int, std::wstring>, BondCMP>& list);

	static bool isBond(std::wstring s);

	static std::wstring formulaEquiv(std::wstring s);

};

#endif //NAMING_HPP
