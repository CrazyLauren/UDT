/*
 *
 * CShareLogArgsParser.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.11.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CSHARELOGARGSPARSER_H_
#define CSHARELOGARGSPARSER_H_

#include <string>
#include <vector>

#include <tclap/Arg.h>
#include <tclap/Constraint.h>

namespace TCLAP
{
	class SHARE_EXPORT Arg;
}
namespace NSHARE
{

class SHARE_EXPORT CShareLogArgsParser: public TCLAP::Arg,NSHARE::CDenyCopying
{
public:
	CShareLogArgsParser(const std::string& flag, const std::string& name,bool aIgnorable=true);
	virtual ~CShareLogArgsParser();
public:
	/**
	 * Handles the processing of the argument.
	 * This re-implements the Arg version of this method to set the
	 * _value of the argument appropriately.  It knows the difference
	 * between labeled and unlabeled.
	 *\param i - Pointer the the current argument in the list.
	 *\param args - Mutable list of strings. Passed from main().
	 */
	virtual bool processArg(int* i, std::vector<std::string>& args);

	 /**
	 * Returns the a short id string.  Used in the usage.
	 *\param val - value to be used.
	 */
	virtual std::string shortID(const std::string& val="val") const;

	/**
	 * Returns the a long id string.  Used in the usage.
	 *\param val - value to be used.
	 */
	virtual std::string longID(const std::string& val = "val") const;

	virtual void reset();

	bool argMatches(const std::string& argFlag) const;
	static std::string sMGetDescription();
private:
	void MHandleValue(const std::string& aOption,const std::string& aValue) const;
	void MParseValue(const std::string& aValue) const;
	//void MParseValue(std::vector<std::string>& args) const;
	bool MIsName(const std::string& argFlag) const;
	bool MIsFlag(const std::string& argFlag) const;

	static const std::pair<const char*,const char*> FOptsCommects[];
};
inline bool CShareLogArgsParser::processArg(int *i,
		std::vector<std::string>& args)
{
	if (_ignoreable && Arg::ignoreRest())
		return false;

	if (_hasBlanks(args[*i]))
		return false;

	std::string flag = args[*i];

	std::string value = "";
	trimFlag(flag, value);

	if (argMatches(flag))
	{

		//When the flag and value are not written in one word, the
		//value has to be contained in next "args"
		if (value == "" && TCLAP::Arg::argMatches(flag))
		{
			(*i)++;
			if (static_cast<unsigned int>(*i) < args.size())
				MParseValue(args[*i]);
			else
				throw(TCLAP::ArgParseException(
						"Missing a value for this argument!", toString()));
		}
		else if (MIsFlag(flag))
		{
			MParseValue(
					flag.substr(flagStartString().length() + _flag.length()));
		}
		else if (MIsName(flag))
		{
			MParseValue(
					flag.substr(nameStartString().length() + _name.length()));
		}
		else
		{
			throw(TCLAP::ArgParseException("Missing a value for this argument!",
					toString()));
		}

		_alreadySet = true;
		_checkWithVisitor();
		return true;
	}
	else
		return false;
}
} /* namespace NSHARE */
#endif /* CSHARELOGARGSPARSER_H_ */
