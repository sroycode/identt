/**
 * @project identt
 * @file src/utils/CfgFileOptions.hpp
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
 *  CfgFileOptions.cc :   Config File Reader implementation
 *
 */

#include <fstream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <boost/algorithm/string.hpp>

#include <utils/CfgFileOptions.hpp>


#define IDENTT_CFGFILE_COLON ':'
#define IDENTT_CFGFILE_SEMICOLON ';'
#define IDENTT_CFGFILE_SQB_OPEN '['
#define IDENTT_CFGFILE_SQB_CLOSE ']'
#define IDENTT_CFGFILE_HASH '#'
#define IDENTT_CFGFILE_EQUALTO '='
#define IDENTT_CFGFILE_MIDLINE_TRIM " \t\r\n"
#define IDENTT_CFGFILE_ENDLINE_TRIM " \t\r\n;"
#define IDENTT_CFGFILE_BLANK_STR ""
#define IDENTT_CFGFILE_DELIM_STR ":"

identt::utils::CfgFileOptions::~CfgFileOptions() {}

/**
 * Check : Check if Value Exists
 *
 */
bool identt::utils::CfgFileOptions::Check(const std::string Section, const std::string Name)
{
	try {
		std::string s = Find<std::string>(Section,Name);
		return true;
	} catch (identt::ConfigException& e) {
		return false;
	}
	return false; // never reaches
}

/**
 * Show: Show all entries
 *
 */
void identt::utils::CfgFileOptions::Show()
{
	for (auto const& it : SectionMap) {
		std::cerr << it.first << " = " << it.second << std::endl;
	}
}

/**
 * Update: Update or Add an entry
 *
 */
void identt::utils::CfgFileOptions::Update(const std::string Section, const std::string Name, std::string Value)
{
	SectionMap.insert(ssMapT::value_type(MakeKey(Section,Name),Value));
}

/**
 * MakeKey: Merge Section and Name
 *
 */
std::string identt::utils::CfgFileOptions::MakeKey(std::string Section, std::string Name)
{
	return Section + IDENTT_CFGFILE_DELIM_STR + Name;
}

/**
 * Constructor : the used constructor
 *
 */
identt::utils::CfgFileOptions::CfgFileOptions(const std::string InputFile)
{
	std::ifstream file(InputFile.c_str());
	if(!file.is_open())
		throw identt::ConfigException("Cannot Open Configfile");
	std::string line, inSection;
	while(std::getline(file,line)) {
		boost::algorithm::trim(line);
		if(! line.length()) continue;
		if(line[0] == IDENTT_CFGFILE_HASH) continue;
		if(line[0] == IDENTT_CFGFILE_SEMICOLON) continue;
		if(line[0] == IDENTT_CFGFILE_SQB_OPEN) {
			if(!line.find(IDENTT_CFGFILE_SQB_CLOSE))
				throw identt::ConfigException("Configfile Syntax Error " + line);

			/** section name */
			inSection=line.substr(1,line.find(IDENTT_CFGFILE_SQB_CLOSE)-1);
			boost::algorithm::trim(inSection);
			std::string inHerits = IDENTT_CFGFILE_BLANK_STR;
			ssMapT::const_iterator it = InheritMap.find(inSection);
			if(it!=InheritMap.end())
				throw identt::ConfigException("Duplicate Section : " + inSection);

			/** inherits name if exists */
			if(line.length() > line.find_last_of(IDENTT_CFGFILE_COLON)) {
				inHerits = line.substr(line.find_last_of(IDENTT_CFGFILE_COLON)+1);
				boost::algorithm::trim(inHerits);
				it = InheritMap.find(inHerits);
				if(it==InheritMap.end())
					throw identt::ConfigException("Invalid Inherit : " +  inSection);
			}
			InheritMap.insert(ssMapT::value_type(inSection,inHerits));
			continue;
		}
		size_t EqPos = line.find(IDENTT_CFGFILE_EQUALTO); /** position */
		if(EqPos==std::string::npos)
			throw identt::ConfigException("Invalid Line : " +  line);
		std::string myname = line.substr(0,EqPos);
		boost::algorithm::trim(myname);
		std::string myval = line.substr(EqPos+1);
		boost::algorithm::trim(myval);
		Update(inSection,myname,myval);
	}
	file.close();
}

