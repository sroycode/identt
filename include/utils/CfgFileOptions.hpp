/**
 * @project identt
 * @file include/utils/CfgFileOptions.hpp
 * @author  S Roychowdhury <sroycode AT gmail DOT com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2017 S Roychowdhury.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *  CfgFileOptions.hpp :   Config File Reader
 *
 */
#ifndef _IDENTT_UTILS_CFGFILEOPTIONS_HPP_
#define _IDENTT_UTILS_CFGFILEOPTIONS_HPP_

#include <map>
#include <list>
#include <boost/utility/value_init.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "BaseUtils.hpp"


namespace identt {
namespace utils {
class CfgFileOptions {
public:
	using pointer=std::shared_ptr<CfgFileOptions>;
	using ssPairT=std::pair<std::string,std::string>;
	using ssMapT=std::map<std::string,std::string>;
	using sListT=std::list<std::string>;

	/**
	 * create
	 *
	 * @param InputFile
	 *   std::string Input File Name
	 *
	 */
	static pointer create(const std::string InputFile)
	{
		pointer p(new CfgFileOptions(InputFile));
		return p;
	}

	virtual ~CfgFileOptions();

	/**
	 * Check : Check if Value Exists
	 *
	 * @param Section
	 *   std::string Section to find it in
	 *
	 * @param Name
	 *   std::string Name to find
	 *
	 * @return
	 *   bool
	 */
	bool Check(const std::string Section, const std::string Name);

	/**
	 * Find: Find an entry
	 *
	 * @param Section
	 *   std::string Section to find it in
	 *
	 * @param Name
	 *   std::string Name to find
	 *
	 * @param NoThrow
	 *   Bool get default-val if true, defult false.
	 *
	 * @return
	 *   Type
	 */
	template<typename T>
	T Find(const std::string Section, const std::string Name, bool NoThrow=false)
	{
		auto it = SectionMap.find(MakeKey(Section,Name));
		if (it != SectionMap.end()) {
			return ConvertAnyToAny<T>(it->second,NoThrow);
		}

		/** else look for inherited section, duplicate is checked so inf loop not possible */
		auto jt = InheritMap.find(Section);
		while ( jt !=InheritMap.end()) {
			it = SectionMap.find(MakeKey(jt->second,Name));
			if (it != SectionMap.end()) {
				return ConvertAnyToAny<T>(it->second,NoThrow);
			}

			/** recurse if not found */
			jt = InheritMap.find(jt->second);
		}
		if(!NoThrow)
			throw identt::ConfigException("Invalid Config Param : " +Name);
		return T();
	}

	/**
	 * FindList: Find several entries
	 *
	 * @param Section
	 *   std::string Section to find it in
	 *
	 * @param NameList
	 *   sListT Names to find
	 *
	 * @param NoThrow
	 *   Bool get default-val if true, defult false.
	 *
	 * @return
	 *   Type
	 */
	template<typename T>
	T FindList(const std::string Section, const sListT NameList, bool NoThrow=false)
	{
		T ValueList;
		for(auto name : NameList) {
			ValueList.push_back( this->Find<T::value_type>(Section,name,NoThrow));
		}
		return ValueList;
	}

	/**
	 * Show: Show all entries
	 *
	 * @return
	 *   none
	 */
	void Show();

	/**
	 * Update: Update or Add an entry
	 *
	 * @param Section
	 *   std::string Section
	 * @param Name
	 *   std::string Name
	 * @param Value
	 *   std::string Value
	 *
	 * @return
	 *   none
	 */
	void Update(const std::string Section, const std::string Name, std::string Value);

	/**
	 * MakeKey: Merge Section and Name
	 *
	 * @param Section
	 *   std::string Section to find it in
	 *
	 * @param Name
	 *   std::string Name to find
	 *
	 * @return
	 *   std::string merged
	 */
	std::string MakeKey(std::string Section, std::string Name);
private:
	ssMapT SectionMap; /** Section + name = value  */
	ssMapT InheritMap; /** Section and inherits */

	/**
	 * Constructor : the default constructor
	 */
	CfgFileOptions() = default;
	CfgFileOptions(const CfgFileOptions&)=delete;

	/**
	 * Constructor : the used constructor
	 *
	 * @param InputFile
	 *   std::string Input File Name
	 *
	 */
	CfgFileOptions(const std::string InputFile);

	/**
	 * FromString: Get the Corresp value in a different Type
	 *
	 * @param InData
	 *   std::string input Data
	 *
	 * @param NoThrow
	 *   (optional) bool get default if error, default FALSE
	 *
	 * @return
	 *   T OutType
	 */
	template<typename T>
	inline T ConvertAnyToAny(std::string InData, bool NoThrow=false)
	{
		T OutData = T();

		try {
			OutData=boost::lexical_cast<T>(InData);

		} catch(std::exception& e) {
			if(! NoThrow)
				throw identt::ConfigException("conversion error");

		} catch(...) {
			if(! NoThrow)
				throw identt::ConfigException("conversion error");
		}

		return OutData;
	}
};
} // namespace utils
} // namespace identt
#endif
