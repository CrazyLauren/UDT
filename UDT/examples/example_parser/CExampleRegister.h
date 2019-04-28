/*
 * CExampleRegister.h
 *
 *  Created on:  22.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CEXAMPLEREGISTER_H_
#define CEXAMPLEREGISTER_H_

/*!\brief The class of object which is
 * added(registered) a parser to the kernel
 *
 * As the kernel is used the plugin idiom  you has
 * to define the class which will added the
 * a parser. Instead of defining a class
 * you can use macro #REGISTRE_ONLY_ONE_PROTOCOL_MODULE
 * (REGISTRE_ONLY_ONE_PROTOCOL_MODULE(CExampleProtocolParser,EXAMPLE_PARSER) )
 * if you what to define class "registrar" this code
 * can be used as example.
 * The code below is trivial for all parsers.
 */
class EXAMPLE_PARSER_EXPORT CExampleRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;///< The unique name of registrar

	/*!\brief A default constructor in which
	 * you must pass the version and unique name
	 *
	 */
	CExampleRegister();

	/*!\brief Function for unregister parser
	 *
	 */
	void MUnregisterFactory() const;
protected:

	/*!\brief Function for register parser
	 *
	 */
	void MAdding() const ;

	/*!\brief Function for check is parser is registered
	 *
	 */
	bool MIsAlreadyRegistered() const ;
};
#endif /* CEXAMPLEREGISTER_H_ */
