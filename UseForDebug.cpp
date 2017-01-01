#include <string>

//Edit make a new cerr
#define IND IL std::cerr

//Edit DBG
#ifdef DBG
#undef DBG
#define DBG IL std::cerr << basename((char*)__FILE__) << ": " << __LINE__ \
				<< " - "<< __PRETTY_FUNCTION__ << std::endl;
#endif

//Use these two
#define FN BL qqq::sce __som_var__;
#define IL std::cerr << std::string(qqq::i*4, ' ');

//Lower level
#define BL std::cerr << std::string(qqq::i*4, ' ') << "{ "; DBG qqq::i++;

//Protect global namespace
namespace qqq {
	class sce {public: sce(){} ~sce();};
	int i = 0;
}

//Destructor prints end of scope string
qqq::sce::~sce(){qqq::i--;std::cerr << "} " << std::string(i*4, ' ') << "<\n-";}

