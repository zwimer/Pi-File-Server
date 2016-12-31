#include <string>

//Edit cerr
#define cerr IL cerr

//Edit DBG
#ifdef DBG
#undef DBG
#define DBG IL std::cout << basename((char*)__FILE__) << ": " << __LINE__ \
				<< " - "<< __PRETTY_FUNCTION__ << std::endl;
#endif

//Use these two
#define FN BL qqq::sce __som_var__;
#define IL std::cout << string(qqq::i*4, ' ');

//Lower level
#define BL std::cout << string(qqq::i*4, ' ') << "{ "; DBG qqq::i++;

//Protect global namespace
namespace qqq {

//For print stack frame fn
#include "/Users/zwimer/School/Other/FunCode/C/myLib.h"
#include "/Users/zwimer/School/Other/FunCode/C/myLib.c"

	//For FN macro
	class sce {public: sce(){} ~sce();};
	int i = 0;
}

//Destructor prints end of scope string
qqq::sce::~sce(){qqq::i--;cout << "} " << string(i*4, ' ') << "<-\n";}

